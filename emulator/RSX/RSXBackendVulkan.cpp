// RSXBackendVulkan.cpp - implementation of the Vulkan host backend.
#include "RSX/RSXBackendVulkan.h"
#include "common/Logging.h"

namespace ps3clone
{

    CellResult RSXBackendVulkan::Initialize()
    {
        m_initialized = true;
        LogInfo("RSX/Vulkan", "Backend initialised");
        return CellResult::CELL_OK;
    }

    void RSXBackendVulkan::Shutdown()
    {
        m_initialized = false;
    }

    CellResult RSXBackendVulkan::Execute(const GCMCommand &command)
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;

        (void)command;
        // TODO: translate into Vulkan draw calls / pipeline binds.
        return CellResult::CELL_OK;
    }

    CellResult RSXBackendVulkan::Present(u32 buffer_id)
    {
        (void)buffer_id;
        return m_initialized ? CellResult::CELL_OK : CellResult::CELL_EINVAL;
    }

    std::string RSXBackendVulkan::Describe() const
    {
        return m_initialized ? "RSX backend: Vulkan (ready)" : "RSX backend: Vulkan (idle)";
    }

} // namespace ps3clone
