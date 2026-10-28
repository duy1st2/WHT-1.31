# Selected-unit camera focus port

Reference evidence:
- WHT/native/gameplay_commands.hpp:1363 implements #focus: stable selected-unit position, then native camera pan.
- WHT-1.29/native/version129.cpp:1158 implements the same command, rejecting arguments and missing unit/camera; pan policy comes from native/pan_camera129.hpp.
- Hash evidence: camera-focus-reference.json.

Independently identified 1.31 camera methods:
| Build | x86 RVA | x64 RVA |
| --- | --- | --- |
| 12164 | 0xaebac0 | 0xc34670 |
| 12173 | 0xaaa9c0 | 0xc7af40 |

Native verification:
- Used the existing unpacked captures memory-14740 / 23884 (x86), memory-6688 / 29928 (x64). Camera object RTTI resolves to CCameraWar3 in both architectures.
- Compared the actual 1.29 method at 0x1e1a00 from the existing captured 1.29 image against the 1.31 camera methods: camera-follow state handling, world-frame camera update, XY projection, distance calculation, camera speed limiting, native position interpolation and optional third-axis flag agree structurally.
- x86 consumes x/y/third floats and a final flag on the stack, retains ECX as camera receiver, returns with ret 0x10. x64 retains RCX as camera, saves XMM1/XMM2/XMM3 as x/y/third and reads the fifth flag from the stack. Typed ABI tests cover both forms.
- Evidence disassembly: focus-reference129.txt, focus-camera-candidates.txt, focus-pan-12164-x64.txt, focus-pan-12173-x64.txt. Small derived PE files in analysis were used only as disassembler containers and never executed.
- Each supported native method has a 24-byte entry guard. On x86 this ends before a relocated SEH-handler address, so the guard remains valid with ASLR. All four guards match captured images, and mutation is rejected.

Runtime behavior:
- Existing authorized Local match dispatch, with settings enabled.
- Existing ReadUnitInfo131 validates stable selected-unit identity/position. Current UI owns the camera; the helper checks camera identity before and after position sampling, then validates its native class again before the pan call.
- Finite XY bounds follow 1.29 (+/-65536). Extra arguments, NUL and newline are refused before native calls.
- Calls native pan with third value 0 and flag 0, matching the reference. It does not modify unit state or configuration.
- Command-owned notifications distinguish invalid selection/camera, refused native call and successfully submitted focus.

Validation: all captured guards, native ABI stub, finite/bounded coordinates, replaced/missing camera, missing selection, malformed arguments and native refusal. Live in-match camera movement has not been tested.
