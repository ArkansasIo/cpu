# run.ps1 - build (if needed) and run the PS3 clone skeleton.
#
# Usage:
#   ./scripts/run.ps1                      # show --help
#   ./scripts/run.ps1 --list-components    # walk the Cell
#   ./scripts/run.ps1 --boot-demo          # boot the system
param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Args
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$exe = "$root/build/Release/ps3clone.exe"

if (-not (Test-Path $exe)) {
    Write-Host "Binary not found, building first..." -ForegroundColor Yellow
    & "$PSScriptRoot/build.ps1"
}

if (-not $Args -or $Args.Count -eq 0) {
    $Args = @("--help")
}

& $exe @Args
