// USBHandler.cpp - implementation of the USB / Bluetooth transport.
#include "Input/USBHandler.h"
#include "common/Logging.h"

#include <sstream>

namespace ps3clone
{

    CellResult USBHandler::Initialize()
    {
        m_devices.clear();
        m_initialized = true;
        LogInfo("USBHandler", "initialised");
        return CellResult::CELL_OK;
    }

    void USBHandler::Shutdown()
    {
        m_devices.clear();
        m_initialized = false;
    }

    CellResult USBHandler::Enumerate(std::vector<USBDevice> &out)
    {
        out = m_devices;
        return CellResult::CELL_OK;
    }

    CellResult USBHandler::SendHidReport(u32 device_handle, const std::vector<u8> &report)
    {
        (void)device_handle;
        (void)report;
        return m_initialized ? CellResult::CELL_OK : CellResult::CELL_EINVAL;
    }

    std::string USBHandler::Describe() const
    {
        std::ostringstream os;
        os << "USB/BT transport: " << m_devices.size() << " device(s)";
        return os.str();
    }

} // namespace ps3clone
