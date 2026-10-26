# Resource UI assets

GreyBackground.tga and FRIZQT__.ttf are unchanged source assets extracted from the user's ETS.mix. The archive stores them as WHTUI_GreyBackground.tga and FRIZQT__.ttf. All runtime font references use the canonical FRIZQT__.ttf; fonts_VN.ttf remains for Vietnamese text. The duplicate WHTUI_FRIZQT__.ttf and redundant AARDV.ttf fallback were removed.

Rebuild with build-tl-overlay.ps1. native/assets/overlay-files.txt lists the archive payload. The script backs up the existing MPQ, rebuilds it, extracts every retained entry and verifies SHA256 and the exact list before replacing the archive. Default CMake builds use the included archive.
