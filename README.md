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
