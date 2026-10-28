# Cursor and selected-unit flags diagnostics

Reference implementations read:
- WHT/native/gameplay_commands.hpp:1422 implements #uflag, reporting GetCursorPos / ScreenToClient coordinates, plus TryGetUnitFlags from a selected unit. Its helper at :563 reads the agent flags at +0x20.
- WHT-1.29/native/version129.cpp:1144 validates the cursor query; :1455 formats the response via CursorFlagsText129 and optional selected-unit flags.
- WHT-1.29/native/unit_info129.hpp validates selected identity before/after the flags read and formats MouseXY / ClientXY / Unit Flags.
- SHA256 references: cursor-flags-reference.json.

1.31 adaptation:
- Uses the existing verified agent flags field already used by world_present131.hpp and Auto Skills: x86 +0x20, x64 +0x38. This follows the original WHT diagnostics field; the distinct 1.29 +0x164 field is not copied into 1.31.
- BossProfile131 / BossNative131 validate the supported build and unit class. SelectedCursorFlags131 samples a stable DropIdentity131 before/after reading, rejecting changed or recycled selected objects.
- Zero flags are valid. Missing/invalid selection omits the flags suffix rather than inventing a zero value.
- Current game HWND supplies ScreenToClient; screen coordinates remain signed for multi-monitor desktops and cursor positions outside the client.
- Uses existing authorized Local match command dispatch and notification rendering. No writes to unit fields, game state, INI or cursor position.
- Rejects arguments, newline and NUL before querying cursor/selection.

Validation: isolated native-class/field-layout checks for both architectures, zero flags, replaced identity, missing selection, screen/client formatting with negative coordinates, unavailable cursor and malformed input. No live Warcraft display test was performed.
