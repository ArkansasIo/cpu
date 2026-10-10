// sys_spu_.cpp - SPU management library implementation.
#include "Cell/Modules/sys_spu_.h"

#include "Cell/Memory/SPU_LocalStore.h"
#include "Cell/SPU/SPU.h"
#include "common/Logging.h"

namespace ps3clone
{

    CellResult SysSpuModule::ImageOpen(SysSpuImage &image, const std::string &path)
    {
        LogInfo("sys_spu", "sys_spu_image_open(path=" + path + ")");
        // TODO: parse the SPU SELF/ELF file and fill `image`.
        image = SysSpuImage{};
        return CellResult::CELL_OK;
    }

    CellResult SysSpuModule::ImageClose(SysSpuImage &image)
    {
        (void)image;
        return CellResult::CELL_OK;
    }

    CellResult SysSpuModule::ThreadGroupCreate(u32 group_id, int spu_count)
    {
        if (spu_count <= 0 || spu_count > kGameSPUCount)
        {
            LogWarn("sys_spu", "Invalid SPU count requested: " + std::to_string(spu_count));
            return CellResult::CELL_EINVAL;
        }
        (void)group_id;
        ++m_next_group_id;
        return CellResult::CELL_OK;
    }

    CellResult SysSpuModule::ThreadInitialize(u32 thread_id, u32 group_id, const SysSpuImage &image)
    {
        (void)thread_id;
        (void)group_id;
        (void)image;
        // TODO: load the image into the target SPU's 256 KB local store.
        return CellResult::CELL_OK;
    }

    CellResult SysSpuModule::ThreadStart(u32 thread_id)
    {
        (void)thread_id; // TODO: unpause the SPU thread
        return CellResult::CELL_OK;
    }

    CellResult SysSpuModule::ThreadGroupStart(u32 group_id)
    {
        (void)group_id;
        return CellResult::CELL_OK;
    }

    CellResult SysSpuModule::ThreadGroupTerminate(u32 group_id)
    {
        (void)group_id;
        return CellResult::CELL_OK;
    }

} // namespace ps3clone
