# Cell Broadband Engine — full specification table

Consolidated specs for the Cell chip, its memory, its interconnect and the RSX.
Sources at the bottom.

## Chip identity

| Item | Value |
|---|---|
| Name | Cell Broadband Engine (Cell/B.E.) |
| Developed by | Sony, Toshiba, IBM ("STI" alliance) |
| Process | 90 nm (original, 2005) → 65 nm → 45 nm (Slim/SuperSlim) |
| Transistors | ~234 million |
| Die area | ~221 mm² (90 nm) |
| Cells per chip | 1 × PPU + 8 × SPU (1 SPU disabled) |

## PPU (main CPU)

| Property | Value |
|---|---|
| ISA | PowerPC 2.02 + AltiVec/VMX SIMD |
| Width | 64-bit RISC, dual-issue, in-order |
| Pipeline | 23 stages |
| Threads | 2 (SMT) |
| L1 cache | 32 KB I + 32 KB D |
| L2 cache | 512 KB |
| Cache line | 128 bytes |

## SPU (co-processors)

| Property | Value |
|---|---|
| Count | 8 physical / 7 usable / 6 for games |
| Width | 128-bit SIMD RISC |
| Local store | 256 KB (scratchpad, not a cache) |
| Register file | 128 × 128-bit |
| Data movement | asynchronous DMA |
| Clock (PPU/SPU) | up to 3.2 GHz |

## EIB (interconnect)

| Property | Value |
|---|---|
| Structure | 4 data rings (2 CW + 2 CCW) + command tree |
| Internal bandwidth | 96 bytes / cycle |
| Outstanding DMA | >100 requests |

## Memory

| Property | Value |
|---|---|
| Main RAM | 256 MB XDR DRAM (4 × 64 MB) |
| Memory controller | MIC, 2 × XDR (XIO) channels |
| Access sizes | 1–8, 16, 32, 64, 128 bytes |
| Peak main bandwidth | ~25.6 GB/s (XDR) |

## RSX (GPU)

| Property | Value |
|---|---|
| Type | Separate die, unified shader architecture |
| Video RAM | 256 MB GDDR3 |
| Bus | FlexIO (coherent) to the BEI |
| Shader throughput | ~1.8 TFLOPS-class marketing figure; real: fill/vertex rates |
| Host mapping | Vulkan / OpenGL / DirectX 12 |

## Reference clocks

| Domain | Value |
|---|---|
| Cell clock | 3.2 GHz |
| RSX core | 500 MHz |
| RSX memory | 650 MHz (GDDR3) |

Sources: [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)),
[IBM CBE SDK — Architecture overview](https://arcb.csc.ncsu.edu/~mueller/cluster/ps3/SDK3.0/docs/accessibility/sdkpt/cbet_1arch_ovw.html),
[PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3).
