// SPUDMA.cpp - SPU <-> main memory DMA.
#include "Cell/Memory/SPUDMA.h"

#include "Cell/Memory/XdrMemory.h"
#include "common/Logging.h"

namespace ps3clone
{

    bool SPUDMA::IsValid(const DMADescriptor &desc) const
    {
        if (desc.size == 0 || desc.size > kMaxTransferSize)
        {
            return false;
        }
        // Small transfers (<= 16 B) may be any naturally aligned size; larger ones
        // must be 16-byte aligned.
        if (desc.size > kAlignment)
        {
            if ((desc.local_store_address % kAlignment) != 0)
                return false;
            if ((desc.main_memory_address % kAlignment) != 0)
                return false;
            if ((desc.size % kAlignment) != 0)
                return false;
        }
        return true;
    }

    CellResult SPUDMA::Queue(const DMADescriptor &desc)
    {
        if (!IsValid(desc))
        {
            LogWarn("SPU-DMA", "Rejected illegal DMA descriptor");
            return CellResult::CELL_EINVAL;
        }
        m_queue.push_back(desc);
        return CellResult::CELL_OK;
    }

    CellResult SPUDMA::ServiceOne()
    {
        if (m_queue.empty())
        {
            return CellResult::CELL_OK;
        }
        const DMADescriptor desc = m_queue.front();
        m_queue.pop_front();

        // TODO: copy between XdrMemory and the SPU's local store, honouring the
        // direction, then notify the SPU DMA channel (mfc) when complete.
        (void)desc;
        return CellResult::CELL_OK;
    }

} // namespace ps3clone
