// cellGcmSys.cpp - graphics command library.
#include "Cell/Modules/cellGcmSys.h"

#include "RSX/RSX.h"
#include "common/Logging.h"

namespace ps3clone
{

    CellGcmSysModule::CellGcmSysModule(RSX &rsx) : m_rsx(rsx) {}

    CellResult CellGcmSysModule::Init(const GcmConfig &config)
    {
        m_config = config;
        m_initialized = true;
        LogInfo("cellGcmSys", "cellGcmInit - RSX command buffer ready");
        m_rsx.Initialize();
        return CellResult::CELL_OK;
    }

    CellResult CellGcmSysModule::GetConfiguration(GcmConfig &out_config) const
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;
        out_config = m_config;
        return CellResult::CELL_OK;
    }

    CellResult CellGcmSysModule::SetFlipCommand(u32 buffer_id)
    {
        // A "flip" presents the finished frame: hand it to the RSX backend.
        return m_rsx.Present(buffer_id);
    }

    CellResult CellGcmSysModule::GetFlipStatus(u32 buffer_id, u32 &status) const
    {
        (void)buffer_id;
        status = 1; // 1 = flip complete
        return CellResult::CELL_OK;
    }

    CellResult CellGcmSysModule::SetDisplayBuffer(u32 buffer_id)
    {
        (void)buffer_id;
        return CellResult::CELL_OK;
    }

    CellResult CellGcmSysModule::SetupWaitForFlip(u32 buffer_id)
    {
        (void)buffer_id;
        return CellResult::CELL_OK;
    }

} // namespace ps3clone
