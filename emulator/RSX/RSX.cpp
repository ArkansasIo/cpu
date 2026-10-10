// RSX.cpp - Reality Synthesizer GPU front-end.
#include "RSX/RSX.h"

#include "RSX/RSXCommandBuffer.h"
#include "RSX/RSXShaderRecompiler.h"
#include "common/Config.h"
#include "common/Logging.h"

#include <memory>

namespace ps3clone
{

    RSX::RSX() = default;
    RSX::~RSX() = default;

    CellResult RSX::Initialize()
    {
        const Config &cfg = Config::Get();
        const char *api = (cfg.graphics_api == GraphicsAPI::Vulkan)   ? "Vulkan"
                          : (cfg.graphics_api == GraphicsAPI::OpenGL) ? "OpenGL"
                                                                      : "Null";
        LogInfo("RSX", std::string("Initialising RSX (Reality Synthesizer) on ") + api);

        m_commands = std::make_unique<RSXCommandBuffer>();
        m_shaders = std::make_unique<RSXShaderRecompiler>();
        m_initialized = true;

        // TODO: create the selected host backend:
        //   GraphicsAPI::Vulkan -> RSXBackendVulkan
        //   GraphicsAPI::OpenGL -> RSXBackendGL
        return CellResult::CELL_OK;
    }

    CellResult RSX::ProcessCommands()
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;
        return m_commands->ExecuteAll();
    }

    CellResult RSX::Present(u32 buffer_id)
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;
        // TODO: swap the host backbuffer.
        (void)buffer_id;
        return CellResult::CELL_OK;
    }

    void RSX::Reset()
    {
        if (m_commands)
            m_commands->Reset();
        m_initialized = false;
    }

    RSXCommandBuffer &RSX::Commands() { return *m_commands; }
    RSXShaderRecompiler &RSX::Shaders() { return *m_shaders; }

    std::string RSX::Describe() const
    {
        return "RSX (Reality Synthesizer) - the PS3 GPU, separate chip";
    }

} // namespace ps3clone
