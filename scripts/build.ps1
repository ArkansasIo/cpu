# build.ps1 - configure and build the PS3 clone skeleton.
#
# Usage:
#   ./scripts/build.ps1            # Release build
#   ./scripts/build.ps1 -Debug     # Debug build
param(
    [switch]$Debug
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot

$config = if ($Debug) { "Debug" } else { "Release" }

Write-Host "Configuring ($config)..." -ForegroundColor Cyan
cmake -S $root -B "$root/build"

Write-Host "Building..." -ForegroundColor Cyan
cmake --build "$root/build" --config $config

Write-Host "Done. Binary: $root/build/$config/ps3clone.exe" -ForegroundColor Green
