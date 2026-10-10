// VirtualFileSystem.cpp - implementation of the PS3 storage tree.
#include "VFS/VirtualFileSystem.h"
#include "common/Logging.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace ps3clone
{

    CellResult VirtualFileSystem::Initialize()
    {
        m_mounts.clear();

        // The standard PS3 mount points.
        Mount("."); // placeholder so the vector is non-empty in the skeleton
        Mount(MountPoint{"/dev_hdd0", "data/dev_hdd0", MountType::Directory, true});
        Mount(MountPoint{"/dev_flash", "data/dev_flash", MountType::ReadOnly, false});
        Mount(MountPoint{"/dev_bdvd", "data/dev_bdvd", MountType::BluRay, false});

        LogInfo("VFS", Describe());
        return CellResult::CELL_OK;
    }

    CellResult VirtualFileSystem::Mount(const MountPoint &mount)
    {
        m_mounts.push_back(mount);
        return CellResult::CELL_OK;
    }

    CellResult VirtualFileSystem::Unmount(const std::string &guest_path)
    {
        for (auto it = m_mounts.begin(); it != m_mounts.end(); ++it)
        {
            if (it->guest_path == guest_path)
            {
                            m_mounts.erase(it);
                            return CellResult::CELL_OK;
                        }
                    }
                    return CellResult::CELL_EINVAL;
                }

    bool VirtualFileSystem::Resolve(const std::string &guest_path, std::string &host_path) const
    {
        // Longest matching mount point wins.
        const MountPoint *best = nullptr;
        for (const auto &mount : m_mounts)
        {
            if (guest_path.rfind(mount.guest_path, 0) == 0)
            {
                if (!best || mount.guest_path.size() > best->guest_path.size())
                    best = &mount;
            }
        }
        if (!best)
            return false;

        std::string suffix = guest_path.substr(best->guest_path.size());
        while (!suffix.empty() && (suffix.front() == '/' || suffix.front() == '\\'))
            suffix.erase(suffix.begin());
        host_path = best->host_path;
        if (!suffix.empty())
            host_path += "/" + suffix;
        return true;
    }

    bool VirtualFileSystem::Exists(const std::string &guest_path) const
    {
        std::string host;
        return Resolve(guest_path, host) && fs::exists(host);
    }

    bool VirtualFileSystem::IsDirectory(const std::string &guest_path) const
    {
        std::string host;
        return Resolve(guest_path, host) && fs::is_directory(host);
    }

    CellResult VirtualFileSystem::ReadFile(const std::string &guest_path, std::vector<u8> &out) const
    {
        std::string host;
        if (!Resolve(guest_path, host))
            return CellResult::CELL_EINVAL;

        std::ifstream file(host, std::ios::binary);
        if (!file)
            return CellResult::CELL_EFAULT;

        out.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return CellResult::CELL_OK;
    }

    CellResult VirtualFileSystem::WriteFile(const std::string &guest_path, const std::vector<u8> &data)
    {
        std::string host;
        if (!Resolve(guest_path, host))
            return CellResult::CELL_EINVAL;

        std::ofstream file(host, std::ios::binary);
        if (!file)
            return CellResult::CELL_EFAULT;

        file.write(reinterpret_cast<const char *>(data.data()),
                   static_cast<std::streamsize>(data.size()));
        return CellResult::CELL_OK;
    }

    std::string VirtualFileSystem::Describe() const
    {
        std::ostringstream os;
        os << "VFS: " << m_mounts.size() << " mount point(s)";
        return os.str();
    }

} // namespace ps3clone
