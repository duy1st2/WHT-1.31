# #udmgb selected weapon 1 base damage
Reference source read: WHT/native/gameplay_commands.hpp BaseDamageApi, TrySetBaseDamageApi, TrySetBaseDamage (363-393), #udmgb (1181); WHT-1.29/native/base_damage129.hpp and version129.cpp #udmgb. Both default50, range0..1000000000, read current base damage, compute signed delta, native add mask1, then confirm exact actual value and stable identity. Recorded reference hashes separately; reference projects unchanged.

Independent 1.31 native getters/adders:
12164 x86 3cb6f0 / 3c9a30; 12173 x86 3a9450 / 3a7790.
12164 x64 43d260 / 43b580; 12173 x64 4a4800 / 4a2b20.

Getter validates weapon index<2 then reads attack base array+a4 x86/+e4 x64. Native add uses mask bits1/2 to add signed delta to weapon1/2 respectively; mask1 is reference command. It refreshes owning CUnit through attack+30 x86/+50 x64, checked by reader. x86 RET4 getter and RET8 adder; x64 receiver RCX, delta EDX, mask R8. Full function code guarded on all four profiles. Existing detailed-tooltip profiles identify AUnitAttack vtables and CUnit attack member460/458 x86,570 x64. No old-version offsets enabled.

Armor investigation remains unverified: candidate c034c0/bc1f20 and x64 d704c0/dae0b0 update a property whose refresh checks mana; no #armor command enabled. These candidates are explicitly not used by this feature.

Implementation validates native CUnit/AUnitAttack classes, unit serial, attack ownership backlink, decoded world/local player, and stable current base damage before computing delta in int64. No direct gameplay-field writes. Skips native add for zero delta but still verifies actual value; refuses signed overflow or mismatched native result. No new timer/hook, persistence or automatic repeat. Tests cover all four full native getter/adder captures, altered code refusal, exact signed-delta/mask ABI, defaults and inclusive limits, zero delta, negative delta to zero, overflow, recycled identity, changed attack/current base, native refusal and no-op, malformed args without writes. No live damage changes exercised.

Final validation: x86 14/14 and x64 13/13 CTests passed. Both 12164/12173 runtime-test clients installed for both architectures with backups. All 12 artifacts match built SHA256 and all 6 existing config/macro/ban snapshot files unchanged. Source ZIP refreshed. No user game stopped and no live damage changed.
