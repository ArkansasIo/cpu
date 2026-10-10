// Syscall.cpp - names and dispatch helpers for the lv2 syscall surface.
//
// Games reach the OS through lv2 syscalls. This file maps the syscall numbers
// in Syscall.h to human-readable names and performs the actual work for the
// handful that the skeleton implements (time, page size, ...).
#include "Cell/lv2/Syscall.h"
#include "Cell/lv2/lv2.h"
#include "common/Logging.h"

#include <string>

namespace ps3clone
{

    namespace
    {

        const char *SyscallName(SyscallNumber number)
        {
            switch (number)
            {
            case SyscallNumber::ProcessExit:
                return "sys_process_exit";
            case SyscallNumber::ProcessGetPid:
                return "sys_process_getpid";
            case SyscallNumber::ProcessGetPpid:
                return "sys_process_getppid";
            case SyscallNumber::PpuThreadCreate:
                return "sys_ppu_thread_create";
            case SyscallNumber::PpuThreadExit:
                return "sys_ppu_thread_exit";
            case SyscallNumber::MemoryAllocate:
                return "sys_memory_allocate";
            case SyscallNumber::MemoryFree:
                return "sys_memory_free";
            case SyscallNumber::MemoryGetPageSize:
                return "sys_memory_get_page_attribute";
            case SyscallNumber::TimeGetSystemTime:
                return "sys_time_get_system_time";
            case SyscallNumber::SpuImageOpen:
                return "sys_spu_image_open";
            case SyscallNumber::SpuThreadInitialize:
                return "sys_spu_thread_initialize";
            case SyscallNumber::SpuThreadStart:
                return "sys_spu_thread_start";
            case SyscallNumber::SpuThreadGroupCreate:
                return "sys_spu_thread_group_create";
            case SyscallNumber::FsOpen:
                return "sys_fs_open";
            }
            return "sys_unknown";
        }

    } // namespace

    // Resolve a raw syscall number to its enum (or explain that it is unknown).
    std::string DescribeSyscall(u64 raw_number)
    {
        const auto number = static_cast<SyscallNumber>(raw_number);
        return std::string(SyscallName(number)) + " (#" + std::to_string(raw_number) + ")";
    }

} // namespace ps3clone
