// RSXTextureCache.h - guest textures -> host textures.
//
// The RSX addresses textures by a guest memory pointer; the host needs a real
// GPU texture. This cache keys on (address, format, size) and re-uploads only
// the regions a game actually touches, which is why it also tracks which
// guest pages were dirtied between frames.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace ps3clone
{

    // The RSX pixel formats a texture can be stored in.
    enum class TextureFormat
    {
        A8R8G8B8,
        X8R8G8B8,
        R5G6B5,
        A1R5G5B5,
        DXT1,
        DXT3,
        DXT5,
        Unknown
    };

    struct TextureKey
    {
        GuestAddr address = 0; // guest memory pointer
        u32 width = 0;
        u32 height = 0;
        TextureFormat format = TextureFormat::Unknown;

        bool operator==(const TextureKey &other) const
        {
            return address == other.address && width == other.width &&
                   height == other.height && format == other.format;
        }
    };

    struct TextureKeyHash
    {
        size_t operator()(const TextureKey &k) const noexcept
        {
            size_t h = std::hash<u64>{}(k.address);
            h ^= std::hash<u32>{}(k.width) + 0x9E3779B9u + (h << 6) + (h >> 2);
            h ^= std::hash<u32>{}(k.height) + 0x9E3779B9u + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(static_cast<int>(k.format)) + 0x9E3779B9u + (h << 6) + (h >> 2);
            return h;
        }
    };

    class RSXTextureCache
    {
    public:
        RSXTextureCache();
        ~RSXTextureCache();

        // Look up (or create) the host texture for a guest texture. Returns
        // nullptr if the guest range is not backed by memory.
        const struct HostTexture *GetOrCreate(const TextureKey &key);

        // Mark a guest range dirty so the next GetOrCreate re-uploads it.
        void InvalidateRange(GuestAddr address, u64 size);

        // Drop everything (e.g. on RSX reset).
        void Clear();

        size_t Count() const { return m_textures.size(); }
        u64 Uploads() const { return m_uploads; }
        u64 Hits() const { return m_hits; }

        static const char *FormatName(TextureFormat format);

        std::string Describe() const;

    private:
        struct HostTexture
        {
            TextureKey key;
            u32 handle = 0; // host GPU object id (Vulkan image / GL name)
            u64 uploaded_frame = 0;
            std::vector<u8> pixels;
        };

        u64 m_frame = 0;
        u64 m_uploads = 0;
        u64 m_hits = 0;
        std::unordered_map<TextureKey, HostTexture, TextureKeyHash> m_textures;
        std::vector<std::pair<GuestAddr, u64>> m_dirty;
    };

} // namespace ps3clone
