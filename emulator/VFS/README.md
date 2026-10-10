# emulator/VFS — storage

The PS3 presents a fixed set of mount points to games; each is backed by a host
folder or an image file on this machine.

| Mount | Backing | Contents |
|---|---|---|
| `/dev_flash` | read-only folder | firmware (XMB, system modules) |
| `/dev_hdd0` | writable folder | installs, saves, game data |
| `/dev_bdvd` | Blu-ray image / folder | the inserted disc |
| `/dev_usb000` | host folder | USB mass storage |

| File | What it is |
|---|---|
| `VirtualFileSystem.h` / `.cpp` | Mount table + guest→host path translation |

Source: [PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3).
