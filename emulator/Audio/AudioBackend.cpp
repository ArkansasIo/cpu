// AudioBackend.cpp - implementation of the host audio output backend.
#include "Audio/AudioBackend.h"
#include "common/Logging.h"

#include <sstream>

namespace ps3clone
{

    AudioBackend::AudioBackend() = default;
    AudioBackend::~AudioBackend() { Shutdown(); }

    CellResult AudioBackend::Initialize(AudioBackendType type)
    {
        m_type = type;
        m_initialized = true;
        m_queued_frames = 0;
        LogInfo("AudioBackend", "initialised (" + Describe() + ")");
        return CellResult::CELL_OK;
    }

    void AudioBackend::Shutdown()
    {
        m_initialized = false;
        m_queued_frames = 0;
    }

    CellResult AudioBackend::Submit(const std::vector<s16> &pcm, u32 sample_count)
    {
        if (!m_initialized)
            return CellResult::CELL_EINVAL;

        // Skeleton: no real device, just account for the queued frames.
        (void)pcm;
        m_queued_frames += sample_count;
        return CellResult::CELL_OK;
    }

    std::string AudioBackend::Describe() const
    {
        std::ostringstream os;
        os << "cellAudio backend";
        switch (m_type)
        {
        case AudioBackendType::Null:
            os << " [null]";
            break;
        case AudioBackendType::WASAPI:
            os << " [WASAPI]";
            break;
        case AudioBackendType::ALSA:
            os << " [ALSA]";
            break;
        case AudioBackendType::CoreAudio:
            os << " [CoreAudio]";
            break;
        case AudioBackendType::SDL:
            os << " [SDL]";
            break;
        }
        os << " " << kSampleRate << " Hz, " << kChannels << "ch, " << kPortCount << " ports";
        return os.str();
    }

} // namespace ps3clone
