// lv2.h - the PS3 hypervisor / kernel layer.
//
// "lv2" is the GameOS kernel the Cell runs. User applications and games talk to
// the hardware through lv2 syscalls (memory, threads, SPU management, time...).
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>

namespace ps3clone
{

    class PPU;
    class XdrMemory;

    class LV2
    {
    public:
        LV2();
        ~LV2();

        CellResult Initialize(PPU &ppu, XdrMemory &memory);

        // Dispatch an lv2 syscall by number. Returns the guest return value in
        // `result`. Syscall numbers follow the real lv2 ABI (e.g. 149 =
        // sys_time_get_system_time).
        CellResult DispatchSyscall(u64 syscall_number, u64 *args, size_t arg_count, u64 &result);

        u64 SyscallCount() const { return m_syscall_count; }

        std::string Describe() const;

    private:
        PPU *m_ppu = nullptr;
        XdrMemory *m_memory = nullptr;
        u64 m_syscall_count = 0;
        bool m_initialized = false;
    };

} // namespace ps3clone
