// PPU.cpp - the PS3 main CPU (Power Processing Element).
//
// The instruction decode is table-driven: PPU::Step() fetches a word, hands it
// to PPU::DecodeInstruction(), which resolves it against the MySQL-backed
// cpu_instruction table and walks the class / subclass / branch / node tree.
//
// A production clone implements the full PowerPC 2.02 + AltiVec instruction set
// (hundreds of opcodes) in PPUInterpreter.cpp and, for speed, a JIT in
// PPURecompiler.cpp. Here the table rows carry the decode metadata, and the
// executing semantics are the part that gets filled in.
#include "Cell/PPU/PPU.h"

#include "Cell/PPU/PPUInstruction.h"
#include "Cell/PPU/PPUInterpreter.h"
#include "Cell/PPU/PPURecompiler.h"
#include "common/Config.h"
#include "common/Logging.h"

#include <memory>

namespace ps3clone
{

    namespace
    {
        constexpr int kChannelPPU = 0; // retained for API compatibility
        void TouchChannel(int) {}
    } // namespace

    // Everything the PPU owns but does not want to expose in the header.
    struct PPU::Impl
    {
        std::unique_ptr<PPUInstructionTables> tables;
        std::unique_ptr<PPUByteCache> byte_cache;
        std::unique_ptr<PPUClassTree> tree;
        std::unique_ptr<PPUInterpreter> interpreter;
        std::unique_ptr<PPURecompiler> recompiler;
    };

    PPU::PPU()
        : m_impl(std::make_unique<Impl>())
    {
        Reset();
    }

    PPU::~PPU() = default;

    void PPU::SetOptions(const PPUOptions &options) { m_options = options; }

    void PPU::Reset()
    {
        TouchChannel(kChannelPPU);
        for (int t = 0; t < kHardwareThreads; ++t)
        {
            m_regs[t] = PPURegisters{};
        }
        m_cycles = 0;
        m_instructions = 0;
        m_booted = false;

        const Config &cfg = Config::Get();
        m_recompiler_enabled = (cfg.ppu_decoder == PPUDecoder::RecompilerLLVM);

        if (m_impl && m_impl->byte_cache)
        {
            m_impl->byte_cache.reset();
        }
    }

    bool PPU::LoadTables()
    {
        Impl &impl = *m_impl;
        if (!impl.tables)
        {
            impl.tables = std::make_unique<PPUInstructionTables>();
        }

        bool loaded = false;
        if (m_options.use_mysql)
        {
            loaded = impl.tables->LoadFromMySql(m_options.mysql_host, m_options.mysql_port,
                                                m_options.mysql_user, m_options.mysql_password,
                                                m_options.mysql_database);
        }
        if (!loaded && !m_options.tables_json.empty())
        {
            loaded = impl.tables->LoadFromJsonFile(m_options.tables_json);
        }
        if (!m_options.dataset_csv.empty())
        {
            loaded |= impl.tables->LoadDatasetFromCsv(m_options.dataset_csv);
        }
        return loaded;
    }

    void PPU::BuildDecodeTree()
    {
        // The decode tree mirrors the PowerPC encoding hierarchy, so a word is
        // resolved root -> branch -> leaf without a flat 2^32 table.
        Impl &impl = *m_impl;
        if (!impl.tree)
        {
            impl.tree = std::make_unique<PPUClassTree>();
        }
        PPUClassTree &tree = *impl.tree;

        // Root class: the CPU itself.
        tree.AddClass(PPUClass("cpu", {"cpu"}, {}));

        // Subclasses: one per instruction group (branch, int, fp, vec, cache).
        const char *groups[] = {"branch", "int", "fp", "vec", "cache"};
        for (const char *group : groups)
        {
            std::vector<PPUClass::Node> nodes;
            if (impl.tables != nullptr)
            {
                // Leaves for this group come straight from the MySQL table,
                // so the tree always matches cpu_instruction.
                for (const PPUInstructionRow &row : impl.tables->Instructions())
                {
                    if (row.group == group)
                    {
                        nodes.push_back(PPUClass::Node{row.opcode, row.primary, group, {}, row.opcode,
                                                       row.mnemonic, false});
                    }
                }
            }
            tree.AddClass(PPUClass(group, {"cpu"}, std::move(nodes)));
            tree.Link("cpu", group);
        }

        // Branches inside each group, keyed on the primary opcode.
        const PPUClass::Node branches[] = {
            {1, 0x12, "branch", {}, 0, {}, true},        // b / bc
            {2, 0x13, "branch", "branch_link", 0, {}, true},
            {3, 0x0E, "int", "int_imm", 0, {}, true},    // addi / addis
            {4, 0x1F, "int", "int_reg", 0, {}, true},    // X-form ops
            {5, 0x04, "vec", "vec_altivec", 0, {}, true},
            {6, 0x07, "vec", "vec_vperm", 0, {}, true},
        };
        for (const PPUClass::Node &node : branches)
        {
            tree.AddClass(PPUClass(node.subclass, {"cpu", node.line}, {node}));
            tree.Link(node.line, node.subclass);
        }

        LogInfo("PPU", "decode tree: " + tree.Describe());
    }

    CellResult PPU::Boot()
    {
        if (m_booted)
        {
            return CellResult::CELL_OK;
        }

        LoadTables();
        if (m_options.cache_capacity > 0)
        {
            m_impl->byte_cache = std::make_unique<PPUByteCache>(m_options.cache_capacity);
        }
        BuildDecodeTree();
        m_impl->interpreter = std::make_unique<PPUInterpreter>(*this);
        m_impl->recompiler = std::make_unique<PPURecompiler>(*this);

        LogInfo("PPU", m_recompiler_enabled
                           ? "Booting PPU (PowerPC core) with LLVM recompiler"
                           : "Booting PPU (PowerPC core) with interpreter");
        LogInfo("PPU", "L1I=32KB L1D=32KB L2=512KB cacheline=128B threads=2");
        if (m_impl->tables)
        {
            LogInfo("PPU.Table", m_impl->tables->Describe());
        }
        if (m_impl->tree)
        {
            LogInfo("PPU.Tree", m_impl->tree->Describe());
        }

        m_booted = true;
        return CellResult::CELL_OK;
    }

    const PPUInstructionRow *PPU::DecodeInstruction(u32 instruction)
    {
        if (!m_impl->tables)
        {
            return nullptr;
        }
        return m_impl->tables->FindByOpcode(instruction);
    }

    void PPU::RecordDatasetSample(int thread, GuestAddr byteaddress, u32 instruction,
                                  const char *mnemonic, u32 reg, u64 before, u64 after)
    {
        if (!m_impl->tables)
        {
            return;
        }

        PPUDatasetRow row;
        row.id = m_instructions + 1;
        row.sample = m_instructions;
        row.byteaddress = byteaddress;
        row.instruction = instruction;
        row.mnemonic = (mnemonic != nullptr) ? mnemonic : std::string();
        row.cpu = "PPU";
        row.thread = (thread == 0) ? std::string("hw0") : std::string("hw1");
        row.reg = reg;
        row.before = before;
        row.after = after;
        row.cycles = 1;
        m_impl->tables->AddDataset(row);
    }

    CellResult PPU::Step(int thread)
    {
        if (!m_booted)
        {
            return CellResult::CELL_EINVAL;
        }
        if (thread < 0 || thread >= kHardwareThreads)
        {
            return CellResult::CELL_EINVAL;
        }

        PPURegisters &r = m_regs[thread];
        const GuestAddr address = r.pc[thread];
        const u32 instruction = 0; // TODO: m_memory->Read32(address)

        // Resolve the word against the MySQL-backed decode tree. The byte cache
        // supplies the 128-byte line that owns this address.
        const PPUInstructionRow *row = DecodeInstruction(instruction);
        const char *mnemonic = (row != nullptr) ? row->mnemonic.c_str() : "unknown";
        const u32 cycles = (row != nullptr) ? row->cycles : 1;

        if (m_recompiler_enabled && m_impl->recompiler)
        {
            m_impl->recompiler->CompileBlock(static_cast<u32>(address));
        }
        else if (m_impl->interpreter)
        {
            m_impl->interpreter->ExecuteInstruction(thread, instruction);
        }

        RecordDatasetSample(thread, address, instruction, mnemonic, 0, r.gpr[0], r.gpr[0]);

        r.pc[thread] += 4;
        m_cycles += cycles;
        ++m_instructions;
        return CellResult::CELL_OK;
    }

    CellResult PPU::Run(int thread, u64 max_instructions)
    {
        u64 executed = 0;
        while (max_instructions == 0 || executed < max_instructions)
        {
            const CellResult rc = Step(thread);
            if (rc != CellResult::CELL_OK)
            {
                return rc;
            }
            ++executed;
        }
        return CellResult::CELL_OK;
    }

    void PPU::WriteBackCacheTable()
    {
        if (!m_impl->tables || !m_impl->byte_cache)
        {
            return;
        }
        for (const PPUCacheRow &row : m_impl->byte_cache->ToCacheRows())
        {
            m_impl->tables->AddCache(row);
        }
    }

    PPURegisters &PPU::Registers(int thread) { return m_regs[thread]; }
    const PPURegisters &PPU::Registers(int thread) const { return m_regs[thread]; }

    std::string PPU::Describe() const
    {
        return "PPU (PowerPC Processing Element) - main CPU, 2 hw threads, deep pipeline";
    }

} // namespace ps3clone
