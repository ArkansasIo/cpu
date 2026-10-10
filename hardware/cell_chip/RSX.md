# The RSX — "Reality Synthesizer" (the GPU)

The RSX is **not** part of the Cell die; it is a separate chip made alongside
it. It is a shader-based GPU that renders what the Cell's command buffers tell
it to.

Software counterpart: `emulator/RSX/`.

## Where it sits

```
     +----------------------+        FlexIO (coherent)      +------------------+
     |   Cell (PPU + SPUs)  |=============================>|       RSX        |
     |   builds GCM buffers |<=============================|   (separate die) |
     +----------------------+         interrupts            +--------+---------+
                                                                     |
                                                             GDDR3 (256 MB)
                                                                     |
                                                              host display out
```

## Specification

| Property | Value |
|---|---|
| Shader model | unified shader architecture (vertex + fragment) |
| Memory | 256 MB GDDR3, separate from the Cell's XDR |
| Host mapping | Vulkan (preferred) / OpenGL / DirectX 12 |
| Command source | GCM command buffers built by a Cell PPU/SPU thread |
| Fixed-function | rasteriser, ZCULL, texture units remain partly fixed-function |

## Role

- Receives **GCM** command buffers (surface, texture, vertex, draw, shader, …).
- Runs RSX shaders, which an emulator must recompile to the host GPU's shaders.
- Presents finished frames to the display.

## Emulation notes

| Concern | RPCS3-style handling |
|---|---|
| Shader translation | recompile RSX shaders → SPIR-V / GLSL |
| Texture uploads | decode swizzled / compressed formats, cache per guest range |
| ZCULL (occlusion) | "precise" for correctness, "approximate/relaxed" for speed |
| Async shaders | compile in the background to avoid frame hitching |

Sources: [PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3),
[RPCS3 default settings](https://wiki.rpcs3.net/index.php?title=Help%3ADefault_Settings).
