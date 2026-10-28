# Emulator-to-FPGA-to-Silicon Roadmap

This is the development sequence for the proposed custom processor. The existing PS3 PPU/SPU/RSX implementation remains a separate compatibility project.

## Stage 1 — executable reference model

Current initial scaffold:
- `emulator/Custom/Memory/MemoryArena.*`: sparse 12 GiB address space, lazily backed by 4 KiB host pages.
- `emulator/Custom/CPU/CPUCore.*`: small deterministic reference ISA with halt, add-immediate, load-32, and store-32 operations.
- `emulator/Custom/GPU/ComputeGPU.*`: queued fill and element-wise unsigned 32-bit addition commands.
- `tests/custom_cpu_gpu_smoke.cpp`: smoke tests for sparse memory, bounds checking, CPU arithmetic, and GPU compute.
- `tests/CMakeLists.txt`: CTest integration.

This is a functional seed for architecture validation, not a full operating CPU or graphics-capable GPU. The cache budget is documented but not yet implemented as a cache simulator.

### Build and test

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

## Stage 1 next milestones

1. Make the ISA specification stable and versioned; define endianness, alignment, traps, and memory ordering.
2. Add instruction-fetch and a small test-program loader.
3. Add CPU exception handling, a minimal MMU, and multicore scheduling.
4. Add GPU queue fences, bounded command buffers, vectorized kernels, and per-command timing.
5. Implement configurable cache simulation (line size, associativity, replacement policy, counters) without allocating the entire 6 GiB budget.
6. Add property tests, malformed-command tests, sanitizer builds, and CI.
7. Define a stable hardware-neutral interface shared by the emulator and RTL testbench.

## Stage 2 — FPGA

- Select a target board and determine available LUTs, BRAM/URAM, DSP blocks, external memory, and PCIe/SoC interfaces.
- Translate the stable CPU ISA and GPU command semantics into synthesizable SystemVerilog or VHDL.
- Start with a single simple CPU core and a small integer/compute GPU.
- Use external DDR/HBM as the main memory; implement only small physically practical on-chip L1/L2 caches.
- Use Verilator or an equivalent RTL simulator and compare traces against the C++ reference model.
- Add synthesis, timing closure, board bring-up, and throughput tests.

## Stage 3 — physical design feasibility

Before a custom ASIC decision, quantify:
- target workloads and required CPU/GPU throughput;
- process node, die area, package, power, and cooling budgets;
- memory technology, bandwidth, capacity, ECC, and controller design;
- cache size, line size, associativity, coherence protocol, and interconnect topology;
- verification strategy, manufacturing test, yield, and cost.

The proposed 12 GB RAM plus 6 GB cache-pool budget is a software-level capacity target. It is not a validated physical memory configuration. In hardware, main memory would ordinarily be external DRAM/HBM and conventional L1/L2 caches would be small on-die SRAM structures.
