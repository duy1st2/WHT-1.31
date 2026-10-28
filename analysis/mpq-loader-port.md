# Custom MPQ loader port

Reference implementations read:
- WHT/native/runtime.cpp:3494 implements #loadmpq, opens a native archive, saves Event/LoadMPQ and lists configured paths for a bare command.
- WHT-1.29/native/version129.cpp:622-665 canonicalizes paths, deduplicates mounted archive handles and starts configured archives once; :1199 persists deduplicated paths.
- WHT-1.29/native/native_ui129.hpp:69 uses custom archive priority 10, with separate priority 100 for embedded tool assets.
- Hash evidence: mpq-loader-reference.json.

Native evidence:
- Reuses the existing validated 1.31 archiveOpen entry points: x86 12164 0x2d7ee0, x86 12173 0x2c1d20, x64 12164 0x3324b0, x64 12173 0x39ff30.
- Original disk executables contain packed code. Disassembly evidence therefore comes from the existing unpacked captured images memory-14740 / 23884 / 6688 / 29928.bin, rather than the disk bytes.
- mpq-open-disasm-*.txt contains disassembly of those captured functions in small analysis-only PE containers. These are not executable test clients and were never launched.
- x86 uses a two-word path span followed by type/flags/priority/parent/boolean; x64 passes the span via the standard aggregate ABI. The disassembly forwards the priority argument to the MPQ opening branch. The existing tool mount uses type 2, flags 0, priority 100, parent 0, boolean false. Custom mount changes only priority to 10, following 1.29.
- All four native entry points have a 32-byte guard checked against the captured profiles before any call. Unknown builds and altered bytes are refused.

Behavior:
- #loadmpq mounts one archive and persists Event/LoadMPQ. Bare command lists configured archives.
- Shares the DLL loader's canonical path parsing; handles quoted/spaced paths and case-insensitive duplicates. Commas/newlines/NUL are rejected.
- Configured archives mount once after the tool's own assets, in an authorized Local match with settings enabled. Opaque providers remain mounted until the game exits.
- A native failure never saves configuration. A persistence failure explicitly reports session-only mounting; retry does not remount.
- Only WHT-1.31 is modified. No Storm.dll imports or 1.29 game addresses are enabled.

Validation: typed ABI test using a stub, actual captured code guards for every profile including mutation rejection, native mount failure policy, path deduplication, startup once, isolated actual INI persistence and save-failure recovery. Native mount execution inside a live Warcraft match, resource lookup from a custom archive, and map precedence remain untested.
