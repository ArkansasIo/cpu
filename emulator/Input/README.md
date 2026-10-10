# emulator/Input — pads, camera, Move, wheels, guns, USB/BT

Everything that turns a physical controller into guest-visible input.

| File | Library | What it is |
|---|---|---|
| `PadHandler.h` / `.cpp` | `cellPad` | DualShock 3 / SIXAXIS: sticks, pressure buttons, motion |
| `GemHandler.h` / `.cpp` | `cellGem` | PlayStation Move: sphere tracking + motion |
| `CameraHandler.h` / `.cpp` | `cellCamera` | PlayStation Eye USB camera |
| `GunConHandler.h` / `.cpp` | `cellPad` | GunCon 3 light gun, racing wheels |
| `USBHandler.h` / `.cpp` | `usb` / `bt` | USB HID reports + Bluetooth transport |

Up to **7 pads** at once (`PadHandler::kMaxPads`).

Source: [PS3 Architecture — Copetti](https://www.copetti.org/writings/consoles/playstation-3).
