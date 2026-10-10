// RSXBackendGL.h - host rendering backend using OpenGL.
//
// The fallback backend, used when Vulkan is unavailable. It maps the GCM
// command stream onto OpenGL draw calls and RSX shaders onto GLSL.
#pragma once

#include "common/Types.h"
#include "RSX/RSXCommandBuffer.h"

#include <string>

namespace ps3clone
{

    class RSXBackendGL
    {
    public:
        CellResult Initialize();
        void Shutdown();

        CellResult Execute(const GCMCommand &command);
        CellResult Present(u32 buffer_id);

        bool IsInitialized() const { return m_initialized; }
        std::string Describe() const;

    private:
        bool m_initialized = false;
    };

} // namespace ps3clone
