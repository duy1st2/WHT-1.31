@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1" -WithTests -Install
if errorlevel 1 (
 echo Build failed.
 pause
 exit /b 1
)
echo Build 1.29 completed.
pause
