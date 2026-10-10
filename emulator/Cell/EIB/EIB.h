// EIB.h - Element Interconnect Bus.
//
// The EIB is the coherent on-chip bus of the Cell. Structure:
//   * 4 rings for data: two clockwise, two counter-clockwise
//   * a tree structure for commands
//   * internal bandwidth: 96 bytes / cycle
//   * supports >100 outstanding DMA memory requests
//
// It connects the PPU, all SPUs, the MIC (memory) and the BEI (I/O).
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>

namespace ps3clone
{

    enum class EIBNode
    {
        PPU,
        SPU0,
        SPU1,
        SPU2,
        SPU3,
        SPU4,
        SPU5,
        SPU6,
        MIC, // memory interface controller
        BEI, // broadband engine interface (I/O)
    };

    class EIB
    {
    public:
        static constexpr size_t kBandwidthBytesPerCycle = 96;
        static constexpr int kRingCount = 4;

        EIB();
        ~EIB();

        CellResult Initialize();

        // Transfer `bytes` between two nodes. Returns the number of EIB cycles the
        // transfer occupies (bandwidth model, no real data movement in skeleton).
        u64 Transfer(EIBNode from, EIBNode to, size_t bytes);

        // Total cycles consumed since reset, i.e. an estimate of bus pressure.
        u64 TotalCycles() const { return m_total_cycles; }

        std::string Describe() const;

    private:
        bool m_initialized = false;
        u64 m_total_cycles = 0;
    };

} // namespace ps3clone
