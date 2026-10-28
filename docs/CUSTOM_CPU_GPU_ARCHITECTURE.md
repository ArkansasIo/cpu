# Custom CPU + GPU Architecture Proposal

## Scope

This document defines an experimental general-purpose CPU/GPU platform alongside the existing PlayStation 3 emulator model. The existing `emulator/Cell/PPU`, `emulator/Cell/SPU`, and `emulator/RSX` remain PS3-specific; the proposed platform is not a claim that the PS3 used these capacities.

## Top-level design

- **CPU:** multicore general-purpose processor for operating-system work, control flow, game logic, and latency-sensitive tasks.
- **GPU:** massively parallel processor for raster graphics, shader execution, image processing, and general-purpose compute.
- **Memory controller:** shared address-space support, memory protection, access arbitration, and CPU/GPU transfer scheduling.
- **Cache subsystem:** configurable per-core and shared cache pools, with explicit metadata and coherence policies.
- **Interconnect:** command queues, DMA engines, interrupts, fences, and memory barriers connecting CPU, GPU, and memory.

## Capacity targets

| Resource | Proposed capacity | Interpretation |
|---|---:|---|
| Main RAM | 12 GB | System-visible physical memory target |
| Total cache budget | 6 GB | Experimental software-managed cache/pool budget, not conventional on-die SRAM |
| Total planned capacity | 18 GB | RAM plus cache budget; excludes firmware, metadata, and device-reserved memory |

A 6 GB cache is not a conventional L1/L2 cache design. Large caches of this size should be modeled as a hierarchy of host-memory-backed software pools or external high-bandwidth memory. Actual L1/L2 caches in a physical CPU are typically far smaller because of silicon area, power, and latency constraints.

## Experimental cache-pool partition

These are initial configurable budgets, not validated silicon specifications:

| Pool | Budget | Intended role |
|---|---:|---|
| L1-like CPU/GPU local caches | 64 MB | Small, low-latency working sets; implementation should subdivide per core/unit |
| L2-like private/cluster caches | 512 MB | Per-core or per-compute-cluster working sets |
| Shared L3-like pool | 1.5 GB | Shared CPU/GPU data and reuse across compute units |
| L4 / large software-managed cache pool | 3.93 GB | Streaming assets, staging buffers, and reusable datasets |
| **Total** | **6 GB** | Budget to validate with profiling |

The L1/L2 labels describe logical levels in the proposed model. They do not imply that these capacities can be implemented as ordinary on-die SRAM.

## CPU subsystem

Recommended first implementation:
1. Define a CPU core interface with architectural state, register file, program counter, exception state, and instruction-step method.
2. Keep instruction decoding and execution semantics independent from storage or database layers.
3. Add multicore scheduling only after a single core has deterministic execution and tests.
4. Add a memory-management unit abstraction for address translation and access permissions.
5. Add performance counters for instruction count, cache hits/misses, stalls, and memory latency.

## GPU subsystem

Recommended first implementation:
1. **Command processor:** validates and dispatches command buffers submitted by the CPU.
2. **Compute units:** groups of SIMD/SIMT execution lanes for shader and compute kernels.
3. **Graphics pipeline:** vertex processing, primitive assembly, rasterization, fragment/pixel shading, depth/stencil, and render-target output.
4. **Resource system:** buffers, textures, samplers, descriptors, and framebuffers.
5. **Memory interface:** GPU-visible virtual addresses, transfer queues, and bounds-checked memory access.
6. **Backend interface:** keep the emulated GPU API separate from host rendering backends such as Vulkan or OpenGL.
7. **Synchronization:** fences, semaphores, barriers, and explicit CPU/GPU ownership transitions.

The existing RSX subsystem is the PS3-specific GPU emulator. Avoid silently treating the custom GPU as a replacement for RSX; keep the two models separate until an explicit shared abstraction is designed.

## CPU–GPU memory model

- Start with a single authoritative memory manager and separate CPU/GPU access APIs.
- Make all reads and writes bounds-checked.
- Track dirty ranges and ownership transitions for non-coherent resources.
- Provide explicit CPU-to-GPU and GPU-to-CPU synchronization.
- Do not allocate the full 18 GB budget automatically on startup. Reserve virtual address ranges or allocate backing pages on demand.
- Treat cache budgets as configurable limits; collect hit/miss and eviction metrics before tuning policies.

## Suggested implementation sequence

1. Add configuration types for memory capacities and cache policy.
2. Implement a sparse, bounds-checked memory arena and unit tests.
3. Implement GPU command queues and a deterministic null backend.
4. Add a simple compute kernel path before attempting full 3D rendering.
5. Add CPU/GPU synchronization and DMA tests.
6. Add a host graphics backend only after command semantics and resource lifetimes are stable.
7. Profile memory pressure, cache hit rates, transfer costs, and frame time.

## Acceptance criteria

- The default demo does not reserve multiple gigabytes unnecessarily.
- Invalid addresses and oversized allocations fail safely.
- CPU and GPU command processing can be tested without a physical GPU.
- Memory ownership transitions are deterministic and testable.
- The documentation clearly distinguishes a software-emulated capacity model from a physically feasible silicon design.
