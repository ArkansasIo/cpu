// XdrMemory.h - main RAM and the Memory Interface Controller (MIC).
//
// The PS3 has 256 MB of XDR DRAM (four 64 MB chips). The MIC provides the
// interface between the EIB and main storage and supports two Rambus XDR (XIO)
// channels with accesses of 1-8, 16, 32, 64 or 128 bytes.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    class XdrMemory
    {
    public:
        static constexpr size_t kMainRamSize = 256ull * 1024 * 1024; // 256 MB

        XdrMemory();
        ~XdrMemory();

        CellResult Initialize();

        // Guest memory accessors. Addresses are PowerPC effective addresses (the
        // guest range starts at 0x0000_0000_0001_0000 on a real system).
        CellResult Read(GuestAddr address, void *buffer, size_t size) const;
        CellResult Write(GuestAddr address, const void *buffer, size_t size);

        u8 Read8(GuestAddr address) const;
        u32 Read32(GuestAddr address) const;
        u64 Read64(GuestAddr address) const;
        void Write32(GuestAddr address, u32 value);
        void Write64(GuestAddr address, u64 value);

        // MIC transfer cost in EIB cycles for a given access width.
        u64 MicCyclesFor(size_t bytes) const;

        size_t Size() const { return m_ram.size(); }

        std::string Describe() const;

    private:
        bool IsValid(GuestAddr address, size_t size) const;

        std::vector<u8> m_ram;
        bool m_initialized = false;
    };

} // namespace ps3clone
