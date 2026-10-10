// config.cpp - configuration defaults and a tiny command-line parser.
#include "common/Config.h"

#include <string>

namespace ps3clone
{

    Config &Config::Get()
    {
        static Config instance;
        return instance;
    }

    Config Config::ParseCommandLine(int argc, char **argv)
    {
        Config cfg;
        for (int i = 1; i < argc; ++i)
        {
            const std::string arg = argv[i];

            if (arg == "--spu-llvm")
            {
                cfg.spu_decoder = SPUDecoder::RecompilerLLVM;
            }
            else if (arg == "--spu-asmjit")
            {
                cfg.spu_decoder = SPUDecoder::RecompilerASMJIT;
            }
            else if (arg == "--spu-interpreter")
            {
                cfg.spu_decoder = SPUDecoder::InterpreterFast;
            }
            else if (arg == "--ppu-interpreter")
            {
                cfg.ppu_decoder = PPUDecoder::InterpreterStatic;
            }
            else if (arg == "--vulkan")
            {
                cfg.graphics_api = GraphicsAPI::Vulkan;
            }
            else if (arg == "--opengl")
            {
                cfg.graphics_api = GraphicsAPI::OpenGL;
            }
            else if (arg == "--dev-flash" && i + 1 < argc)
            {
                cfg.path_dev_flash = argv[++i];
            }
            else if (arg == "--dev-hdd0" && i + 1 < argc)
            {
                cfg.path_dev_hdd0 = argv[++i];
            }
        }
        return cfg;
    }

} // namespace ps3clone
