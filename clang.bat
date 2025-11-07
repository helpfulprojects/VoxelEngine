setlocal
:: Run cpt command in PowerShell to export JSON DB
powershell -Command "cpt -dir 'D:\Dev\VoxelEngine\VoxelEngine.sln' -export-jsondb -clang-flags '-Wall -fms-compatibility-version=19.10 -Wmicrosoft -Wno-invalid-token-paste -Wno-unknown-pragmas -Wno-unused-value'"
endlocal
