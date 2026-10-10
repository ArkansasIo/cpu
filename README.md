# PS3 Clone Reference Project

> A complete reference layout for a PlayStation 3 "clone": the **Cell Broadband Engine**
> (PPU + SPUs), the **RSX GPU**, and every other subsystem — organised the same way a
> working emulator organises it, plus the hardware-side component map.

This project was generated to answer one question: **"where is the CPU and where is
everything else?"** Every part gets its own folder and file, mirroring the real PS3.

---

## Three layers

| Layer | Folder | What it describes | Real-world equivalent |
|---|---|---|---|
| 1. Docs | `docs/` | Architecture overview, component cheat-sheet | Whitepapers / Copetti / IBM CBE SDK |
| 2. Software clone | `emulator/` | JIT-recompiling emulator skeleton (PPU, SPU, RSX, modules, input, audio) | [RPCS3](https://github.com/RPCS3/rpcs3) |
| 3. Hardware clone | `hardware/` | Cell die component map, FPGA mapping, spec sheets | Cell BE chip / FPGA reimplementation |

---

## Where is the "CPU"?

On a PS3 the word *CPU* is ambiguous, so this project keeps it explicit:

- **The main CPU** = the **PPU** (Power Processing Element) — a 64-bit PowerPC core.
  Software side: `emulator/Cell/PPU/`. Hardware side: `hardware/cell_chip/PPU.md`.
- **The co-processors** = 8 × **SPU** (Synergistic Processor Elements), 7 usable.
  Software side: `emulator/Cell/SPU/`. Hardware side: `hardware/cell_chip/SPU.md`.
- **The GPU** = the **RSX** (Reality Synthesizer), separate from Cell.
  Software side: `emulator/RSX/`. Hardware side: `hardware/cell_chip/RSX.md`.

See `docs/ARCHITECTURE.md` for the full component table and `docs/WHERE_IS_EVERYTHING.md`
for the one-page locator.

---

## Layout

```
.
├── README.md                      <- you are here
├── docs/
│   ├── ARCHITECTURE.md            <- Cell + RSX + I/O architecture overview
│   ├── COMPONENTS.md              <- table: every part -> file -> purpose
│   ├── WHERE_IS_EVERYTHING.md     <- one-page locator ("CPU and other parts")
│   └── BUILD_AND_RUN.md           <- how to build/run the emulator skeleton
├── emulator/                      <- software clone (RPCS3-style source tree)
│   ├── Cell/                      <- the Cell Broadband Engine emulation
│   │   ├── PPU/                   <- CPU main core  (interpreter + recompiler)
│   │   ├── SPU/                   <- co-processors  (interpreter + recompiler)
│   │   ├── EIB/                   <- Element Interconnect Bus
│   │   ├── Memory/                <- XDR main RAM + SPU local store
│   │   ├── Modules/               <- OS/firmware libraries (cell*, sys_*)
│   │   └── lv2/                   <- hypervisor / kernel layer
│   ├── RSX/                       <- GPU (Reality Synthesizer)
│   ├── Audio/                     <- cellAudio
│   ├── Input/                     <- pads, DualShock, camera, wheels, guns
│   ├── Loader/                    <- SELF / ELF / PKG / PUP loading
│   ├── VFS/                       <- virtual filesystem (dev_flash, dev_hdd0)
│   └── common/                    <- shared types, logging, config
├── hardware/                      <- hardware clone layer
│   ├── cell_chip/                 <- per-component die description
│   ├── fpga/                      <- FPGA reimplementation plan
│   └── specs/                     <- spec tables (clock, cache, bandwidth)
├── scripts/                       <- build / run helpers
└── CMakeLists.txt                 <- top-level build file
```

## Quick start

```powershell
# Configure + build the skeleton
cmake -S . -B build
cmake --build build --config Release

# Run
./build/ps3clone --help
```

`docs/BUILD_AND_RUN.md` has the details.

## Sources

- [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor))
- [Cell Broadband Engine Architecture overview (IBM CBE SDK)](https://arcb.csc.ncsu.edu/~mueller/cluster/ps3/SDK3.0/docs/accessibility/sdkpt/cbet_1arch_ovw.html)
- [PlayStation 3 Architecture — Rodrigo Copetti](https://www.copetti.org/writings/consoles/playstation-3)
- [RPCS3 project](https://github.com/RPCS3/rpcs3)
- [RPCS3 Cell CPU SPU breakthrough — Tom's Hardware](https://www.tomshardware.com/video-games/playstation/rpcs3-ps3-emulator-gets-cell-cpu-breakthrough-that-improves-performance-in-all-games)
