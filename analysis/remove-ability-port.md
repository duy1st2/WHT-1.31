# #rabi native ability removal
Read WHT/native/gameplay_commands.hpp #rabi1228 and WHT-1.29 remove_ability129.hpp/version129.cpp1086. Both parse four-character or hex rawcodes, look up selected unit ability, remove natively, refresh UI and confirm absence. Reference hashes recorded; reference projects unchanged.

Verified native removal targets from direct removal branch in already verified native Avul setter:
12164 x86 bb15f0 / UI refresh b17220; 12173 x86 b70aa0 / ad6b10.
12164 x64 d10810 / UI refresh c64ba0; 12173 x64 d4f260 / ca7c30.
Native removal takes CUnit receiver and CAbility pointer, x86RET4. It performs native OnRemove/unlink/member cleanup/refcount lifecycle. Native global refresh takes unit (cdecl x86, RCX x64), resolves current UI and refreshes unit panel; independently matched the 1.29 refresh1f6d00 semantics. Current 1.31 refresh is also reached by verified base-damage/armor native updates.

Existing SellProfile ability lookup used with flags (code,0,1,1,1,0), extending old 1.29 flags with the verified trailing0 of the 1.31 API. Full removal and UI refresh guarded plus exact direct caller target from Avul setter. Native removal body bounds verified by reachable control flow (multiple returns), not first-ret scanning. No 1.29 executable offsets reused.

Implementation parses printable four-byte IDs or exact nonzero eight-digit hex IDs; rejects extra tokens, NUL/newline before native access. Reads selected-unit serial/world/local player twice, plus native lookup ability pointer, in-module vtable and serial bytes. No stale ability dereference after native removal; a new lookup must confirm absence before and after native UI refresh. Built-in ability serial bytes are compared without assuming a registered nonzero agent ID. No direct gameplay-field writes, hooks/timers or persistent config. Tests compare all four full native profiles and direct caller target, check native removal and cdecl refresh ABI, parse/hex edge cases, ability identity bounds, stable dispatch order, already-absent no-op, replaced unit/ability, native refusal/no-op and invalid args without writes. No live abilities removed.

Final validation: x86 18/18 and x64 17/17 CTests passed. Both 12164/12173 runtime-test clients installed for both architectures with backups. All 12 installed artifacts match built SHA256 and all 6 preexisting config/macro/ban snapshot files unchanged. Source ZIP refreshed. No user game stopped and no live ability removed.
