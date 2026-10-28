#include "Custom/Memory/MemoryArena.h"

#include <algorithm>
#include <cstring>

namespace ps3clone::custom
{
    MemoryArena::MemoryArena(std::uint64_t capacityBytes) : m_capacity(capacityBytes) {}

    bool MemoryArena::ValidRange(std::uint64_t address, std::size_t bytes) const noexcept
    {
        return address <= m_capacity && static_cast<std::uint64_t>(bytes) <= m_capacity - address;
    }

    CellResult MemoryArena::Read(std::uint64_t address, void* destination, std::size_t bytes) const
    {
        if (bytes != 0 && destination == nullptr) return CellResult::CELL_EINVAL;
        if (!ValidRange(address, bytes)) return CellResult::CELL_EFAULT;
        auto* out = static_cast<u8*>(destination);
        while (bytes != 0)
        {
            const auto pageIndex = address / kPageSize;
            const auto offset = static_cast<std::size_t>(address % kPageSize);
            const auto count = std::min(bytes, kPageSize - offset);
            const auto it = m_pages.find(pageIndex);
            if (it == m_pages.end()) std::memset(out, 0, count);
            else std::memcpy(out, it->second->data() + offset, count);
            address += count; out += count; bytes -= count;
        }
        return CellResult::CELL_OK;
    }

    CellResult MemoryArena::Write(std::uint64_t address, const void* source, std::size_t bytes)
    {
        if (bytes != 0 && source == nullptr) return CellResult::CELL_EINVAL;
        if (!ValidRange(address, bytes)) return CellResult::CELL_EFAULT;
        auto* in = static_cast<const u8*>(source);
        while (bytes != 0)
        {
            const auto pageIndex = address / kPageSize;
            const auto offset = static_cast<std::size_t>(address % kPageSize);
            const auto count = std::min(bytes, kPageSize - offset);
            auto& page = m_pages[pageIndex];
            if (!page)
            {
                page = std::make_unique<std::array<u8, kPageSize>>();
                page->fill(0);
            }
            std::memcpy(page->data() + offset, in, count);
            address += count; in += count; bytes -= count;
        }
        return CellResult::CELL_OK;
    }

    void MemoryArena::Clear() { m_pages.clear(); }
}
