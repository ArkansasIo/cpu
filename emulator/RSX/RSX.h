// RSX.h - the PS3 GPU ("Reality Synthesizer").
//
// The RSX is a separate chip from Cell. Games submit a GCM command buffer that
// the RSX front-end decodes; the backend then renders it on the host GPU using
// Vulkan, OpenGL or DirectX 12.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    class RSXCommandBuffer;
    class RSXBackendVulkan;
    class RSXBackendGL;
    class RSXShaderRecompiler;

    class RSX
    {
    public:
        RSX();
        ~RSX();

        // Bring up the GPU: pick a backend, create resources.
        CellResult Initialize();

        // Decode and execute queued GPU commands.
        CellResult ProcessCommands();

        // Present the finished frame from `buffer_id` (a "flip").
        CellResult Present(u32 buffer_id);

        void Reset();

        RSXCommandBuffer &Commands();
        RSXShaderRecompiler &Shaders();

        std::string Describe() const;

    private:
        bool m_initialized = false;

        // Backends (only one is active at a time, chosen by Config::graphics_api).
        std::unique_ptr<RSXCommandBuffer> m_commands;
        std::unique_ptr<RSXShaderRecompiler> m_shaders;
    };

} // namespace ps3clone
