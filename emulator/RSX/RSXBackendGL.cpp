// RSXBackendGL.cpp - implementation of the OpenGL host backend.
#include "RSX/RSXBackendGL.h"
#include "common/Logging.h"

namespace ps3clone
{

    CellResult RSXBackendGL::Initialize()
    {
        m_initialized = true;
        LogInfo("RSX/GL", "Backend initialised");
        return CellResult::CELL_OK;
    }

    void RSXBackendGL::Shutdown()
    {
        m_initialized = false;
    }

    CellResult RSXBackendGL::Execute(const GCMCommand &command)
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;

        (void)command;
        // TODO: translate into OpenGL draw calls.
        return CellResult::CELL_OK;
    }

    CellResult RSXBackendGL::Present(u32 buffer_id)
    {
        (void)buffer_id;
        return m_initialized ? CellResult::CELL_OK : CellResult::CELL_EINVAL;
    }

    std::string RSXBackendGL::Describe() const
    {
        return m_initialized ? "RSX backend: OpenGL (ready)" : "RSX backend: OpenGL (idle)";
    }

} // namespace ps3clone
