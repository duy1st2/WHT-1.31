@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "E:\GPlay\Game\New folder\Warcraft3-124\WHT-1.31\install-runtime.ps1" -BuildDir "./build-wht" -Architecture x86 -Targets "%~dp0."
if errorlevel 1 exit /b 1
"%~dp0x86\WHT131Launcher.exe" --client "%~dp0."
pause
