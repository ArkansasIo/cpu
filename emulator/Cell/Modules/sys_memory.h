// sys_memory.h - guest memory allocation library.
#pragma once

#include "common/Types.h"

#include <cstddef>

namespace ps3clone
{

    class XdrMemory;

    class SysMemoryModule
    {
    public:
        explicit SysMemoryModule(XdrMemory &memory);

        // Allocate `size` bytes with the given page alignment; returns the guest
        // address in `address`.
        CellResult Allocate(u32 size, u32 alignment, GuestAddr &address);

        CellResult Free(GuestAddr address);
        CellResult GetPageSize(u32 &page_size) const;
        CellResult GetUserMemorySize(u64 &size) const;

    private:
        XdrMemory &m_memory;
        GuestAddr m_next_allocation = 0x00100000; // start above the low guest area
    };

} // namespace ps3clone
