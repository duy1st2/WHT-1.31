# WHT 1.29 only

This is an independent source/build/assets copy for Warcraft 1.29.2.9231 x86.
Work only here for 1.29 changes. Do not edit or deploy to WHT (1.24/1.26).
Build: powershell -NoProfile -ExecutionPolicy Bypass -File build.ps1 -WithTests
Release outputs: build-wht/Release/WHT.mix, mss32.dll, WHT.dll.
After a successful tested build, install into runtime-test/Warcraft3-129 and
E:/Warcraft III/Warcraft3-129 using install-runtime.ps1. Preserve INI, logs and
macro scripts; back up replacements and verify hashes. Do not stop user game
processes or replace loaded files; report pending installations.
