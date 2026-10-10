// RSXShaderRecompiler.cpp - RVX / RVP -> SPIR-V / GLSL translation.
#include "RSX/RSXShaderRecompiler.h"

#include "common/Logging.h"

namespace ps3clone
{

    RSXShaderRecompiler::RSXShaderRecompiler() = default;
    RSXShaderRecompiler::~RSXShaderRecompiler() = default;

    const char *RSXShaderRecompiler::StageName(Stage stage)
    {
        return (stage == Stage::Vertex) ? "vertex" : "fragment";
    }

    const char *RSXShaderRecompiler::TargetName(Target target)
    {
        return (target == Target::SPIRV) ? "SPIR-V" : "GLSL";
    }

    CellResult RSXShaderRecompiler::Compile(Stage stage, u64 ucode_hash, u32 instruction_count)
    {
        if (ucode_hash == 0)
        {
            LogWarn("RSX.Shader", "refusing to compile an empty program");
            ++m_stats.failed;
            return CellResult::CELL_EINVAL;
        }

        auto it = m_translations.find(ucode_hash);
        if (it != m_translations.end())
        {
            ++m_stats.cached;
            return CellResult::CELL_OK;
        }

        // TODO: decode the RVX/RVP ucode and emit real host code. Until then we
        // record a stand-in translation so callers get a deterministic handle.
        Translation tr;
        tr.stage = stage;
        tr.instructions = instruction_count;
        tr.source = std::string("// rsx ") + StageName(stage) + " shader -> " +
                    TargetName(m_target) + " (" + std::to_string(instruction_count) + " instrs)";
        m_translations.emplace(ucode_hash, tr);
        ++m_stats.compiled;

        LogInfo("RSX.Shader", std::string("Compiled ") + StageName(stage) + " shader (" +
                                  std::to_string(instruction_count) + " instructions) to " +
                                  TargetName(m_target));
        return CellResult::CELL_OK;
    }

    bool RSXShaderRecompiler::IsCompiled(u64 ucode_hash) const
    {
        return m_translations.find(ucode_hash) != m_translations.end();
    }

    void RSXShaderRecompiler::ClearCache()
    {
        m_translations.clear();
        m_stats = Stats{};
        LogInfo("RSX.Shader", "shader cache cleared");
    }

    std::string RSXShaderRecompiler::Describe() const
    {
        return std::string("RSXShaderRecompiler{target=") + TargetName(m_target) +
               ", cached=" + std::to_string(m_translations.size()) +
               ", compiled=" + std::to_string(m_stats.compiled) +
               ", failed=" + std::to_string(m_stats.failed) + "}";
    }

} // namespace ps3clone
