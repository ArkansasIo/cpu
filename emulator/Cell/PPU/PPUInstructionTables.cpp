// PPUInstructionTables.cpp - the MySQL-backed CPU tables plus the
// class / subclass / tree / node structure PPU.cpp decodes through.
//
// Three storage layers, all for the same dataset:
//
//   MySQL server  <--create_cpu_dataset.sql-->  database `ps3clone`
//        |  SELECT row-by-row (PPUMySqlClient.h)
//        v
//   tables.json / CSV bridge (third_party/cpu_mysql/tables.json)
//        |  LoadFromJson / LoadFromCsv
//        v
//   in-memory rows: cpu_instruction / cpu_dataset / cpu_cache
//        |  BuildClassTree()
//        v
//   class / subclass / branch / node tree used by PPU::DecodeInstruction
#include "Cell/PPU/PPUInstruction.h"

#include "common/Logging.h"

#include <cstdio>
#include <fstream>
#include <sstream>

namespace ps3clone
{

    namespace
    {
        // ---- tiny, dependency-free JSON helpers --------------------------
        // Enough to read the AdonisJS-style tables.json blob without pulling
        // in a JSON library. It is intentionally forgiving: unknown keys are
        // skipped, and quotes are stripped from scalars.
        std::string StripQuotes(const std::string &s)
        {
            std::string out = s;
            if (!out.empty() && out.front() == '"')
            {
                out.erase(out.begin());
            }
            if (!out.empty() && out.back() == '"')
            {
                out.pop_back();
            }
            return out;
        }

        std::string ToLower(std::string s)
        {
            for (char &c : s)
            {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            return s;
        }

        bool IsDigits(const std::string &s)
        {
            if (s.empty())
            {
                return false;
            }
            for (char c : s)
            {
                if (!std::isdigit(static_cast<unsigned char>(c)))
                {
                    return false;
                }
            }
            return true;
        }

        u64 ToU64(const std::string &s, u64 fallback = 0)
        {
            std::string t = StripQuotes(s);
            if (t.empty())
            {
                return fallback;
            }
            // Accept 0x-prefixed hex as well as decimal - the tables use both.
            try
            {
                std::size_t consumed = 0;
                u64 v = std::stoull(t, &consumed, 0);
                return (consumed == t.size()) ? v : fallback;
            }
            catch (...)
            {
                return fallback;
            }
        }

        std::vector<std::string> SplitCsvLine(const std::string &line)
        {
            std::vector<std::string> cells;
            std::string cell;
            bool in_quotes = false;
            for (char c : line)
            {
                if (c == '"')
                {
                    in_quotes = !in_quotes;
                }
                else if (c == ',' && !in_quotes)
                {
                    cells.push_back(cell);
                    cell.clear();
                }
                else
                {
                    cell.push_back(c);
                }
            }
            cells.push_back(cell);
            return cells;
        }
    } // namespace

    // =====================================================================
    // PPUInstructionTables
    // =====================================================================

    PPUInstructionTables::PPUInstructionTables() { SeedBuiltins(); }

    void PPUInstructionTables::SeedBuiltins()
    {
        // A handful of PowerPC 2.02 + AltiVec rows so the CPU can decode
        // before any external dataset is attached. The full table is in the
        // dataset (csv/cpu_instruction.csv, tables.json, or MySQL).
        const PPUInstructionRow builtins[] = {
            {0x00000000, 0x00, 0x00, "nop", "int", 0, 1, false},
            {0x7C0802A6, 0x1F, 0x2A6, "mflr", "int", 1, 1, false},
            {0x7C0803A6, 0x1F, 0x3A6, "mtlr", "int", 1, 1, false},
            {0x4E800020, 0x13, 0x20, "blr", "branch", 0, 1, false},
            {0x48000000, 0x12, 0x00, "b", "branch", 1, 1, false},
            {0x38000000, 0x0E, 0x00, "addi", "int", 2, 1, false},
            {0x7C000214, 0x1F, 0x214, "add", "int", 3, 1, false},
            {0x10000000, 0x04, 0x00, "vaddfp", "vec", 3, 1, true},
            {0x7C0000CE, 0x1F, 0x0CE, "lvx", "vec", 2, 1, true},
        };
        for (const PPUInstructionRow &row : builtins)
        {
            AddInstruction(row);
        }
    }

    void PPUInstructionTables::AddInstruction(const PPUInstructionRow &row)
    {
        auto it = m_by_opcode.find(row.opcode);
        if (it != m_by_opcode.end())
        {
            m_instructions[it->second] = row;
            return;
        }
        m_by_opcode[row.opcode] = m_instructions.size();
        m_by_name[row.mnemonic] = m_instructions.size();
        m_instructions.push_back(row);
    }

    void PPUInstructionTables::AddDataset(const PPUDatasetRow &row) { m_dataset.push_back(row); }

    void PPUInstructionTables::AddCache(const PPUCacheRow &row) { m_cache.push_back(row); }

    const PPUInstructionRow *PPUInstructionTables::FindByOpcode(u32 instruction) const
    {
        // Exact 32-bit match first, then fall back to the primary opcode so
        // a partially decoded instruction still resolves to a row.
        auto it = m_by_opcode.find(instruction);
        if (it != m_by_opcode.end())
        {
            return &m_instructions[it->second];
        }
        const u16 primary = static_cast<u16>(instruction >> 26);
        for (const PPUInstructionRow &row : m_instructions)
        {
            if (row.primary == primary)
            {
                return &row;
            }
        }
        return nullptr;
    }

    const PPUInstructionRow *PPUInstructionTables::FindByMnemonic(const std::string &mnemonic) const
    {
        auto it = m_by_name.find(mnemonic);
        return (it == m_by_name.end()) ? nullptr : &m_instructions[it->second];
    }

    const PPUDatasetRow *PPUInstructionTables::DatasetAt(u64 index) const
    {
        return (index < m_dataset.size()) ? &m_dataset[index] : nullptr;
    }

    bool PPUInstructionTables::ParseTableBlob(const std::string &json_text)
    {
        // Walk the "rows": [ { ... }, { ... } ] arrays and pick fields by key.
        size_t pos = 0;
        int loaded_instructions = 0;
        int loaded_dataset = 0;
        int loaded_cache = 0;

        while ((pos = json_text.find('{', pos)) != std::string::npos)
        {
            const size_t end = json_text.find('}', pos);
            if (end == std::string::npos)
            {
                break;
            }
            const std::string obj = json_text.substr(pos, end - pos + 1);
            pos = end + 1;

            auto field = [&obj](const std::string &key) -> std::string
            {
                const std::string needle = "\"" + key + "\"";
                size_t k = obj.find(needle);
                if (k == std::string::npos)
                {
                    return {};
                }
                size_t colon = obj.find(':', k + needle.size());
                if (colon == std::string::npos)
                {
                    return {};
                }
                size_t start = colon + 1;
                while (start < obj.size() && (obj[start] == ' ' || obj[start] == '"'))
                {
                    ++start;
                }
                size_t stop = start;
                while (stop < obj.size() && obj[stop] != ',' && obj[stop] != '"' && obj[stop] != '}')
                {
                    ++stop;
                }
                return StripQuotes(obj.substr(start, stop - start));
            };

            if (obj.find("\"opcode\"") != std::string::npos &&
                obj.find("\"mnemonic\"") != std::string::npos)
            {
                PPUInstructionRow row;
                row.opcode = static_cast<u32>(ToU64(field("opcode")));
                row.primary = static_cast<u16>(row.opcode >> 26);
                row.secondary = static_cast<u16>(row.opcode & 0x7FF);
                row.mnemonic = field("mnemonic");
                row.group = field("group");
                row.operands = static_cast<u8>(ToU64(field("operands")));
                row.cycles = static_cast<u8>(ToU64(field("cycles"), 1));
                row.altivec = ToLower(field("altivec")) == "true" || field("altivec") == "1";
                AddInstruction(row);
                ++loaded_instructions;
            }
            else if (obj.find("\"byteaddress\"") != std::string::npos &&
                     obj.find("\"before\"") != std::string::npos)
            {
                PPUDatasetRow row;
                row.id = ToU64(field("id"));
                row.sample = ToU64(field("sample"));
                row.byteaddress = ToU64(field("byteaddress"));
                row.instruction = static_cast<u32>(ToU64(field("instruction")));
                row.mnemonic = field("mnemonic");
                row.cpu = field("cpu");
                row.thread = field("thread");
                row.reg = static_cast<u32>(ToU64(field("reg")));
                row.before = ToU64(field("before"));
                row.after = ToU64(field("after"));
                row.cycles = static_cast<u32>(ToU64(field("cycles")));
                AddDataset(row);
                ++loaded_dataset;
            }
            else if (obj.find("\"byteaddress\"") != std::string::npos &&
                     obj.find("\"line\"") != std::string::npos)
            {
                PPUCacheRow row;
                row.clock = ToU64(field("clock"));
                row.byteaddress = ToU64(field("byteaddress"));
                row.line = field("line");
                row.cpu = static_cast<u32>(ToU64(field("cpu")));
                row.bytes = static_cast<u32>(ToU64(field("bytes")));
                AddCache(row);
                ++loaded_cache;
            }
        }

        LogInfo("PPU.Table", "Parsed tables.json: instruction=" + std::to_string(loaded_instructions) +
                                 " dataset=" + std::to_string(loaded_dataset) +
                                 " cache=" + std::to_string(loaded_cache));
        return loaded_instructions > 0 || loaded_dataset > 0 || loaded_cache > 0;
    }

    bool PPUInstructionTables::LoadFromJson(const std::string &json_text)
    {
        return ParseTableBlob(json_text);
    }

    bool PPUInstructionTables::LoadFromJsonFile(const std::string &path)
    {
        std::ifstream in(path, std::ios::binary);
        if (!in)
        {
            LogWarn("PPU.Table", "tables.json not found: " + path);
            return false;
        }
        std::ostringstream buf;
        buf << in.rdbuf();
        return LoadFromJson(buf.str());
    }

    bool PPUInstructionTables::ParseCsvInto(const std::string &csv, int which)
    {
        std::istringstream in(csv);
        std::string line;
        bool header = true;
        size_t rows = 0;
        while (std::getline(in, line))
        {
            if (line.empty())
            {
                continue;
            }
            if (header)
            {
                header = false;
                continue; // skip the header row
            }
            const std::vector<std::string> c = SplitCsvLine(line);
            if (which == 0 && c.size() >= 3)
            {
                PPUInstructionRow row;
                row.opcode = static_cast<u32>(ToU64(c[0]));
                row.primary = static_cast<u16>(row.opcode >> 26);
                row.secondary = static_cast<u16>(row.opcode & 0x7FF);
                row.mnemonic = c[1];
                row.group = c[2];
                row.operands = (c.size() > 3) ? static_cast<u8>(ToU64(c[3])) : 0;
                row.cycles = (c.size() > 4) ? static_cast<u8>(ToU64(c[4], 1)) : 1;
                row.altivec = (c.size() > 5) ? (ToLower(c[5]) == "true" || c[5] == "1") : false;
                AddInstruction(row);
                ++rows;
            }
            else if (which == 1 && c.size() >= 7)
            {
                PPUDatasetRow row;
                row.id = ToU64(c[0]);
                row.sample = ToU64(c[1]);
                row.byteaddress = ToU64(c[2]);
                row.instruction = static_cast<u32>(ToU64(c[3]));
                row.mnemonic = c[4];
                row.cpu = c[5];
                row.thread = c[6];
                row.reg = (c.size() > 7) ? static_cast<u32>(ToU64(c[7])) : 0;
                row.before = (c.size() > 8) ? ToU64(c[8]) : 0;
                row.after = (c.size() > 9) ? ToU64(c[9]) : 0;
                row.cycles = (c.size() > 10) ? static_cast<u32>(ToU64(c[10])) : 0;
                AddDataset(row);
                ++rows;
            }
            else if (which == 2 && c.size() >= 3)
            {
                PPUCacheRow row;
                row.clock = ToU64(c[0]);
                row.byteaddress = ToU64(c[1]);
                row.line = (c.size() > 2) ? c[2] : std::string();
                row.cpu = (c.size() > 3) ? static_cast<u32>(ToU64(c[3])) : 0;
                row.bytes = (c.size() > 4) ? static_cast<u32>(ToU64(c[4])) : 0;
                AddCache(row);
                ++rows;
            }
        }
        LogInfo("PPU.Table", "Loaded " + std::to_string(rows) + " CSV rows (table " +
                                 std::to_string(which) + ")");
        return rows > 0;
    }

    namespace
    {
        bool ReadWholeFile(const std::string &path, std::string &out)
        {
            std::ifstream in(path, std::ios::binary);
            if (!in)
            {
                return false;
            }
            std::ostringstream buf;
            buf << in.rdbuf();
            out = buf.str();
            return true;
        }
    } // namespace

    bool PPUInstructionTables::LoadInstructionsFromCsv(const std::string &path)
    {
        std::string text;
        if (!ReadWholeFile(path, text))
        {
            LogWarn("PPU.Table", "csv not found: " + path);
            return false;
        }
        return ParseCsvInto(text, 0);
    }

    bool PPUInstructionTables::LoadDatasetFromCsv(const std::string &path)
    {
        std::string text;
        if (!ReadWholeFile(path, text))
        {
            LogWarn("PPU.Table", "csv not found: " + path);
            return false;
        }
        return ParseCsvInto(text, 1);
    }

    bool PPUInstructionTables::LoadCacheFromCsv(const std::string &path)
    {
        std::string text;
        if (!ReadWholeFile(path, text))
        {
            LogWarn("PPU.Table", "csv not found: " + path);
            return false;
        }
        return ParseCsvInto(text, 2);
    }

    bool PPUInstructionTables::SaveInstructionsToCsv(const std::string &path) const
    {
        std::ofstream out(path, std::ios::binary);
        if (!out)
        {
            return false;
        }
        out << "opcode,mnemonic,group,operands,cycles,altivec\n";
        for (const PPUInstructionRow &r : m_instructions)
        {
            char hex[16];
            snprintf(hex, sizeof(hex), "0x%08X", r.opcode);
            out << hex << ',' << r.mnemonic << ',' << r.group << ',' << +r.operands << ','
                << +r.cycles << ',' << (r.altivec ? "true" : "false") << '\n';
        }
        return true;
    }

    bool PPUInstructionTables::SaveDatasetToCsv(const std::string &path) const
    {
        std::ofstream out(path, std::ios::binary);
        if (!out)
        {
            return false;
        }
        out << "id,sample,byteaddress,instruction,mnemonic,cpu,thread,reg,before,after,cycles\n";
        for (const PPUDatasetRow &r : m_dataset)
        {
            out << r.id << ',' << r.sample << ',' << r.byteaddress << ',' << r.instruction << ','
                << r.mnemonic << ',' << r.cpu << ',' << r.thread << ',' << r.reg << ',' << r.before
                << ',' << r.after << ',' << r.cycles << '\n';
        }
        return true;
    }

    bool PPUInstructionTables::SaveCacheToCsv(const std::string &path) const
    {
        std::ofstream out(path, std::ios::binary);
        if (!out)
        {
            return false;
        }
        out << "clock,byteaddress,line,cpu,bytes\n";
        for (const PPUCacheRow &r : m_cache)
        {
            out << r.clock << ',' << r.byteaddress << ',' << r.line << ',' << r.cpu << ',' << r.bytes
                << '\n';
        }
        return true;
    }

    std::string PPUInstructionTables::Describe() const
    {
        return "PPUInstructionTables{instruction=" + std::to_string(m_instructions.size()) +
               ", dataset=" + std::to_string(m_dataset.size()) +
               ", cache=" + std::to_string(m_cache.size()) +
               ", line=" + std::to_string(kLineBytes) + "B, way=" + std::to_string(kLinesPerWay) +
               " lines}";
    }

    // =====================================================================
    // PPUByteCache - byteaddress -> cpu register window, LRU eviction.
    // =====================================================================

    PPUByteCache::PPUByteCache(size_t capacity) : m_capacity(capacity ? capacity : 1) {}

    bool PPUByteCache::Lookup(GuestAddr byteaddress, Entry &out)
    {
        // Wind the clock and fold the address onto its cache line (deflate mod
        // kLineBytes), then probe from most- to least-recently used.
        const GuestAddr key = LineBase(byteaddress);

        auto it = m_map.find(key);
        if (it == m_map.end())
        {
            ++m_misses;
            return false;
        }

        // Hit: promote to the front of the recency list.
        m_lru.splice(m_lru.begin(), m_lru, it->second.lru);
        out = it->second.entry;
        ++m_hits;
        ++m_clock;
        return true;
    }

    void PPUByteCache::Insert(GuestAddr byteaddress, u32 cpu, const u8 *line_bytes)
    {
        const GuestAddr key = LineBase(byteaddress);
        ++m_clock;

        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            it->second.entry.cpu = cpu;
            if (line_bytes)
            {
                std::copy(line_bytes, line_bytes + PPUInstructionTables::kLineBytes,
                          it->second.entry.bytes.begin());
            }
            m_lru.splice(m_lru.begin(), m_lru, it->second.lru);
            return;
        }

        if (m_map.size() >= m_capacity)
        {
            // Evict the least-recently used: the tail of the list.
            const GuestAddr victim = m_lru.back();
            m_lru.pop_back();
            m_map.erase(victim);
        }

        Node node;
        node.entry.byteaddress = key;
        node.entry.cpu = cpu;
        if (line_bytes)
        {
            std::copy(line_bytes, line_bytes + PPUInstructionTables::kLineBytes,
                      node.entry.bytes.begin());
        }
        m_lru.push_front(key);
        node.lru = m_lru.begin();
        m_map.emplace(key, node);
    }

    std::string PPUByteCache::Describe() const
    {
        return "PPUByteCache{entries=" + std::to_string(m_map.size()) + "/" +
               std::to_string(m_capacity) + ", hits=" + std::to_string(m_hits) +
               ", misses=" + std::to_string(m_misses) + ", clock=" + std::to_string(m_clock) + "}";
    }

    std::vector<PPUCacheRow> PPUByteCache::ToCacheRows() const
    {
        std::vector<PPUCacheRow> rows;
        rows.reserve(m_map.size());
        for (const GuestAddr key : m_lru) // front (MRU) first
        {
            auto it = m_map.find(key);
            if (it == m_map.end())
            {
                continue;
            }
            PPUCacheRow row;
            row.clock = m_clock;
            row.byteaddress = it->second.entry.byteaddress;
            row.line = std::to_string(PPUByteCache::LineIndex(key));
            row.cpu = it->second.entry.cpu;
            row.bytes = PPUInstructionTables::kLineBytes;
            rows.push_back(row);
        }
        return rows;
    }

    // =====================================================================
    // PPUClass (class / subclass / branch / node)
    // =====================================================================

    PPUClass::PPUClass(std::string name, std::vector<std::string> path, std::vector<Node> nodes)
        : m_name(std::move(name)), m_path(std::move(path)), m_nodes(std::move(nodes))
    {
    }

    std::string PPUClass::QualifiedName() const
    {
        std::string out;
        for (const std::string &part : m_path)
        {
            if (!out.empty())
            {
                out += '.';
            }
            out += part;
        }
        if (!out.empty())
        {
            out += '.';
        }
        out += m_name;
        return out;
    }

    std::string PPUClass::Describe() const
    {
        size_t branches = 0;
        for (const Node &n : m_nodes)
        {
            if (n.branch)
            {
                ++branches;
            }
        }
        return "PPUClass{" + QualifiedName() + ", nodes=" + std::to_string(m_nodes.size()) +
               ", branches=" + std::to_string(branches) +
               ", leaves=" + std::to_string(m_nodes.size() - branches) + "}";
    }

    // =====================================================================
    // PPUClassTree
    // =====================================================================

    void PPUClassTree::AddClass(const PPUClass &cls)
    {
        if (m_root.empty())
        {
            m_root = cls.Name();
        }
        m_classes[cls.Name()] = cls;
    }

    void PPUClassTree::Link(const std::string &parent, const std::string &child)
    {
        m_children[parent].push_back(child);
    }

    const PPUClass *PPUClassTree::Find(const std::string &name) const
    {
        auto it = m_classes.find(name);
        return (it == m_classes.end()) ? nullptr : &it->second;
    }

    const PPUClass *PPUClassTree::Root() const { return Find(m_root); }

    std::vector<const PPUClass *> PPUClassTree::Children(const std::string &name) const
    {
        std::vector<const PPUClass *> out;
        auto it = m_children.find(name);
        if (it == m_children.end())
        {
            return out;
        }
        for (const std::string &child : it->second)
        {
            if (const PPUClass *c = Find(child))
            {
                out.push_back(c);
            }
        }
        return out;
    }

    int PPUClassTree::Depth(const std::string &name) const
    {
        // Depth = number of parent links from the root (root == 0).
        for (const auto &kv : m_children)
        {
            for (const std::string &child : kv.second)
            {
                if (child == name)
                {
                    return 1 + Depth(kv.first);
                }
            }
        }
        return 0;
    }

    std::string PPUClassTree::Describe() const
    {
        return "PPUClassTree{classes=" + std::to_string(m_classes.size()) +
               ", root=" + (m_root.empty() ? std::string("<none>") : m_root) + "}";
    }

} // namespace ps3clone
