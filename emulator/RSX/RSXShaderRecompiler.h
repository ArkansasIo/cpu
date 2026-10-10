// RSXShaderRecompiler.h - translate RSX shaders into host shaders.
//
// The RSX uses two programmable units:
//   * Vertex shader   - RSX vertex program (RVX), a 512-instruction limit
//   * Fragment shader - RSX fragment program (RVP)
//
// Both are recompiled ahead of time (async) into SPIR-V for Vulkan or GLSL for
// OpenGL. In async mode a placeholder shader runs until the real one is ready,
// which is why the first frames of a draw can look wrong for a moment.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <unordered_map>

namespace ps3clone
{

    class RSXShaderRecompiler
    {
    public:
        // Which programmable stage a program belongs to.
        enum class Stage
        {
            Vertex,
            Fragment
        };

        // The host shader language we emit.
        enum class Target
        {
            SPIRV, // Vulkan
            GLSL   // OpenGL
        };

        struct Stats
        {
            size_t compiled = 0;
            size_t cached = 0;
            size_t failed = 0;
        };

        RSXShaderRecompiler();
        ~RSXShaderRecompiler();

        // Recompile a guest program (identified by its ucode hash) into host
        // code. Returns CELL_OK on success.
        CellResult Compile(Stage stage, u64 ucode_hash, u32 instruction_count);

        // True if `ucode_hash` already has a compiled translation.
        bool IsCompiled(u64 ucode_hash) const;

        // Drop every cached translation (e.g. on shader cache clear).
        void ClearCache();

        void SetTarget(Target target) { m_target = target; }
        Target GetTarget() const { return m_target; }

        const Stats &GetStats() const { return m_stats; }
        size_t CachedCount() const { return m_translations.size(); }

        static const char *StageName(Stage stage);
        static const char *TargetName(Target target);

        std::string Describe() const;

    private:
        struct Translation
        {
            Stage stage = Stage::Vertex;
            u32 instructions = 0;
            std::string source; // SPIR-V blob description / GLSL text
        };

        Target m_target = Target::SPIRV;
        std::unordered_map<u64, Translation> m_translations;
        Stats m_stats;
    };

} // namespace ps3clone
