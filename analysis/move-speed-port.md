# Native #ms port evidence
Read WHT/native/gameplay_commands.hpp SafeSetUnitMoveSpeed and #ms handler (1886), WHT-1.29/native/move_speed129.hpp and version129.cpp #ms handler. Reference command sets selected unit movement speed through native setter; 1.29 defaults 522, clamps 0..2000 and reports actual speed after modification. No reference project changed.

1.29 getter 291a50 -> movement getter 768160; setter 29cf90 -> 769520. Independently matched 1.31 CUnit wrappers and complete movement setter semantics: virtual set(value,1) of base-speed property at component+70 x86/+b0 x64 followed by native movement recalculation. 1.29 setter body structurally identical to x86 1.31 except relative recalculation target; 1.31 getter returns zero if immobile (+80 x86/+d0 x64), otherwise invokes same derived speed getter already validated in detailed stats. Component identity uses existing movement vtable, CUnit member and backlink from courier movement validation.

Verified wrappers/getter/setter implementation RVAs:
12164 x86 ba95b0 / bab220 / 3bd420 / 3be850; movement member454.
12173 x86 b68a40 / b6a690 / 39b260 / 39c670; movement member44c.
12164 x64 d068d0 / d088c0 / 42df70 / 42f810; movement member560.
12173 x64 d453d0 / d47380 / 495710 / 496f30; movement member560.

All entries guard exact complete wrapper bodies and getter/setter implementations. x86 default-zero absolute references relocate individually; x64 RIP-relative operands stay exact. No inferred or old-version executable address enabled. Captured code disassembly retained in analysis/disasm-*.

Implementation uses native setter on selected CUnit, with stable unit serial, movement component vtable/backlink, decoded world and local-player identity. Rejects malformed/nonfinite input before game access. Confirms actual native read after update; reports game result rather than claiming requested speed was accepted exactly. No direct speed/property write, new hook or timer. Tests cover four captured profiles and ASLR relocation, altered guard refusal, typed unit/real-pointer ABI, default/clamping/invalid input, actual-versus-requested reporting, unavailable native/selection, recycled selected identity and replaced movement component. Native multiplayer gameplay changes were not exercised.

Final validation: x86 12/12 and x64 11/11 CTests pass. All four 12164/12173 runtime-test variants installed with backups. All 12 artifacts match built SHA256 and all 6 existing config/macro/ban snapshot files are unchanged. Source ZIP refreshed. No user game stopped; no live game speed changed.
