# Start sound

`game_started.wav` is the unmodified asset extracted from the user's ETS.mix,
not newly authored source or an independently licensed sound. Retain the
original asset's rights/permissions when distributing this reconstruction.

Size: 42864 bytes. SHA256:
`47E87C075B5AD4CC5E44C0B14A058B1BE064188AA38841D2E805576481C61B7E`.

Embedded as WAVE resource 101, so runtime needs no loose sound file or ETS.mix.

# Runtime asset inventory

`overlay-files.txt` is the payload manifest for `tl_overlay.mpq`. Rebuild with the project-root `build-tl-overlay.ps1`; it verifies the exact list and every retained file after extraction. The archive keeps 18 runtime assets, including Vietnamese fonts and all menu/HUD textures. Font users share canonical `FRIZQT__.ttf`; duplicate `WHTUI_FRIZQT__.ttf` and redundant AARDV fallback are retired. Loose `WHTMacroRecord.tga` and the unused `native/assets/WHT_IJL15_Original.dll` copy were removed with backups under analysis. The real JPEG forwarding DLL beside the 1.24/1.26 game remains required and must not be deleted.
