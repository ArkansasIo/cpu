// cellGcmSys.h - the graphics command library (cellGcmSys).
//
// Games build RSX command buffers with cellGcm* calls; the RSX emulation then
// decodes and executes them. Key entry points:
//   cellGcmInit            - initialise the GCM context
//   cellGcmGetConfiguration- query the framebuffer/config
//   cellGcmSetFlipCommand  - present a frame
#pragma once

#include "common/Types.h"

namespace ps3clone
{

    class RSX;

    struct GcmConfig
    {
        GuestAddr address = 0;
        u32 size = 0;
        u32 io_address = 0;
        u32 io_size = 0;
        u32 frequency = 0;
    };

    class CellGcmSysModule
    {
    public:
        explicit CellGcmSysModule(RSX &rsx);

        CellResult Init(const GcmConfig &config);
        CellResult GetConfiguration(GcmConfig &out_config) const;
        CellResult SetFlipCommand(u32 buffer_id);
        CellResult GetFlipStatus(u32 buffer_id, u32 &status) const;
        CellResult SetDisplayBuffer(u32 buffer_id);
        CellResult SetupWaitForFlip(u32 buffer_id);

    private:
        RSX &m_rsx;
        GcmConfig m_config;
        bool m_initialized = false;
    };

} // namespace ps3clone
