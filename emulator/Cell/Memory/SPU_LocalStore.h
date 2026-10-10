typ// SPU_LocalStore.h - the per-SPU 256 KB scratchpad.
//
// Unlike a cache, the SPU local store is fully software-managed: the SPU cannot
// address main memory directly. Data moves in and out only via DMA.
#pragma once

#include "common/Types.h"

#include <array>
#include <cstddef>

namespace ps3clone {

class SPU_LocalStore {
public:
    static constexpr u32 kSize = 256 * 1024; // 256 KB

    CellResult Read(u32 address, void* buffer, size_t size) const;
    CellResult Write(u32 address, const void* buffer, size_t size);

    u32 Read32(u32 address) const;
    void Write32(u32 address, u32 value);

    void Clear();

    u8* Data() { return m_store.data(); }
    const u8* Data() const { return m_store.data(); }

private:
    bool IsValid(u32 address, size_t size) const {
        return static_cast<size_t>(address) + size <= kSize;
    }

    std::array<u8, kSize> m_store{};
};

} // namespace ps3clone
