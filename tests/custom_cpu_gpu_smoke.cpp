#include "Custom/CPU/CPUCore.h"
#include "Custom/GPU/ComputeGPU.h"
#include "Custom/Memory/MemoryArena.h"

#include <array>
#include <cassert>
#include <cstdint>

using namespace ps3clone;
using namespace ps3clone::custom;

int main()
{
    MemoryArena memory(12ull * 1024 * 1024 * 1024);
    assert(memory.Capacity() == 12ull * 1024 * 1024 * 1024);
    assert(memory.ResidentPages() == 0);

    const std::uint32_t marker = 0x1234abcd;
    assert(memory.Write(4094, &marker, sizeof(marker)) == CellResult::CELL_OK);
    std::uint32_t roundTrip = 0;
    assert(memory.Read(4094, &roundTrip, sizeof(roundTrip)) == CellResult::CELL_OK);
    assert(roundTrip == marker);
    assert(memory.ResidentPages() == 2);
    assert(memory.Read(memory.Capacity(), &roundTrip, sizeof(roundTrip)) == CellResult::CELL_EFAULT);

    CPUCore cpu(memory);
    const auto addi = (static_cast<u32>(Opcode::AddImmediate) << 24) | (1u << 16) | 7u;
    assert(cpu.Step(addi) == CellResult::CELL_OK);
    assert(cpu.Register(1) == 7);

    ComputeGPU gpu(memory);
    GPUCommand fill;
    fill.operation = GPUOperation::FillU32;
    fill.destination = 0x10000;
    fill.value = 10;
    fill.elementCount = 4;
    assert(gpu.Submit(fill) == CellResult::CELL_OK);
    assert(gpu.ExecuteAll() == CellResult::CELL_OK);

    std::array<u32, 4> input{1, 2, 3, 4};
    assert(memory.Write(0x11000, input.data(), sizeof(input)) == CellResult::CELL_OK);
    assert(memory.Write(0x12000, input.data(), sizeof(input)) == CellResult::CELL_OK);
    GPUCommand add;
    add.operation = GPUOperation::AddU32;
    add.sourceA = 0x11000;
    add.sourceB = 0x12000;
    add.destination = 0x13000;
    add.elementCount = input.size();
    assert(gpu.Submit(add) == CellResult::CELL_OK);
    assert(gpu.ExecuteAll() == CellResult::CELL_OK);
    std::array<u32, 4> output{};
    assert(memory.Read(0x13000, output.data(), sizeof(output)) == CellResult::CELL_OK);
    assert((output == std::array<u32, 4>{2, 4, 6, 8}));
    return 0;
}
