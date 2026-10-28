# CMakeLists.txt - top-level build file for the PS3 clone skeleton.
#
# Builds a single executable, `ps3clone`, from every .cpp under emulator/.
# The tree is a reference skeleton: it compiles and runs, and each PS3 part
# has a real file with a stub to fill in.
cmake_minimum_required(VERSION 3.20)

project(ps3clone
    VERSION 0.1.0
    DESCRIPTION "PS3 / Cell Broadband Engine clone reference skeleton plus experimental custom CPU/GPU model"
    LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# Sources: everything under emulator/, grouped by subsystem for readability.
# ---------------------------------------------------------------------------
set(PS3CLONE_SOURCES
    emulator/main.cpp

    # common
    emulator/common/Config.cpp
    emulator/common/Logging.cpp
    emulator/common/Types.cpp

    # Cell - PPU (main CPU)
    emulator/Cell/PPU/PPU.cpp
    emulator/Cell/PPU/PPUInterpreter.cpp
    emulator/Cell/PPU/PPURecompiler.cpp
    emulator/Cell/PPU/PPUInstructionTables.cpp

    # Cell - SPU (co-processors)
    emulator/Cell/SPU/SPU.cpp
    emulator/Cell/SPU/SPUInterpreter.cpp
    emulator/Cell/SPU/SPURecompiler.cpp

    # Cell - memory
    emulator/Cell/Memory/XdrMemory.cpp
    emulator/Cell/Memory/SPU_LocalStore.cpp
    emulator/Cell/Memory/SPUDMA.cpp

    # Cell - interconnect
    emulator/Cell/EIB/EIB.cpp
    emulator/Cell/EIB/BEI.cpp

    # Cell - lv2 + modules
    emulator/Cell/lv2/lv2.cpp
    emulator/Cell/lv2/Syscall.cpp
    emulator/Cell/Modules/sys_spu_.cpp
    emulator/Cell/Modules/sys_memory.cpp
    emulator/Cell/Modules/cellGcmSys.cpp
    emulator/Cell/Modules/cellSysutil.cpp

    # RSX (GPU)
    emulator/RSX/RSX.cpp
    emulator/RSX/RSXCommandBuffer.cpp
    emulator/RSX/RSXShaderRecompiler.cpp
    emulator/RSX/RSXTextureCache.cpp
    emulator/RSX/RSXBackendVulkan.cpp
    emulator/RSX/RSXBackendGL.cpp

    # Audio
    emulator/Audio/AudioBackend.cpp
    emulator/Audio/AudioMixer.cpp

    # Input
    emulator/Input/PadHandler.cpp
    emulator/Input/GemHandler.cpp
    emulator/Input/CameraHandler.cpp
    emulator/Input/GunConHandler.cpp
    emulator/Input/USBHandler.cpp

    # Loader
    emulator/Loader/SelfLoader.cpp
    emulator/Loader/PkgLoader.cpp

    # VFS
    emulator/VFS/VirtualFileSystem.cpp
)

add_executable(ps3clone ${PS3CLONE_SOURCES})

# Headers are included as "Cell/...", "RSX/...", "common/...", so emulator/ is
# the include root.
target_include_directories(ps3clone PRIVATE emulator)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(MSVC)
    target_compile_options(ps3clone PRIVATE /W4 /permissive-)
else()
    target_compile_options(ps3clone PRIVATE -Wall -Wextra)
endif()
