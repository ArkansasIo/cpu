// Syscall.h - lv2 syscall numbers and helpers.
#pragma once

#include "common/Types.h"

namespace ps3clone {

// A subset of the real lv2 syscall numbers, for orientation.
enum class SyscallNumber : u64 {
    ProcessExit          = 0x01,
    ProcessGetPid        = 0x14,
    ProcessGetPpid       = 0x15,
    PpuThreadCreate      = 0x35,
    PpuThreadExit        = 0x2C,
    MemoryAllocate       = 0x42,
    MemoryFree           = 0x4A,
    MemoryGetPageSize    = 0x4C,
    TimeGetSystemTime    = 0x95, // syscall 149
    SpuImageOpen         = 0xE5,
    SpuThreadInitialize  = 0xE6,
    SpuThreadStart       = 0xE7,
    SpuThreadGroupCreate = 0xE8,
    FsOpen               = 0x33CC,
};

} // namespace ps3clone
