// PadHandler.h - DualShock / SIXAXIS controller input (cellPad).
//
// Maps host keyboard/gamepad input onto the PS3 pad state: two analog sticks,
// pressure-sensitive buttons, and the motion sensors (accelerometer/gyro).
#pragma once

#include "common/Types.h"

#include <array>
#include <string>

namespace ps3clone
{

    // Digital buttons of a standard pad (bitmask).
    enum class PadButton : u32
    {
        None = 0,
        Select = 1u << 0,
        L3 = 1u << 1,
        R3 = 1u << 2,
        Start = 1u << 3,
        Up = 1u << 4,
        Right = 1u << 5,
        Down = 1u << 6,
        Left = 1u << 7,
        L2 = 1u << 8,
        R2 = 1u << 9,
        L1 = 1u << 10,
        R1 = 1u << 11,
        Triangle = 1u << 12,
        Circle = 1u << 13,
        Cross = 1u << 14,
        Square = 1u << 15,
        PS = 1u << 16,
    };

    struct PadState
    {
        u32 buttons = 0;       // PadButton bitmask
        u8 lx = 128, ly = 128; // left stick, centre 128
        u8 rx = 128, ry = 128; // right stick
        u8 l2_pressure = 0;
        u8 r2_pressure = 0;
        // Motion sensors in the PS3's fixed-point range.
        s16 accel_x = 0, accel_y = 0, accel_z = 0;
        s16 gyro_x = 0, gyro_y = 0, gyro_z = 0;
    };

    class PadHandler
    {
    public:
        static constexpr int kMaxPads = 7;

        CellResult Initialize();
        void Poll();

        const PadState &GetState(int pad) const;
        bool IsPressed(int pad, PadButton button) const;

        std::string Describe() const;

    private:
        std::array<PadState, kMaxPads> m_pads{};
        std::array<bool, kMaxPads> m_connected{};
    };

} // namespace ps3clone
