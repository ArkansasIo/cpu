// BEI.h - Cell Broadband Engine Interface (EIB <-> I/O bridge).
//
// Responsibilities: address translation, command processing, an internal
// interrupt controller and bus interfacing. It exposes two Rambus FlexIO
// channels: one non-coherent (I/O devices) and one configurable coherent
// channel that can extend the EIB to another Cell.
#pragma once

#include "common/Types.h"

#include <string>

namespace ps3clone
{

    enum class FlexIOChannel
    {
        NonCoherent = 0,
        Configurable = 1
    };

    class BEI
    {
    public:
        BEI();
        ~BEI();

        CellResult Initialize();

        // Submit an interrupt to the PPU (the BEI owns the interrupt controller).
        void RaiseInterrupt(u32 irq_number);

        // Configure the configurable channel for coherent vs non-coherent use.
        void SetCoherentMode(bool coherent);

        u32 PendingInterrupts() const { return m_pending_interrupts; }

        std::string Describe() const;

    private:
        bool m_initialized = false;
        bool m_coherent = false;
        u32 m_pending_interrupts = 0;
    };

} // namespace ps3clone
