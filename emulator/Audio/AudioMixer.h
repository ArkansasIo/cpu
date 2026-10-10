// AudioMixer.h - the 8-port cellAudio software mixer.
//
// Each of the 8 output ports can be fed by an SPU audio job. The mixer sums the
// ports into a single stereo stream and hands it to the AudioBackend.
#pragma once

#include "common/Types.h"
#include "Audio/AudioBackend.h"

#include <array>
#include <vector>

namespace ps3clone
{

    class AudioMixer
    {
    public:
        AudioMixer();

        CellResult Initialize();
        void Reset();

        // Open/close one of the 8 output ports.
        CellResult PortOpen(int port, u32 channels, u32 sample_rate);
        CellResult PortClose(int port);

        // Push guest PCM into a port's ring buffer.
        CellResult PushSample(int port, const std::vector<s16> &pcm);

        // Sum all open ports into `out` and submit to the backend.
        CellResult Mix(std::vector<s16> &out, u32 sample_count);

        std::string Describe() const;

    private:
        std::array<bool, AudioBackend::kPortCount> m_open{};
        AudioBackend m_backend;
    };

} // namespace ps3clone
