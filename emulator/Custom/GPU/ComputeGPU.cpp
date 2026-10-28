#include "Custom/GPU/ComputeGPU.h"
#include "Custom/Memory/MemoryArena.h"

#include <limits>

namespace ps3clone::custom
{
    CellResult ComputeGPU::Submit(const GPUCommand& command)
    {
        if (command.elementCount > (std::numeric_limits<std::size_t>::max() / sizeof(u32)))
            return CellResult::CELL_EINVAL;
        m_queue.push_back(command);
        return CellResult::CELL_OK;
    }

    CellResult ComputeGPU::ExecuteAll()
    {
        for (const auto& command : m_queue)
        {
            const auto result = Execute(command);
            if (result != CellResult::CELL_OK) return result;
        }
        m_queue.clear();
        return CellResult::CELL_OK;
    }

    CellResult ComputeGPU::Execute(const GPUCommand& command)
    {
        if (command.elementCount > std::numeric_limits<std::uint64_t>::max() / sizeof(u32))
            return CellResult::CELL_EINVAL;
        const auto bytes = command.elementCount * sizeof(u32);
        if (command.operation == GPUOperation::FillU32)
        {
            for (std::size_t i = 0; i < command.elementCount; ++i)
            {
                const u32 value = command.value;
                const auto result = m_memory.Write(command.destination + i * sizeof(u32), &value, sizeof(value));
                if (result != CellResult::CELL_OK) return result;
            }
            return CellResult::CELL_OK;
        }
        if (command.operation == GPUOperation::AddU32)
        {
            if (command.sourceA > std::numeric_limits<std::uint64_t>::max() - bytes ||
                command.sourceB > std::numeric_limits<std::uint64_t>::max() - bytes ||
                command.destination > std::numeric_limits<std::uint64_t>::max() - bytes)
                return CellResult::CELL_EFAULT;
            for (std::size_t i = 0; i < command.elementCount; ++i)
            {
                u32 a = 0, b = 0;
                auto result = m_memory.Read(command.sourceA + i * sizeof(u32), &a, sizeof(a));
                if (result != CellResult::CELL_OK) return result;
                result = m_memory.Read(command.sourceB + i * sizeof(u32), &b, sizeof(b));
                if (result != CellResult::CELL_OK) return result;
                const u32 sum = a + b;
                result = m_memory.Write(command.destination + i * sizeof(u32), &sum, sizeof(sum));
                if (result != CellResult::CELL_OK) return result;
            }
            return CellResult::CELL_OK;
        }
        return CellResult::CELL_ENOSYS;
    }
}
