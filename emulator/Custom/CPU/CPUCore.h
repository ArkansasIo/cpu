#pragma once

#include "common/Types.h"

#include <array>
#include <cstdint>

namespace ps3clone::custom
{
    class MemoryArena;

    // Small deterministic reference ISA for validating the emulator/RTL contract.
    // Encoding: [31:24] opcode, [23:16] rd, [15:8] ra, [7:0] signed immediate.
    enum class Opcode : u8 { Halt = 0, AddImmediate = 1, Load32 = 2, Store32 = 3 };

    class CPUCore
    {
    public:
        explicit CPUCore(MemoryArena& memory) : m_memory(memory) {}
        CellResult Step(u32 instruction);
        void Reset() noexcept;
        u64 Register(std::size_t index) const noexcept;
        u64 ProgramCounter() const noexcept { return m_pc; }
        bool Halted() const noexcept { return m_halted; }

    private:
        MemoryArena& m_memory;
        std::array<u64, 32> m_registers{};
        u64 m_pc = 0;
        bool m_halted = false;
    };
}
