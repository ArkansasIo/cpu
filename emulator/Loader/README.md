# emulator/Loader — SELF / ELF / PKG / PUP

How content enters the system.

| File | Handles | Notes |
|---|---|---|
| `SelfLoader.h` / `.cpp` | `SELF`, plain `ELF` | Executables. SELF = Signed ELF, the retail format |
| `PkgLoader.h` / `.cpp` | `PKG`, `PUP` | Installable packages (games, patches, DLC) and firmware updates |

The SELF loader produces `ElfSegment` entries (vaddr / offset / size / flags) that
the memory subsystem maps into guest RAM; `PkgLoader` extracts files through the VFS.

Sources: [PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3),
[RPCS3](https://github.com/RPCS3/rpcs3).
