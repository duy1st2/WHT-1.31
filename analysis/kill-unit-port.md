# #kill native CUnit lifecycle
Reference WHT gameplay_commands.hpp TryKillUnit139/handler976 and WHT-1.29 kill_unit129.hpp/version129.cpp1065 read. Both invoke native KillUnit; 1.29 resolves exact CUnit life virtual method with engine zero constant. Source hashes recorded; references unchanged.

Independently verified CUnit virtual slot124 x86 /248 x64 on 1.31. Targets ba33e0/b62a90 x86 and d006e0/d3f310 x64 (12164/12173). Same structural semantics as old268cb0: clamps life to >=engine zero, invokes managed life property setter at unit+1a4 x86/+200 x64, then native unit lifecycle/death processing. Full wrapper guarded with x86 zero-address relocations and vtable ownership; engine zero must remain bitwise+0. Existing native HP reader supplies live-unit preflight. No direct HP/flags writes.

Wrong earlier inferred slot11c/238 is a boolean query, explicitly unused. No 1.29 executable address or unit layout copied.

Implementation checks native managed HP with existing architecture-specific state readers, requires life>.405, stable CUnit class/serial and decoded world/local-player context across two snapshots, and invokes the native method once with guarded engine zero. No pointer access or repeated action after native death dispatch. Success text explicitly reports invocation, not an unverified death outcome. No new timer/hook or INI setting. Tests cover four captured full methods, relocation and virtual slot/zero ownership/tampering, exact CUnit/zero-pointer ABI, changed identity, invalid/dead HP, malformed args, native refusal and no post-death dereference. No live unit killed.

Final validation: x86 17/17 and x64 16/16 CTests passed. Both 12164/12173 runtime-test clients installed for both architectures with backups. All 12 installed artifacts match built SHA256 and all 6 preexisting config/macro/ban snapshot files unchanged. Source ZIP refreshed. No user game stopped and no live unit killed.
