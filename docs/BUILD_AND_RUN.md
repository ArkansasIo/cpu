# Building and running the clone skeleton

This tree is a **reference skeleton**: it compiles, links and runs, with every PS3 part
given a real file and a stub/`TODO` implementation you can fill in. It does not play
commercial games out of the box (that is a multi-year project).

## Requirements

- CMake 3.20+
- A C++20 compiler: MSVC 2022, Clang 15+, or GCC 12+
- (optional, for the real thing) Vulkan SDK

## Build (PowerShell)

```powershell
cmake -S . -B build
cmake --build build --config Release
```

## Run

```powershell
./build/Release/ps3clone.exe --help
./build/Release/ps3clone.exe --list-components
./build/Release/ps3clone.exe --boot-demo
```

`--list-components` walks the Cell: PPU, all 7 SPUs, the EIB, MIC, BEI and the RSX, and
prints each one's identity — a quick way to see "where everything is" at runtime.

## Emulator settings (RPCS3-style, for reference)

| Setting | Values | Notes |
|---|---|---|
| PPU decoder | LLVM Recompiler / Interpreter (static/dynamic) | LLVM is fastest |
| SPU decoder | LLVM / ASMJIT / Interpreter (fast/precise) | ASMJIT if LLVM crashes |
| SPU threads | 1–6 or auto | limit to fix audio stutter |
| SPU XFloat accuracy | accurate / approximate / relaxed | fixes float bugs |
| Graphics API | Vulkan / OpenGL | Vulkan recommended |
| Shader mode | Async / Async w/ interpreter / Interpreter | async avoids hitching |
| ZCULL accuracy | precise / approximate / relaxed | perf vs accuracy |

Source: [RPCS3 default settings](https://wiki.rpcs3.net/index.php?title=Help%3ADefault_Settings).

## Where to start reading code

1. `emulator/main.cpp` — process entry, boots the system
2. `emulator/Cell/PPU/PPU.cpp` — the CPU
3. `emulator/Cell/SPU/SPU.cpp` — the co-processors
4. `emulator/Cell/EIB/EIB.cpp` — how they talk
5. `emulator/RSX/RSX.cpp` — the GPU
