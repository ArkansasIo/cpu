
// PPUInstruction.h - the interface between PPU.cpp and the MySQL-backed
// instruction/dataset libraries.
//
// PPU.cpp deliberately keeps an empty `switch (instruction >> 26)` so the
// skeleton runs end-to-end. Everything that "does the work" lives in the two
// libraries declared here:
//
//   1. PPUInstructionTables - the mysql tables for this CPU:
//          cpu_instruction  (one row per PowerPC 2.02 / AltiVec opcode)
//          cpu_dataset      (one row per recorded instruction trace sample)
//          cpu_cache        (one row per LRU slot: byteaddress -> cpu register)
//      They are parsed from  third_party/cpu_mysql/tables.json (the AdonisJS
//      `config/database.ts` shape), from CSV, or queried from a real MySQL
//      server over the wire (see PPUMySqlClient.h).
//
//   2. PPUByteCache - the LRU bytes cache from the paste. Keys are guest
//      *byteaddresses*, values are the *cpu register* window the bytes decode
//      into, evicted least-recently-used first. The deflate math (mod 64)
//      turns a byte address into the 128-byte cache line the hub stores
//      (64 cache lines = 8 KB way).
//
// Nothing here includes a MySQL vendor SDK: the tables are just data, so the
// clone keeps building with a stock C++ toolchain (MSVC / clang / gcc).
#pragma once

#include "common/Types.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace ps3clone
{

    // ---------------------------------------------------------------------
    // One decoded CPU instruction row (table: cpu_instruction).
    // ---------------------------------------------------------------------
    struct PPUInstructionRow
    {
        u32 opcode = 0;              // full 32-bit instruction word (the "opcode")
        u16 primary = 0;             // bits 0..5  (word >> 26)
        u16 secondary = 0;           // bits 26..31 (w & 0x7FF) for extended opcodes
        std::string mnemonic;        // e.g. "addi", "lvx", "vperm"
        std::string group;           // "int", "fp", "vec", "branch", "cache"
        u8 operands = 0;             // how many of the 3 register fields are used
        u8 cycles = 1;               // nominal execution cycles
        bool altivec = false;        // VMX / AltiVec instruction
    };

    // ---------------------------------------------------------------------
    // One dataset sample row (table: cpu_dataset).
    // Mirrors the columns a trace/telemetry export would carry, so the same
    // struct can be written to MySQL, CSV or XLSX without translation.
    // ---------------------------------------------------------------------
    struct PPUDatasetRow
    {
        u64 id = 0;                  // sample id (auto increment in MySQL)
        u64 sample = 0;              // capture index
        u64 byteaddress = 0;         // guest address the bytes came from
        u32 instruction = 0;         // the 32-bit CPU word fetched there
        std::string mnemonic;        // decoded mnemonic
        std::string cpu;             // "PPU" / "SPU" / "RSX" tag column
        std::string thread;          // "hw0" / "hw1"
        u32 reg = 0;                 // destination CPU register
        u64 before = 0;              // register value before the step
        u64 after = 0;              // register value after the step
        u32 cycles = 0;              // cycles charged to this sample
    };

    // ---------------------------------------------------------------------
    // One LRU cache slot (table: cpu_cache).
    // ---------------------------------------------------------------------
    struct PPUCacheRow
    {
        u64 clock = 0;               // global LRU clock (counter)
        u64 byteaddress = 0;         // key: guest byteaddress
        std::string line;            // key: cache line tag
        u32 cpu = 0;                 // value: which CPU register holds the window
        u32 bytes = 0;               // size of the cached window
    };

    // ---------------------------------------------------------------------
    // The tables for one CPU. Owns the instruction rows, the dataset rows
    // and the LRU cache rows; can load them from JSON (AdonisJS tables.json),
    // from CSV, or from a live MySQL server, and can dump them back out.
    // ---------------------------------------------------------------------
    class PPUInstructionTables
    {
    public:
        static constexpr u32 kLineBytes = 128;  // PPU cache line = 128 B
        static constexpr u32 kLinesPerWay = 64; // 64 lines -> 8 KB way
        static constexpr u32 kWayBytes = kLineBytes * kLinesPerWay;

        PPUInstructionTables();

        // ---- loaders ---------------------------------------------------
        // tables.json shape: { "tables": { "cpu_instruction": { "rows": [..] } } }
        bool LoadFromJsonFile(const std::string &path);
        bool LoadFromJson(const std::string &json_text);

        // CSV exports (the XLSX-friendly form): header row + one row per line.
        bool LoadInstructionsFromCsv(const std::string &path);
        bool LoadDatasetFromCsv(const std::string &path);
        bool LoadCacheFromCsv(const std::string &path);

        // Load every table from a live MySQL server. Implemented in
        // PPUInstructionTables.cpp on top of PPUMySqlClient.h.
        bool LoadFromMySql(const std::string &host, u16 port,
                           const std::string &user, const std::string &password,
                           const std::string &database);

        // ---- writes -----------------------------------------------------
        bool SaveInstructionsToCsv(const std::string &path) const;
        bool SaveDatasetToCsv(const std::string &path) const;
        bool SaveCacheToCsv(const std::string &path) const;

        // ---- queries ----------------------------------------------------
        const PPUInstructionRow *FindByOpcode(u32 instruction) const;
        const PPUInstructionRow *FindByMnemonic(const std::string &mnemonic) const;

        const PPUDatasetRow *DatasetAt(u64 index) const;
        const std::vector<PPUDatasetRow> &Dataset() const { return m_dataset; }
        const std::vector<PPUInstructionRow> &Instructions() const { return m_instructions; }

        // ---- table growth -------------------------------------------------
        void AddInstruction(const PPUInstructionRow &row);
        void AddDataset(const PPUDatasetRow &row);
        void AddCache(const PPUCacheRow &row);

        // Seed the table with the bootstrap PowerPC rows so the emulator can
        // decode before any external dataset is loaded.
        void SeedBuiltins();

        // ---- stats --------------------------------------------------------
        size_t InstructionCount() const { return m_instructions.size(); }
        size_t DatasetCount() const { return m_dataset.size(); }
        size_t CacheCount() const { return m_cache.size(); }
        std::string Describe() const;

    private:
        bool ParseTableBlob(const std::string &json_text);
        bool ParseCsvInto(const std::string &csv, int which);

        std::vector<PPUInstructionRow> m_instructions;
        std::unordered_map<u32, size_t> m_by_opcode;   // opcode   -> row index
        std::unordered_map<std::string, size_t> m_by_name; // mnemonic -> row index

        std::vector<PPUDatasetRow> m_dataset;
        std::vector<PPUCacheRow> m_cache;
    };

    // ---------------------------------------------------------------------
    // LRU bytes cache: byteaddress -> cached cpu register window.
    //
    // Lookup walks the recency list from most-recent to least-recent and the
    // hit moves to the front. A miss decodes the 128-byte cache line that owns
    // the address (byteaddress mod kLineBytes) and installs it, evicting the
    // tail when the capacity is reached - exactly the eviction the dataset's
    // `cpu_cache` table records.
    // ---------------------------------------------------------------------
    class PPUByteCache
    {
    public:
        struct Entry
        {
            GuestAddr byteaddress = 0;
            u32 cpu = 0;                 // cpu register holding the window
            std::array<u8, PPUInstructionTables::kLineBytes> bytes{};
        };

        explicit PPUByteCache(size_t capacity = 256);

        bool Lookup(GuestAddr byteaddress, Entry &out);
        void Insert(GuestAddr byteaddress, u32 cpu, const u8 *line_bytes);

        // The deflate math: fold a byte address onto a cache line index.
        static u32 LineIndex(GuestAddr byteaddress)
        {
            return static_cast<u32>((byteaddress / PPUInstructionTables::kLineBytes) %
                                    PPUInstructionTables::kLinesPerWay);
        }
        static GuestAddr LineBase(GuestAddr byteaddress)
        {
            return byteaddress - (byteaddress % PPUInstructionTables::kLineBytes);
        }

        size_t Capacity() const { return m_capacity; }
        size_t Size() const { return m_map.size(); }
        u64 Hits() const { return m_hits; }
        u64 Misses() const { return m_misses; }
        std::string Describe() const;

        // Snapshot the LRU order as cpu_cache rows (for the MySQL table).
        std::vector<PPUCacheRow> ToCacheRows() const;

    private:
        struct Node
        {
            Entry entry;
            std::list<GuestAddr>::iterator lru;
        };

        size_t m_capacity;
        std::unordered_map<GuestAddr, Node> m_map;
        std::list<GuestAddr> m_lru; // front = most recently used
        u64 m_clock = 0;
        u64 m_hits = 0;
        u64 m_misses = 0;
    };

    // ---------------------------------------------------------------------
    // The CPU class / subclass tree.
    //
    // The dataset is hierarchical: a CPU *subclass* (a class branch) owns a
    // numbered list of *nodes*, and each node is either
    //   * a leaf  - points at the cpu_instruction table entry it decodes to;
    //   * a branch - dispatches on a bit range of the instruction word and
    //                points at a child subclass (e.g. the primary opcode 0x04
    //                branch splits PS-form and VX-form instructions).
    //
    // Walking root -> branch -> branch -> leaf is exactly what
    // `PPUInstructionRow *PPU::DecodeInstruction(u32)` does in PPU.cpp.
    // ---------------------------------------------------------------------
    class PPUClass
    {
    public:
        struct Node
        {
            u32 id = 0;
            u32 flag = 0;                 // raw flag word from the export
            std::string line;             // cache line tag
            std::string subclass;         // child subclass name (branch nodes)
            u32 instruction_opcode = 0;   // leaf payload: opcode
            std::string instruction_mnemonic;
            bool branch = false;
        };

        PPUClass() = default;
        PPUClass(std::string name, std::vector<std::string> path,
                 std::vector<Node> nodes);

        const std::string &Name() const { return m_name; }
        const std::vector<std::string> &Path() const { return m_path; }
        const std::vector<Node> &Nodes() const { return m_nodes; }

        // Human-readable dotted path, e.g. "cpu.ppu.branch.opcode_04".
        std::string QualifiedName() const;

        std::string Describe() const;

    private:
        std::string m_name;
        std::vector<std::string> m_path; // root -> ... -> this subclass
        std::vector<Node> m_nodes;
    };

    // The whole tree: named subclasses plus parent -> children links.
    class PPUClassTree
    {
    public:
        void AddClass(const PPUClass &cls);
        void Link(const std::string &parent, const std::string &child);

        const PPUClass *Find(const std::string &name) const;
        const PPUClass *Root() const;

        // Direct children of `name` (empty for a leaf / unknown name).
        std::vector<const PPUClass *> Children(const std::string &name) const;

        size_t ClassCount() const { return m_classes.size(); }
        int Depth(const std::string &name) const;
        std::string Describe() const;

    private:
        std::unordered_map<std::string, PPUClass> m_classes;
        std::unordered_map<std::string, std::vector<std::string>> m_children;
        std::string m_root;
    };

} // namespace ps3clone
