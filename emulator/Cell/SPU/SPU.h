// SPU.h - the PS3 co-processors: Synergistic Processor Elements.
//
// The Cell contains 8 SPUs; Sony ships the PS3 with 7 usable:
//   * 6 available to games
//   * 1 reserved for the operating system
//
// Each SPU is a 128-bit SIMD RISC core with:
//   * 256 KB software-managed local store (NOT a cache)
//   * a 128-entry x 128-bit unified register file
//   * asynchronous DMA to move data between local store and main memory
//
// SPU emulation is the largest CPU bottleneck in a PS3 clone: every active SPU
// is recompiled and executed on its own host CPU thread.
#pragma once

#include "common/Types.h"

#include <array>
#include <string>

namespace ps3clone
{

    // On the real chip one SPU is deliberately disabled; 7 remain usable.
    constexpr int kPhysicalSPUCount = 8;
    constexpr int kUsableSPUCount = 7;
    constexpr int kGameSPUCount = 6; // 7th is reserved for the OS

    // Placeholder so the header stays readable; see Types.h.
    // (Real code uses a proper 128-bit vector type / __m128i.)
    // Declared BEFORE SPURegisters: a type may not be used above its declaration.
    using u128_placeholder = std::array<u64, 2>;

    struct SPURegisters
    {
        std::array<u128_placeholder, 128> gpr{}; // r0..r127, 128-bit each
        u32 pc = 0;
        u32 lr = 0;
        u32 sp = 0; // preferred slot pointer
    };

    class SPU
    {
    public:
        static constexpr u32 kLocalStoreSize = 256 * 1024; // 256 KB

        SPU(int index);
        ~SPU();

        int Index() const { return m_index; }
        bool IsUsable() const { return m_index < kUsableSPUCount; }
        bool IsReservedForOS() const { return m_index == kGameSPUCount; }

        // Initialise the SPU: clear local store, reset registers, attach DMA.
        CellResult Boot();

        // Execute one SPU instruction.
        CellResult Step();

        // Run the SPU program.
        CellResult Run(u64 max_instructions = 0);

        void Reset();

        // Load an SPU program image (SPU_ELF / ".spu") into the local store.
        CellResult LoadImage(const void *data, size_t size, u32 local_store_address);

        std::string Describe() const;

    private:
        int m_index;
        SPURegisters m_regs;
        std::array<u8, kLocalStoreSize> m_local_store{};
        bool m_booted = false;
        bool m_stopped = true;
    };

} // namespace ps3clone
