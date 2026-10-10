# Cell chip — PPU (Power Processing Element)

The PPU is the *main CPU* of the PlayStation 3: one general-purpose 64-bit
PowerPC core sitting on the same die as the eight SPUs. If someone says "the PS3
CPU", they almost always mean the PPU.

Software counterpart: `emulator/Cell/PPU/`.

## Block diagram

```
        +--------------------------------------------------+
        |                     PPU                          |
        |                                                  |
  IB ---> L1I (32 KB) --> Instruction fetch / decode        |
        |                        |                         |
        |                   Dispatch (dual-issue)           |
        |        +--------+--------+--------+--------+      |
        |        |  FXU   |  FPU   |  VMX   |  LSU   | ...  |
        |        +--------+--------+--------+--------+      |
        |                        |                         |
  DB <--> L1D (32 KB) <----------> Load/store unit          |
        |                        |                         |
        |                     L2 (512 KB)                   |
        +------------------------+-------------------------+
                                 |
                          EIB (96 B/cycle)
```

## Specification

| Property | Value |
|---|---|
| ISA | PowerPC 2.02 + AltiVec/VMX SIMD |
| Width | 64-bit RISC, dual-issue, in-order |
| Threads | 2 (SMT), 23-stage pipeline |
| L1 cache | 32 KB instruction + 32 KB data |
| L2 cache | 512 KB, unified |
| Cache line | 128 bytes |
| Execution units | FXU (32-bit fixed), FPU (64-bit), AltiVec/VMX, LSU, BRU/BXU |
| Endianness | Big-endian and little-endian (dual) |

## Role

- Boots and runs the **lv2** hypervisor (the PS3 OS kernel).
- Loads and schedules **SPU** programs via `sys_spu`.
- Runs game logic that is not offloaded to the SPUs.
- Drives the **RSX** by building GCM command buffers.

## Emulation notes

- Decoders: LLVM Recompiler (fastest), Interpreter (static/dynamic).
- The PPU is far less of a bottleneck than the SPUs; a modern CPU runs it
  comfortably because it is "just" PowerPC.

Sources: [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)),
[PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3).
