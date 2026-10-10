// PPURecompiler.cpp - PowerPC -> host JIT (stub).
#include "Cell/PPU/PPURecompiler.h"

#include "Cell/PPU/PPU.h"
#include "common/Logging.h"

namespace ps3clone
{

    PPURecompiler::PPURecompiler(PPU &ppu) : m_ppu(ppu) {}

    PPURecompiler::~PPURecompiler() = default;

    CellResult PPURecompiler::CompileBlock(u32 address)
    {
        (void)address;
        // TODO: decode PowerPC instructions, build an LLVM IR / ASM basic block,
        //       optimise it, emit host code and store it in the translation cache.
        ++m_cached_blocks;
        return CellResult::CELL_OK;
    }

    void *PPURecompiler::LookupBlock(u32 address) const
    {
        (void)address;
        return nullptr; // TODO: read the translation cache
    }

    void PPURecompiler::Invalidate(u32 address)
    {
        (void)address; // TODO: drop the affected basic blocks
    }

    void PPURecompiler::ClearCache()
    {
        m_cached_blocks = 0;
        LogInfo("PPU-JIT", "Translation cache cleared");
    }

} // namespace ps3clone
