# Pending #mh native port

Actual source evidence recorded before any runtime edits in fog-modifier-reference-evidence.json: WHT/native/gameplay_commands.hpp:1931 and WHT-1.29/native/gameplay_commands.hpp:1733 both create a visible(4) local-player FogModifier covering GetWorldBounds, use shared vision=true/afterShared=false, start it, then stop/destroy it on disable. No-argument form toggles; explicit0 disables, nonzero enables. This is not equivalent to toggling a render checkbox or reporting a boolean without a modifier.

1.29 registration and wrapper analysis is saved in fog-modifier-old-research.log. Verified old entries: GetWorldBounds97ef0, CreateFogModifierRect8e140, FogModifierStart90690, FogModifierStop906b0, DestroyFogModifier8f400. Create resolves actual script player and rectangle, copies four rectangle coordinates, allocates a native CFogModifier through factory8b8e0 and invokes its configuration virtual method. It converts the object to a script handle and releases the local native reference. Start/stop resolve a fog-modifier handle then set/clear bit0x40 in the old native object's flags. These old object offsets and addresses must not be reused in1.31.

Independent RTTI discovery identifies the primary CFogModifier vtables in all supplied1.31 captures:

| Build / architecture | CFogModifier vtable |
| --- | --- |
| 12164 x86 | 18081fc |
| 12173 x86 | 17bd22c |
| 12164 x64 | 1bc8708 |
| 12173 x64 | 1bf76e8 |

fog-modifier-vtables.py records the associated method addresses; fog-modifier-classes.py distinguishes CFogModifier, CFogModifierTimed, CFogMask and unrelated CloudOfFog ability classes. This establishes class identity only; it does not yet verify factory ABI, bounds, start/stop state or destruction/reference ownership.

Required before enabling: resolve live CreateFogModifierRect/GetWorldBounds/Start/Stop/Destroy bindings, verify1.31 allocation/configuration and native lifetime handling, map actual bounds and owner mask, and bind created-object identity to its world so cleanup cannot touch an object from a previous match. Do not call JASS with a raw CPlayer/CUnit pointer or fabricate handles. Do not assume the1.29 flag offset or virtual slot numbers.

fog-modifier-live-bindings.py is prepared to read these exact names from a supplied runtime-test PID/base without executing or modifying anything. No Warcraft III process was present during this audit; the user was asked to open12164 x64 and enter a match. No runtime source, binaries, configuration or loaded process was changed in this audit. #mh remains pending.

## Live follow-up, PID1416

The user opened12164 x64. Read-only heap name/reference scans resolved GetWorldBounds9fa950, CreateFogModifierRect9e3680, FogModifierStart9ea800, FogModifierStop9ea820 and DestroyFogModifier9e63a0. Longer names are stored outside the inline native-name record: following their pointer references finds the actual native pointer immediately adjacent to the record's name pointer. A pointer merely appearing near a pooled string was not accepted as a binding.

CreateFogModifierRect9e3680 resolves a real player handle and a real rectangle handle, copies four floats from the rectangle, performs native class/instance allocation, retains a CFogModifier reference, calls virtual configuration slotc0 with the physical-player mask, state, rectangle, shared/after-shared/start booleans, creates a script handle, and releases its local native reference. The native1.31 x64 modifier flags are at38, independently confirmed by the actual Start/Stop wrapper operations. These are not the old1.29 object offsets.

Full inspected code is retained in fog-modifier-workers.log. Safe allocation/handle ownership and complete teardown on world transitions still require verification before enabling runtime commands. No native function was invoked and no game state was changed during this follow-up. The port is not complete; no new feature was installed in this audit.

## Lifecycle implementation and second x64 profile

native/fog_modifier_lifecycle.hpp now implements the ownership state independently of any native address. It is intentionally not included by version131.cpp/version64.cpp and does not enable #mh. The backend contract requires real native handles, independently verified object class/registry serials, confirmed Start/Stop state, and Live/Gone/Unknown inspection. World identity includes a generation token, not merely reusable pointer addresses. Cleanup may use the same world after a local-player change; creation/activation require the exact player context.

Bounds and modifier references remain owned on partial native failure. Bounds destruction must be confirmed before activation. Failed cleanup blocks new allocation; explicit off requests can retry, while there is no automatic native retry loop. Unknown context or identity never authorizes destruction or silently discards ownership. Confirmed world teardown forgets old handles without calling into the engine. A reentry guard and operation epoch prevent a native callback that ends the world from publishing stale handles or a false active state. Already-on responses require fresh liveness/activation confirmation through the backend.

tests/fog-modifier-lifecycle-tests.cpp exercises partial creation, unconfirmed bounds destruction, unconfirmed activation, failed stop/destroy, already-gone handles, unknown context/identity, generation reuse, player changes, teardown during each allocation/start callback, reentrant requests and later explicit cleanup. The complete existing suites passed43/43 x86 and42/42 x64. Only the new test target was built; all12 installed program artifacts still match the existing build outputs. Runtime binaries were not replaced. See fog-lifecycle-verification.json.

The user opened12173 x64 (PID2232). Read-only binding/name-reference scans resolved GetWorldBounds=a45fe0, CreateFogModifierRect=a2f690, FogModifierStart=a36050, FogModifierStop=a36070, DestroyFogModifier=a32160, RemoveRect=a514a0, GetLocalPlayer=a3def0. Results are in fog-modifier-bindings-12173-x64.json and the per-PID raw scans. The native Stop/Start independently manipulate0x40 in modifier flags+38. RemoveRect and DestroyFogModifier resolve their own typed handle, then dispatch destruction through the native object's virtual slotb8.

The next required evidence is the corresponding x86 native bindings and complete verified native handle backend. The user was asked to open12164 x86. No native handle was allocated or destroyed in the live game during this work, and the runtime command remains disabled until the backend is ready.
