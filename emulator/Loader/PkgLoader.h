// PkgLoader.h - PKG / PUP installer.
//
// A PKG is Sony's installable package (games, patches, DLC). A PUP is a
// firmware update package. Both wrap their content in an encrypted header and
// a set of files that this loader extracts into the VFS.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    constexpr u32 kPkgMagic = 0x7F504B47; // "\x7FPKG"
    constexpr u32 kPupMagic = 0x53434555; // "SCEU"

    struct PkgHeader
    {
        u32 magic = 0;
        u32 version = 0;
        std::string content_id; // e.g. "UP0001-BLUS12345_00-..."
        u64 data_size = 0;
    };

    struct PkgFileEntry
    {
        std::string name;
        u64 offset = 0;
        u64 size = 0;
    };

    class PkgLoader
    {
    public:
        static bool IsPkg(const std::vector<u8> &data);
        static bool IsPup(const std::vector<u8> &data);

        CellResult Open(const std::string &path, PkgHeader &header);
        CellResult ListFiles(std::vector<PkgFileEntry> &files) const;

        // Extract every file into the virtual filesystem under `dest`.
        CellResult Extract(const std::string &dest);

        std::string Describe() const;

    private:
        PkgHeader m_header;
        std::vector<PkgFileEntry> m_files;
    };

} // namespace ps3clone
