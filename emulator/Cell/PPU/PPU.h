// PPU.h - the PS3 main CPU: Power Processing Element.
//
// The PPU is the general-purpose PowerPC core of the Cell Broadband Engine.
// It runs the OS (lv2), manages resources and dispatches work to the 7 SPUs.
//
//  - ISA:      PowerPC 2.02 + AltiVec/VMX, with the 2026 additions:
//              ISA 3.1 vector-pair / scalar ops, PCR, MMA-lite dot product
//  - Width:    64-bit RISC, dual-issue, in-order, deep pipeline
//  - Threads:  2 (simultaneous multithreading)
//  - L1:       32 KB I + 32 KB D, L2: 512 KB, cache line 128 B
//
// The decode table is NOT hard-coded here: it lives in the MySQL-backed
// PPUInstructionTables (cpu_instruction / cpu_dataset / cpu_cache), so the same
// binary can boot from tables.json, from CSV or from a live MySQL server.
#pragma once

#include "common/Types.h"

#include <array>
#include <memory>
#include <string>

namespace ps3clone
{

    class PPUInstructionTables;
    class PPUByteCache;
    class PPUClassTree;
    class PPUInterpreter;
    class PPURecompiler;
    struct PPUInstructionRow;

    // PowerPC general-purpose register file (64-bit).
    struct PPURegisters
    {
        std::array<u64, 32> gpr{}; // r0..r31
        std::array<u64, 32> fpr{}; // f0..f31 (floating point)
        std::array<u64, 2> lr{};   // link register per thread
        std::array<u64, 2> ctr{};  // count register per thread
        u64 pc[2] = {0, 0};        // program counter per hardware thread
        u32 cr = 0;                // condition register
        u32 msr = 0;               // machine state register
        u32 xer = 0;               // fixed-point exception register
    };

    // Where the CPU's instruction / dataset tables come from.
    struct PPUOptions
    {
        std::string tables_json;  // AdonisJS tables.json bridge
        std::string dataset_csv;  // optional cpu_dataset.csv
        bool use_mysql = false;   // pull the tables from a server instead
        std::string mysql_host = "127.0.0.1";
        u16 mysql_port = 3306;
        std::string mysql_user = "root";
        std::string mysql_password;
        std::string mysql_database = "ps3clone";
        size_t cache_capacity = 256; // LRU byte-cache slots
    };

    class PPU
    {
    public:
        static constexpr int kHardwareThreads = 2;
        static constexpr u32 kL1InstructionCacheSize = 32 * 1024;
        static constexpr u32 kL1DataCacheSize = 32 * 1024;
        static constexpr u32 kL2CacheSize = 512 * 1024;
        static constexpr u32 kCacheLineSize = 128; // == PPUInstructionTables::kLineBytes

        PPU();
        ~PPU();

        // Attach the MySQL-backed tables / dataset before Boot().
        void SetOptions(const PPUOptions &options);

        // Boot the PPU core: initialise registers, the decode tables, the byte
        // cache and the two hw threads.
        CellResult Boot();

        // Execute one instruction on hardware thread `thread` (0 or 1).
        // Dispatches to the interpreter or the JIT recompiler based on config.
        CellResult Step(int thread);

        // Run until the thread executes a stop/return or hits `max_instructions`.
        CellResult Run(int thread, u64 max_instructions = 0);

        // Reset to power-on state.
        void Reset();

        // Decode an instruction word against the MySQL table (cpu_instruction).
        // Returns nullptr when the opcode is not in the table.
        const PPUInstructionRow *DecodeInstruction(u32 instruction);

        // Record one executed instruction into the cpu_dataset table.
        void RecordDatasetSample(int thread, GuestAddr byteaddress, u32 instruction,
                                 const char *mnemonic, u32 reg, u64 before, u64 after);

        PPURegisters &Registers(int thread);
        const PPURegisters &Registers(int thread) const;

        PPUInstructionTables *Tables() const { return m_tables.get(); }
        PPUClassTree *DecodeTree() const { return m_tree.get(); }
        PPUByteCache *ByteCache() const { return m_byte_cache.get(); }

        std::string Describe() const;

    private:
        bool LoadTables();
        void BuildDecodeTree();
        void WriteBackCacheTable();

        PPURegisters m_regs[kHardwareThreads];
        bool m_booted = false;
        u64 m_cycles = 0; // timebase
        u64 m_instructions = 0;
        bool m_recompiler_enabled = true;

        PPUOptions m_options;

        // Owned by unique_ptr so this header stays free of the table/cache
        // definitions (the destructor is defined out-of-line in PPU.cpp).
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        // Convenience views onto the members of Impl, used by PPU.cpp.
        PPUInstructionTables *m_tables = nullptr;
        PPUClassTree *m_tree = nullptr;
        PPUByteCache *m_byte_cache = nullptr;
        PPUInterpreter *m_interpreter = nullptr;
        PPURecompiler *m_recompiler = nullptr;
    };

} // namespace ps3clone
