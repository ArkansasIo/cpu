# emulator/Audio — cellAudio

The PS3's audio block. Games call the `cellAudio` library to open up to **8 output
ports**; each port is fed by a software mixer (in practice usually one SPU running an
audio job). The mixed 48 kHz stereo PCM is then handed to the host.

| File | What it is |
|---|---|
| `AudioBackend.h` / `AudioBackend.cpp` | Host output backend (null / WASAPI / ALSA / CoreAudio / SDL) |
| `AudioMixer.h` / `AudioMixer.cpp` | The 8-port mixer that sums guest ports into one stream |

Source: [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)).
