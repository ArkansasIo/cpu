// SPUInterpreter.cpp - SPU instruction interpreter (stub).
#include "Cell/SPU/SPUInterpreter.h"

#include "Cell/SPU/SPU.h"
#include "common/Logging.h"

namespace ps3clone
{

    SPUInterpreter::SPUInterpreter(SPU &spu) : m_spu(spu) {}

    CellResult SPUInterpreter::ExecuteInstruction(u32 instruction)
    {
        (void)instruction;
        // TODO: implement the full SPU ISA. This is the accurate (slow) path; when
        // selected it is used to debug cases the LLVM/ASMJIT recompiler miscompiles.
        LogWarn("SPU-Interp", "Unimplemented SPU instruction");
        return CellResult::NOT_IMPLEMENTED;
    }

} // namespace ps3clone
