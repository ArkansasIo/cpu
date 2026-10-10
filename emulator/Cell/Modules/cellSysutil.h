// cellSysutil.h - the system utility library (cellSysutil).
//
// Handles system dialogs, message dialogs, save-data UI and the callback
// dispatcher that must be pumped every frame.
#pragma once

#include "common/Types.h"

#include <string>

namespace ps3clone
{

    class CellSysutilModule
    {
    public:
        CellResult Init();
        CellResult RegisterCallback(GuestAddr callback, GuestAddr userdata);
        CellResult UnregisterCallback(GuestAddr callback);
        CellResult CheckCallback();
        CellResult MessageDialogOpen(const std::string &text);
        CellResult SaveDataListLoad();
        CellResult GetSystemParamInt(u32 param_id, s32 &value) const;

    private:
        GuestAddr m_callback = 0;
        GuestAddr m_userdata = 0;
        bool m_initialized = false;
    };

} // namespace ps3clone
