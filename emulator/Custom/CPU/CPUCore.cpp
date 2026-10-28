#include "Custom/CPU/CPUCore.h"
#include "Custom/Memory/MemoryArena.h"

#include <array>
#include <cstring>

namespace ps3clone::custom
{
    void CPUCore::Reset() noexcept { m_registers.fill(0); m_pc = 0; m_halted = false; }
    u64 CPUCore::Register(std::size_t index) const noexcept
    {
        return index < m_registers.size() ? m_registers[index] : 0;
    }

    CellResult CPUCore::Step(u32 instruction)
    {
        if (m_halted) return CellResult::CELL_EINVAL;
        const auto opcode = static_cast<Opcode>((instruction >> 24) & 0xff);
        const auto rd = static_cast<std::size_t>((instruction >> 16) & 0xff);
        const auto ra = static_cast<std::size_t>((instruction >> 8) & 0xff);
        const auto imm = static_cast<std::int8_t>(instruction & 0xff);
        if (rd >= m_registers.size() || ra >= m_registers.size()) return CellResult::CELL_EINVAL;

        switch (opcode)
        {
        case Opcode::Halt:
            m_halted = true;
            break;
        case Opcode::AddImmediate:
            if (rd != 0) m_registers[rd] = m_registers[ra] + static_cast<std::int64_t>(imm);
            break;
        case Opcode::Load32:
        {
            const auto address = m_registers[ra] + static_cast<std::int64_t>(imm);
            std::array<u8, 4> bytes{};
            const auto result = m_memory.Read(address, bytes.data(), bytes.size());
            if (result != CellResult::CELL_OK) return result;
            u32 value = 0;
            std::memcpy(&value, bytes.data(), sizeof(value));
            if (rd != 0) m_registers[rd] = value;
            break;
        }
        case Opcode::Store32:
        {
            const auto address = m_registers[ra] + static_cast<std::int64_t>(imm);
            const u32 value = static_cast<u32>(m_registers[rd]);
            return m_memory.Write(address, &value, sizeof(value));
        }
        default:
            return CellResult::CELL_ENOSYS;
        }
        ++m_pc;
        m_registers[0] = 0;
        return CellResult::CELL_OK;
    }
}
