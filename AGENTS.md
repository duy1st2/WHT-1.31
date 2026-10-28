# WHT 1.31

Independent port for the two supplied 1.31.1 builds. Never modify WHT,
WHT-1.29, or either original under E:/Warcraft III.
All game writes and launches must stay inside this project's runtime-test.
Build and test both profiles, back up replaced artifacts, preserve INI/logs/
macro scripts and verify SHA256. Do not replace loaded files or stop user
processes. A test process created by this project may be closed for testing.
Never enable inherited 1.29 gameplay code without a verified 1.31 profile.
Keep the F8 settings menu above every feature overlay. Reserve native frame
levels 7/8/9 for menu background/controls/popups; feature frames must stay at
level 6 or below. Use ToolMenuOverlay::Frame for feature hosts. When a native
constructor creates an owned feature frame directly, set its level through
NativeSetOwnedFrameLevel before adding regions; re-register through Hide/Show.
Do not change stock game frames to implement this priority rule.

# Reference feature scope

User instruction (2026-10-27): for requests to continue the next features,
port only features already implemented in BOTH WHT and WHT-1.29. Read the
actual reference implementations and record source evidence before porting.
Do not invent unrelated utilities or commands. Explicit later user requests
for a different feature take precedence.

User scope update (2026-10-28): if WHT-1.29 lacks an implementation, use
the actual WHT implementation as the reference. Record source evidence;
placeholder-only handlers still do not establish a working feature.
