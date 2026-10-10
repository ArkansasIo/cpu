# scripts/ — build & run helpers

| Script | What it does |
|---|---|
| `build.ps1` | Configure + build with CMake (`-Debug` for a debug build) |
| `run.ps1` | Build if needed, then run `ps3clone` with the given arguments |

## Examples (PowerShell)

```powershell
./scripts/build.ps1
./scripts/build.ps1 -Debug

./scripts/run.ps1 --help
./scripts/run.ps1 --list-components
./scripts/run.ps1 --boot-demo
```

See `docs/BUILD_AND_RUN.md` for details.
