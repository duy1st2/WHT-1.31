# Hero skill points command port

Reference implementations read:
- WHT/native/gameplay_commands.hpp:1003 handles #sp along with hero attributes, default 500 and accepted range 1..10000, reporting the observed addition.
- WHT-1.29/native/version129.cpp:1053 dispatches #sp; native/hero_skill_points129.hpp validates a stable hero, uses native modification and reads back the actual increase/total.
- Hash evidence: hero-skill-points-reference.json.

Independently verified 1.31 profiles:
| Build / architecture | CUnit modifier wrapper | CAbilityHero implementation |
| --- | --- | --- |
| 12164 x86 | 0xbac3a0 | 0x3f3ee0 |
| 12173 x86 | 0xb6b810 | 0x3d09b0 |
| 12164 x64 | 0xd09c80 | 0x4696b0 |
| 12173 x64 | 0xd48740 | 0x4d03a0 |

Native evidence:
- Used existing unpacked captures memory-14740/23884/6688/29928.bin.
- CUnit wrappers validate uppercase hero rawcode and the illusion flag before loading the hero component (+0x450 / +0x448 x86, +0x558 x64) and tail-calling the hero modifier.
- CAbilityHero implementations clamp the request against available skill capacity, add to the exact existing SkillProfile131 point field (+0x94 x86 / +0xe4 x64), then notify native unit/UI services.
- 12164 x86 implementation disassembly: disasm-14740-3f3ee0.txt, including add [edi+0x94],ecx. x64 implementation: skill-points-impl-12164-x64.txt, including add [rdi+0xe4],ebp. x64 wrapper: skill-points-wrapper-12164-x64.txt. The other build wrappers/targets were independently matched against their captured code and same point-field policy.
- Runtime guards cover the complete 48-byte x86 or 40-byte x64 wrapper, plus the first 40 bytes of the capped hero implementation. These regions contain no absolute relocated pointers. Every guard is tested against its captured executable profile and mutations are refused.
- Typed native return is bool (AL), with a CUnit receiver and one signed integer amount. ABI stubs exercise both x86 thiscall-compatible and x64 argument passing.

Behavior:
- #sp [1..10000], default 500, existing authorized Local match and enabled settings.
- Reuses SkillProfile131 / HeroPoints131. Samples selected unit, identity, hero component and points twice before calling native, and verifies the same unit/identity/component after calling.
- Reports the actual readback addition. Zero increase/cap, native refusal or changed hero are not reported as success. No retries, no direct writes to the point field, no INI changes.

Validation: full captured profile guards, typed native ABI, synthetic valid/invalid unit/hero layouts, selection and recycled-identity changes, amount bounds/default/malformed input, native refusal, partial cap, unchanged points and replaced hero after dispatch. Live hero point changes and multiplayer behavior have not been tested.
