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
