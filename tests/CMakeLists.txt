enable_testing()

add_executable(custom_cpu_gpu_smoke
    custom_cpu_gpu_smoke.cpp
    ../emulator/Custom/Memory/MemoryArena.cpp
    ../emulator/Custom/CPU/CPUCore.cpp
    ../emulator/Custom/GPU/ComputeGPU.cpp
)
target_include_directories(custom_cpu_gpu_smoke PRIVATE ../emulator)
add_test(NAME custom_cpu_gpu_smoke COMMAND custom_cpu_gpu_smoke)
