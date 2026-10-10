// PPUInterpreter.h - the accurate, slow PowerPC interpreter.
//
// The interpreter is the reference implementation: when a game misbehaves with
// the recompiler, switching PPU decoder to "Interpreter (static)" is the first
// troubleshooting step (see docs/BUILD_AND_RUN.md).
#pragma once

#include "common/Types.h"

namespace ps3clone
{

    class PPU;

    class PPUInterpreter
    {
    public:
        explicit PPUInterpreter(PPU &ppu);

        // Decode and execute a single PowerPC instruction at the current PC.
        // Returns CELL_OK, or an error if the PC is unmapped / illegal.
        CellResult ExecuteInstruction(int thread, u32 instruction);

    private:
        PPU &m_ppu;
    };

} // namespace ps3clone
