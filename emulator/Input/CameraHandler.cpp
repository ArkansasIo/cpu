// CameraHandler.cpp - implementation of the camera input handler.
#include "Input/CameraHandler.h"
#include "common/Logging.h"

namespace ps3clone
{

    CellResult CameraHandler::Initialize()
    {
        m_running = false;
        LogInfo("CameraHandler", "initialised");
        return CellResult::CELL_OK;
    }

    CellResult CameraHandler::Start(int camera_index)
    {
        (void)camera_index;
        m_running = true;
        return CellResult::CELL_OK;
    }

    CellResult CameraHandler::Stop(int camera_index)
    {
        (void)camera_index;
        m_running = false;
        return CellResult::CELL_OK;
    }

    CellResult CameraHandler::CaptureFrame(int camera_index, CameraFrame &out)
    {
        (void)camera_index;
        // TODO: pull a frame from the host webcam and convert to YUYV.
        out = m_last_frame;
        return m_running ? CellResult::CELL_OK : CellResult::CELL_EINVAL;
    }

    std::string CameraHandler::Describe() const
    {
        return std::string("cellCamera: ") + (m_running ? "streaming" : "idle");
    }

} // namespace ps3clone
