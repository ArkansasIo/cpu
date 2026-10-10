// GunConHandler.h - light guns and racing wheels.
//
// Older PS3 titles support the GunCon 3 (an IR-pointing gun) and USB racing
// wheels (Logitech Driving Force, G25/G27, ...). Both surface through the same
// cellPad / Linux-raw-HID path, so they share a handler here.
#pragma once

#include "common/Types.h"
#include "Input/PadHandler.h"

#include <string>

namespace ps3clone
{

    enum class GunConMode
    {
        GunCon3,
        Wheel,
    };

    struct GunConState
    {
        PadState pad; // buttons + axes reuse the pad layout
        // Absolute aim point, normalised 0..1 over the screen.
        float aim_x = 0.5f;
        float aim_y = 0.5f;
    };

    class GunConHandler
    {
    public:
        static constexpr int kMaxDevices = 2;

        CellResult Initialize();
        CellResult Attach(int index, GunConMode mode);
        void Poll();

        const GunConState &GetState(int index) const;
        std::string Describe() const;

    private:
        GunConState m_devices[kMaxDevices];
        GunConMode m_modes[kMaxDevices] = {GunConMode::GunCon3, GunConMode::GunCon3};
        bool m_attached[kMaxDevices] = {false, false};
    };

} // namespace ps3clone
