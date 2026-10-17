# Multiplayer audit - 2026-10-17

Scope: source comparison of the active 12164/12173 x86/x64 runtimes against WHT and WHT-1.29. This is a static review plus local contract tests, not a multiplayer certification. No two-peer match, replay checksum comparison, or network disconnect reproduction was performed.

## Finding corrected: process-wide speed scaling

Before this review, native/speed131.hpp hooked QueryPerformanceCounter, timeGetTime and GetTickCount for the entire client. Command authorization (RunMode::Local) identifies the tool mode, not an offline Warcraft session. No verified single-player gate prevented speed changes in multiplayer. Clock scaling can affect network timing as well as simulation; matching older implementations does not establish safety.

Removed those hooks from the 1.31 speed implementation. #speed with no argument or 1 remains an accepted reset/no-op; acceleration is rejected on all four profiles, including single-player, until both session classification and an isolated clock implementation are verified. There is no fallback to the unverified 1.29 game-speed setter. Both dispatchers log the restriction. Existing loaded binaries require restarting the test client to use this correction.

WHT/native/runtime.cpp currently also installs process clock hooks (SpeedHookQPC/SpeedHookTimeGetTime/SpeedHookGetTickCount), and WHT-1.29/native/version129.cpp uses scaled clocks and SetGameSpeed129. WHT/FEATURES.md describes an isolated simulation clock, but that description does not match the inspected runtime source. Neither reference is treated as proof of network safety. The reference projects were not modified.

## Active command paths inspected

| Path | Current implementation | Remaining qualification |
| --- | --- | --- |
| Drop / drop all | quick_drop_network131.hpp submits action 0x13 through the native sender; validates selected unit, ownership, session and object IDs | Manually encoded 39-byte payload and native ABI still need real peer testing |
| Sell / sell all | sell_native131.hpp uses native action 0x13 targeting a merchant; no direct gold/item mutation | Verify merchant range, retries and inventory changes in multiplayer |
| Auto pick | auto_pick_native131.hpp submits native synchronized smart orders (action 0x12), with ownership and item revalidation | Local four-profile confirmation exists; multiplayer remains unverified |
| Cast on Self / target | self_cast131.hpp and target_cast131.hpp activate the native button, cancel targeting, then submit the saved order through the native wrapper | ABI contracts do not prove all custom-map target behaviors |
| Learn / slot hotkeys | learn_slot131.hpp, all_skills131.hpp and slot_remap131.hpp dispatch native UI clicks | Learn-all can submit bounded repeated requests before points update; test under network delay |
| Resource, labels, cooldown, regen, bars, menu, camera | presentation/read paths; the active runtimes do not include inherited gameplay_commands.hpp/services129.hpp | Native frame lifetime and custom-map compatibility can still cause client faults |
| Reveal invisible | reveal131.hpp changes a guarded world-frame display flag, restored on disable | No peer test or checksum evidence; presentation intent is not an absolute safety guarantee |

The inspected active drop/cast/pick/sell paths do not call direct SetUnit/SetPlayerState/UnitRemoveItem JASS mutators. Legacy helper files remain in the source tree; their presence is not evidence that they are used by the shipped runtimes. DropSession131's channel/replay checks are not an offline detector.

## Validation and release boundary

Build and contract logs: analysis/build131-multiplayer-audit.log (runtime + MIX), analysis/build64-multiplayer-audit.log (runtime). Regression checks exercise the actual speed entry point: reset accepted, acceleration/invalid input rejected, multiplier remains 1. No OS clock hook remains in speed131.hpp.

Install only into runtime-test, preserving configuration/logs/macros, backing up replaced artifacts and checking SHA256. Original Warcraft directories, WHT and WHT-1.29 remain untouched.

Before declaring multiplayer support verified, run two real peers on the same map and build. Test baseline first, then each command group independently, including repeated drop/sell/pick, self/target casts, skill learning, selection/death changes, delayed orders, match end and a second match. Repeat for 12164 x86/x64 and 12173 x86/x64; record both client logs and any desync/crash evidence. Cross-build interoperability is a separate test. This review cannot guarantee absence of disconnects or server enforcement.


## 2026-10-17: #speed restored by user request

Supersedes the acceleration restriction above. #speed [1..70] is enabled again on 12164/12173 x86/x64; no argument or 1 restores 1x. Uses the same continuous process-clock scaling policy as the inspected WHT/WHT-1.29 source (QPC, timeGetTime, GetTickCount), with lazy installation, rollback on hook failure and existing match/UI reset. GetTickCount64 remains unscaled for tool scheduling. No unverified 1.29 native game-speed RVA is called: this restores multiplier behavior, not complete implementation parity with its native base-speed setter. Multiplayer safety remains unverified.
