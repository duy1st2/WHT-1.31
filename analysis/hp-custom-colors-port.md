# HP bar color command port

Reference evidence:
- WHT/native/runtime.cpp:3279 implements #chp / #chpe and persists HPBar / Color or Color Enemy, then reloads configuration.
- WHT-1.29/native/version129.cpp:1300 implements the same commands and validates exactly eight ARGB hex digits, optional 0x.
- WHT-1.29/native/hp_color129.hpp uses player colors when Color Player is enabled; otherwise normal/enemy configuration. Observer and neutral slots use normal color.
- WHT native overhead implementation currently uses player-color mode; the configured-color visual policy for this port specifically follows the 1.29 implementation. Do not claim WHT's configured-color visual path was independently verified.

1.31 implementation:
- Reuses HpColorProfile131 tint/owner validation and ResourceProfile131 world/player/alliance access for the four supported executable profiles. No new offsets or inherited 1.29 game calls.
- Resolves alliance using the target player's physical id, with explicit self/observer/neutral cases. Missing alliance data retains native paint instead of guessing enemy.
- Normal custom colors apply with Color Player off, like 1.29. Player-color mode takes precedence. Config and mode changes invalidate cached desired colors immediately; owner/alliance changes are sampled every 200 ms.
- Existing paint callbacks reapply tints and preserve original engine paint for restoration. Both x86 StateBars131 and x64 Gameplay64 paths are wired.
- Commands validate a supported native profile before persistence. Isolated tests cover ARGB/alpha, persistence failures, malformed input, ally/enemy/observer/neutral policy, physical ids, unknown alliance and existing texture ownership/restore behavior.

Validation: full x86 4/4 and x64 3/3 suites passed. Live multiplayer display has not been tested.
