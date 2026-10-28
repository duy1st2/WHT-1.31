# Native map trade restriction

Both reference implementations take no arguments for #mtrade and call SetMapFlag(0x100,0), then report Trade restriction disabled. Actual reference code was read and SHA256 recorded before source edits in map-trade-native-evidence.json. No resource transfer, alliance change or unrelated command is added.

## Mapping evidence

Live native-name records store the actual function pointer16 bytes before the name on x86 and32 bytes before it on x64. Nearby function pointers in larger scan windows are not sufficient to identify the native. In particular,12173 x86 SetMapFlag is8e89d0, not the nearby8e78a0 candidate from earlier exploratory output. The latter operates on a different flag word and is not used.

| Build / architecture | Actual SetMapFlag | Guarded setter operation | Guarded IsMapFlagSet operation | CWorld setup pointer | Setup flags | CMapSetupWar3 vtable |
| --- | --- | --- | --- | --- | --- | --- |
| 12164 x86 | 91b330 | 91b40a..91b43d | 8e7bd6..8e7bea | +9c | +38 | 1806b6c |
| 12173 x86 | 8e89d0 | 8e8a96..8e8ac9 | 8b6142..8b6156 | +dc | +38 | 17bbb9c |
| 12164 x64 | a321d0 | a322c0..a322f1 | 9fdf07..9fdf19 | +e8 | +60 | 1bb3bc0 |
| 12173 x64 | a7e060 | a7e115..a7e146 | a4912c..a4913e | +d0 | +60 | 1be2b98 |

Ranges are half-open. All four setter operations independently decode the same CWorld global/key as the existing ResourceProfile131, load the setup pointer at the listed offset, and read/OR/store or NOT/AND/store the listed32-bit flag word according to the supplied boolean. There are no other native effects after this operation. Corresponding getter tails test the supplied mask against that same field. x64 setter prefixes move the boolean toR8; no protected prefix or script dispatcher is invoked by this port.

RTTI identifies each listed primary vtable as .?AVCMapSetupWar3@@ with COL offset0. Guard blocks cover the complete decoded setter operation, getter operation and RTTI type name. HIGHLOW relocations are normalized on x86; there are no DIR64 relocations in guarded x64 spans. Exact class identity is required before reading/writing the setup field.

Live read-only evidence (PID6548,12164 x64): map-trade-live-context.json confirms CWorld vtable1bb3d30, setup vtable1bb3bc0, RTTI name CMapSetupWar3 and flags0x300648. The0x100 restriction bit was already clear. map-trade-live-guard.json confirms all three guard blocks match the running module at its actual ASLR base. map-trade-live-bindings-6548.json confirms the actual SetMapFlag/IsMapFlagSet pointers at the native record offsets. No engine call, flag write or game action was made by these inspection scripts.

## Runtime behavior

#mtrade accepts no arguments. Existing Local/settings authorization is retained. The reader checks verified native bytes, registered CWorld identity/local player, exact setup class, alignment, and two stable reads. A second full snapshot must match before any mutation. If0x100 is already clear, the command returns an already-disabled message without writing.

When required, a single InterlockedCompareExchange clears only0x100 in the verified flag word, preserving every other bit. It refuses a changed source word, setup or world. The result is read back and must match the desired complete word in the same world/setup/local-player context before success is reported. Post-write changes report an unconfirmed result; no retry or rollback is attempted. There is no INI setting, new packet, JASS handle, hook or overlay.

Tests use captured native blocks for both builds per architecture, relocation/tamper checks, synthetic typed world/setup objects with real profile offsets, exact one-bit preservation, compare/exchange conflict rejection, wrong-class rejection, malformed args, already-clear behavior, source changes and post-write verification failures. Actual in-game flag mutation and multiplayer synchronization remain untested. Clearing this map flag does not claim to override host-bot or server rules.

This verified operation supersedes the unresolved mapping in map-trade-research.md. Installation hashes, preserved files and test results are recorded in map-trade-verification.json.
