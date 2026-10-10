// CameraHandler.h - PlayStation Eye / USB camera input.
//
// The camera delivers VGA (or QVGA) frames plus audio to the guest. Games read
// frames via cellCamera (the "camera" library) and, on the Move, cellGem.
#pragma once

#include "common/Types.h"

#include <string>
#include <vector>

namespace ps3clone
{

    enum class CameraResolution
    {
        QVGA, // 320x240
        VGA,  // 640x480
    };

    struct CameraFrame
    {
        CameraResolution resolution = CameraResolution::VGA;
        u32 width = 640;
        u32 height = 480;
        std::vector<u8> yuyv; // packed YUYV, 2 bytes/pixel
    };

    class CameraHandler
    {
    public:
        CellResult Initialize();
        CellResult Start(int camera_index);
        CellResult Stop(int camera_index);

        // Grab the most recent frame from the host webcam (or a file).
        CellResult CaptureFrame(int camera_index, CameraFrame &out);

        std::string Describe() const;

    private:
        bool m_running = false;
        CameraFrame m_last_frame;
    };

} // namespace ps3clone
