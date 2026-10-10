// PPURecompiler.h - the fast JIT that translates PowerPC into host code.
//
// This is the default PPU decoder in every serious PS3 clone: the game's
// executable is recompiled into host machine code (x86-64 / AArch64) once, then
// executed natively. "LLVM Recompiler" is by far the fastest option.
#pragma once

#include "common/Types.h"

#include <string>

namespace ps3clone
{

    class PPU;

    class PPURecompiler
    {
    public:
        explicit PPURecompiler(PPU &ppu);
        ~PPURecompiler();

        // Recompile the basic block starting at `address`. Returns CELL_OK on
        // success or an error if the block could not be translated.
        CellResult CompileBlock(u32 address);

        // Look up a previously compiled block; nullptr if not compiled yet.
        void *LookupBlock(u32 address) const;

        // Invalidate cached translations for `address` (self-modifying code).
        void Invalidate(u32 address);

        // Flush the whole translation cache.
        void ClearCache();

        size_t CachedBlockCount() const { return m_cached_blocks; }

    private:
        PPU &m_ppu;
        size_t m_cached_blocks = 0;
        bool m_using_llvm = true; // false => fall back to interpreter
    };

} // namespace ps3clone
