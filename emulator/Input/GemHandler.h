// GemHandler.h - PlayStation Move motion controller (cellGem).
//
// cellGem binds a camera + Move controller, tracks the glowing sphere in the
// camera image to get a 3D position, and exposes the controller's motion
// sensors and buttons to the guest.
#pragma once

#include "common/Types.h"
#include "Input/PadHandler.h"

#include <string>

namespace ps3clone
{

    struct GemState
    {
        bool tracked = false;
        float pos_x = 0.0f, pos_y = 0.0f, pos_z = 0.0f; // camera-space, metres
        PadState pad;                                   // buttons + motion
        u8 sphere_r = 0, sphere_g = 0, sphere_b = 0;    // sphere LED
    };

    class GemHandler
    {
    public:
        static constexpr int kMaxGems = 4;

        CellResult Initialize();
        CellResult BindCamera(int camera_index);
        void Poll();

        const GemState &GetState(int gem_index) const;

        std::string Describe() const;

    private:
        GemState m_gems[kMaxGems];
        int m_camera_index = -1;
    };

} // namespace ps3clone
