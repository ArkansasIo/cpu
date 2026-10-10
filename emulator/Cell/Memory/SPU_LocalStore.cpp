es // SPU_LocalStore.cpp - per-SPU 256 KB scratchpad.
#include "Cell/Memory/SPU_LocalStore.h"

#include <cstring>

namespace ps3clone {

CellResult SPU_LocalStore::Read(u32 address, void* buffer, size_t size) const {
    if (!IsValid(address, size)) return CellResult::CELL_EFAULT;
    std::memcpy(buffer, m_store.data() + address, size);
    return CellResult::CELL_OK;
}

CellResult SPU_LocalStore::Write(u32 address, const void* buffer, size_t size) {
    if (!IsValid(address, size)) return CellResult::CELL_EFAULT;
    std::memcpy(m_store.data() + address, buffer, size);
    return CellResult::CELL_OK;
}

u32 SPU_LocalStore::Read32(u32 address) const {
    if (!IsValid(address, 4)) return 0;
    u32 value = 0;
    std::memcpy(&value, m_store.data() + address, sizeof(value));
    return value;
}

void SPU_LocalStore::Write32(u32 address, u32 value) {
    if (!IsValid(address, 4)) return;
    std::memcpy(m_store.data() + address, &value, sizeof(value));
}

void SPU_LocalStore::Clear() { m_store.fill(0); }

} // namespace ps3clone
