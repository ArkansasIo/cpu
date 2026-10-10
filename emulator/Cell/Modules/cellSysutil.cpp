// cellSysutil.cpp - system utility library.
#include "Cell/Modules/cellSysutil.h"

#include "common/Logging.h"

namespace ps3clone
{

    CellResult CellSysutilModule::Init()
    {
        m_initialized = true;
        LogInfo("cellSysutil", "cellSysutil initialised");
        return CellResult::CELL_OK;
    }

    CellResult CellSysutilModule::RegisterCallback(GuestAddr callback, GuestAddr userdata)
    {
        m_callback = callback;
        m_userdata = userdata;
        return CellResult::CELL_OK;
    }

    CellResult CellSysutilModule::UnregisterCallback(GuestAddr callback)
    {
        (void)callback;
        m_callback = 0;
        m_userdata = 0;
        return CellResult::CELL_OK;
    }

    CellResult CellSysutilModule::CheckCallback()
    {
        // TODO: dispatch pending system events to the registered PPU callback.
        return CellResult::CELL_OK;
    }

    CellResult CellSysutilModule::MessageDialogOpen(const std::string &text)
    {
        LogInfo("cellSysutil", "Message dialog: " + text);
        return CellResult::CELL_OK;
    }

    CellResult CellSysutilModule::SaveDataListLoad()
    {
        // TODO: enumerate save data under dev_hdd0/home/.../savedata.
        return CellResult::CELL_OK;
    }

    CellResult CellSysutilModule::GetSystemParamInt(u32 param_id, s32 &value) const
    {
        (void)param_id;
        value = 0;
        return CellResult::CELL_OK;
    }

} // namespace ps3clone
