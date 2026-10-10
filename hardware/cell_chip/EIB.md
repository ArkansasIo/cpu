# Cell chip — EIB, MIC and BEI (the interconnect and its two doors)

Everything inside (and everything leaving) the Cell goes through one of three
blocks: the **EIB** bus, the **MIC** to memory, and the **BEI** to I/O.

Software counterparts: `emulator/Cell/EIB/` and `emulator/Cell/Memory/`.

## EIB — Element Interconnect Bus

```
        PPU ----+
        SPU0 ---+                                    +---- SPU4
        SPU1 ---+            +-----------+           +---- SPU5
        SPU2 ---+<---------->|    EIB    |<--------->+---- SPU6
        SPU3 ---+            |  4 rings  |           +---- SPU7 (disabled)
        MIC  ---+            +-----------+           +---- BEI
                    (2 clockwise + 2 counter-clockwise data rings,
                     command tree on top)
```

| Property | Value |
|---|---|
| Structure | 4 data rings (2 CW + 2 CCW) + command tree |
| Internal bandwidth | **96 bytes / cycle** (writing + reading) |
| Outstanding requests | >100 simultaneous DMA transfers |
| Coherency | Memory-coherent across PPU, SPUs, MIC, BEI |

The EIB is why the SPUs can stream data at huge rates: DMA runs concurrently
across all four rings, not through a single shared bus.

## MIC — Memory Interface Controller

Bridges the EIB to main storage.

| Property | Value |
|---|---|
| DRAM type | Rambus **XDR** |
| Channels | 2 XDR (XIO) |
| Access sizes | 1–8, 16, 32, 64, 128 bytes |
| Main RAM | 256 MB (4 × 64 MB XDR chips) |

## BEI — Cell Broadband Engine Interface

Bridges the EIB to the outside world.

| Property | Value |
|---|---|
| Function | address translation, command processing, interrupt control |
| Channels | 2 Rambus **FlexIO** (one non-coherent, one configurable coherent) |
| Attaches to | the **RSX** GPU and the southbridge (I/O) |

Sources: [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)),
[IBM CBE SDK — Architecture overview](https://arcb.csc.ncsu.edu/~mueller/cluster/ps3/SDK3.0/docs/accessibility/sdkpt/cbet_1arch_ovw.html).
