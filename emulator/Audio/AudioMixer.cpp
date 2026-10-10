// AudioMixer.cpp - implementation of the 8-port cellAudio mixer.
#include "Audio/AudioMixer.h"
#include "common/Logging.h"

#include <sstream>

namespace ps3clone
{

    AudioMixer::AudioMixer() = default;

    CellResult AudioMixer::Initialize()
    {
        m_open.fill(false);
        return m_backend.Initialize(AudioBackendType::Null);
    }

    void AudioMixer::Reset()
    {
        m_open.fill(false);
    }

    CellResult AudioMixer::PortOpen(int port, u32 channels, u32 sample_rate)
    {
        if (port < 0 || port >= AudioBackend::kPortCount)
            return CellResult::CELL_EINVAL;
        (void)channels;
        (void)sample_rate;
        m_open[static_cast<size_t>(port)] = true;
        return CellResult::CELL_OK;
    }

    CellResult AudioMixer::PortClose(int port)
    {
        if (port < 0 || port >= AudioBackend::kPortCount)
            return CellResult::CELL_EINVAL;
        m_open[static_cast<size_t>(port)] = false;
        return CellResult::CELL_OK;
    }

    CellResult AudioMixer::PushSample(int port, const std::vector<s16> &pcm)
    {
        if (port < 0 || port >= AudioBackend::kPortCount || !m_open[static_cast<size_t>(port)])
            return CellResult::CELL_EINVAL;
        (void)pcm;
        // TODO: accumulate into the per-port ring buffer.
        return CellResult::CELL_OK;
    }

    CellResult AudioMixer::Mix(std::vector<s16> &out, u32 sample_count)
    {
        out.assign(static_cast<size_t>(sample_count) * AudioBackend::kChannels, 0);
        // TODO: sum the open ports into `out` with saturation.
        return m_backend.Submit(out, sample_count);
    }

    std::string AudioMixer::Describe() const
    {
        int open = 0;
        for (bool p : m_open)
            open += p ? 1 : 0;

        std::ostringstream os;
        os << "cellAudio mixer: " << open << "/" << AudioBackend::kPortCount << " ports open";
        return os.str();
    }

} // namespace ps3clone
