// USBHandler.h - USB HID + Bluetooth transport for wireless peripherals.
//
// The PS3 exposes USB host ports (used by pads while charging, wheels, guns,
// keyboards) and an internal Bluetooth 2.0+EDR radio for wireless DualShock 3 /
// SIXAXIS controllers. This module moves HID reports between host and guest.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    struct USBDevice
    {
        u16 vendor_id = 0;
        u16 product_id = 0;
        std::string name;
        bool is_bluetooth = false;
    };

    class USBHandler
    {
    public:
        CellResult Initialize();
        void Shutdown();

        // Enumerate devices attached to the host that we expose to the guest.
        CellResult Enumerate(std::vector<USBDevice> &out);

        // Deliver a raw HID input report to the guest's usb stack.
        CellResult SendHidReport(u32 device_handle, const std::vector<u8> &report);

        std::string Describe() const;

    private:
        std::vector<USBDevice> m_devices;
        bool m_initialized = false;
    };

} // namespace ps3clone
