// PkgLoader.cpp - implementation of the PKG / PUP installer.
#include "Loader/PkgLoader.h"
#include "common/Logging.h"

#include <fstream>

namespace ps3clone
{

    bool PkgLoader::IsPkg(const std::vector<u8> &data)
    {
        return data.size() >= 4 && data[0] == 0x7F && data[1] == 'P' && data[2] == 'K' &&
               data[3] == 'G';
    }

    bool PkgLoader::IsPup(const std::vector<u8> &data)
    {
        return data.size() >= 4 && data[0] == 'S' && data[1] == 'C' && data[2] == 'E' &&
               data[3] == 'U';
    }

    CellResult PkgLoader::Open(const std::string &path, PkgHeader &header)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            LogError("PkgLoader", "Cannot open " + path);
            return CellResult::CELL_EINVAL;
        }

        std::vector<u8> data((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        if (!IsPkg(data) && !IsPup(data))
            return CellResult::CELL_EINVAL;

        // TODO: parse and decrypt the package header / file table.
        m_header = PkgHeader{};
        m_files.clear();
        header = m_header;
        LogInfo("PkgLoader", "Opened " + path);
        return CellResult::CELL_OK;
    }

    CellResult PkgLoader::ListFiles(std::vector<PkgFileEntry> &files) const
    {
        files = m_files;
        return CellResult::CELL_OK;
    }

    CellResult PkgLoader::Extract(const std::string &dest)
    {
        (void)dest;
        // TODO: decrypt each entry and write it through the VFS.
        return CellResult::CELL_OK;
    }

    std::string PkgLoader::Describe() const
    {
        return "PKG/PUP installer (" + std::to_string(m_files.size()) + " file(s))";
    }

} // namespace ps3clone
