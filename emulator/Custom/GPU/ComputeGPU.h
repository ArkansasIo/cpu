#pragma once

#include "common/Types.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ps3clone::custom
{
    class MemoryArena;

    enum class GPUOperation : u8 { FillU32, AddU32 };

    struct GPUCommand
    {
        GPUOperation operation = GPUOperation::FillU32;
        std::uint64_t destination = 0;
        std::uint64_t sourceA = 0;
        std::uint64_t sourceB = 0;
        std::uint32_t value = 0;
        std::size_t elementCount = 0;
    };

    // Deterministic host-side reference model; parallel backends can replace
    // Execute while preserving command validation and memory semantics.
    class ComputeGPU
    {
    public:
        explicit ComputeGPU(MemoryArena& memory) : m_memory(memory) {}
        CellResult Submit(const GPUCommand& command);
        CellResult ExecuteAll();
        void ClearQueue() { m_queue.clear(); }
        std::size_t PendingCommands() const noexcept { return m_queue.size(); }

    private:
        CellResult Execute(const GPUCommand& command);
        MemoryArena& m_memory;
        std::vector<GPUCommand> m_queue;
    };
}
