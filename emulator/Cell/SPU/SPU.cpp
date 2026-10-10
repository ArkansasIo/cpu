// SPU.cpp - Synergistic Processor Element (co-processor).
#include "Cell/SPU/SPU.h"

#include "common/Config.h"
#include "common/Logging.h"

#include <cstring>

namespace ps3clone
{

    SPU::SPU(int index) : m_index(index) { Reset(); }
    SPU::~SPU() = default;

    void SPU::Reset()
    {
        m_regs = SPURegisters{};
        m_local_store.fill(0);
        m_booted = false;
        m_stopped = true;
    }

    CellResult SPU::Boot()
    {
        if (m_index < 0 || m_index >= kPhysicalSPUCount)
        {
            return CellResult::CELL_EINVAL;
        }
        if (!IsUsable())
        {
            LogInfo("SPU", "SPU " + std::to_string(m_index) + " is disabled on this chip");
            return CellResult::CELL_OK;
        }
        LogInfo("SPU", "Boot SPU " + std::to_string(m_index) + " (256KB local store)" +
                           (IsReservedForOS() ? " [reserved for OS]" : " [available to games]"));
        m_booted = true;
        return CellResult::CELL_OK;
    }

    CellResult SPU::Step()
    {
        if (!m_booted)
        {
            return CellResult::CELL_EINVAL;
        }
        // TODO: decode one 32-bit SPU instruction (all instructions are 32-bit and
        // fixed-length), execute it, advancing m_regs.pc. Real clones implement the
        // full SPU ISA here and a JIT in SPURecompiler.cpp.
        m_regs.pc += 4;
        return CellResult::CELL_OK;
    }

    CellResult SPU::Run(u64 max_instructions)
    {
        if (m_stopped)
        {
            return CellResult::CELL_OK;
        }
        u64 executed = 0;
        while (max_instructions == 0 || executed < max_instructions)
        {
            CellResult rc = Step();
            if (rc != CellResult::CELL_OK)
            {
                return rc;
            }
            ++executed;
        }
        return CellResult::CELL_OK;
    }

    CellResult SPU::LoadImage(const void *data, size_t size, u32 local_store_address)
    {
        if (!data || size == 0)
        {
            return CellResult::CELL_EINVAL;
        }
        if (static_cast<size_t>(local_store_address) + size > kLocalStoreSize)
        {
            return CellResult::CELL_ENOMEM;
        }
        std::memcpy(m_local_store.data() + local_store_address, data, size);
        return CellResult::CELL_OK;
    }

    std::string SPU::Describe() const
    {
        return "SPU #" + std::to_string(m_index) + " (128-bit SIMD, 256KB local store)" +
               (IsUsable() ? "" : " [DISABLED]");
    }

} // namespace ps3clone
