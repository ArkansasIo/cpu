// AudioBackend.h - host audio output for the emulated Cell audio block.
//
// The guest sees the "cellAudio" module: 8 output ports fed by software mixers
// (usually one SPU per port). This backend receives the mixed 48 kHz PCM stream
// and pushes it to the host (WASAPI / ALSA / CoreAudio / SDL).
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    enum class AudioBackendType
    {
        Null,      // headless / no sound
        WASAPI,    // Windows
        ALSA,      // Linux
        CoreAudio, // macOS
        SDL,
    };

    class AudioBackend
    {
    public:
        static constexpr int kPortCount = 8;
        static constexpr u32 kSampleRate = 48000;
        static constexpr int kChannels = 2;

        AudioBackend();
        ~AudioBackend();

        CellResult Initialize(AudioBackendType type = AudioBackendType::Null);
        void Shutdown();

        // Submit one mixed stereo buffer for playback.
        CellResult Submit(const std::vector<s16> &pcm, u32 sample_count);

        // Number of frames queued but not yet played (for A/V sync).
        u32 QueuedFrames() const { return m_queued_frames; }

        std::string Describe() const;

    private:
        AudioBackendType m_type = AudioBackendType::Null;
        bool m_initialized = false;
        u32 m_queued_frames = 0;
    };

} // namespace ps3clone
