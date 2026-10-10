// PPUInterpreter.cpp - PowerPC instruction interpreter (stub).
#include "Cell/PPU/PPUInterpreter.h"

#include "Cell/PPU/PPU.h"
#include "common/Logging.h"

namespace ps3clone
{

    PPUInterpreter::PPUInterpreter(PPU &ppu) : m_ppu(ppu) {}

    CellResult PPUInterpreter::ExecuteInstruction(int thread, u32 instruction)
    {
        (void)thread;
        const u32 opcode = instruction >> 26;

        // TODO: implement the full PowerPC 2.02 instruction set, grouped by opcode.
        // Some groups a real implementation must cover:
        //   0x0E  addi / addis / addic
        //   0x1F  extended: add, subf, mullw, divw, shifts, compares
        //   0x20  lwz
        //   0x24  stw
        //   0x28  ld
        //   0x2C  std
        //   0x38  branch (b, bl, bc, bclr, bcctr)
        //   0x3F  extended: extsb/extsh, mfspr, mtspr, dcbf, dcbst, icbi
        switch (opcode)
        {
        default:
            LogWarn("PPU-Interp", "Unimplemented opcode 0x" + std::to_string(opcode));
            return CellResult::NOT_IMPLEMENTED;
        }
    }

} // namespace ps3clone
