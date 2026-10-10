# Component map

Every PS3 part, mapped to the file that holds it in this project.

## Cell Broadband Engine (the CPU + co-processors)

| # | Component | Emulator file | Hardware doc | Purpose in the PS3 |
|---|---|---|---|---|
| 1 | PPU (main CPU) | `emulator/Cell/PPU/PPU.cpp` | `hardware/cell_chip/PPU.md` | PowerPC core: OS, game logic, scheduling |
| 2 | PPU interpreter | `emulator/Cell/PPU/PPUInterpreter.cpp` | — | Slow, most accurate fallback |
| 3 | PPU recompiler | `emulator/Cell/PPU/PPURecompiler.cpp` | — | JIT to host x86/ARM (the fast path) |
| 4 | SPU (co-processors) | `emulator/Cell/SPU/SPU.cpp` | `hardware/cell_chip/SPU.md` | 7 SIMD cores: physics, audio, decompression |
| 5 | SPU interpreter | `emulator/Cell/SPU/SPUInterpreter.cpp` | — | Accurate fallback |
| 6 | SPU recompiler | `emulator/Cell/SPU/SPURecompiler.cpp` | — | JIT; the biggest CPU bottleneck |
| 7 | SPU local store | `emulator/Cell/Memory/SPU_LocalStore.cpp` | `hardware/cell_chip/SPU.md` | 256 KB scratchpad per SPU |
| 8 | SPU DMA | `emulator/Cell/Memory/SPUDMA.cpp` | `hardware/cell_chip/SPU.md` | async local-store ↔ main RAM transfers |
| 9 | EIB | `emulator/Cell/EIB/EIB.cpp` | `hardware/cell_chip/EIB.md` | 4-ring coherent interconnect |
| 10 | MIC | `emulator/Cell/Memory/XdrMemory.cpp` | `hardware/cell_chip/EIB.md` | Memory Interface Controller |
| 11 | BEI | `emulator/Cell/EIB/BEI.cpp` | `hardware/cell_chip/EIB.md` | Cell ↔ I/O bridge |
| 12 | XDR main RAM | `emulator/Cell/Memory/XdrMemory.cpp` | `hardware/specs/CELL_SPECS.md` | 256 MB XDR DRAM |
| 13 | lv2 hypervisor | `emulator/Cell/lv2/lv2.cpp` | — | kernel / syscall surface |
| 14 | syscalls | `emulator/Cell/lv2/Syscall.cpp` | — | sys_memory, sys_spu, sys_time, … |

## System modules (HLE libraries)

| Module | File | What it does |
|---|---|---|
| sys_spu | `emulator/Cell/Modules/sys_spu_.cpp` | load / start / stop SPU programs |
| sys_memory | `emulator/Cell/Modules/sys_memory.cpp` | guest memory allocation |
| cellGcmSys | `emulator/Cell/Modules/cellGcmSys.cpp` | submit GPU commands to RSX |
| cellAudio | `emulator/Audio/AudioBackend.cpp` | software audio mixing |
| cellPad | `emulator/Input/PadHandler.cpp` | controller input |
| cellGem | `emulator/Input/GemHandler.cpp` | PlayStation Move |
| cellSysutil | `emulator/Cell/Modules/cellSysutil.cpp` | system UI callbacks |

## RSX (the GPU)

| Component | File | Purpose |
|---|---|---|
| RSX core | `emulator/RSX/RSX.cpp` | GPU command processor front-end |
| Command buffer | `emulator/RSX/RSXCommandBuffer.cpp` | decode GCM command stream |
| Backend Vulkan | `emulator/RSX/RSXBackendVulkan.cpp` | host rendering via Vulkan |
| Backend OpenGL | `emulator/RSX/RSXBackendGL.cpp` | host rendering via OpenGL |
| Shader recompiler | `emulator/RSX/RSXShaderRecompiler.cpp` | RSX shaders → host SPIR-V/GLSL |
| Texture cache | `emulator/RSX/RSXTextureCache.cpp` | guest textures → host textures |

## Everything else

| Component | File |
|---|---|
| Pads / DualShock | `emulator/Input/PadHandler.cpp` |
| Wheels / guns | `emulator/Input/GunConHandler.cpp` |
| Camera | `emulator/Input/CameraHandler.cpp` |
| Bluetooth / USB HID | `emulator/Input/USBHandler.cpp` |
| SELF / ELF loader | `emulator/Loader/SelfLoader.cpp` |
| PKG / PUP installer | `emulator/Loader/PkgLoader.cpp` |
| Virtual filesystem | `emulator/VFS/VirtualFileSystem.cpp` |
| Logging | `emulator/common/Logging.cpp` |
| Config | `emulator/common/Config.cpp` |
