# Where is everything? — one-page locator

You asked: **"PS3 clone CPU and other parts — where?"**
Here is the direct answer, in this project.

## The CPU

| Question | Answer | Software file | Hardware file |
|---|---|---|---|
| Where is the main **CPU**? | The **PPU** (Power Processing Element), a 64-bit PowerPC core | `emulator/Cell/PPU/PPU.cpp` | `hardware/cell_chip/PPU.md` |
| Where are the **co-processors**? | 8 × **SPU** (7 usable, 6 for games) | `emulator/Cell/SPU/SPU.cpp` | `hardware/cell_chip/SPU.md` |
| Where is the **GPU**? | Separate chip: the **RSX** | `emulator/RSX/RSX.cpp` | `hardware/cell_chip/RSX.md` |

## Everything else

| Part | What it is | Where it lives |
|---|---|---|
| Interconnect | **EIB** — 4-ring coherent bus, 96 B/cycle | `emulator/Cell/EIB/EIB.cpp` |
| Main memory | 256 MB **XDR DRAM** | `emulator/Cell/Memory/XdrMemory.cpp` |
| SPU memory | **256 KB local store** per SPU | `emulator/Cell/Memory/SPU_LocalStore.cpp` |
| OS / kernel | **lv2** hypervisor layer | `emulator/Cell/lv2/lv2.cpp` |
| Firmware libs | `cellGcm`, `cellAudio`, `sys_spu`, `sys_memory`, … | `emulator/Cell/Modules/*.cpp` |
| Audio | cellAudio mixer + host backend | `emulator/Audio/AudioBackend.cpp` |
| Input | DualShock / wheels / guns / camera | `emulator/Input/PadHandler.cpp` |
| Loader | SELF / ELF / PKG / PUP | `emulator/Loader/SelfLoader.cpp` |
| Filesystem | dev_flash, dev_hdd0, BD | `emulator/VFS/VirtualFileSystem.cpp` |
| Memory controller | **MIC** (EIB ↔ XDR) | `hardware/cell_chip/EIB.md` (§MIC) |
| I/O interface | **BEI** (EIB ↔ RSX/I/O) | `hardware/cell_chip/EIB.md` (§BEI) |

## Quick mental model

```
PPU  = the CPU you usually mean           (hardware/cell_chip/PPU.md)
SPU  = the 7 co-processors that matter    (hardware/cell_chip/SPU.md)
EIB  = the bus tying them together        (emulator/Cell/EIB/)
RSX  = the GPU, a separate chip           (emulator/RSX/)
MIC/BEI = the two doors out of Cell       (hardware/cell_chip/EIB.md)
```

## Running the clone

See `docs/BUILD_AND_RUN.md`.
