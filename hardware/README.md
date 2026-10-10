# hardware/ — the hardware clone layer

Where the *silicon* side of the PS3 is described: what each block on the Cell
die is, what a hardware reimplementation would cost, and the numbers involved.

| Folder | Contents |
|---|---|
| `cell_chip/` | Per-component die description: `PPU.md`, `SPU.md`, `EIB.md`, `RSX.md` |
| `specs/` | `CELL_SPECS.md` — clock, cache, bandwidth, memory |
| `fpga/` | `FPGA_PLAN.md` — what an FPGA clone would require |

## The chip at a glance

```
+---------------- Cell Broadband Engine ----------------+
|   PPU(2th)   +   SPU x8 (7 usable)   +  EIB (4 rings) |
|   MIC -> 256 MB XDR        BEI -> RSX / southbridge   |
+-------------------------------------------------------+
                        |
                 RSX (separate die) + 256 MB GDDR3
```

See `cell_chip/PPU.md`, `cell_chip/SPU.md`, `cell_chip/EIB.md` and
`cell_chip/RSX.md` for each block in detail.

Sources: [Cell (processor) — Wikipedia](https://en.wikipedia.org/wiki/Cell_(processor)),
[PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3).
