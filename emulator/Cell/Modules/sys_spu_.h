// sys_spu_.h - SPU management library (sys_spu).
//
// The sys_spu module lets the PPU load, start and stop SPU programs. Typical
// flow: sys_spu_image_open() a SELF image, then create a thread group, then
// initialize and start SPU threads.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>

namespace ps3clone
{

    // Mirrors the guest-visible sys_spu_image_t: pointers to a loaded SPU image.
    struct SysSpuImage
    {
        u32 entry_point = 0;    // e_entry
        u32 segment_count = 0;  // e_phnum
        GuestAddr segments = 0; // pointer to the ELF program headers
        u32 type = 0;
    };

    class SysSpuModule
    {
    public:
        CellResult ImageOpen(SysSpuImage &image, const std::string &path);
        CellResult ImageClose(SysSpuImage &image);

        CellResult ThreadGroupCreate(u32 group_id, int spu_count);
        CellResult ThreadInitialize(u32 thread_id, u32 group_id, const SysSpuImage &image);
        CellResult ThreadStart(u32 thread_id);
        CellResult ThreadGroupStart(u32 group_id);
        CellResult ThreadGroupTerminate(u32 group_id);

    private:
        u32 m_next_group_id = 1;
    };

} // namespace ps3clone
