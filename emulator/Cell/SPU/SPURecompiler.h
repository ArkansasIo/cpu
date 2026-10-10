// SPURecompiler.h - SPU -> host JIT (the hot path of the whole emulator).
//
// Two backends exist in practice:
//   * LLVM   - fastest, recompiles the SPU LLVM cache before running (default)
//   * ASMJIT - fast, very compatible; use when LLVM crashes on a game
// SPU XFloat accuracy (accurate / approximate / relaxed) only applies to the
// dynamic/LLVM SPU decoders and trades float precision for speed.
#pragma once

#include "common/Types.h"

#include <string>

namespace ps3clone
{

    class SPU;

    enum class SPUBackend
    {
        LLVM,
        ASMJIT
    };

    class SPURecompiler
    {
    public:
        SPURecompiler(SPU &spu, SPUBackend backend);
        ~SPURecompiler();

        // Compile the SPU block at `local_store_pc` into host code.
        CellResult CompileBlock(u32 local_store_pc);

        // Run a previously compiled block.
        CellResult ExecuteBlock(u32 local_store_pc);

        void ClearCache();

        size_t CachedBlockCount() const { return m_cached_blocks; }
        std::string BackendName() const;

    private:
        SPU &m_spu;
        SPUBackend m_backend;
        size_t m_cached_blocks = 0;
    };

} // namespace ps3clone
