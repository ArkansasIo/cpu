// config.h - emulator configuration (RPCS3-style settings).
#pragma once

#include <string>

namespace ps3clone
{

    enum class PPUDecoder
    {
        RecompilerLLVM,
        InterpreterStatic,
        InterpreterDynamic
    };
    enum class SPUDecoder
    {
        RecompilerLLVM,
        RecompilerASMJIT,
        InterpreterFast,
        InterpreterPrecise
    };
    enum class GraphicsAPI
    {
        Vulkan,
        OpenGL,
        Null
    };
    enum class ShaderMode
    {
        Async,
        AsyncWithInterpreter,
        InterpreterOnly
    };

    struct Config
    {
        // CPU
        PPUDecoder ppu_decoder = PPUDecoder::RecompilerLLVM;
        SPUDecoder spu_decoder = SPUDecoder::RecompilerLLVM;
        int spu_threads = 0; // 0 = auto
        bool spu_loop_detection = false;

        // Video
        GraphicsAPI graphics_api = GraphicsAPI::Vulkan;
        ShaderMode shader_mode = ShaderMode::Async;
        int resolution_scale = 100; // percent
        bool vsync = false;

        // Audio
        int audio_volume = 100;

        // Paths
        std::string path_dev_flash;
        std::string path_dev_hdd0;
        std::string path_rsx_cache;

        static Config &Get();
        static Config ParseCommandLine(int argc, char **argv);
    };

} // namespace ps3clone
