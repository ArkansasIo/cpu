// SelfLoader.cpp - implementation of the SELF / ELF loader.
#include "Loader/SelfLoader.h"
#include "common/Logging.h"

#include <fstream>
#include <sstream>

namespace ps3clone
{

    bool SelfLoader::IsSelf(const std::vector<u8> &data)
    {
        return data.size() >= 4 &&
               data[0] == 0x00 && data[1] == 0x43 && data[2] == 0x45 && data[3] == 0x53; // "SCE\0" BE
    }

    bool SelfLoader::IsElf(const std::vector<u8> &data)
    {
        return data.size() >= 4 && data[0] == 0x7F && data[1] == 'E' && data[2] == 'L' &&
               data[3] == 'F';
    }

    CellResult SelfLoader::LoadFromMemory(const std::vector<u8> &data, SelfHeader &header,
                                          std::vector<ElfSegment> &segments)
    {
        header = SelfHeader{};
        segments.clear();

        if (!IsSelf(data) && !IsElf(data))
            return CellResult::CELL_EINVAL;

        header.name = IsSelf(data) ? "SELF image" : "ELF image";
        // TODO: parse the SELF header, decrypt the segments, then parse the inner
        // ELF program headers into `segments`.
        LogInfo("SelfLoader", "Parsed " + header.name);
        return CellResult::CELL_OK;
    }

    CellResult SelfLoader::LoadFromFile(const std::string &path, SelfHeader &header,
                                        std::vector<ElfSegment> &segments)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            LogError("SelfLoader", "Cannot open " + path);
            return CellResult::CELL_EINVAL;
        }

        std::vector<u8> data((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        return LoadFromMemory(data, header, segments);
    }

    std::string SelfLoader::Describe() const
    {
        return "SELF/ELF loader (SCE + ELF container support)";
    }

} // namespace ps3clone
