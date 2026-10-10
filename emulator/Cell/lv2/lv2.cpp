// lv2.cpp - the PS3 kernel / hypervisor layer.
#include "Cell/lv2/lv2.h"

#include "Cell/PPU/PPU.h"
#include "Cell/Memory/XdrMemory.h"
#include "common/Logging.h"

namespace ps3clone {

LV2::LV2() = default;
LV2::~LV2() = default;

CellResult LV2::Initialize(PPU& ppu, XdrMemory& memory) {
    m_ppu = &ppu;
    m_memory = &memory;
    m_syscall_count = 0;
    m_initialized = true;
    LogInfo("LV2", "lv2 kernel initialised (syscall surface ready)");
    return CellResult::CELL_OK;
}

CellResult LV2::DispatchSyscall(u64 syscall_number, u64* args, size_t arg_count, u64& result) {
    (void)args;
    (void)arg_count;
    if (!m_initialized) {
        return CellResult::CELL_EINVAL;
    }
    ++m_syscall_count;
    result = 0;

    // TODO: implement the lv2 syscall table. Common entries:
    //   0x01  sys_process_exit
    //   0x14  sys_process_getpid
    //   0x15  sys_process_getppid
    //   0x21  sys_process_get_number_of_object
    //   0x35  sys_ppu_thread_create
    //   0x42  sys_memory_allocate
    //   0x4A  sys_memory_free
    //   0x95  sys_time_get_system_time   (syscall 149)
    //   0xE5  sys_spu_image_open
    switch (syscall_number) {
        default:
            return CellResult::CELL_ENOSYS;
    }
}

std::string LV2::Describe() const {
    return "lv2 - PS3 kernel / hypervisor, exposes the guest syscall surface";
}

} // namespace ps3clone
