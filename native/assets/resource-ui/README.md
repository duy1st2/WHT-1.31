# Resource UI assets

GreyBackground.tga and FRIZQT__.ttf are byte-identical assets extracted from the user's ETS.mix in analysis/assets. No raster editing or font conversion was performed. They are packaged under unique WHTUI_* names in ../tl_overlay.mpq, so mounting the archive does not replace stock Warcraft assets.

Rebuild the archive with tests/build-resource-assets.ps1 and an existing MPQEditor installation. The script requires a new output (preserve an existing archive first) and verifies both embedded files after extraction. The default CMake build uses the included archive, not MPQEditor. If regenerating it changes archive bytes, update the expected archive hash in runtime.cpp after verifying the extracted content.

Backdrop borders and dropdown arrow are stock game paths, not copied from user map archives. Runtime mounts this archive as data through Storm SFileOpenArchive; it never loads ETS.mix as executable code.
