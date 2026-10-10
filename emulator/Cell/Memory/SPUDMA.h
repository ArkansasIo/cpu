adn // SPUDMA.h - asynchronous DMA between SPU local store and main memory.
//
// DMA rules (from the Cell programming reference):
//   * transfers must be a multiple of 16 bytes and 16-byte aligned, up to 16 KB
//   * Cell also supports 1/2/4/8-byte naturally aligned transfers
//   * transfers of <= 16 bytes are atomic
//   * a word is 4 bytes; a quadword is 16 bytes
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <deque>

namespace ps3clone {

struct DMADescriptor {
    u32 local_store_address = 0;
    GuestAddr main_memory_address = 0;
    size_t size = 0;
    u32 tag = 0;
    bool direction_local_to_main = false; // false => main -> local
};

class SPUDMA {
public:
    static constexpr size_t kMaxTransferSize = 16 * 1024; // 16 KB
    static constexpr size_t kAlignment = 16;

    // Queue a DMA transfer.
    CellResult Queue(const DMADescriptor& desc);

    // Complete the next pending transfer. Returns NOT_IMPLEMENTED when empty.
    CellResult ServiceOne();

    size_t PendingCount() const { return m_queue.size(); }
    void Reset() { m_queue.clear(); }

private:
    bool IsValid(const DMADescriptor& desc) const;
    std::deque<DMADescriptor> m_queue;
};

} // namespace ps3clone
