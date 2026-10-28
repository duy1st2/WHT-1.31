# Native skill-card investigation ? 2026-10-28

Research only; #ssp is not enabled or advertised as working.
Read actual WHT/native/show_unit_skills.hpp and common.hpp; hashes are in
clear-target-evidence.json. Old callbacks 34fdc0/34fe00 accept unit in ECX and
panel in EDX and enumerate via thiscall 277fe0/278090. A separate gate 28ecf0
also participates. Old panel submenu offset 1bc is not established for 1.31.

Read-only captures identify x86 12164 enum c18630/c186e0 and x86 12173
enum bd6530/bd65e0. They iterate CUnit ability links at 408/40c and call
ability virtual slots 1c0/1c4, comparing order to native default stored at
182f564/17e4444 (=0xd0142). Direct callback callers are b59bc9/b59e7c
and b19a27/b19cf8. The protected callback wrappers use stack arguments
[ebp+8] unit and [ebp+c] order at these calls, unlike the old fastcall ABI.
Do not infer the callback's second input is the old panel pointer. Callback
selection/display gate, submenu semantics and all x64 equivalents still need
independent verification. No address from this note is a production profile.
