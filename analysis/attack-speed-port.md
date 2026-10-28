# #as attack speed multiplier
Reference implementations read: WHT/native/gameplay_commands.hpp TrySetUnitAttackSpeed and #as (1908); WHT-1.29/native/attack_speed129.hpp and version129.cpp #as. Both expose default multiplier2 and clamp0.1..50. Port follows the verified native bounded-property setter and native animation/stat refresh used by 1.29, not the original WHT direct unit-field write. Source hashes recorded separately. No reference project changed.

Independent 1.31 analysis: existing detailed-tooltip profiles identify AUnitAttack vtable, bounded speed property miniB vtable, multiplier field (+1b4 x86/+270 x64), and map minimum/maximum. Vtable first slot identifies native bounded float setter; disassembly matches 1.29 setter70130, including property lower/upper clamps and RET8 x86. x64 equivalent takes property in RCX, real pointer RDX, policy R8=1.

Verified native setter / refresh RVAs:
12164 x86 2fab40 / 3cdeb0; 12173 x86 2df6d0 / 3abbb0.
12164 x64 354060 / 43fc20; 12173 x64 3bf250 / 4a70c0.

All four refresh routines read attack multiplier, clamp with map minimum/maximum, update unit model animation speed, refresh derived stats and set attack dirty flag100. Semantics match reference refresh722c90. Typed refresh(this=attack, unit): RET4 x86; RCX/RDX x64. Entire setter and refresh functions are guarded, with individual x86 absolute relocations. Existing attack member offsets:460/458 x86,570 x64. Property1b0 x86/268 x64; field after pointer holds multiplier. No old-version addresses enabled.

Implementation verifies bounded property vtable and first-slot setter, stable CUnit/attack identity and decoded world/local-player context. Setter and refresh are called on UI thread only; no direct gameplay field writes or new timer/hook. Reads back actual multiplier before refresh and again afterwards. Malformed/nonfinite input is rejected before any native access. Session-only change; no INI setting. Tests cover both complete native functions for all four captures, ASLR relocation/mutation, exact property/real-pointer/policy and attack/unit ABI, defaults/clamping, effective map limit reporting, native refusal/no-op, recycled identity and replaced attack, invalid map limits and zero writes on malformed args. No live game action performed.

Final validation: x86 13/13 and x64 12/12 CTests pass. Both 12164/12173 runtime-test clients installed for both architectures with backups. All 12 artifacts match built SHA256; all 6 preexisting config/macro/ban snapshot files unchanged. Source ZIP refreshed. No user game stopped and no live attack-speed/animation changes tested.
