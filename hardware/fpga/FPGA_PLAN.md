# FPGA clone plan

A *hardware* clone reimplements the PS3 in reconfigurable logic instead of
emulating it in software. This document maps the Cell onto plausible FPGA
resources and lists what a real design would have to solve.

## Why it is hard

The Cell runs at **3.2 GHz** and its SPUs are 128-bit SIMD. Even a large modern
FPGA tops out far below that for an equivalent logic depth, so an FPGA clone
must either use many FPGAs in parallel or run the Cell at a lower clock while
keeping the rest of the system cycle-plausible (as MiSTer-style cores do for
older consoles).

## Resource sketch (single mid/large FPGA, one SPU lane)

| Block | Rough cost | Notes |
|---|---|---|
| PPU core | large | PowerPC decode + AltiVec execution |
| One SPU lane | medium | 128-bit SIMD ALU + 256 KB local store in BRAM |
| EIB | small–medium | ring arbitration, coherent DMA |
| MIC | small | XDR controller (or DDR substitute) |
| BEI | small | FlexIO / bridge to RSX block |
| RSX | very large | shader array — usually the deal-breaker |
| DSP/BRAM for local store | 7 × 256 KB | ~1.75 MB of on-chip RAM just for SPUs |

## Phased plan

1. **PPU first.** Get a PowerPC + AltiVec core booting a tiny test program and
   talking to a simulated MIC.
2. **One SPU.** Add a single SPU lane with its local store and DMA over a first
   version of the EIB. Prove SPU↔memory transfers.
3. **Scale SPUs.** Replicate the lane; decide between time-multiplexing one
   physical lane and instantiating several.
4. **RSX.** Either a real shader core (heroic) or a bridge that hands command
   buffers to a host GPU (a hybrid, more practical).
5. **I/O.** USB/Bluetooth southbridge, storage, video out.

## Software-vs-hardware trade-off

| Approach | Runs at | Accuracy | Effort |
|---|---|---|---|
| Software clone (this repo's `emulator/`) | host speed | depends on HLE | years |
| FPGA clone | lower clock, cycle-plausible | high for CPU, hard for GPU | heroic |
| Hybrid (FPGA Cell + host GPU) | mixed | good CPU, approximated GPU | pragmatic |

## What this repo provides

- `hardware/cell_chip/` — per-component die description (PPU, SPU, EIB, RSX).
- `hardware/specs/CELL_SPECS.md` — the numbers a design must hit.
- `emulator/Cell/` — the behavioural reference the RTL would be validated against.

Sources: [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)),
[PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3).
