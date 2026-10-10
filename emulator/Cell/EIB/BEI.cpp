// BEI.cpp - Cell Broadband Engine Interface.
#include "Cell/EIB/BEI.h"

#include "common/Logging.h"

namespace ps3clone
{

    BEI::BEI() = default;
    BEI::~BEI() = default;

    CellResult BEI::Initialize()
    {
        LogInfo("BEI", "Initialising Cell I/O interface (2x FlexIO channels)");
        m_initialized = true;
        m_pending_interrupts = 0;
        m_coherent = false;
        return CellResult::CELL_OK;
    }

    void BEI::RaiseInterrupt(u32 irq_number)
    {
        (void)irq_number;
        ++m_pending_interrupts;
    }

    void BEI::SetCoherentMode(bool coherent) { m_coherent = coherent; }

    std::string BEI::Describe() const
    {
        return std::string("BEI (Cell BE Interface) - ") +
               (m_coherent ? "coherent" : "non-coherent") + " FlexIO mode";
    }

} // namespace ps3clone
