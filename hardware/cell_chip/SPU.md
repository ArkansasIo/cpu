# Cell chip — SPU (Synergistic Processor Element)

The SPUs are the Cell's eight SIMD co-processors. Sony ships the chip with one
disabled, so a PS3 has **7 usable SPUs**: **6 for games**, **1 reserved for the
OS**. They are the reason the PS3 was hard to develop for *and* the reason it
stayed fast — and they are the single biggest target an emulator must optimise.

Software counterpart: `emulator/Cell/SPU/` and `emulator/Cell/Memory/`.

## Block diagram

```
        +-------------------------------------------+
        |                  SPU                      |
        |                                           |
        |   +-----------------------------------+   |
        |   |  Local Store (256 KB SRAM)        |   |
        |   |  software-managed, NOT a cache    |   |
        |   +----------------+------------------+   |
        |                    |                      |
        |   +----------------v------------------+   |
        |   |  128 x 128-bit unified registers  |   |
        |   +----------------+------------------+   |
        |                    |                      |
        |   +----------------v------------------+   |
        |   |  SIMD pipeline (integer + float)  |   |
        |   +----------------+------------------+   |
        +--------------------|----------------------+
                             |
                    DMA <----+----> EIB (to main memory / other SPUs)
```

## Specification

| Property | Value |
|---|---|
| Width | 128-bit SIMD RISC |
| Local store | 256 KB per SPU (software-managed scratchpad, not a cache) |
| Register file | 128 × 128-bit |
| Data movement | asynchronous DMA to/from main memory |
| Threads | 1 per active SPU |
| OS-capable | No — single-issue, no MMU, not intended to run an OS |
| Count | 8 physical, 7 usable, 6 for games |

## Role

Data-rich, compute-heavy, embarrassingly parallel work:

- physics, collision, animation, procedural geometry
- audio synthesis and mixing (cellAudio)
- decompression (textures, streams)
- some graphics compute (vertex processing, post-effects)

## Emulation notes

- Every active SPU is recompiled (LLVM / ASMJIT) and runs on **its own host
  thread**. This is where a PS3 emulator spends most of its CPU time.
- 6 active SPUs means an emulator needs ~6 free host cores before it even
  starts running the PPU and RSX work.

Sources: [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)),
[RPCS3 Cell CPU SPU breakthrough — Tom's Hardware](https://www.tomshardware.com/video-games/playstation/rpcs3-ps3-emulator-gets-cell-cpu-breakthrough-that-improves-performance-in-all-games).
