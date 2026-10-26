@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "E:\GPlay\Game\New folder\Warcraft3-124\WHT-1.31\install-runtime.ps1" -BuildDir "./build-wht-x64" -Architecture x64 -Targets "%~dp0."
if errorlevel 1 exit /b 1
"%~dp0x86_64\WHT131Launcher.exe" --client "%~dp0."
pause
