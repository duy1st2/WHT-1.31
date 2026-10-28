# DLL loader port

Reference evidence read before implementation:
- WHT/native/runtime.cpp:3457 handles #loaddll, loads the library, appends Event / LoadDll, reloads configuration, and lists configured libraries for a bare command.
- WHT-1.29/native/version129.cpp:622 resolves quoted relative paths beside the tool; Is32BitDll129 checks the PE machine/DLL flag; LoadPlugin129 uses LoadLibraryExW with LOAD_WITH_ALTERED_SEARCH_PATH and deduplicates paths.
- WHT-1.29/native/version129.cpp:662 processes configured startup libraries once; :1199 deduplicates persisted paths.
- SHA256 evidence is in dll-loader-reference.json. Only WHT-1.31 was modified.

1.31 behavior:
- Shared loader for both builds and architectures, with no game offsets.
- Startup runs once on the UI thread after an authorized Local session, with settings enabled. Libraries remain loaded for process lifetime as in the reference.
- Explicit #loaddll is dispatched through existing authorized match command handling; its own success/failure notification prevents duplicate generic feedback.
- x86 requires I386 / PE32; x64 requires AMD64 / PE32+. Both require DLL characteristics. Windows performs the full image validation before loading.
- Canonical absolute paths identify loaded modules and persisted entries without changing process current directory. Relative paths use the directory containing WHT.mix.
- Rejects newline/NUL, malformed quote, drive-relative paths and comma paths (the reference list is comma-separated).
- When a load succeeds but persistence fails, the notification explicitly says session-only. Retrying persistence does not load another module.
- Empty command lists configured paths. No plugin is unloaded while the game is running.

Tests load only the newly built benign fixture DLL in a dedicated test process. They cover actual Windows loading, PE rejection, quoted paths/spaces, case-insensitive and relative-path deduplication, isolated actual INI roundtrip, startup once, reentrant load refusal, missing file and failed save recovery. No user DLL or live Warcraft process was used for testing.
