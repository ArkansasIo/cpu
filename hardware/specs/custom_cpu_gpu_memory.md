# Experimental CPU/GPU Memory Budget

Status: proposal only; not a silicon-validated specification.

## Capacity

- Main RAM target: 12 GB.
- Cache/pool target: 6 GB.
- Combined modeled capacity: 18 GB, excluding device reservations and metadata.

## Important distinction

The 6 GB cache target is a software-managed hierarchy/pool budget. It must not be described as 6 GB of conventional L1/L2 SRAM. A physical implementation would need a separate feasibility study for silicon area, power, packaging, bandwidth, and latency. A software emulator can model the hierarchy using host-memory-backed pools.

## Initial pool budgets

| Logical level | Capacity | Notes |
|---|---:|---|
| L1-like | 64 MB | Divide into small per-core/per-unit caches; tune later |
| L2-like | 512 MB | Private or cluster-local pools |
| L3-like | 1.5 GB | Shared CPU/GPU pool |
| L4/software-managed | 3.93 GB | Staging, streaming, and reusable datasets |
| Total | 6 GB | Sum of proposed budgets |

## GPU memory behavior

- GPU command buffers and resource descriptors must be validated before execution.
- Textures, vertex/index buffers, compute buffers, and render targets require explicit size and alignment rules.
- Large allocations should be lazy or sparse rather than eagerly committed.
- CPU/GPU writes require defined visibility and synchronization rules.
- Cache invalidation and writeback must be observable in debug builds.
- Out-of-range DMA and GPU accesses must return an error rather than corrupt unrelated memory.

## Future physical-design questions

Before considering FPGA or custom silicon, define the target process, die-area envelope, power envelope, memory technology, external memory bandwidth, GPU throughput target, and expected workloads. The current capacity figures alone are insufficient to establish feasibility.
