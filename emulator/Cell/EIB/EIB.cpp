// EIB.cpp - Element Interconnect Bus model.
#include "Cell/EIB/EIB.h"

#include "common/Logging.h"

namespace ps3clone
{

    EIB::EIB() = default;
    EIB::~EIB() = default;

    CellResult EIB::Initialize()
    {
        LogInfo("EIB", "Initialising 4-ring Element Interconnect Bus (96 bytes/cycle)");
        m_initialized = true;
        m_total_cycles = 0;
        return CellResult::CELL_OK;
    }

    u64 EIB::Transfer(EIBNode from, EIBNode to, size_t bytes)
    {
        (void)from;
        (void)to;
        if (!m_initialized)
        {
            return 0;
        }
        // Cost = ceil(bytes / bandwidth). Real emulators also model ring
        // arbitration, latency and outstanding-request limits.
        const u64 cycles = (bytes + kBandwidthBytesPerCycle - 1) / kBandwidthBytesPerCycle;
        m_total_cycles += cycles;
        return cycles;
    }

    std::string EIB::Describe() const
    {
        return "EIB (Element Interconnect Bus) - 4 rings, 96 B/cycle, coherent";
    }

} // namespace ps3clone
