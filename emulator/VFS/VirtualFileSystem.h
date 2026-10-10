// VirtualFileSystem.h - the PS3 storage tree (dev_flash, dev_hdd0, dev_bdvd...).
//
// The guest sees a fixed set of mount points, each backed by a host folder or
// an image file. Paths are translated from guest form ("/dev_hdd0/game/...")
// to host form before touching the disk.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    enum class MountType
    {
        Directory, // backed by a host folder
        Image,     // backed by a .img / .iso file
        BluRay,    // optical disc image
        ReadOnly,
    };

    struct MountPoint
    {
        std::string guest_path; // e.g. "/dev_hdd0"
        std::string host_path;  // e.g. "data/dev_hdd0"
        MountType type = MountType::Directory;
        bool writable = true;
    };

    class VirtualFileSystem
    {
    public:
        CellResult Initialize();
        CellResult Mount(const MountPoint &mount);
        CellResult Unmount(const std::string &guest_path);

        // Translate "/dev_hdd0/game/x" -> "data/dev_hdd0/game/x". Returns false
        // when no mount point covers the guest path.
        bool Resolve(const std::string &guest_path, std::string &host_path) const;

        bool Exists(const std::string &guest_path) const;
        bool IsDirectory(const std::string &guest_path) const;

        CellResult ReadFile(const std::string &guest_path, std::vector<u8> &out) const;
        CellResult WriteFile(const std::string &guest_path, const std::vector<u8> &data);

        std::string Describe() const;

    private:
        std::vector<MountPoint> m_mounts;
    };

} // namespace ps3clone
