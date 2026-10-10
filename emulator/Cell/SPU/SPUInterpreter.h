// SPUInterpreter.h - accurate SPU interpreter.
#pragma once

#include "common/Types.h"

namespace ps3clone
{

    class SPU;

    class SPUInterpreter
    {
    public:
        explicit SPUInterpreter(SPU &spu);

        // Execute one 32-bit SPU instruction. SPU instruction groups to implement:
        //   * load/store to local store
        //   * integer arithmetic & logical
        //   * floating-point (single precision, with XFloat accuracy modes)
        //   * shuffle/permute (byte rotation, gather bits)
        //   * channels (SPU <-> SPU / SPU <-> PPU mailboxes, DMA commands)
        CellResult ExecuteInstruction(u32 instruction);

    private:
        SPU &m_spu;
    };

} // namespace ps3clone
