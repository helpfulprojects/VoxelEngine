setlocal

:: Build the solution in Debug mode (x64)
msbuild VoxelEngine.sln /p:Configuration=Debug /p:Platform=x64

:: Change directory to the correct working directory
cd /d ".\MinecraftClone"

:: Run the game executable from the build output
"..\bin\Debug-windows-x86_64\MinecraftClone\MinecraftClone.exe"

endlocal
