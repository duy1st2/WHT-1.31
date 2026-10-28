# External HUD port

Reference features read before porting:
- WHT/native/runtime.cpp:2775 and WHT-1.29/native/version129.cpp:730 implement #externalhud, Performance / External HUD.
- Both native/external_hud.hpp implementations publish numeric timer/memory data and require a fresh matching acknowledgement before suppressing native HUD.
- WHT-1.29/native/version129.cpp:1841 reads simulation time on the game thread; its background worker maintains the helper.
- WHT-1.29/native/external_hud_main.cpp renders a transparent, click-through DX11/D2D window; its hidden self-test checks rendered pixels and window styles.
- Reference SHA256 values are in external-hud-reference.json.

1.31 integration:
- Uses existing verified ClockFpsProfile / SimulationSeconds131 for all four executable profiles. No new game offsets or inherited 1.29 game calls.
- Uses the existing authentication worker for helper startup; all game data publication remains on the UI thread.
- Initializes IPC before installing window callbacks, and uses a WHT131-specific mapping namespace.
- Windowed mode only. F8 menu, disabled settings, unauthorized state and missing UI withdraw external display flags.
- Native HUD is suppressed only for acknowledged timer/memory flags. Missing/stale acknowledgement (500 ms) restores it.
- x64 memory text omits the x86 2GB limit.
- Default off; existing INIs are preserved. Command: #externalhud 1 / 0, bare command toggles.
- Helper is distributed, backed up and hash-verified alongside MIX and launcher.

Validation: isolated settings tests, publication/ack/version/wrap contracts, GPU pixel/style self-test, and complete x86/x64 suites. No live Warcraft match test was performed.
