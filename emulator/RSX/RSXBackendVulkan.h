// RSXBackendVulkan.h - host rendering backend using Vulkan.
//
// The preferred backend: maps the RSX's GCM command stream (draws, blits,
// clears) onto Vulkan command buffers, and RSX shaders onto host SPIR-V.
#pragma once

#include "common/Types.h"
#include "RSX/RSXCommandBuffer.h"

#include <string>
#include <vector>

namespace ps3clone
{

    class RSXBackendVulkan
    {
    public:
        CellResult Initialize();
        void Shutdown();

        // Execute one decoded command on the host GPU.
        CellResult Execute(const GCMCommand &command);

        // Present the finished frame (swapchain).
        CellResult Present(u32 buffer_id);

        bool IsInitialized() const { return m_initialized; }
        std::string Describe() const;

    private:
        bool m_initialized = false;
        u32 m_swapchain_images = 3;
    };

} // namespace ps3clone
