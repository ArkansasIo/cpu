// SPURecompiler.cpp - SPU -> host JIT (stub).
#include "Cell/SPU/SPURecompiler.h"

#include "Cell/SPU/SPU.h"
#include "common/Logging.h"

namespace ps3clone
{

    SPURecompiler::SPURecompiler(SPU &spu, SPUBackend backend)
        : m_spu(spu), m_backend(backend) {}

    SPURecompiler::~SPURecompiler() = default;

    std::string SPURecompiler::BackendName() const
    {
        return m_backend == SPUBackend::LLVM ? "LLVM" : "ASMJIT";
    }

    CellResult SPURecompiler::CompileBlock(u32 local_store_pc)
    {
        (void)local_store_pc;
        // TODO: translate SPU instructions to host code via LLVM/ASMJIT.
        ++m_cached_blocks;
        return CellResult::CELL_OK;
    }

    CellResult SPURecompiler::ExecuteBlock(u32 local_store_pc)
    {
        (void)local_store_pc;
        return CellResult::CELL_OK;
    }

    void SPURecompiler::ClearCache()
    {
        m_cached_blocks = 0;
        LogInfo("SPU-JIT", BackendName() + " cache cleared");
    }

} // namespace ps3clone
