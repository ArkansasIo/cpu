// SelfLoader.h - SELF / ELF loader.
//
// A retail PS3 executable is a "SELF" (Signed ELF): an ELF wrapper whose
// segments are encrypted/compressed and signed with keys that only the console
// holds. This loader parses the container and stages the segments.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    // Magic numbers for the containers we recognise.
    constexpr u32 kSelfMagic = 0x53434500; // "SCE\0"
    constexpr u32 kElfMagic = 0x7F454C46;  // "\x7FELF"

    struct SelfHeader
    {
        u32 magic = 0;
        u32 version = 0;
        u32 flags = 0;
        u32 header_size = 0; // offset to the embedded ELF
        std::string name;
    };

    struct ElfSegment
    {
        GuestAddr vaddr = 0;
        u64 offset = 0;
        u64 file_size = 0;
        u64 mem_size = 0;
        u32 flags = 0; // PF_R / PF_W / PF_X
    };

    class SelfLoader
    {
    public:
        // Detect the container type from the first bytes.
        static bool IsSelf(const std::vector<u8> &data);
        static bool IsElf(const std::vector<u8> &data);

        // Parse a SELF or plain ELF image and fill `header` / `segments`.
        CellResult LoadFromMemory(const std::vector<u8> &data, SelfHeader &header,
                                  std::vector<ElfSegment> &segments);

        CellResult LoadFromFile(const std::string &path, SelfHeader &header,
                                std::vector<ElfSegment> &segments);

        std::string Describe() const;
    };

} // namespace ps3clone
