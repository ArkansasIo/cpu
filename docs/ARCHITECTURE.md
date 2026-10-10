# PS3 / Cell Broadband Engine — Architecture Overview

The PlayStation 3 is built around the **Cell Broadband Engine (Cell/B.E.)**, a 64-bit RISC
multi-core processor developed by Sony, Toshiba and IBM (the "STI" alliance), plus a
separate GPU called the **RSX** ("Reality Synthesizer").

The Cell is *nine* processors on one chip, all connected through a high-bandwidth,
memory-coherent bus called the **EIB** (Element Interconnect Bus).

Sources:
[Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)),
[IBM CBE SDK — Architecture overview](https://arcb.csc.ncsu.edu/~mueller/cluster/ps3/SDK3.0/docs/accessibility/sdkpt/cbet_1arch_ovw.html),
[PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3).

---

## 1. The Cell chip

```
                        +---------------- Cell Broadband Engine ----------------+
                        |                                                       |
   +---------+          |   +-------+   +-------+   +-------+   +-------+       |
   |  PPU    |<-------->|   | SPE 0 |   | SPE 1 |   | SPE 2 |   | SPE 3 |       |
   | (main   |          |   | 256KB |   | 256KB |   | 256KB |   | 256KB |       |
   |  CPU)   |          |   +---+---+   +---+---+   +---+---+   +---+---+       |
   | 2 hw    |          |       |           |           |           |           |
   | threads |          |   +---v-----------v-----------v-----------v---+       |
   +----+----+          |   |            E I B  (4 rings)             |       |
        |               |   +---^-----------^-----------^-----------^---+       |
        |               |       |           |           |           |           |
        |               |   +---+---+   +---+---+   +---+---+   +---+---+       |
        |               |   | SPE 4 |   | SPE 5 |   | SPE 6 |   | SPE 7*|       |
        |               |   | 256KB |   | 256KB |   | 256KB |   | disab.|       |
        |               |   +-------+   +-------+   +-------+   +-------+       |
        |               |                                                       |
        |               |   +----------------+     +--------------------+       |
        |               |   | MIC (memory    |     | BEI (I/O interface)|       |
        |               |   | interface ctl) |     | -> RSX, southbridge|       |
        |               |   +-------+--------+     +---------+----------+       |
        +---------------+-----------+------------------------+-----------------+
                                    |                        |
                              XDR DRAM               RSX GPU / I/O
```

`*` Sony ships Cell with 8 SPUs physically, but disables one (often a defective one,
and always one even when the die is perfect) so every PS3 has exactly **7 usable SPUs**;
**6 are available to games**, the **7th is reserved for the OS**.

### 1.1 PPU — Power Processing Element (the "CPU")

The main general-purpose core. It runs the OS, manages resources and dispatches work
to the SPUs. It runs conventional PowerPC software.

| Property | Value |
|---|---|
| ISA | PowerPC 2.02 + AltiVec/VMX SIMD |
| Width | 64-bit RISC, dual-issue, in-order |
| Threads | 2 (SMT), 23-stage pipeline |
| L1 cache | 32 KB instruction + 32 KB data |
| L2 cache | 512 KB |
| Cache line | 128 bytes |
| Units | FXU (32-bit fixed), FPU (64-bit), AltiVec/VMX, LSU, BRU/BXU |

### 1.2 SPU — Synergistic Processor Elements (the co-processors)

Eight identical SIMD co-processors for data-rich, compute-heavy work: physics,
animation, audio, decompression, some graphics compute.

| Property | Value |
|---|---|
| Width | 128-bit SIMD RISC |
| Local store | 256 KB per SPU (software-managed, not a cache) |
| Register file | 128-bit × 128 entries |
| Data movement | asynchronous DMA to/from main memory |
| Threads | 1 per active SPU |
| OS-capable? | No — not intended to run an OS |

### 1.3 EIB — Element Interconnect Bus

- 4-ring structure (two clockwise, two counter-clockwise) for data; tree for commands.
- Internal bandwidth **96 bytes / cycle**; supports >100 outstanding DMA requests.
- Coherently links PPU, all SPUs, memory and I/O.

### 1.4 MIC — Memory Interface Controller

Bridges EIB to main storage. Two Rambus **XDR** memory channels (the PS3 uses four
64 MB XDR DRAM chips = 256 MB main RAM).

### 1.5 BEI — Cell Broadband Engine Interface

Bridges EIB to I/O devices: address translation, command processing, internal interrupt
controller, two Rambus **FlexIO** channels (one non-coherent, one configurable coherent
so a second Cell can be attached).

---

## 2. RSX — the GPU

The RSX is **not** part of Cell; it is a separate chip. It is a fixed-function-ish
shader-based GPU fed by command buffers from a Cell SPU/PPU thread. In a clone it maps to
the host GPU via Vulkan / OpenGL / DirectX 12.

Software side: `emulator/RSX/`. Hardware side: `hardware/cell_chip/RSX.md`.

---

## 3. I/O subsystems

| Subsystem | What it is | Folder |
|---|---|---|
| Audio | cellAudio software mixer + backends | `emulator/Audio/` |
| Input | DualShock, wheels, guns, camera, GEM | `emulator/Input/` |
| Storage | dev_hdd0 / dev_flash / Blu-ray | `emulator/VFS/` |
| Network | PSN-style sockets, P2P | `emulator/Cell/Modules/` |
| Bluetooth/USB | controllers, peripherals | `emulator/Input/`, `emulator/Cell/Modules/` |

---

## 4. Execution model of a clone

A software clone ("emulator") does not reimplement silicon — it **recompiles** the
original Cell instructions into the host CPU's native code at runtime (JIT), then
executes them. On RPCS3:

- **PPU decoder** options: LLVM Recompiler (fastest), Interpreter (static/dynamic).
- **SPU decoder** options: LLVM Recompiler, ASMJIT Recompiler, Interpreter (fast/precise).
- SPU emulation is the **single largest CPU bottleneck** — every active SPU is recompiled
  and runs on its own host CPU thread.

See `docs/BUILD_AND_RUN.md` and the `emulator/` tree.

---

## 5. Further reading

- `docs/COMPONENTS.md` — every component mapped to a file in this repo.
- `docs/WHERE_IS_EVERYTHING.md` — the one-page "CPU and other parts" locator.
- `hardware/specs/CELL_SPECS.md` — full spec table.
- `hardware/fpga/FPGA_PLAN.md` — what a hardware clone would require.
