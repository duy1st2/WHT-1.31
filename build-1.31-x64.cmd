@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1" -Architecture x64 -WithTests -Install
exit /b %errorlevel%
