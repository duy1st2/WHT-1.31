# WHT #wide projection override — 2026-10-28

Reference: WHT/native/runtime.cpp PerspectiveHook and the #wide Event/WideScreen
setting path. Source hashes were recorded before edits in wide-native-evidence.json.
WHT-1.29's generic handler toggles unused state; user permits WHT-only execution.

`#wide` toggles, `#wide 1` enables, `#wide 0` disables. Saves the existing
Event/WideScreen setting and confirms the reloaded value. Enabling requires a
verified native hook; failure does not save a newly enabled value. A saved ON
setting applies while an authorized Local match and settings are active.

| Build | Bits | Projection RVA | Sole direct caller RVA |
|---|---|---|---|
| 12164 | 32 | d53360 | f1634f |
| 12173 | 32 | d0e360 | ed532f |
| 12164 | 64 | ef7e50 | 10f6399 |
| 12173 | 64 | f33c40 | 1132df9 |

All addresses are module RVAs. The x86 native function is cdecl with float fov,
aspect, near, far, then float* output; the caller cleans its arguments. Win64
uses XMM0..3 for those floats and the fifth stack argument for output. This
differs from the old thiscall output-first routine and must not reuse its ABI.

Full native bodies (327 bytes x86 / 318 x64), the direct caller instruction
and referenced numeric constants are guarded. x86 HIGHLOW fields are normalized
and rebased; guarded x64 spans have no DIR64 relocation. The bodies validate
fov/aspect/clipping, compute diagonal-FOV conversion using sqrt(1+aspect^2),
half-angle tangent and all sixteen output lanes. Native FOV is in radians.

The hook calls the native routine first, then applies exactly the WHT policy:

    output[0] /= (clientWidth / clientHeight) / (4.0f / 3.0f)

At 4:3 this is unchanged; at 16:9 it multiplies the horizontal coefficient by
0.75. All other lanes, including vertical projection and clipping, remain
native. This is an additional WHT-style horizontal view override; it does not
replace the engine's existing aspect-ratio calculation. Visual parity and
whether users prefer the resulting width require in-game confirmation.

The callback checks finite valid parameters and the expected perspective
matrix shape before writing its own native output buffer. It never caches or
reuses that pointer. Each native recomputation supplies fresh output, so the
effect does not accumulate. OFF and lost authorization stop future correction;
the next native projection computation restores the ordinary matrix.

MinHook installs lazily. Owned entry bytes are captured after enable; each UI
update checks the complete untouched body/constants, the captured entry and
that the entry reaches this hook (direct x86 jump / x64 relay). Unexpected
changes disable correction. No foreign detour is overwritten. An installed
hook remains a pass-through while OFF, avoiding detour removal under rendering.
Active state and the client-size divisor are atomic; the render callback does
not access mutable configuration or game-player objects. UI-thread publication
requires the checked active local-player/world identity, independently of gold,
lumber or food values. UI teardown
clears active correction. No packets, feature frames or menu priority changes.

Validation: x86 47/47 and x64 46/46 contracts. Tests cover parser and persistence
failure, gate behavior, output-last five-argument ABI, WHT arithmetic, preservation
of the other matrix elements, invalid output/input, no accumulation, owned hook
enable/pass-through/cleanup, rebased captured profiles and tamper refusal.
Only the test executable's own function was hooked during automated execution.
The actual game projection function and visual output were not exercised.
Runtime hook installation can still be refused by client memory protections;
the command reports failure rather than merely toggling an unused variable.

Four runtime-test variants are installed with backups and SHA256 verification.
Original WHT/WHT-1.29 and external Warcraft installations remain unchanged.
This supersedes earlier unresolved #wide mapping notes. #findCP remains a fixed
"None detected" response in both references, without an actual detector to port.
