#pragma once

#include "common/Types.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace ps3clone::custom
{
    // Sparse guest-physical memory. The configured capacity is an address-space
    // limit; backing pages are allocated only when written.
    class MemoryArena
    {
    public:
        static constexpr std::size_t kPageSize = 4096;

        explicit MemoryArena(std::uint64_t capacityBytes = 12ull * 1024 * 1024 * 1024);
        CellResult Read(std::uint64_t address, void* destination, std::size_t bytes) const;
        CellResult Write(std::uint64_t address, const void* source, std::size_t bytes);
        std::uint64_t Capacity() const noexcept { return m_capacity; }
        std::size_t ResidentPages() const noexcept { return m_pages.size(); }
        void Clear();

    private:
        bool ValidRange(std::uint64_t address, std::size_t bytes) const noexcept;
        std::uint64_t m_capacity;
        std::unordered_map<std::uint64_t, std::unique_ptr<std::array<u8, kPageSize>>> m_pages;
    };
}
