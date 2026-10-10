// XdrMemory.cpp - main RAM (XDR DRAM) and MIC model.
#include "Cell/Memory/XdrMemory.h"

#include "common/Logging.h"

#include <cstring>

namespace ps3clone
{

    XdrMemory::XdrMemory() { m_ram.resize(kMainRamSize, 0); }
    XdrMemory::~XdrMemory() = default;

    CellResult XdrMemory::Initialize()
    {
        LogInfo("MIC", "Initialising 256 MB XDR main memory (2 x XIO channels)");
        std::fill(m_ram.begin(), m_ram.end(), 0);
        m_initialized = true;
        return CellResult::CELL_OK;
    }

    bool XdrMemory::IsValid(GuestAddr address, size_t size) const
    {
        return address + size <= m_ram.size();
    }

    CellResult XdrMemory::Read(GuestAddr address, void *buffer, size_t size) const
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;
        if (!IsValid(address, size))
            return CellResult::CELL_EFAULT;
        std::memcpy(buffer, m_ram.data() + address, size);
        return CellResult::CELL_OK;
    }

    CellResult XdrMemory::Write(GuestAddr address, const void *buffer, size_t size)
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;
        if (!IsValid(address, size))
            return CellResult::CELL_EFAULT;
        std::memcpy(m_ram.data() + address, buffer, size);
        return CellResult::CELL_OK;
    }

    u8 XdrMemory::Read8(GuestAddr address) const
    {
        if (!IsValid(address, 1))
            return 0;
        return m_ram[address];
    }

    u32 XdrMemory::Read32(GuestAddr address) const
    {
        if (!IsValid(address, 4))
            return 0;
        u32 value = 0;
        std::memcpy(&value, m_ram.data() + address, sizeof(value));
        return value;
    }

    u64 XdrMemory::Read64(GuestAddr address) const
    {
        if (!IsValid(address, 8))
            return 0;
        u64 value = 0;
        std::memcpy(&value, m_ram.data() + address, sizeof(value));
        return value;
    }

    void XdrMemory::Write32(GuestAddr address, u32 value)
    {
        if (!IsValid(address, 4))
            return;
        std::memcpy(m_ram.data() + address, &value, sizeof(value));
    }

    void XdrMemory::Write64(GuestAddr address, u64 value)
    {
        if (!IsValid(address, 8))
            return;
        std::memcpy(m_ram.data() + address, &value, sizeof(value));
    }

    u64 XdrMemory::MicCyclesFor(size_t bytes) const
    {
        // MIC supports 1-8, 16, 32, 64, 128 byte accesses; approximate the cost.
        return (bytes + 127) / 128;
    }

    std::string XdrMemory::Describe() const
    {
        return "XDR main RAM - 256 MB, Memory Interface Controller (MIC)";
    }

} // namespace ps3clone
