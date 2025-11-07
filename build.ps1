# build.ps1
param(
    [string]$Configuration = "Debug",
    [string]$Platform = "x64"
)

Write-Host "=== Building VoxelEngine.sln ($Configuration | $Platform) ===" -ForegroundColor Cyan

# Build the solution
msbuild "VoxelEngine.sln" /p:Configuration=$Configuration /p:Platform=$Platform
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed." -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "=== Exporting JSON DB with cpt ===" -ForegroundColor Cyan

# Run cpt
cpt -dir "D:\Dev\VoxelEngine\VoxelEngine.sln" `
    -export-jsondb `
    -clang-flags "-Wall -fms-compatibility-version=19.10 -Wmicrosoft -Wno-invalid-token-paste -Wno-unknown-pragmas -Wno-unused-value"

if ($LASTEXITCODE -ne 0) {
    Write-Host "cpt command failed." -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "=== Launching MinecraftClone ===" -ForegroundColor Cyan

# Change to project folder and run the executable
Set-Location ".\MinecraftClone"
Start-Process "..\bin\Debug-windows-x86_64\MinecraftClone\MinecraftClone.exe" -NoNewWindow -Wait

Write-Host "=== Done! ===" -ForegroundColor Green
