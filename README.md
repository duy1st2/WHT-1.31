# WHT 1.31 — bản port đang phát triển

Source độc lập dựa trên WHT / WHT-1.29, dành cho **x86 / x64** Warcraft III 1.31.1.12164 và 1.31.1.12173. Đây chưa phải bản đạt đầy đủ tính năng của WHT-1.29.

## Build và chạy

Chạy `build-1.31.cmd` để build Release, chạy tests và cài vào hai bản sao bên dưới. Yêu cầu Visual Studio C++ Win32, CMake và .NET 10. `WHT.ini` đang có được giữ nguyên; file tool cũ được backup và file mới được kiểm tra SHA-256.

- `runtime-test/Warcraft3-1.31.1.12164/Run-WHT-1.31.cmd`
- `runtime-test/Warcraft3-1.31.1.12173/Run-WHT-1.31.cmd`
- `runtime-test/Warcraft3-1.31.1.12164/Run-WHT-1.31-x64.cmd`
- `runtime-test/Warcraft3-1.31.1.12173/Run-WHT-1.31-x64.cmd`

Đã copy đầy đủ hai client, gồm Data/CASC, x86, x86_64 và các thành phần đi kèm. Tool chạy **x86 / x64**. Không copy WHT.mix vào thư mục gốc của người dùng. Installer và launcher từ chối đường dẫn ngoài runtime-test của project; installer bỏ qua client đang chạy. Đóng client test rồi chạy lại installer nếu cần cập nhật.

1.31 không có Miles/mss32 như 1.29, nên dùng `Run-WHT-1.31.cmd` để cài build hiện tại và nạp DLL. Mở trực tiếp `Warcraft III.exe` không nạp tool. Launcher tự tìm đúng client test đang chạy để attach; nếu WHT đã nạp thì không nạp trùng. Muốn nhận DLL mới cần đóng game rồi chạy lại script. DLL xác minh SHA-256 của EXE và chữ ký các hàm native theo từng build trước khi cài hook luồng UI.

## Phần đã triển khai

- Profile riêng cho 12164 / 12173: CGameUI, CCameraWar3, CFloatVariable và các hàm frame/font/texture native.
- Menu native dùng danh mục tám tab của WHT, ngôn ngữ, phím menu theo cấu hình, cấu hình camera/FogZ, memory và macro. Các mục chưa port được khóa.
- Camera distance / far clipping qua hàm native, không sửa trang mã được game bảo vệ. Camera được tìm bằng RTTI vtable; giá trị được đọc qua getter native vì 1.31 mã hóa dữ liệu này.
- Macro ghi/phát/dừng/sửa; Ctrl+Shift+F9/F10/F12/F11 hoặc phím trong INI.
- Lệnh `#cam`, `#zoom`, `#fogz`, `#tool`, `#menu`, `#commandsettings`, `#full`, `#lock`, `#showmem`, các lệnh macro.
- Helper LoadFileMapWar3/session và updater riêng `toolinfo-1.31`, kiểm tra ProductName `WHT 1.31`, không chọn payload 1.29.

Gameplay cần phiên đăng nhập hợp lệ, mode Local và trận đang mở. `#fogz` ở đây là camera far clipping, chưa phải hệ thống scene fog. Chat command hiện nhận phím Enter/WM_CHAR, chưa có hook trạng thái chat native.

Runtime mount MPQ trong WHT.mix bằng API archive/provider native 1.31 và kiểm tra các tên font/icon/transparency được game giải quyết về đúng `CArchiveMpq` của tool trước khi dựng menu. Không thay icon hoặc texture trong suốt bằng hình stock, không đổi registry. Build cũng trích xuất 20 asset nguyên bản sang `dist/client-assets`; bản sao `WHT-assets` được giữ để kiểm tra/fallback. Font `fonts_VN.ttf` có glyph tiếng Việt; alpha của `Transparent.tga` bằng 0. Không nạp Storm.dll helper vào game; nó chỉ chạy trong chương trình trích xuất riêng.

## Chưa hoàn tất / chưa xác nhận

Chưa port các nhóm resource, t?y ch?nh thanh HP, MTH, time boss, auto skill/item, player/network và các chức năng khác không được mở trong menu; các header `*129` chỉ là tài liệu tham khảo và không được đưa vào runtime 1.31.

Build, helper self-tests và 2/2 CTest đã pass. Nạp DLL, chữ ký profile, callback luồng UI, session, mount MPQ/provider và camera readback đã pass trên cả hai client sao chép. Người dùng đã xác nhận #cam hoạt động trên cả 12164/12173, và button/dropdown/icon hết lem sau sửa asset/backdrop. Font/texture dùng đúng MPQ WHT; cờ visible, EdgeSize/inset và adapter widescreen dùng layout riêng 1.31. Macro, các hành vi chuyển trận và nhóm gameplay còn khóa chưa được đánh dấu pass. Computer-use đọc được cửa sổ nhưng chụp War3 timeout; visual pass ở lượt này dựa trên xác nhận người dùng.

Xem [PORT131.md](PORT131.md) và `analysis/build131-final.log`, `analysis/deployment-*.json`, log `x86/WHT.log` của từng client. Gói build nằm trong `dist/client-x86`; source có thể đóng bằng `package-source.ps1`.


## Ghi ch? x64

Source x64: `native/profile64.hpp`, `version64.cpp`, `gameplay64.hpp`. Profile ???c x?c minh t? memory image/RTTI/disassembly ri?ng cho m?i EXE x64, kh?ng d?ng offset x86. ?? n?i menu/font/texture, camera/#cam/#fogz, cooldown, HP/s?MP/s, HP/Mana values/max, level hero v? n?n gi?n theo s?; macro/menu config/memory d?ng ph?n chung. C?c nh?m ch?a port c?a x86 v?n kh?a ? x64. C?c l?nh c?a s? `#full/#lock` v? updater x64 ch?a n?i; kh?ng t? ?p payload updater x86 l?n x64.

Build x64, ki?m tra ki?n tr?c PE c?a DLL/launcher v? tests con tr?/v?ng ??i frame/geometry pass. Hai b?n x64 ?? ???c c?i v?i backup, preserve INI v? hash, launcher n?p DLL. Menu/MPQ c?a build n?n x64 ?? c? log native pass tr?n 12173; build gameplay m?i v?n c?n ki?m tra trong tr?n tr??c khi ??nh d?u ch?c n?ng hi?n th? pass. Computer-use tr? `window capture timed out` sau retry; kh?ng d?ng ?? ?? suy ra visual pass.

## SellFast parity

Set the SellFast key in the menu, select your hero, hover one inventory item and press the key to sell only that item. Empty or unpawnable slots do not select another item. `#sellslot 1..6` selects an explicit slot. The Sell All icon and `#sellall` retain bulk sale behavior. Both architectures use the native synchronized sale worker. Gameplay confirmation of the shortcut remains pending.


## Multiplayer review (2026-10-17)

See [MULTIPLAYER-AUDIT.md](MULTIPLAYER-AUDIT.md). Process-wide #speed acceleration has been removed: only reset/no-op at 1x is accepted until an offline gate and isolated clock are verified. Earlier #speed 1..70 notes are superseded. Native order paths were reviewed, but real multiplayer validation is still pending; parity with older ports does not guarantee no disconnect.


## 2026-10-17: #speed restored by user request

Supersedes the acceleration restriction above. #speed [1..70] is enabled again on 12164/12173 x86/x64; no argument or 1 restores 1x. Uses the same continuous process-clock scaling policy as the inspected WHT/WHT-1.29 source (QPC, timeGetTime, GetTickCount), with lazy installation, rollback on hook failure and existing match/UI reset. GetTickCount64 remains unscaled for tool scheduling. No unverified 1.29 native game-speed RVA is called: this restores multiplier behavior, not complete implementation parity with its native base-speed setter. Multiplayer safety remains unverified.


## Auto Click Left / Right

In the shortcut tab, assign Auto Click Left and/or Auto Click Right. Press the assigned key once to start, again to stop. Each button toggles independently. Clicking pauses during chat, menus, macros, loss of focus and physical mouse/modifier input; it resumes on return. A new match resets both toggles. HotKeyHero/AutoClickInterval in WHT.ini controls the requested interval (5..1000ms, default10ms); actual cadence follows the existing UI scheduler. Available on both builds and architectures; live confirmation pending.


## Hero F1-F7 shortcuts

Assign Hero F1 through Hero F7 in the shortcut tab. These select hero portraits 1 through 7 using the game's native button callback. Press twice to use the game's normal camera-centering behavior. Empty/hidden slots do nothing; modifier/chat/menu/macro input does not trigger these shortcuts. Both 12164/12173 x86/x64 are implemented; live confirmation pending.


## Map multiboard shortcut

Assign the expand/collapse information-board key in the other-shortcuts tab to toggle the map's native multiboard. Choose a key other than Tab while Show Resource is enabled: its existing Tab shortcut retains priority. Maps without a visible initialized multiboard leave the key unchanged. Implemented on both builds/architectures; in-match confirmation pending.


## Hide Time Of Day UI

In Graphics, enable Hide Time Of Day UI to hide the stock day/night orb. Disable it to restore the previous local visibility. The tool match timer is independent. Available on 12164/12173 x86/x64; in-match visual confirmation pending.


## Hide Console UI

In Graphics, enable Hide Console UI to hide the game's Console frame. Disable it to restore its previous requested visibility. The tool menu uses a separate host. Implemented for both builds/architectures; live appearance remains unconfirmed.


## Hide Black Bar UI

In Graphics, enable Hide Black Bar UI to expand the game's scene viewport using the same anchor policy as WHT-1.29. Disable it to restore the captured layout. Console hiding is a separate setting. Available on both builds/architectures; in-match appearance remains unconfirmed.


## Mouse ToolTip UI

In Graphics, enable Mouse ToolTip UI to move game tooltips near the cursor using WHT-1.29's placement policy. Disabling, opening the tool menu or leaving the client restores the previous anchor when it has not been changed by the map. Both builds/architectures implemented; visual confirmation pending.


## Hide bars on invulnerable units

In Graphics, enable the option to hide HP/Mana bars when Invulnerable. Bars return through the game's native visibility checks when invulnerability ends or the option is disabled. Unit state and engine-requested visibility are unchanged. Both builds/architectures implemented; in-match confirmation pending.


## Player-colored HP bars

In Graphics, enable Color Player to tint HP bars with their owner's player color, using WHT-1.29's palette. Disable it to restore the captured engine tint when it has not been replaced by the game. Available on both builds/architectures; visual confirmation pending.


## HP/Mana text

In Graphics, enable Text Life/Mana Commas to group portrait HP/Mana numbers (e.g.12,345 / 67,890). Text Life/Mana No Hide keeps the numeric label when the game clears it solely for exceeding its width. The two options are independent; disabling restores native formatting/width policy. Both builds/architectures implemented; in-match confirmation pending.


## Stat text commas

In Graphics, enable Text Stats Commas to group native damage, armor and hero attributes (e.g. 12,345 - 67,890). Native bonus colors are preserved. Disable to restore captured native numbers, provided the game has not replaced that label. Both builds and architectures pass contracts; in-match visual confirmation pending.


## Match start sound

Enable Other Hotkeys -> Sound Start Game before entering a match. Plays the embedded start sound once on the first verified simulation clock sample within 0..5 seconds. Enabling mid-match or attaching late stays silent. Both 12164/12173 x86/x64 implemented; audible in-match confirmation pending.


## Graphics runtime repair (2026-10-17)

The current implementation supersedes the earlier font-hook approach: HP/Mana and stat formatting update the current native labels without patching executable pages. On x64 the stat panel scan includes the real 0x620..0x650 fields. Hide Console and mouse tooltip use the final HUD console, not the auxiliary console of the same class. HP player colors and invulnerability visibility are applied through guarded per-instance native stat-bar callbacks so the engine cannot immediately repaint them; all other native methods, RTTI, unit argument and return register are preserved.

Component discovery caches UI member offsets, rejects unreadable/scalar fields without exception-driven probing and throttles missing components. Static overhead font/layout writes happen only when needed. Both builds/architectures pass contracts; 12164 x86 and 12173 x64 have direct in-process text/color/console evidence. Actual perceived FPS and remaining visual combinations still need user confirmation; multiplayer was not tested.

Selection flicker repair (2026-10-17): the mouse-dispatch refresh was insufficient and has been removed. A guarded per-instance CGameUI::Update (slot12) callback now reformats current native HP/Mana and cached stat fonts after the engine updates the UI. It preserves the complete64-entry table, RTTI COL, two float arguments, context pointer and native return register. Discovery remains throttled100ms; the per-update path rechecks only current/cached portrait/stat fonts, without running gameplay commands, broad frame discovery or menu reconstruction. The four exact native profiles are recorded in analysis/ui-refresh-native-profiles.json. User confirmed12173x64 stat text no longer alternates between grouped/ungrouped values during repeated selection.

Remaining overhead blink had a separate policy error: native pooled HP frames detach between render passes. Mana/state/level eligibility no longer depends on that transient attached flag; children keep requested visibility and inherit actual clipping from the native parent. Authorization, per-unit settings and explicit invulnerability filtering still apply. This does not force hidden/offscreen parents to show. Visual confirmation of the latest overhead change is pending. Contracts cover512 native UI ABI/ordering/recursion calls,512 repeated native stat replacements without timer delay, stale selection ownership and overhead eligibility. x86 and x64 tests pass.


## 2026-10-17: Remove Mana bar; move overhead updates into native bar callbacks

Mana bar has been removed at the user's request. The three General menu controls, default INI section, x86/x64 CStatBar allocation/bind/tint paths and runtime logging for tool-created mana bars are gone. Legacy runtime INIs are preserved, but ManaBar settings are ignored and loaded flags are forced off. Native Warcraft HUD mana and numeric MP/s/HP/Mana value features remain available.

The previous parent-visibility-only adjustment did not resolve the user's reported overhead blinking. Timer-based world writes are now replaced by native CStatBar callbacks: Show27 refreshes the currently bound unit before native attachment, and Bind30/Update31 refresh after the native binding/update. The timer only discovers guarded HP roots and prunes caches; it does not keep hiding/showing overhead hosts or repainting HP outside the native bar phase. Cached unit ownership is refreshed at each paint callback, including pooled frames rebound to another unit. The existing exact four native profiles, RTTI, all unmodified virtual methods, unit arguments and native return values are preserved. Re-entry is bounded; no gameplay orders or simulation writes are introduced.

Owned value/level hosts retain requested visibility while the native parent clips them. Show/Hide and level-background geometry are applied only when needed. Mana text values and hero level remain independently configurable. The CGameUI text-formatting callback remains separate, because the user confirmed it stopped grouped/ungrouped stat-text alternation.

Validation: x86 contracts2/2, x64 contracts1/1; menu metadata has235 valid controls and no ManaBar entries. Contracts exercise512 pooled native bind/update transitions, callback ordering and recursion guards, plus512 repeated hidden/visible parent checks without repeated registration. Four runtime-test installs have backups and matching MIX/launcher SHA256 hashes (analysis/graphics-repair-installed-hashes.json). User confirmed on12173x64 that repeated selection no longer makes the HP bar and HP/Mana values blink. Live native logs show the verified Show/Bind/Update callbacks running during the match, with HP/Mana value and level display active. Multiplayer was not tested.

Final native check:12164x86 also executes the native UI and stat-bar callbacks in-match. Direct manager inspection found21 native HP roots, zero nested CStatBar mana frames, and11 owned value/level hosts, all retaining requested visibility (10 detached0100,1 attached0101). Evidence: analysis/native-bar-no-mana-live-12164-x86.json. This checks lifecycle policy without forcing hidden native parents to display. The project-created x86 verification client was closed after the check; no user client or original installation was stopped or modified.


## Quick Chat (2026-10-17)

In Chat Nhanh, enable Quick Chat and assign up to nine shortcut/text pairs. A fresh press sends that line using the native chat editor and its normal Enter handler. Use a shortcut not already assigned to menu, resource, macro, hero or skill/item actions: existing actions retain priority. Existing open chat, tool menus, modifiers, held/repeated keys, macros, loss of focus, disabled settings/session and UI changes prevent dispatch. Text must be valid UTF-8, visible, single-line and at most128 bytes; invalid text is rejected, never truncated. No repeat/automatic chat loop is added.

Both12164/12173 x86/x64 have exact guarded CChatEditBar/CChatEditBox profiles. Tests pass on both architectures, including native-call ordering/ABI, nine bindings, UTF-8 limits and changed-editor cancellation. User confirmed successful in-match message delivery on12173x64 after the actual CChatEditBox profile correction. The removed Mana bar stays removed.


## Repeat messages (2026-10-18)

Added `#repeat <1..10000> <text>` and `#repeat stop`, following WHT-1.29's finite count and 250ms cadence. Delivery reuses the exact guarded 1.31 native Quick Chat editor/Enter path on 12164/12173 x86/x64; no packet construction, simulation mutation, new hooks or timers. Text is one visible UTF-8 line, up to128 bytes; hash commands, malformed/control/multiline input and oversized text are rejected without truncation or replacing an existing queue. Stop clears the queue.

The existing UI scheduler submits at most one message per update. Native refusal retains the count and backs off250ms; late updates never replay missed periods. Typing, tool menu, macros, skill-learning sequence and background focus pause dispatch. Lost authorization, Local mode, command-settings enablement or UI/match replacement clears pending work. Quick Chat's master checkbox remains independent, as in1.29. Owned native chat state is revalidated between calls by the existing sender. No actual chat messages were sent by automated verification.

Usage: enter `#repeat 3 hello` to send hello three times; enter `#repeat stop` to cancel. Both architecture contract suites cover count boundaries, cadence, refusal, gating, completion, cancellation, new match and Vietnamese/UTF-8 input. In-match and multiplayer confirmation of this command remain pending.

Validation: analysis/build131-repeat.log2/2 and build64-repeat.log1/1 passed. All four runtime-test installs completed with backups, preserved INIs/logs/macros and verified MIX/launcher hashes (analysis/repeat-installed-hashes.json). Original installations untouched.


## Time Boss (2026-10-18)

Enabled MTH/TimeBoss and the MTH49Enable/G17Enable master controls on both12164/12173 x86/x64. Enable one master and Time Boss in the MTH tab. Other unported MTH actions remain unavailable. This uses the same TimeBossState rawcode groups and countdown policy as WHT/WHT-1.29: initial493/300/300-second schedule, periodic1007-second boss, observed group death31-second respawn, same-identity revival resets the corresponding timer. Intended for the matching MTH maps; it does not detect map identity or discover arbitrary boss types.

New time_boss131.hpp profiles exact native CUnit tables and rawcode offsets34(x86)/58(x64). Each profile guards CUnit virtual55 and its DWORD rawcode read, plus the corresponding verified hero-level function's high-byte A..Z classification instruction. Captured-image evidence and matching bytes are recorded in analysis/time-boss-native-profiles.json. Existing native object ID pairs distinguish reused pointers; unrelated classes/types are refused. Cached units are rechecked for exact class/code/identity before existing guarded native health readers are called. Invalid/disappeared objects are forgotten without inventing a death. Collection is bounded8192 entries.

Reuses the existing native HP Show/Bind/Update callback and registered HP snapshot, as the reference tracks observed/rendered units. No global unit enumeration, executable patch, new timer/hook, gameplay-state write or outgoing command. Active Time Boss participates in existing bar callback registration even if the other HP overlays are off. Health observations occur once per simulation second; clock reads/display cadence is at most4Hz, using the verified native simulation clock so pausing does not advance countdowns. Unknown clock hides timers. Disabled settings/auth/master/TimeBoss suspend collection and hide display; UI/match replacement clears state. Existing ToolMenuOverlay::UpdateBoss frames use the feature-level factory, preserving menu priority.

Validation: build131-time-boss.log2/2, build64-time-boss.log1/1 passed. Contracts cover exact per-profile object/rawcode layouts, supported groups, death countdown/no repeated death, revival and second death, reused identities, foreign classes, periodic schedule and match reset. Four runtime-test variants installed with backups, preserved INIs/logs/macros and verified MIX/launcher SHA256 (analysis/time-boss-installed-hashes.json). Original installations untouched. Actual MTH-map appearance/death timing and multiplayer testing remain pending.


## Manual screen/chat clear commands (2026-10-18)

Added the WHT-1.29 aliases `#clear` / `#cls` for native screen text and `#clearc` / `#clsc` for the on-screen native chat region. Commands accept no arguments. They are independent of the Delay Clear checkboxes/periods and do not change configuration. Available on12164/12173 x86/x64 using the existing exact MessageProfile131 clear routine/frame guards; no new native address, hook, timer, gameplay mutation or outgoing command.

New ManualClear131 queues at most one request per region for the current CGameUI. The existing UI scheduler waits for both the input typing latch and the exact native CChatEditBar mode to be closed, so command processing before native Enter completion does not clear the active editor. Tool menu/background context pauses dispatch; lost authorization, Local/settings gates, changed UI or a two-second deadline cancels the request. Requests are consumed before native calls, including refusal/re-entry, and never replayed. Unknown native editor/frame identity prevents clearing.

Validation: analysis/build131-manual-clear.log2/2 and build64-manual-clear.log1/1 passed. Contracts cover aliases, scope isolation/coalescing, native typing, context gates, deadline/UI/session cancellation, native refusal, unreadable editor and re-entry without duplicate execution. Existing per-architecture message-frame/clear-native contracts still pass. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 in analysis/manual-clear-installed-hashes.json. Source packaged. Actual in-match visual confirmation and multiplayer testing remain pending; original installations untouched.


## Detailed damage/armor tooltips (2026-10-18)

Enabled Graphics/Detailed Stats Tooltip on12164/12173 x86/x64. Hover Damage to retain the native description and append attacks/sec, attack interval, raw attack-speed bonus and maximum attacks/sec; hover Armor to append numeric movement speed. The existing ASCII AttackSpeedTooltip/MovementSpeedTooltip text policy follows WHT/WHT-1.29. Toggling off restores native body text only while it still equals this feature's output. Ordinary and extended tooltips preserve their original title, body and extra description; the corresponding native tooltip builder recalculates size. No new feature frame or stock frame-level change.

New detailed_tooltip131.hpp and detail_tooltip_profile131.hpp use the existing post-native CGameUI Update callback. No executable patch, font detour, new timer, synthetic input, outgoing order or simulation write. Bodies are bounded1024 bytes including appended details; oversized results and unrelated/newer text are refused without truncation. The native panel's cached strings are read only. Requests never show a hidden tooltip or re-anchor it, so Mouse ToolTip policy remains independent. UI/match replacement forgets stale labels; menu/auth/settings gates suppress the extension and restore an owned visible body.

Profiles independently match CInfoPanelIconDamage/Armor, CAbilityAttack/Move, FloatMini/FloatMiniB and CToolTipWar3/CUberToolTipWar3 in all four captures. Native CAbilityAttack is unit+460/458 on12164/12173x86 and+570 onx64. Attack speed is FloatMiniB+1B0/value1B4(x86),+268/value270(x64); primary/secondary cooldown FloatMini+158/+160(x86) and+1D8/+1E8(x64). Read each property's native vtable before values. Exact native clamp code references the map minimum/maximum globals, rather than assuming0.2/5; both bounds participate in rate calculation. The four supplied captures currently contain approximately0.2/5. Native movement getters are3BD490/39B2D0(x86),42DFE0/495780(x64), called only for the current exact CAbilityMove whose owner back-pointer still matches the selected unit. Missing owner refuses the getter's native global-unit fallback.

Both normal/extended native builders have guarded entry bytes (masked x86 relocated absolute operands) and verified direct callers. Getter prologues, native attack/movement member-load instructions and exact minimum/maximum instruction targets are also guarded. Full evidence/signatures: analysis/detail-tooltip-native-profiles.json. The normal builder's reset/cost/item arguments and extended builder's extra/cost/item arguments match the native Damage/Armor calls. Selection identity is included in text ownership. Sources must match the exact native panel helper and current live CInfoBar member, not a tooltip text keyword or screen coordinate.

Panel discovery runs only while a native tooltip is visible; a complete three-panel cache validates its current member slots/classes/helper links instead of repeatedly scanning CInfoBar. Incomplete/changed discovery is throttled100ms. Text/layout calls occur only when current body differs from the desired body; native changes are reformatted after that native UI update. When disabled with no owned text, no tooltip polling is added.

Validation: analysis/build131-detail-tooltip.log2/2 and build64-detail-tooltip.log1/1 passed. The contract executables additionally loaded the real captured code for both builds in each architecture, relocated the two known x86 clamp operands to the audit buffer and verified every native code guard. Tests cover512 normal/extended ABI calls, primary/secondary periods, map min/max, movement owner/ABI, unchanged-write suppression, toggle restoration, foreign text, changed identity, oversize rejection, live panel-slot cache invalidation and hidden frame refusal. Four runtime-test installs completed with backups, preserved INIs/logs/macros and matching MIX/launcher SHA256 (analysis/detail-tooltip-installed-hashes.json). Source packaged. Actual in-match tooltip appearance and multiplayer testing remain pending; original installations untouched.


## Display-setting command shortcuts (2026-10-18)

Ported the following existing WHT-1.29 command aliases for already available display features on12164/12173 x86/x64. A bare command toggles its checkbox; append0 to disable or1 to enable, e.g. `#timer 1`, `#resource 0`, `#statscommas 1`. Only bare/0/1 syntax is accepted, following1.29. Settings persist in the current client's INI and the tool menu refreshes through the existing config reload. No new native address, timer, hook, gameplay mutation or outgoing action is introduced.

| Commands | Setting |
| --- | --- |
| `#timer`, `#showtimer` | Show Timer |
| `#res`, `#resource` | Show Resource |
| `#showhotkey` | Show Hotkey In Panel |
| `#statstooltip` | Detailed Stats Tooltip |
| `#statscommas` | Text Stats Commas |
| `#hpmpcommas`, `#hpmpnohide` | Text Life/Mana Commas / No Hide |
| `#sound` | Sound Start Game |
| `#showdropall`, `#showautopick`, `#showsellall` | Show corresponding inventory icon |
| `#console`, `#consoleui` | Hide Console UI (1 hides) |
| `#showskills` | Show Ins All Skills icon |
| `#showlevel` | Show Level Hero |
| `#blackbar`, `#mousetooltip` | Hide Black Bar UI / Mouse ToolTip UI |
| `#hpvalue`, `#manavalue` | HP / mana numeric value sub-option, subject to HP/Mana value master |

New view_commands131.hpp uses a shared fixed mapping for both architectures, validates the complete arguments before writing, and reloads only after successful persistence. Existing Local/session/current-match command gates apply. Each target continues to use its own existing runtime policy, restoration and dependencies; the command does not force a disabled parent option. Existing menu/window commands and unported features retain their prior availability. No runtime INI was modified by the implementation or tests.

Validation: analysis/build131-view-commands.log2/2 and build64-view-commands.log1/1 passed. Added isolated temporary-INI integration checks that persist/reload actual config, verify menu agreement and aliases, refuse malformed/multiline/NUL input without writes/reloads, and refuse failed persistence without publishing config. Temporary INI is cleaned up; no user config is used. Existing native profile and feature contracts also pass. Four runtime-test installs completed with backups/preserved INI-logs-macros and matching MIX/launcher SHA256 in analysis/view-commands-installed-hashes.json. Source packaged. Live command-entry/display confirmation remains pending; original installations untouched.


## Auto Click commands (2026-10-18)

Added WHT-1.29-compatible command control for the existing AutoClick131 worker on12164/12173 x86/x64:

| Command | Action |
| --- | --- |
| `#autol`, `#autoclickl` | Toggle repeated left click; append0/1 to disable/enable |
| `#autor`, `#autoclickr` | Toggle repeated right click; append0/1 to disable/enable |
| `#clickdelay 25` | Persist interval in milliseconds, range5..1000 |

Requires Local mode, authorization, an active match and command settings enabled. Left/right remain independent. Toggle state is session-only and resets on UI teardown; only the interval persists as HotKeyHero/AutoClickInterval. Successful changes reset the next-click deadline. Failed persistence and malformed arguments leave cadence/state intact. No new native profile, hook or timer. All dispatch retains the existing scheduler's focus, cursor-inside-client, physical-button/modifier, typing, menu, macro and inventory/skill-worker guards. Late scheduler updates never replay missed clicks; actual cadence remains limited by existing UI updates.

Validation: analysis/build131-auto-click-commands.log2/2 and build64-auto-click-commands.log1/1 passed. Isolated temporary-INI tests verify alias/toggle independence, interval persistence/boundaries, malformed/overflow/NUL refusal, failed-save behavior, blocked dispatch, paired mouse events, no catch-up bursts and teardown cancellation. Tests use a recording callback and send no actual clicks. Four runtime-test installs completed with backups and preserved user settings/logs/macros; MIX/launcher hashes match in analysis/auto-click-commands-installed-hashes.json. Live in-match command confirmation and multiplayer testing remain pending. Original installations untouched.


## Inventory drop commands (2026-10-18)

Added the reference WHT/WHT-1.29 inventory command interface for12164/12173 x86/x64:

| Command | Action |
| --- | --- |
| `#drop`, `#dropall` | Drop carried items from the selected owned living unit |
| `#dropitem <1..6>` | Drop only the item in that one-based inventory slot |
| `#quickdrop [0|1]` | Toggle or explicitly disable/enable Shift + right-click quick drop |

Quick Drop is a persisted Event/Quick Drop Item setting, matching the existing menu checkbox. The drop actions use the existing verified1.31 synchronized DropAllState/SendDropSnapshot131 path, including single-slot drops; they do not enable the inherited1.29 raw native inventory mutation path. No new native address, timer, executable hook, packet format or simulation write.

New drop_commands131.hpp parses complete arguments, captures the actual unit/session/object identities and requested item identities at command entry, then waits for both the tool typing latch and verified native chat mode to close. Focus/menu/macro/skill/inventory worker gates apply; pending requests expire after two seconds and are cleared on lost authorization/settings or UI replacement. A second pending request is refused. Before starting, the selected live owned unit/session is re-read through the existing native DropSnapshot131 guards. Only requested items still held with matching handle AND object identity enter the worker; newly acquired/replaced items are excluded. The request is consumed before worker start, including refusal/re-entry. Auto Pick stops only after a drop worker successfully starts. The existing worker revalidates each outgoing action, observes native inventory confirmation and cancels changed owners.

Show Drop All now governs icon visibility/input; it no longer cancels an active drop worker merely because the icon is hidden, allowing command use with no icon. Normal authorization/settings/focus/typing and worker exclusion gates remain. Idle command processing only checks a pending flag, with no periodic inventory scan.

Validation: analysis/build131-drop-commands.log2/2 and build64-drop-commands.log1/1 passed. Contracts cover full/slot syntax, all six slots, native open/unreadable chat, context pauses, current item filtering, replacement item/unit identity, session/UI/auth cancellation, timeout, empty slot/inventory, failed capture/start, one-shot consumption, worker submission/completion and actual temporary-INI Quick Drop persistence. Automated checks send no real game actions. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 (analysis/drop-commands-installed-hashes.json). Source packaged. In-match and multiplayer validation remain pending. Original installations untouched.


## Quick Drop parity audit against current WHT-1.29 (2026-10-18)

Compared current1.29 quick_drop129.hpp, quick_drop_network129.hpp and version129.cpp input/command paths with1.31 quick_drop131.hpp, quick_drop_network131.hpp and both architecture dispatchers. Shift+right-click already uses a single synchronized action13/orderD0021 with queued flag1 and native unit/item identities; the DropAction serializer matches the current1.29 source exactly.1.31 retains its independently verified native API/layouts and CDataStore buffer ABI. No raw UnitRemoveItem call was enabled.

Fixed missing parity details: block new gestures while the native target-selection cursor is active or console HUD is hidden; enforce foreground-window gating onx86 as already done onx64; validate inventory hit bounds remain within0..0.8/0..0.6 like1.29; retain a consumed right-button gesture through duplicate down/double-click messages until its up event, even when modifiers/context change. The accepted gesture submits at most once before release. Off-inventory input passes through. Existing menu/chat/macros/auth/settings gates and native living/local-owner/slot/session/object-ID revalidation remain.

Scope distinction: this audit aligns the Shift+right-click gesture. The1.31 chat command #dropitem still intentionally waits for native chat closure and uses the previously documented bounded drop worker, whereas1.29 submits its command synchronously. Icon/drop-all worker policy is unchanged by this audit. No claim of byte-identical architecture-specific native adapters or live multiplayer validation.

Validation: analysis/build131-quick-drop-parity.log2/2 and build64-quick-drop-parity.log1/1 passed. Regression contracts cover duplicate down, latched double-click with changed context, one consumed release, canvas bounds, hidden button and classic/widescreen hit tests; existing native sender/session/item contracts continue to pass. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 in analysis/quick-drop-parity-installed-hashes.json. Source packaged. Live gesture/multiplayer confirmation remains pending. Original WHT/WHT-1.29 sources and original Warcraft installations unchanged.


## Inventory execution flow correction (2026-10-18)

Supersedes the previous chat-wait queue and single-slot worker behavior documented in Inventory drop commands/Quick Drop parity audit. User requested reference flows rather than additional1.31 scheduling/retry policies.

Both x86/x64 now use one BeginQuickDrop helper for #dropitem and Shift+right-click. In the current1.29 order, it stops Sell All, defers Auto Pick for2000ms and submits one QuickDropNetwork131 synchronized action immediately. No native-chat wait, pending command queue, timeout, worker scheduling or automatic retry is used for a single item. Native refusal also keeps the reference pickup delay. The existing native session/unit/item/slot identity and ownership guards remain. Auto Pick's enable state and pending target are preserved during the pickup delay; x64 no longer turns Auto Pick off on a quick drop. Both architectures use the same flow helper policy. A hidden/blocked native target cursor, menu, input modifier and gesture-latch gates remain as audited earlier.

Removed DropCommandQueue131 and its runtime tick/reset paths entirely. #drop/#dropall now capture inventory and start the existing batch worker directly; no extra native-chat-close queue is layered ahead of it. The batch worker/DropAction serializer remains source-identical to WHT/native/drop_all.hpp (line endings normalized), including its one queued action per tick,100ms spacing and bounded confirmation/retry policy. This follows that WHT synchronized batch reference; WHT-1.29's separate raw-local UnitRemoveItem batch implementation is not enabled on1.31. Native layouts/ABIs remain independently verified1.31 profiles rather than inherited addresses. These references have distinct implementations and are not represented as identical.

Related outgoing entry-point review: Auto Pick uses the native selected-unit target order builder; Sell All uses the existing guarded synchronized shop action; skill learning uses native button activation; self/target cast retains cancel-before-submit ordering; Quick Chat/Repeat uses the native editor/Enter flow. None of those adapters was replaced or given additional sends by this change. This review is not a proof that every scheduling policy across all versions is identical. Broader Auto Pick/Sell All cadence/selection-policy equivalence and multiplayer validation remain open; no multiplayer disconnect-free guarantee is made. Reference source SHA256 snapshots: analysis/inventory-flow-reference-hashes.json.

Validation: final analysis/build131-inventory-flow-parity.log2/2 and build64-inventory-flow-parity.log1/1 passed. Contracts verify stop-sell/pause/send order, one submission per invocation, no retry on refusal,2000ms delay, retained Auto Pick switch and teardown reset, alongside the existing native ABI/object/session tests. Superseded queue tests were removed. Four runtime-test installs completed with backups and preserved INI/logs/macros; all eight MIX/launcher SHA256 match in analysis/inventory-flow-parity-installed-hashes.json. Source packaged. Original installations and WHT/WHT-1.29 sources unchanged. In-match/multiplayer validation pending.


## All Skills native listener compatibility (2026-10-18)

User reported the All Skills icon does not spend points. Existing12173x86 runtime log shows icon Start succeeds but sequences finish with submitted0. Found a concrete callback-guard incompatibility: SkillClick131 rejected any receiver vtable outside the executable image, including the registered per-instance CGameUI refresh table introduced for anti-flicker presentation.

New SkillListener131 keeps ordinary native receiver checks and permits only the exact registered UI owner with the exact registered proxy table, an original native table inside this executable, an unchanged virtual3 callback equal to that original table, and an invocation target inside the executable image. An unrelated object sharing the proxy, changed callback or invalid original table is refused. Native click entry/event signature guards remain; no callback is replaced, no hook/timer is added and no native address changes. All Skills still uses the existing reference open/learn/back sequence and bounded passes; no direct hero point mutation or new outgoing action format.

Validation: analysis/build131-skills-listener.log2/2 and build64-skills-listener.log1/1 passed. Added regression fixtures for original native receiver, registered UI proxy, foreign owner, altered callback and invalid original table; existing skill-point/open/learn/back/selection/click contracts remain. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 in analysis/skills-listener-installed-hashes.json. Source packaged. This fixes a reproducible guard defect; actual in-match icon learning and multiplayer confirmation remain pending. Original installations untouched.


## Auto Pick toggle retention and Sell All confirmation cadence (2026-10-18)

Auto Pick's manual enable flag is no longer cleared by transient selection/session/native validation failure, hiding its icon, starting Sell All or the Drop All icon. Those events reset/pause only the current worker; the flag remains on until manual toggle or match/UI teardown. Full inventory already returned Waiting; the surrounding dispatcher now also retains enable state after temporary invalid snapshots. Invalid worker reads/submissions back off250ms instead of polling every UI tick. Selection/identity/ownership/life/session guards are unchanged; retaining the toggle never permits an invalid send. Sell/drop workers exclude pickup while active. Existing quick-drop2s delay remains; drop-all completion defers pickup2s and sell completion defers pickup1s as in WHT's inventory worker. Icon visibility is presentation-only. Both architectures share this policy.

Sell All was unnecessarily honoring its350ms/4s refusal/movement deadline even after native inventory confirmed the pending item disappeared. Ported WHT/native/sell_all.hpp's waiting_/confirming_ branch: confirmed disappearance replaces that deadline with25ms for independently updated resource state, then advances to the next original item. While the item is still held, the existing350ms/4s deadline and bounded shop/pass policy remain, so this does not add repeated outgoing orders. Native sender, packet content, shop access/pawnability/object/session checks and one-submit-per-Step limit are unchanged.

Validation: analysis/build131-pick-sell-cadence.log2/2 and build64-pick-sell-cadence.log1/1 passed. Contracts cover repeated full-bag waits, automatic resume after freeing a slot, transient invalid-snapshot backoff/recovery, near-shop confirmation advancing in25ms, far-shop early confirmation bypassing4s, unconfirmed far item still waiting, native sale confirmation/gold accounting, six single-slot cases and existing native sender guards. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 (analysis/pick-sell-cadence-installed-hashes.json). Source packaged. In-match cadence/toggle and multiplayer validation remain pending. Original Warcraft and WHT/WHT-1.29 sources unchanged.


## Auto Pick Goku (2026-10-18)

Enabled MTHKEY/PickGoku and PickGokuDraw on12164/12173 x86/x64. Enable MTH49Enable or G17Enable, then use the assigned Pick Goku key, the Goku toolbar button, or `#goku` / `#autopickgoku`. Bare command toggles; append0/1 to disable/enable, matching1.29. The button uses the first visible reference MTH toolbar position(.305,.142,.038,.024), yellow when off and green when active, built through ToolMenuOverlay::Frame below settings-menu levels and hidden while that menu is open. Other unported MTH actions remain locked.

Only item rawcode I016(0x49303136) is eligible. Mode completes only after reading that type in the selected owned living unit's actual inventory, including the last slot of a full inventory; submission or disappearance from the ground alone does not count. Full bag without Goku waits. MTH master/session/settings loss clears Goku mode; ordinary Auto Pick's manual toggle remains independent. UI teardown resets the feature. Normal Auto Pick and Goku share one existing AutoPick131 worker and native order sender; Goku filtering takes priority while active and ordinary pickup resumes afterward only if its toggle was already on. Existing250ms scan cadence,777 radius, walking timeout/exclusion and sell/drop pause policies remain; no second order stream, new timer or simulation mutation.

New goku_pick131.hpp independently guards CItem virtual37 construction entries and exact prefixes that store the supplied rawcode at+34(x86)/+58(x64).12164/12173 constructors are BD31F0/B92DE0(x86),D39B70/D78510(x64), with the corresponding already verified exact CItem tables. Evidence: analysis/goku-item-code-profiles.json. Runtime code reads only the rawcode after exact CItem identity validation. No constructor is called. Snapshot filters candidates before selection and the type is checked again before the existing native pickup sender. Existing unit/item/session/ownership guards remain. Inventory completion is revalidated against current selection/session/life/ownership before disabling Goku.

Validation: analysis/build131-goku-pick.log2/2 and build64-goku-pick.log1/1 passed. Tests load both captured native images per architecture and verify table/constructor signatures (refusing modified bytes), check rawcode byte order/exact class, actual inventory capacity and full last-slot completion, unrelated item refusal, MTH control availability, button reset/input/double-click behavior, plus all existing Auto Pick native and policy contracts. Tests send no actual game actions. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 (analysis/goku-pick-installed-hashes.json). Source packaged. Actual MTH-map pickup/button appearance and multiplayer validation remain pending; original installations untouched.


## MTH notifications and Goku button command (2026-10-18)

Enabled MTHKEY/Noti for the implemented Goku action on12164/12173 x86/x64. Successful manual On/Off and actual inventory-confirmed Goku acquisition now produce a four-second local in-game message, respecting the checkbox and Vietnamese/English language setting. Opening the settings menu hides the notice without extending its deadline; disabled notifications or lost session/settings/MTH masters clear it. UI teardown forgets owned frames. One reusable frame/font is created lazily via ToolMenuOverlay::Frame, below settings-menu levels. Text changes and visible transitions are applied only when necessary. No chat message is sent, native action callback added, outgoing order changed or simulation state written.

Added `#gokudraw`: bare toggles Show Goku Button;0/off/false disables and1/on/true enables, case-insensitive through the same pure ParseMthDrawValue129 helper as the reference. Only the already available Goku draw setting is exposed; unported MTH crafting/routes remain locked. The command persists MTHKEY/PickGokuDraw and reloads after successful save. Invalid/multiline/NUL/extra input and failed persistence leave settings unpublished. Button display remains subject to the MTH master; hiding it does not change active pickup mode.

Validation: analysis/build131-mth-notice.log2/2 and build64-mth-notice.log1/1 passed. Contracts cover lifetime, menu masking, preference/session cancellation, reset, bilingual text kinds, exact reference boolean aliases, isolated actual INI persistence, malformed input and failed-save behavior. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 in analysis/mth-notice-installed-hashes.json. Source packaged. In-match appearance and multiplayer validation remain pending. Original installations and WHT/WHT-1.29 sources unchanged.


## Selected-unit MTH Chicken/Wolf routes (2026-10-18)

Enabled six route hotkey controls and their count dropdowns in MTH on12164/12173 x86/x64: ChayGa1..3 and ChaySoi1..3,1..20 laps. Commands #chicken1/#chicken2/#chicken3/#wolf1/#wolf2/#wolf3 take no arguments and use the corresponding configured count. Requires MTH49Enable or G17Enable. Select a living owned nonhero and keep it selected; changing selection/identity/session cancels future route orders. This implements the reference1.29 unassigned/selected-unit path. Pinned/assigned courier operation, delivery, base return and route toolbar buttons remain unavailable; the port does not silently change selection or advertise those paths as supported.

New mth_route131.hpp uses the exact six endpoint pairs from Mth129::routes,100ms polling,48-unit arrival threshold and30-second destination timeout. One Move order starts each leg; it is never repeated while walking. A lap decrements only after reaching the second endpoint. New valid requests replace an old route as in1.29. Disabled master/session resets; menu/chat/background/macro/skill/inventory workers pause dispatch. ReadSelfCastSource131 revalidates living local ownership, native session and identity; existing guarded CUnit rawcode/high-byte classification refuses heroes. Identity is re-read immediately before each native order. No raw position writes, new timer/hook or executable patch.

Independently verified native point-order builders emit action0x11, distinct from Auto Pick's action0x12 target-order builder. Entries C5F8E0/C1AA90(x86),DD7D30/E13EF0(x64). Five arguments are order0xD0012, auxiliary0, x/y float pointers, flags0. x86 direct callers B662E6/B26016 clean20 stack bytes; x64 callers CBBE77/CFBE87 use the matching Windows ABI and stack flags. Guards require entry prefix, action0xA0011 initialization, exact native point-action table reference and direct native caller; x86 additionally checks stack cleanup. Evidence: analysis/mth-route-point-profiles.json. Native point functions generate their own synchronized actions; no custom movement packet is constructed.

Validation: analysis/build131-mth-route.log2/2 and build64-mth-route.log1/1 passed. Contracts load both captured images per architecture and validate exact point profiles, refusing altered callers; x86 relocates only the known table operand into the audit buffer. Route tests exercise all six coordinates, two laps/four orders each,100ms cadence, no repeated walking orders, invalid route/count, pause, timeout, changed identity, disabled master, replacement request and native refusal. No actual game movement was sent by automated checks. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 (analysis/mth-route-installed-hashes.json). Source packaged. Actual MTH-map movement and multiplayer testing remain pending. Original installations untouched.


## MTH route toolbar buttons (2026-10-18)

Enabled AutoGaDraw1..3/AutoSoiDraw1..3 menu controls on12164/12173 x86/x64. Six on-screen buttons Ch1..Ch3/Wolf1..Wolf3 call the same existing #chicken/#wolf handlers as assigned keys. No new movement order, packet, timer, native address or route policy is added. The selected-unit requirement remains; assigned/pinned couriers are not enabled by this change.

Added reference #ga1draw/#ga2draw/#ga3draw, #soi1draw/#soi2draw/#soi3draw and group #gadraw/#soidraw. Commands reuse FindMthDrawCommand129/ParseMthDrawValue129 pure settings logic:0/off/false disables,1/on/true enables; bare group disables all if any group member is on, otherwise enables all. Only implemented route display keys are accepted. Unported crafting display commands remain unavailable. Settings persist sequentially as in the reference, with one reload after successful completion; a failed write returns failure without publishing a reload. This is not an atomic multi-key transaction.

Toolbar packs visible Goku/route buttons into the reference6-column grid at(.305+.041*column,.142+.028*row), size(.038,.024). Hiding a button leaves no gap. Generalized the existing Goku button renderer to accept a static title, relocate an existing owned frame when layout changes and avoid unchanged label/geometry writes. Hosts use ToolMenuOverlay::Frame at feature level<=6 and hide while settings is open. Active route is green. Button mouse-down dispatches once; double-click does not repeat the request, and consumed release remains consumed after context changes. Chat/menu/background/macro/auth/settings gates remain.

Validation: analysis/build131-route-buttons.log2/2 and build64-route-buttons.log1/1 passed. Added actual temporary-INI tests for individual/group settings, reference aliases, bare-group behavior, malformed/NUL/extra input and failed save. Layout/input contracts cover seven-button row wrap, hidden-button compaction, logical route dispatch after compaction, double-click/release/context gating and UI reset. Existing native route profile/sequence tests remain passing. Four runtime-test installs completed with backups, preserved INI/logs/macros and matching MIX/launcher SHA256 in analysis/route-buttons-installed-hashes.json. Source packaged. In-match visual/button activation and multiplayer confirmation remain pending. Original installations untouched.

## Courier routes and toolbar alignment correction (2026-10-18)

Supersedes the selected-unit-only restriction above. Chicken/Wolf requests now prefer an explicitly assigned courier, otherwise an eligible selected unit, otherwise search the verified native unit list for an owned, living, moving nonhero with inventory (preferring one carrying items). `#setga` assigns the selected eligible courier. Discovery depends on the native list; explicit assignment remains available when discovery cannot find it. Buildings and immobile units are refused through independently guarded CAbilityMove getters. Pinned routes retain their courier identity while the player selects a hero/building.

Route dispatch uses the independently verified native selection sender (action 0x16) to temporarily select the courier, then the existing native point sender (action 0x11), then restores the full original selection through guaranteed cleanup. Both native selection sets must agree before dispatch. No simulation values or executable code are written. Native profiles, selection ABI and list offsets are verified separately for 12164/12173 x86/x64; evidence is in analysis/courier-selection-profiles.json. Actual multiplayer behavior still requires live testing.

Toolbar frames use one position anchor plus explicit size. Labels are centered using resolved native button bounds and the raw native frame API, matching the reference layout approach and avoiding a second viewport transform.

Validation: analysis/build131-courier-ui-fix.log passed 2/2; build64-courier-ui-fix.log passed 1/1. Contracts cover captured native signatures and modified-signature refusal, selection lists/cycles/capacity, native action arguments, full selection restoration including an injected exception, and resolved button rectangles. All four runtime-test targets were installed with backups and preserved settings; all eight MIX/launcher hashes match the builds (analysis/courier-ui-fix-installed-hashes.json). Original installations and WHT/WHT-1.29 sources untouched. A 12164 x86 test client was reopened; in-match visual and movement confirmation remains pending.

## Reference 1.29 native route queue update (2026-10-19)

Supersedes per-leg polling for Chicken/Wolf. The reference now submits a Stop followed by all configured pairs of Move endpoints into the engine queue. Version 1.31 now follows that dispatch policy on both builds and architectures: one native Stop (D0004, ability 0, flags 1, mode 4), then 2*count native world point calls (D0012, ability/target 0, order flags FFFFFFFF, queue 1), then restores the original selection. No further route position polling, arrival-based movement commands or automatic replay occurs after submission. Temporary selection is restored through SEH cleanup if a native call faults; already submitted game orders cannot be rolled back.

New mth_queue131.hpp verifies complete native wrapper instruction sequences, including direct native action-builder calls and ABI argument/return code. Independently captured RVAs: x86 12164 Stop B66200 / Point B66440; 12173 B25F30 / B26170. x64 12164 CBBD50 / CBC040; 12173 CFBD60 / CFC050. Captured evidence: analysis/mth-queue-profiles.json. No inherited 1.29 address is called.

Courier resolution now prioritizes the eligible selected unit, then the assigned courier, matching current 1.29. An explicitly selected/assigned moving hero-class map courier is accepted. Automatic search first prefers nonheroes, then permits hero-class fallback, preferring carried items and stable lowest identity among equal candidates. Automatic discovery still uses the previously verified 1.31 native CWorldFrame unit list, rather than the 1.29 rectangular query; discovery of units absent from that list is not established. #setga remains available. Original selection restoration preserves the full roster, including a building selection.

Validation: x86 2/2 and x64 1/1 passed in analysis/build131-native-route-queue.log and build64-native-route-queue.log. Captured wrapper checks pass for all four profiles. Native ABI fixtures validate Stop/move flags, twenty laps = forty alternating endpoints, selection restoration after success and exception, malformed count refusal, gated one-shot dispatch and no subsequent polling/replay. Four runtime-test installations completed with backups/settings preservation, and all eight build/installed artifact SHA256 hashes match (analysis/native-route-queue-installed-hashes.json). Actual in-map execution and multiplayer testing remain pending. WHT/WHT-1.29 sources and original Warcraft installations were not modified.
