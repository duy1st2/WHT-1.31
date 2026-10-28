# All owned-unit abilities

Source evidence recorded before implementation in player-abilities-reference-evidence.json and player-abilities-native-evidence.json: WHT/native/gameplay_commands.hpp:1254 and WHT-1.29/native/gameplay_commands.hpp:1075 enumerate local-player units with GroupEnumUnitsOfPlayer(null filter), then add/remove the requested ability. The port snapshots the same ownership scope and reuses the verified #sabi/#rabi workers. It does not substitute the current selection or render list.

## Registry enumeration mapping

The 1.29 registration resolves GroupEnumUnitsOfPlayer to98220. It resolves player/group/filter, passes the player's physical ID toCGroup::EnumPlayer102540, which invokes the global agent registry iterator3f4ba0 with CUnit type2b773375, callback1028e0 and child-class traversal false. The callback filters owner and populates the group.

In1.31, CWorldFrame's unit-list refresh calls that version's CUnit class getter and global registry iterator, before applying its render-list callback. The port calls the registry iterator directly with its own collecting callback; no CWorldFrame buffer, rendering predicate or callback is changed.

| Build / architecture | Registry iterator | Registry walker | CUnit class getter | Verified caller |
| --- | --- | --- | --- | --- |
| 12164 x86 | d60d80 | d60e30 | ba40e0 | b20a10 |
| 12173 x86 | d1bd30 | d1bdb0 | b63730 | ae0220 |
| 12164 x64 | f08610 | f086f0 | d011a0 | c6ecd0 |
| 12173 x64 | f44400 | f444e0 | d3fe10 | cb1c40 |

Each class getter evaluates to2b773375. The real callers establish the iterator ABI: cdecl(type,callback,context,bool children), children=false. x86 walkers push context then unit, call callback and clean8 bytes; x64 walkers pass CUnit inRCX/context inRDX. A false callback return stops enumeration. Walkers resolve registry IDs and reject deleted agent entries before visiting CUnits. No visibility, camera rectangle, position or owner test occurs before this callback. Owner filtering is applied by the port using the existing independently verified native owner getter and physical local-player ID.

The x86 iterator entry includes protected code; it is mapped through its actual native call site, not guessed from a decompilation or inherited1.29 address. The JASS GroupEnumUnitsOfPlayer wrapper remains protected and is not called. No JASS group, handle allocation or script-context lookup is needed. Full iterator, walker, class getter and caller bytes are guarded, with x86 HIGHLOW relocations normalized. x64 full function ranges follow chained unwind roots; no DIR64 relocation occurs in guarded spans. The native iterator remains engine code; it is not patched or reimplemented. All five12164 x64 blocks matched the running PID6548 at its actual ASLR base (player-units-live-guard.json). No remote engine invocation occurred during this inspection.

## Execution and failure reporting

#addall rawcode adds level1 only when absent; existing ability levels remain unchanged. #raddall rawcode removes the ability when present; absence is already satisfied. Both accept the existing exact four-byte ASCII or0xXXXXXXXX rawcode format, rejecting additional arguments. Local/settings authorization remains required.

Collection makes no ability changes. It validates world/local player and owned CUnit registry identity, limits traversal to65536 visited objects and8192 owned units, rejects duplicate IDs, and fails before mutations if collection aborts or native enumeration yields no callbacks. A verified registry with zero owned units reports0/0. Before the first write, the entire snapshot is validated again. Each update rechecks world/player/owner/registry identity through the existing ability flow and confirms native presence/absence. Changes to ownership, deletion, callback failures or world changes stop the batch and report only the confirmed fraction; the latest native invocation may already have applied. No retry, rollback, delete, new packet or selection change is performed. A batch reentry guard is released on all exits.

This is a snapshot of owned units at collection time; units created later are not added to the running batch. Direct native modifications use the existing Local execution mode. Multiplayer synchronization and actual in-game all-unit mutations have not been tested by the agent.

Tests load full captured code blocks for both builds under each architecture, verify ASLR normalization/tamper rejection, exercise the cdecl iterator/callback ABI with early stop in owned mocks, and cover whole-roster preflight, partial failure counts, changes after native callbacks, duplicates, bounds, invalid args and reentry. Installation/retained-file hashes and test counts are recorded in player-abilities-verification.json.

This completed registry route supersedes the pending-enumeration note in unit-name-port.md. Rendering and protected JASS wrappers remain untouched.
