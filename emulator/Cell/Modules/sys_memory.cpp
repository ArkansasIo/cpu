// sys_memory.cpp - guest memory allocation.
#include "Cell/Modules/sys_memory.h"

#include "Cell/Memory/XdrMemory.h"
#include "common/Logging.h"

namespace ps3clone
{

    SysMemoryModule::SysMemoryModule(XdrMemory &memory) : m_memory(memory) {}

    CellResult SysMemoryModule::Allocate(u32 size, u32 alignment, GuestAddr &address)
    {
        if (alignment == 0)
        {
            alignment = 4096;
        }
        // Align the next allocation pointer up.
        m_next_allocation = (m_next_allocation + alignment - 1) & ~(static_cast<GuestAddr>(alignment) - 1);

        if (m_next_allocation + size > m_memory.Size())
        {
            LogError("sys_memory", "Out of guest memory");
            return CellResult::CELL_ENOMEM;
        }

        address = m_next_allocation;
        m_next_allocation += size;
        LogInfo("sys_memory", "sys_memory_allocate(" + std::to_string(size) + " -> 0x" +
                                  std::to_string(address) + ")");
        return CellResult::CELL_OK;
    }

    CellResult SysMemoryModule::Free(GuestAddr address)
    {
        (void)address; // TODO: track the allocation map and actually reclaim.
        return CellResult::CELL_OK;
    }

    CellResult SysMemoryModule::GetPageSize(u32 &page_size) const
    {
        page_size = 4096;
        return CellResult::CELL_OK;
    }

    CellResult SysMemoryModule::GetUserMemorySize(u64 &size) const
    {
        size = m_memory.Size();
        return CellResult::CELL_OK;
    }

} // namespace ps3clone
