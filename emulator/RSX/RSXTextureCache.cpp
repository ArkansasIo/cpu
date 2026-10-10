// RSXTextureCache.cpp - implementation of the guest->host texture cache.
#include "RSX/RSXTextureCache.h"
#include "common/Logging.h"

#include <sstream>

namespace ps3clone
{

    RSXTextureCache::RSXTextureCache() = default;
    RSXTextureCache::~RSXTextureCache() = default;

    const RSXTextureCache::HostTexture *RSXTextureCache::GetOrCreate(const TextureKey &key)
    {
        auto it = m_textures.find(key);
        if (it != m_textures.end())
        {
            ++m_hits;
            return &it->second;
        }

        // TODO: decode the guest texture (swizzle / DXT) and upload it to the host.
        HostTexture host;
        host.key = key;
        host.handle = static_cast<u32>(++m_uploads);
        host.uploaded_frame = m_frame;

        auto inserted = m_textures.emplace(key, std::move(host));
        return &inserted.first->second;
    }

    void RSXTextureCache::InvalidateRange(GuestAddr address, u64 size)
    {
        m_dirty.emplace_back(address, size);
    }

    void RSXTextureCache::Clear()
    {
        m_textures.clear();
        m_dirty.clear();
        m_uploads = 0;
        m_hits = 0;
        m_frame = 0;
    }

    const char *RSXTextureCache::FormatName(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::A8R8G8B8:
            return "A8R8G8B8";
        case TextureFormat::X8R8G8B8:
            return "X8R8G8B8";
        case TextureFormat::R5G6B5:
            return "R5G6B5";
        case TextureFormat::A1R5G5B5:
            return "A1R5G5B5";
        case TextureFormat::DXT1:
            return "DXT1";
        case TextureFormat::DXT3:
            return "DXT3";
        case TextureFormat::DXT5:
            return "DXT5";
        case TextureFormat::Unknown:
            break;
        }
        return "Unknown";
    }

    std::string RSXTextureCache::Describe() const
    {
        std::ostringstream os;
        os << "RSX texture cache: " << m_textures.size() << " texture(s), " << m_uploads
           << " upload(s), " << m_hits << " hit(s)";
        return os.str();
    }

} // namespace ps3clone
