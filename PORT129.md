# Warcraft III 1.29 port status

Exact supported executable: **1.29.2.9231 (1.29b), x86**. SHA256:
`A1950F17905B9CD7D5461D45E6723AF36DDA5304E12DBA27A1FEA85593B15F3F`.

The independent WHT-1.29 project uses `version129.cpp`. It has its own copied headers, assets and dependencies; WHT (1.24/1.26) is not a build dependency.
It is **not a complete gameplay port**. Do not install the legacy runtime into 1.29.

Implemented in the 1.29 target:

- Version branding and live resolution updates.
- Local `#cam`, `#zoom`, `#fogz`; user confirmed these work in game.
- Native menu bridge: shared eight tabs, input/editing, tooltips and icons.
- Embedded UI MPQ mounted from WHT.mix using the merged archive API.
- Vietnamese/English setting and memory display.
- Resource board: 24 player slots, self/allies/observers, gold/lumber/food,
  left-player status and Tab collapse/expand. Read-only native player calls;
  UTF-8 names use the non-allocating name getter. Native bordered rows and four
  colored columns follow the 1.24 board geometry. Physical neutral slots 24..27
  are excluded, including old-map remapping of slots 12..15. Rendering still
  needs live verification after this change.
- Macro recording/playback/stop/editor, repeat count, mouse and keyboard,
  toolbar buttons, configured hotkeys and Ctrl+Shift+F9/F10/F11/F12 fallbacks.
- Normal command, inventory and shop slot activation using native button callbacks.
  1.29 stores pointers in row/slot arrays; legacy contiguous-button arithmetic is
  not used. Quick Cast and automatic targeting remain disabled.
- Hero F1..F7 bindings invoke CHeroBarButton callbacks, retaining native selection
  and double-click camera behavior. Multiboard binding calls native minimize/
  maximize methods. These additions are statically mapped and mock-tested;
  live behavior still requires verification.
- Quick chat with all nine bindings, UTF-8 text, editing/clearing and native
  chat open/set/submit calls. Native typing state suspends hotkey interception.
- Auto Click Left/Right hotkeys. Playback/recording, chat, open tool menu,
  inactive window and off-client cursor suspend click generation.
- Match clock from the simulation scheduler (speed and pause apply), native FPS,
  Time of Day visibility with restoration of the original visibility.
- Periodic text/chat clearing, typing suspension and `#clear`/`#cls`,
  `#clearc`/`#clsc`; interval changes rearm instead of clearing immediately.
- `#full [0|1]`, `#lock [0|1]`, `#timer [0|1]` / `#showtimer [0|1]`.
- Command-settings hotkey and `#commandsettings`: suspend camera/fog overrides,
  restore the last engine-requested values, then reapply saved settings on resume.
- `#allskills` / `#skills`: native learn-menu clicks with column-first traversal,
  a 20-pass bound, deferred start and selection identity checks before each
  click. Stops on chat/menu/focus loss, macro activity or changed selection.
  Portrait selection is CPortraitButton +248 in this executable. The native HUD icon and its show/hide setting are implemented;
  live learning behavior still requires verification.

Menu, resources and macro integration require live-game verification. Tests
validate the frame layout and calls with mocks, setting persistence and archive
contents; they do not prove rendering or all gameplay behavior on 1.29.

Unported gameplay controls are grey and reject changes in the 1.29 target.
They remain fully available in 1.24/1.26. Pending groups include Quick Cast,
automatic targets, mana/HP/level/cooldown/regen overlays,
MTH/G17/Time Boss, automatic skills, item manager, console/graphics patches,
multiplayer tools. The new services are statically mapped and
mock-tested; live behavior still needs verification on this executable.

`native/services129.hpp` contains the separate button/chat/message/clock profile.
Notable RVAs: message clear 48B7B0, button callback 474BD0, FPS mode 24E810,
FDF show/hide 470020/46D8C0, chat open/text/submit 22E320/481C70/1EE2F0.
Simulation manager D5E9EC -> scheduler +44 -> seconds +40. None of these
changes select the legacy `ActiveGameOffsets()` for the 1.29 runtime.

Native UI profile is in `native/native_ui129.hpp`: frame size 0x128, font 0xc8,
texture 0xec; level 0x88, visible 0x94, layer gates 0xbc, backdrop 0xd0;
Hide/Show vtable offsets 0x68/0x6c. The old profile remains the default.
The archive opener and allocator use cdecl; the allocator adapter preserves the
stdcall contract expected by shared UI code. UI work runs on the game window
thread after an active game UI is available, never on the loader worker.

Build this independent project with `build.ps1 -BuildDir build-wht -WithTests`.
1.29 artifacts are in `build-wht/Release` and the test client is
`runtime-test/Warcraft3-129`. Keep the exact original Miles library as
`WHT_MSS32_Original.dll`; `mss32.dll` forwards its exports and loads WHT.mix.
INI, log and macro script remain external. Back up replaced files and preserve
settings. Never overwrite loaded files while the target game is running.

All Skills now performs native learn/back clicks within one UI update, matching
the 1.24/1.26 sequence so the skill menu is not held open between frames.
It does not invoke local JASS skill mutations. Live-game verification is pending.

Batch 2026-10-07: All Skills HUD icon (normal/pressed texture and bilingual
help), Console UI hide/restore via exact CSimpleConsole virtual methods,
and F10 menu blocking (key and centered classic toolbar mouse region).
Commands: #showskills [0|1], #console / #consoleui [0|1], #menu / #hidemenu [0|1].
Console lookup uses the native SimpleConsole pointer at CGameUI +468 and validates RTTI; prehidden consoles
stay hidden after disabling the setting. The tool menu remains independently
hosted. Gameplay and rendering of this batch still need live verification.

Next batch: start-match sound is embedded as WAVE resource 101 in WHT.mix.
#sound [0|1] persists the setting. The first valid simulation clock for each
active UI consumes the transition; enabling mid-match is silent. Sound is
played only within the first five simulation seconds. This is a UI/clock
observer, not the legacy main-channel event hook; live timing is unverified.
#loaddll <path> and #loadmpq <path> load and persist custom resources, including
quoted paths with spaces. Relative paths resolve beside WHT.mix. Successful
loads are deduplicated in the process. Existing INI lists load once on the native main-menu thread before opening
the map browser. Failures are written to WHT.log; archives stay
mounted for the process lifetime. Third-party DLL compatibility is independent
of these commands and is not verified by the port.

MPQ/DLL fix: custom archives now use priority 10 and flags 0, separate from
embedded WHT assets (priority 100). Stock map archive calls use priority 15/17;
custom assets cannot replace the active map script. The inspected custom.mpq
contains a root war3map.j hash entry, explaining the previous priority collision.
DLLs must be x86 PE DLLs; LoadLibraryExW with LOAD_WITH_ALTERED_SEARCH_PATH
resolves adjacent dependencies. File/format/load failures include Windows errors
in WHT.log. Tests exercise actual system-DLL loading, deduplication, invalid
DLL rejection, custom archive priority and output-handle validation. Live map
creation with custom.mpq still requires verification after restarting Warcraft.

HP/MP text batch: Graphics/Text Life/Mana Commas and Text Life/Mana No Hide
are available in the 1.29 menu. Commands #hpmpcommas [0|1] and #hpmpnohide
[0|1] persist these settings. CSimpleFontString text hook 487A40 is scoped to
CPortraitButton +250/+254 fonts and verified caller return sites 25A0A3/25A299.
Only width-triggered clears (25A0D5/25A2CB) are suppressed by No Hide; other
clears remain intact. Original numeric text and native clears are restored on
disable. Cache identities include UI, selected unit generation and both fonts.
Tests cover grouping, native clear suppression, restoring settings and selection
changes; static signatures/call sites passed. Actual rendering is still unverified.

Stats batch: Graphics/Text Stats Commas and #statscommas [0|1] format native
selected-unit damage, armor and hero Strength/Agility/Intelligence values.
The shared 1.24/1.26 GroupStatsText parser preserves signed bonuses and colors.
Scoped sites: wrapper 2591A0 -> font setter return 259210 with the verified
outer return slot at +218h; damage/armor outer sites 25544A/254D69. Inline
attribute setters return at 255C06/255D25/255E48. Cached text is restored on
disable only while UI/unit generation/font/current text remain consistent.
Static signatures/call sites and mock formatting/restoration contracts passed;
live rendering still requires verification. Legacy runtime/builds are unchanged.

Hotkey labels batch: Event/Show Hotkey In Panel and #showhotkey [0|1]
now display enabled configured command/shop/inventory keys on native 1.29
buttons, with yellow text and two black shadows. Inventory falls back to
N7/N8/N4/N5/N1/N2. Command/shop native default keys remain unmapped; no
legacy +5AC offset or guessed default is used. Labels follow button parents,
clear when disabled and forget destroyed UI generations. Key-selection/name
and command contracts passed; actual in-game rendering remains unverified.
Legacy runtime/builds are unchanged.

Hotkey visibility correction: fonts use stock button layer 2, matching the
legacy overlay, rather than tool host layer 4. Native default command/shop
keys now come from button+194 -> command data+5AC, verified in the stock
keyboard dispatcher at RVA 238F21/238F27/238F2D. Configured enabled bindings
retain priority. No hover condition controls label visibility. Live pending.

Quick Cast batch: enabled configured QuickCastCommand/Inventory/Shop bindings
activate the native button first, then inject one OS mouse down/up pair only
when a new CTargetMode becomes active. Exact profile: UI+1C4 active mode,
UI+250 target instance (constructor store 1E8942), RTTI vtable A927C0;
stock comparison at 1EADC0. Foreground/window, held left button, world cursor
region, macro recording/playback and stale target checks guard injection.
This uses the legacy ModeCast=2 OS input path; native dispatch ModeCast=1
and automatic target/self-target remain unported. ABI/state/region contracts
are mock-tested; actual spell targeting requires live verification.

Mouse tooltip batch (2026-10-08): Graphics/MouseTooltipUI and
#mousetooltip [0|1] follow the cursor using legacy quadrant offsets, scaled
for the 1.29 full-width viewport. Native lazy tooltip UI+1DC, vtable A94FE4
(CToolTipWar3 subclass CUberToolTipWar3); stock anchor initialized at
1EC94B..1EC978 against UI+468. Relative-anchor factory 493910 writes
vtable AC9A2C and parent/point/x/y at +4/+8/+C/+10. Only verified stock
relative anchors are changed. Disable/focus loss restores saved anchors
only if still equal to the tool write; UI generations discard stale pointers.
Position/restore/command contracts pass; actual game rendering is unverified.
Legacy 1.24/1.26 runtime and packages remain unchanged.

Black bar batch: Graphics/BlackBarUI and #blackbar [0|1] extend the scene
viewport by matching the legacy point-6/point-2 offsets. Exact 1.29 init
1E7B35..1E7BA0 anchors UI+3FC scene layout (+BC) to UI layout (+BC).
Both relative anchors must pass type/parent/finite-value checks before any
write. Disable restores saved anchors only when still matching tool writes.
Mock restore and invalid-anchor contracts passed; live rendering pending.
Legacy runtimes/packages remain unchanged.

Individual learn shortcuts: enabled learn/shop/book slot bindings can now
learn a specific hero skill from the normal command card. Native plus/learn/
back callbacks complete in the same input update, rechecking portrait unit
identity before learn/back. No local JASS mutation or repeated learning.
Normal command/inventory bindings retain priority for duplicate keys.
A manually opened learn menu stays open; no-points or invalid-slot cases
do not create clicks. Mock contracts cover one upgrade, unavailable slot,
manual menu and absent plus button. Live behavior is unverified.
Legacy 1.24/1.26 runtime/builds remain unchanged.

Hotkey duplicate correction: retain fonts across empty command-data transitions.
The physical button resolver optionally accepts empty/noninteractive slots;
labels clear text while empty and reuse the same fonts when commands return.
Previously empty slots discarded the font pointers while native parents kept
them alive, causing old/new keys to overlap. Mock regression covers Q -> empty
-> E without additional allocations, and disabling the label option.
Requires a game restart to remove fonts created by the previous runtime.

Cooldown overlay batch: Event/ShowCooldown and #cooldown [0|1] display
remaining cooldown centered on command/inventory/shop buttons with white
text and black shadow. Native CCommandButton query RVA 235650 returns float
via x87; it resolves command data+6D4 and calls the bound ability virtual
+2E0 with data+8 order at 2356D2..2356EC. This preserves native simulation
speed/pause and stock/ability-specific rules; no guessed ability timer offset.
Formatting matches legacy ceil-seconds, subsecond decimal and minutes:seconds.
Native signature and button identity/data gates precede calls. Zero/invalid
values clear the same cached font objects. Tests cover countdown formatting,
completion, disabled setting, invalid floats and command persistence.
Actual in-game rendering/cooldown behavior still requires verification.
Legacy 1.24/1.26 runtime/builds remain unchanged.

Cooldown correction: 235650 / ability virtual +2E0 returns normalized
progress, not seconds (live report: 0.1 -> 0.2). Replaced by remaining timer
query +18: CAgentTimer A6E238/3FC070, Extended A7BAF8/3FC0B0.
Spell layout verified at 74A270: flags +20, timer +D4. Only abilities whose
+2E0 query equals 74A270 use that layout. SellUnit/SellItem stock query
7343C0 maps order D0, flags 9C, timers 31C/1C8, stride1C; checks stock arrays
and waiting flags. Stopped timers clear labels; no progress-to-time guess.
Regression uses decreasing native timer seconds down to zero, active flags
and stopped timer guards. Live rendering and stock behavior remain pending.

Overhead hero level batch (2026-10-08): StateTag/ShowLevelHero and
#showlevel [0|1] display FRIZQT level text and shadow beside the native world
HP bar. Exact CStatBar A95104, kind 5 (world HP construction 1FB1F4), bound
CUnit smart pointer +158, bind virtual +78 / 25B7E0, destructor +8 / 2278E0.
Native GetHeroLevel 92967 tail-calls CUnit level getter 291920; hero rawcode
and illusion flags match native eligibility. Fonts belong to the HP parent
and follow its visibility/position; destructor hook forgets children before
native destruction, preventing stale pointers or overlapping allocations.
Raw native anchors avoid the widescreen UI placement cache. Disabling clears
the cached text; no unit handle allocation or gameplay mutation. This batch
adds level text/shadow; the legacy translucent backing is not implemented.
Mock contracts cover hero/bar-kind filtering, changed level without allocation,
disable/re-enable, zero level, cap, parent destruction and command persistence.
Static signatures/RTTI/virtual slots verified; live rendering remains pending.
Legacy 1.24/1.26 runtime/builds remain unchanged.

Hero level backing correction: added the legacy solid translucent black
texture (ARGB C0000000) before the text/shadow, on the same HP parent/layer.
Native CSimpleFontString width query 486FA0 sizes the backing to text width
+ .002; height .012546875, centered on the number. CSimpleTexture solid
color setter 487BA0; empty/disabled level clears the backing to transparent.
Raw native width/anchors avoid widescreen stretching of world labels.
Mock checks cover width growth, height, disable/re-enable and zero clearing.
Live rendering remains unverified. This supersedes the backing limitation
in the preceding batch; legacy runtimes remain unchanged.

HP dimensions batch: #whp <width> and #hhp <height> persist the shared
Event/whp and Event/hhp settings for 1.29. Only native world HP bars
(CStatBar A95104, kind 5) are changed, after the stock bind callback.
Width/height fields remain +58/+5C; native setters 473B40/473830, raw
coordinates independent of widescreen console scaling. Zero/disabled local
mode restores the latest observed game size only if the current size still
matches the tool write. Map/other-module changes are retained. Destructor
hook removes entries before native frame release. No HP value writes.
Mock contracts cover both dimensions, stock refresh, restoration, external
changes, exclusion of mana bars, destruction and command validation.
Live rendering pending; legacy 1.24/1.26 builds remain unchanged.

### Selected-unit regeneration (2026-10-08)

ShowRegenHP and ShowRegenMana now display HP/s and MP/s beside selected-unit stats. Native CUnit life/mana properties (+1A0/+1C0) resolve tags (+8/+C) through cdecl RVA 707F0; the rate at +7C is verified against current-stat reader 400D80. Invalid or zero rates are hidden; negative rates are red. Labels clear on selection loss, console hiding, and UI recreation. Commands: #regenhp / #regenmp (0 or 1); #regen / #showregen alias HP. This change is limited to the 1.29 build. Automated contracts cover both properties, resolver arguments, negative/invalid rates, and INI commands; visual in-game verification remains pending.

### Overhead HP/mana numbers (2026-10-08)

StateTag Enable, hero/nonhero filters, ShowMaxValues and ShowManaValue now work in the 1.29 target. CStatBar kind 5 binds CUnit at +158; child fonts follow native bar visibility and destruction. Current/maximum values use native stat-property readers 400D80/400E20 after tag resolution validation, avoiding elapsed-time estimates. Values are clamped, floored and filtered for finite positive maxima. Labels clear when disabled or rebound; parent destruction removes cached entries. Native anchors bypass widescreen HUD scaling. Gameplay rendering still needs live verification.

### Native overhead mana bars (2026-10-08)

ManaBar Enable and hero/nonhero filters now use the 1.29 native CStatBar constructor 2218D0 (parent, CUnit, kind=0; ret 12). The constructor stores the safe unit reference at +158 and the kind at +13C. Bind 25B7E0 kind 0 calls native mana fraction 235A00 and updates the status fraction through vtable +70. Child bars follow their HP parent's visibility and destruction. Rebinding refreshes the safe unit reference; disabled/invalid/no-mana states clear binding and hide. Native texture +138 uses vertex color setter 486360. Raw anchors place mana below HP, outside widescreen HUD placement caching. #wmp/#hmp accept 0..100; zero follows current HP dimensions. Tests cover eligibility/fog visibility guards and dimension command persistence; actual rendering remains unverified in game.

### Player-colored overhead HP (2026-10-08)

Graphics / Color Player and #colorplayer / #colorp (0 or 1) color world HP textures using the twelve-color palette of the legacy tool. Extended colors 12+ retain native coloring. Native GetOwningPlayer 94410 calls object getter 292340, which returns a player object; GetPlayerColor 94540 reads +2C4 after player-handle resolution. HP foreground is +138, and native texture setter 486360 stores A/B/G/R at +6C..+6F. Only CStatBar kind 5 is modified. Original colors are remembered/restored; external map/native color changes become the new baseline, and replaced textures reset the lease. Native parent destruction forgets caches. Tests cover palette, repeated writes, toggle restoration, external changes, replacement textures and command persistence. Live rendering remains pending.

### Hide invulnerable overhead bars (2026-10-08)

Graphics / BarInvul and #barinvul (0 or 1) suppress world CStatBar kind 5 for CUnit +20 bit 8. Native widget flag setter 273300 sets/clears that bit; unit damage paths 1FB365/1FE8C7 test it. A signature/vtable-guarded hook on Show 489110 suppresses actual rendering through Hide 488960. Native Hide changes actual +98 while leaving requested +94 intact; Show checks requested +94 and parent actual +98. Disabling or loss of invulnerability restores via native Show without overwriting engine visibility. Mana/level/value children inherit hidden parent visibility. Native destruction forgets suppressed bars. Tests cover toggle restoration, unchanged requested visibility, fog hiding during suppression, vulnerability changes, destruction and command validation. Live verification remains pending.

### Remove scene fog effect (2026-10-08)

#nfe [0|1] persists Event / Remove Fog Effect. Native SetTerrainFogEx AA870 calls scene setter 216ED0 with style/start/end/density/color (thiscall, ret 20). Scene parameters are +3E4; slots +C, count +8. Setter writes style property 1, start 3, end 4, density 2 and color 0. Captured map values and copied color restore on disable; scene/parameter/table/slot identity guards reject stale records. Masked tool writes do not replace captured map values. Native real updates refresh the cache while masked, and #fogz distance writes are suspended while NFE is active. No gameplay fog-of-war changes are made. Tests cover snapshot copying, no-op repeat toggles, restore, invalid floats and replaced parameter slots. Live rendering remains pending.

NFE follow-up: default scene fog can exist without a scene setter call after hook installation, and loading-state resets can discard an earlier capture. ReadCurrent now seeds the cache from native parameter getter 56C440 (cdecl parameters/index/type/output); the getter evaluates/copies 12 bytes. Slots validate types 1/5/6/6/6 and writable flags before native reads. New tests cover a scene with no previous setter event, exact restore and rejected type mismatch. This feature controls terrain distance fog; unexplored/hidden fog-of-war regions are separate.
`#nfe` follows the original ETS.mix scene-fog hook: style=1, start/end/density=0, original color preserved. The experimental Gfx NoFog cache approach has been removed. Existing scene parameters are read for mid-match enable; disable restores the captured map settings. Visual parity remains unverified.

### Keyboard and mouse camera scroll (2026-10-08)

#kscroll and #mscroll accept integer 0..700 and persist Event/Keyboard Scroll Speed and Event/Mouse Scroll Speed. Exact 1.29 raw setters 1E3A90 and 1E3B00 write camera +4F0 (keyboard) and +4EC (mouse), verified against camera serialization and movement consumers. Unlike legacy normalized setters, these take the stored speed directly: percent/100; zero maps to the engine default of 1. Command-settings suspension and local-mode disable restore the last observed native speed only while the current value still matches the tool write. Camera identity changes/teardown clear leases. Signature checks guard setter availability. Tests cover independent axes, speed changes, restoration, external writers, invalid range and teardown. Visual/input behavior in-game remains unverified. #nfe work is paused at user request.

### Scoped unit vertex alpha (2026-10-08)

Graphics/VertexColor and #vertexcolor [0|1] now work in the 1.29 target. Exact CUnit vtable A9C504 +B4 points to UpdateColor 26DCA0, whose normal color path reads packed ARGB at unit +4E0. The guarded hook temporarily sets alpha at +4E3 to 255, forwards the native callback once, preserves its return value, and restores alpha if the unit/vtable and temporary value still match. RGB and newer native writers remain untouched; no unit pointer is retained between calls. This matches the scoped 1.24/1.26 implementation. Signature and vtable checks gate the menu control. Tests cover enabled/disabled callbacks, return value, copied RGB, restoration, another class, external writes and command persistence/validation. Actual in-game rendering remains unverified. Legacy runtimes are unchanged.

### Extended player colors (2026-10-08)

Player-colored overhead HP and resource player names now share the same palette,
including indices 12..15 (maroon, navy, turquoise, violet), copied from the legacy
resource palette. HP still restores original colors when disabled; left-player
names remain gray and unmapped indices keep native HP / white resource names.
Tests cover extended colors, invalid indices and left-player precedence. Colors
16..23 retain the existing fallback; this batch does not claim their support.
Live rendering remains pending. Only WHT-1.29 is changed.

### Complete classic 24-color palette (2026-10-08)

Added indices 16..23 to the shared HP/resource palette: wheat, peach, mint,
lavender, coal, snow, emerald and peanut. This supersedes the 16..23 limitation
above. Reference: https://www.hiveworkshop.com/threads/color-code-for-players-12.304942/
Tests cover every added color, gray left-player precedence, the last resource
color, and rejecting index 24 without changing the output value. Existing
restoration/texture-lease contracts remain in place. Live rendering is pending.
Only the independent WHT-1.29 project is changed.

### Smart Cast on key release

ModeCast=1 arms a newly activated native targeting mode from a quick-cast
command/inventory/shop binding and casts at the cursor only on that key's
release. Selection unit/identity, targeting mode, focus, chat, tool menu and
macro activity gate the release; invalidated pending actions are forgotten.
ModeCast=0 and 2 retain immediate cursor casting. #mc / #modecast accept 0..2
or cycle without an argument, persisting Event/ModeCast. Tests cover release
exactly once, unrelated key release, changed selection/identity/mode, blocked
input and command validation. Live behavior still requires verification.

### Enemy mana bar color

Native overhead mana now honors ManaBar/Color Enemy for non-allied owners and
ManaBar/Color for self/allies. Uses existing mapped CUnit owner getter 292340
and player/local/alliance calls; owner slot and resolved object must agree.
Invalid/neutral ownership and local observers retain the normal color. No
visibility rules change. Color is recalculated on each bind/update, including
selection/ownership/alliance/config changes. Tests cover self, ally, enemy,
neutral/out-of-range and mismatched owner resolution, and custom colors.
Live rendering is pending; only WHT-1.29 is changed.

### Configured HP colors and color commands

With Graphics/Color Player off, overhead HP uses HPBar/Color for self/allies
and HPBar/Color Enemy for enemies, using the validated ownership/alliance
reader already used by mana. With Color Player on, the 24-player palette
takes precedence. Local-mode suspension restores the observed native color
through the existing texture lease, retaining newer external writes.
#chp/#chpe/#cmp/#cmpe persist normal/enemy HP/mana ARGB colors: exactly
eight hex digits, optionally prefixed 0x; invalid/trailing input is rejected.
Tests cover both policies, player precedence, existing restoration contracts
and command persistence/validation. Live rendering still needs verification.

### Allow Local Files preference

#allowlocal / #alf / #localfiles accept 0/1 or toggle. They save Event/Allow
Local Files and update HKCU/Software/Blizzard Entertainment/Warcraft III/
Allow Local Files as REG_DWORD. Restart Warcraft to apply its archive search
policy; no unverified executable patch or legacy offsets are used. The registry is changed only when a user runs one of these commands; startup
does not overwrite the shared Windows preference from the INI. Registry failure rolls back the INI
value and reports failure. Tests inject a writer (never modify the real
registry) and cover aliases, toggle, invalid/trailing input and rollback.
This registry preference is shared by Warcraft installations for the same
Windows user; enabling local overrides may affect other game versions too.
Source/binaries for WHT 1.24/1.26 remain unchanged.

### Casting mode menu

Other Hotkeys now has a named Quick Cast / Smart Cast / Quick Cast Old selector,
bilingual help and INI persistence. Existing indices stay unchanged. Per-slot
Quick Cast must be enabled. Tests cover labels, menu persistence and invalid
choices. Live layout verification is pending. Only WHT-1.29 changes.

### Overhead HP/Mana font size

StateTag/FontSize now uses the legacy 0.0015 scale (1..20, default 4), rather
than fixed .008. Existing fonts reflow without allocation when the size changes;
HP and mana offsets follow the font size. Other Hotkeys includes a bilingual
numeric control. Tests cover bounds, persistence, default/NaN scaling and
reflow once per change. Live rendering remains pending.

### Independent HP numbers and overhead grouping

Other Hotkeys adds StateTag/ShowHpValue, dependent on StateTag/Enable.
#hpvalue and #manavalue accept 0/1 or toggle their independent overhead
number flags. Graphics/Text Life/Mana Commas now also groups overhead
current/maximum digits before applying color tags, including single values.
Existing fonts update in place. Tests cover boundaries, clamping, invalid
floats, independent settings and command validation. Live rendering pending.

### Fast Menu transition wait

Mapped native transition RVA 1893F0: six stack arguments, plain RET and x87
float return (cdecl). Reads glue root through 15CBE0 (global D3C8F4), frame
slots +1A0/+1A4 and model getter 478190. A signature-gated hook always calls
the original once, retaining callback/context; positive finite durations
above .001 become .001 when enabled in local mode. #fastmenu 0/1/toggle and
a bilingual Other Hotkeys checkbox persist Event/Fast Menu. Native model
animation rates are not changed in this batch; visual transition parity
with the legacy speed-100 model patches remains pending. Tests cover original
argument forwarding, disabled/zero/invalid durations and command persistence.

Fast Menu correction: animate both glue menu models at speed 100, following 1.29 frame +17C/+180 and CModel +20 -> renderer handle +98 -> speed +48 (native getters 563BC0/51C8E0/40E360). Restore captured speed when disabled, without overwriting an external speed change. Preserve original transition callback setup. Live animation still requires game verification.

### Window and match-start preferences in menu
Other Hotkeys adds Borderless, Lock Cursor and Sound Start Game checkboxes with Vietnamese/English labels and help. They persist the same INI keys as #full, #lock and #sound, using the existing window service and match-start observer. Sound enabled mid-match stays silent; focus loss releases clipping. Tests cover menu availability and toggling/persistence. Live layout verification remains pending.

Graphics menu adds keyboard/mouse camera scrolling numeric controls (0..700%, default 100), persisting existing keys and applying mapped scroll setters on save. Fixed Vietnamese encoding for the three previous window/sound controls. Live layout remains unverified.

### Time Boss first 1.29 implementation
New runtime service enables MTH/G17 Time Boss: first boss at 493 seconds, then every 1007 simulation seconds; Human/Undead initial countdown 300 seconds and respawn 31 seconds after observed death, matching legacy TimeBossState. Uses the native simulation clock so pause/speed apply. Tracks group units discovered by the native overhead-bar bind hook, validates class/identity/life on each second, and drops invalid/reused pointers. This is not the full legacy world-unit scan: units never discovered by a bar are not observed. Menu mode flags are exclusive; other MTH crafting/routes remain unavailable. Tests cover countdown/death deduplication/identity replacement and menu availability. Live timing/rendering verification pending.

Time Boss layout correction: user screenshot shows the running 276-second timer in the bottom-left corner. Move all three timer frames below the memory row, add borders and Boss/Undead/Human labels, format mm:ss and reconcile native visibility every update. Countdown rules unchanged. Live position requires verification.

### MTH49 Time Boss world coordinates
Replace HUD timers with permanent native text tags at the centers read from Map Tong Hop v 49.0: gg_rct_Home (2928,304), udg_rect49 Undead (-3008,-5312), udg_rect48 Human (3008,4800). Terrain height plus 80; tags project through the engine camera, display mm:ss, hide at zero and destroy when disabled. Native registry identifies CreateTextTag 8EAC0, SetTextTagPos AAAF0, visibility AACF0, permanent AAAC0, color AA9F0; direct manager text setter EE020 avoids manufacturing JASS strings. World manager is resolved at world+1C through 77710. Tags belong to each world, identities reset between matches. These coordinates are verified for MTH49; G17 coordinate parity remains unverified. Tests cover placement, reuse, hiding and destruction; live camera behavior pending.

World Time Boss readability adjustment: read-only live inspection found all three tags allocated at the expected map positions with font objects and visible flags. Boss terrain -128 plus height 80 gave absolute Z -48. Increase height above terrain to 200, size from .023 to .035 and include Boss/Undead/Human labels. This improves readability; does not establish the root cause of the user-reported missing rendering. Live visibility remains pending.

Time Boss display recovery: restored the previously visible native frame timers and moved Boss to the central map area, Undead/Human below on the left/right. Native text tags are disabled. These are screen positions, not camera-tracking world positions; world attachment remains unverified.

Time Boss rebuilt from WHT: removed experimental boss_world129.hpp and native text-tag APIs/tests. Copied legacy TimeBossState unchanged; restored plain yellow seconds, original .016 font and no timer backdrops. Kept adjusted screen locations. Native clock and validated 1.29 unit reads adapt the WHT loop. Unit discovery still comes from overhead binding, not a verified full-world unit scan; no claim of world-coordinate attachment or complete respawn parity.

Time Boss placement: moved existing visible timer fonts to the three user-marked minimap positions (Boss middle-right, Human upper-right, Undead lower-left). Changed only frame rectangles; countdown/read/visibility unchanged. Positions normalized from the 1600x900 client reference image.

Time Boss minimap readability: font .016 -> .012, move all three frames upward .005 UI units, and set timer frame level 9 (maximum supported bucket) instead of 8 to draw above minimap. Live occlusion verification pending.

Fast Menu correction: executable 1893F0 selects glue frames at root+19C+kind*4 (kinds 0..2), while kind 4 selects 1A0/1A4. Process all three frames, not only the last two. Apply speed before original transition so native duration/callback setup sees the accelerated model, then reconcile after animation setup. Configuration confirmed Fast Menu=1/local. Live transition verification pending.

Fast Menu actual animation source: native CModel update 562B80 reads model+188 and forwards it to 51CF10; initialization 562835 does the same. Previous renderer+48 writes were overwritten by animation updates. Lease model+188 instead, preserving original speed on disable. Getter 563BF0 confirms offset. Tests updated to model layout; live verification pending.

FPS limit port: #fps / #fpslimit accepts 0 or 60..1000, persists Graphics/Fps Limit. Maps exact cdecl native context 5DF90, getter 43FAA0 and setter 440230 with signature gates; follows maxfps callback 2ADCA millisecond interval truncation. Applies on game thread, restores original interval at zero/disabled only while still owned. Tests cover interval/reuse/restoration/external changes/context replacement. This sets the native scheduler cap; does not port the separate unlock-FPS patch. Live FPS verification pending.

Local command notifications: port legacy NotificationText colors (yellow messages, green ON/value, red OFF). Signature-gated CGameUI timed-text method 1F4860 mapped through DisplayTimedTextToPlayer 8FBE0; raw UTF-8 text avoids JASS strings/network chat. Successful tool/menu commands emit 4-second feedback; camera/fog commands report values. Removed duplicate NFE chat feedback. Unknown/failed commands are not announced as success. Native rendering remains pending.

Unlock Limit FPS: maps legacy flag-mask patch to 1.29 RVA445852 (AND EAX,FB -> FF) with full surrounding instruction signature, ownership/conflict guard and restoration. Adds Graphics checkbox and #unlockfps 0/1/toggle. #fps positive enables unlock automatically; disabling unlock restores owned native interval. Keeps timer placement/legacy builds unchanged. Live FPS behavior pending.

Unlock FPS scheduler correction: with unlock enabled and no explicit cap, set native frame interval to zero rather than leaving engine default unchanged. Request/release 1ms Windows timer resolution through legacy FpsTimerState for accurate small intervals. Log successful native interval readback, patch and timer status. Explicit caps retain 1000/FPS interval; disable restores captured interval. Live 60FPS report not yet resolved/verified; renderer or driver synchronization may still cap presentation.

Unlock FPS presentation follow-up: live log confirms requested144/native interval6ms/patch1/timer1 while display remains60. D3D9 parameter builder 3DBA60 zero-initializes the 0x38 struct; fullscreen explicitly writes PresentationInterval+34=DEFAULT, windowed retains zero. Signature-gated builder hook changes only field13 to IMMEDIATE when local UnlockFPS enabled and VSync off. Runs on native device creation/reset; changing setting with an already-created device requires recreation/restart, not a forced reset. Tests preserve resolution/disabled path. Hardware presentation verification pending; user restart required.

FPS limit correction: preserve scheduler skip-wait flag only for unlimited (Fps Limit=0). Positive caps restore native flag clearing while retaining Direct3D9 IMMEDIATE. Round whole-millisecond interval up (144 -> 7 ms) to avoid exceeding requested cap. Contract checks cover bounded/unlimited transitions; live verification pending.

User requested unlimited-only behavior: removed #fps/#fpslimit command handlers. Unlock FPS always uses skip-wait patch and zero scheduler interval, ignoring saved Fps Limit values. Direct3D9 IMMEDIATE remains enabled.

Auto Click command parity: #autol/#autoclickl and #autor/#autoclickr accept 0/1 or toggle, sharing existing hotkey state. #clickdelay 5..1000 persists HotKeyHero/AutoClickInterval. Invalid arguments leave state unchanged. Both commands and key toggles emit local colored feedback. Existing focus/chat/menu guards and match teardown reset remain in effect. Live command behavior pending verification.

Runtime Unlock FPS correction: D3D9 creation/reset chooses IMMEDIATE whenever local mode has VSync disabled, independent of the initial Unlock FPS checkbox. This keeps later in-match unlock from retaining a DEFAULT presentation interval. Native scheduler still restores its captured interval when unlock is disabled. Latest failed run lacks presentation hook log; initialization timing or another rendering path remains possible and requires live verification.

Mute chat port: #mute/#unmute <slot 1..24> filters only native player-chat channels 0..2 in local mode. Rejects own slot and malformed arguments; atomic bitmask resets on match teardown. System messages and command processing remain intact. Tests cover boundary slots, own slot, unmute/reset and non-chat channels. Live sender-slot correspondence pending verification.

Repeat chat parity: #nhailai <1..24|all>, no argument stops. Uses native quick chat submission after incoming chat dispatch; excludes own messages and hash commands, caps queue at 64 and text at 128 bytes, spaces successful sends by 250 ms. Pauses for typing/tool menu/focus loss/macros; resets on match teardown. Contract tests cover filtering, slots, send cadence and reset. Live chat verification pending.

Selected unit inspection: #upos reports X/Y to five decimals; #cname reports rawcode and numeric ID. Exact CUnit vtable A9C504, code+34 and selected identity validation. Native GetUnitX/GetUnitY wrappers 97D70/97DB0 use vtable+B8 (26E0A0 returns unit+26C), then thiscall 3FCC50 to decode coordinates into three floats. Reader gates decoder signature and rejects missing/changed selection or nonfinite coordinates. Text-format contracts tested; live inspection pending.

Camera position command: #ppos X Y moves local camera through exact native PanCameraTo path. Registry wrapper A3CC0 obtains CGameUI camera using 1EAD20 (UI+294), calls thiscall 1E1A00 with x/y/z=0/flag=0, callee RET10. Direct call gates initial signature; finite bounded coordinates and exact two arguments required. No-argument cursor reporting is not implemented. Contract parser checks passed; live camera movement pending.

Mouse world coordinates: #mpos and argument-free #ppos project the engine cursor through native CWorldFrame 20DD70 (thiscall, two UI floats, xyz output, flag=0, RET10). Verified native caller 216BB0 reads cursor pair D61B54/D61B58 and invokes this path. CWorldFrame constructor 208C10 installs A942B0; CGameUI owns frame+3FC. Signature/class/finite/result validation; display real terrain Z and game client resolution. Explicit #ppos X Y remains unchanged. Format/null contracts pass; live projection pending verification.

Inventory inspection parity: #islot lists occupied slots 0..5 and item rawcodes, or [Empty], matching legacy output. Verified UnitInventorySize AF390 and UnitItemInSlot AF400 call CUnit inventory gate 27EC60, size 2960F0 and slot 291820. These use inventory+404; GetItemTypeId 92FC0 confirms item code+34. Reader gates all three signatures, validates CUnit/selection identity and bounds capacity to six. No handles fabricated or inventory writes. Format/null contracts pass; live inventory verification pending.

Cursor/flag inspection: #uflag displays desktop and game-client mouse pixel coordinates, appending selected CUnit flags in hex when available. Uses the captured game HWND rather than title lookup. Exact 1.29 CUnit flags+164 corroborated by UpdateColor 26DCA0 (tests masks 800/400) and inventory gate 27EC60. Validates class/identity and rejects extra arguments. Formatting/null contracts pass; live output pending. Model-path command remains unported; no heuristic pointer probing added.

Window size parity: #wsize width height uses captured game HWND and SetWindowPos, retaining position/Z order/focus. Matches legacy outer-window dimensions; feedback additionally reports actual client size. Requires 320x240..16384x16384, exact arguments and a valid nonminimized window. Existing viewport updates and cursor clipping reconcile on next frame. Native rendering resize behavior still requires live verification.

Ability button visibility: #hideb/#showb <four-character rawcode> acts on selected CUnit. Exact BlzUnitHideAbility wrapper AF220 calls thiscall 2D7A10(code,bool); native lookup 2D6570(unit,code,1,1,1,1) resolves ability, increments/decrements hidden counter+40 and refreshes through 6E070. Gates signatures, selection identity, counter sanity and verifies mutation. Show at zero is a no-op to avoid negative counters. Repeated hides retain native reference-count semantics. Parser/null tests pass; live panel rendering pending. Full-world #hideball/#showball remains unported.

Selected-unit scale: #size matches legacy numeric conversion (values above 10 interpreted as percent, clamp .05..20, default 100%). Exact SetUnitScale ABB60 resolves CUnit model through 2A2900 (unit+28), calls cdecl model property setter 56CD30(model,2,scale). Gates setter signature, CUnit and property type/flags before native call. Only visual model scale changes; no movement/stat writes. Parser/null contracts pass; live model rendering pending.

Size failure diagnostics: user reports no success notification, indicating early rejection. Added per-stage errors to local notification and WHT.log, plus explicit syntax help. Exact setter signature matches disk. No running game was available for memory inspection; root cause and live scale behavior remain unresolved pending the diagnostic stage.

Size signature-mismatch root cause: existing FogNative hook patches the same generic property setter at 56CD30 during initialization. #size was checking the patched entry against original bytes on every invocation. It now uses the original validated setFogReal trampoline; unhooked fallback retains the full entry signature check. Regression test verifies patched entries are rejected without a trusted trampoline and accepted with the installed original. No removal of fog hook or relaxation for arbitrary jumps. Live size change still needs confirmation.

Network ping parity: #ping uses the existing legacy passive W3GS echo observer via send/recv/closesocket hooks. Preserves socket return values/errors, excludes MSG_PEEK, observes only successful bytes, removes closed sockets. Hook setup rolls back partial installation. No packets injected. Reports echo RTT or labeled TCP fallback; offline/unmeasured and local proxy limitations remain explicit. Tests cover fragmented outgoing echo, correlated response RTT and socket cleanup. Overlapped WSARecv/WSASend paths are not observed; live LAN measurement pending.

Selected-unit invulnerability: #invul / #vul use the native SetUnitInvulnerable path, registry wrapper AB600 -> CUnit vtable+A8 -> 2D8DA0. Native getter 7A4820 returns Avul (4176756C), lookup 2D6570 uses flags 0,0,1,1. The native setter adds/removes the ability and is idempotent. Resolve signatures and vtable are checked; the selected identity is validated via 707F0 and the same +agl/agent+20 checks as the JASS wrapper. Read back Avul and effective unit flag+20 bit8; report a remaining map invulnerability source after #vul rather than promising vulnerability. No direct legacy flag/HP writes copied. Tests cover x86 argument forwarding, repeated on/off, rejected writes, stale unit identity, inactive agent, wrong class and a second invulnerability source. Live game damage behavior remains unverified.

Teleport selected unit: #tele reads cursor terrain coordinates through the existing CWorldFrame projector, then follows native SetUnitPosition ABA00: thiscall 290690(unit,1) stops its old order; CUnit vtable+180 -> 26BEF0 receives &x,&y,1,1,-1,0,0,0,-1,0 and returns with RET28. Signature/class/agent identity gates reject unsupported or replaced units. Revalidate after stopping, before moving, and after moving. Native position decoder 3FCC50 reads back actual XY, so feedback reflects engine pathing adjustments. Reject extra command arguments and invalid/nonfinite cursor coordinates; errors reach local notifications/log. ABI tests cover ten arguments, adjusted position readback, invalid input with no mutation and unit replacement during the stop callback. Live in-game teleport remains unverified.

Remove selected ability: #rabi <four-character rawcode|0xXXXXXXXX> follows UnitRemoveAbility AF590: lookup 2D6570(unit,code,0,1,1,1), thiscall removal 2D8530(unit,ability), then cdecl panel refresh 1F6D00(unit). Signatures plus CUnit/live agent/selection identity checks gate mutation; the removed ability pointer is never read afterward. Requery after refresh to detect rejected removals or map re-additions. Already absent abilities are reported explicitly. Parser rejects extra tokens, malformed hex and zero rawcodes. Tests cover x86 lookup/removal/refresh arguments, absent/repeated commands, rejected mutation, re-added ability and unit replacement preventing refresh. Live game ability behavior remains unverified.

Add selected ability: #sabi <rawcode|0xXXXXXXXX> [level 1..10000] follows UnitAddAbility AE8F0's raw-unit path: lookup 2D6570(0,1,1,1), callbacks 2E0D20/2DD220 around cdecl factory 6F3BA0(unit,code,0,0,0), then native UI refresh. All entry signatures/callback pointers and live unit identity are checked. Optional levels above one follow SetUnitAbilityLevel AAE20: cap with 6F5C0, read level+50, step CAbility vtable+2E4/+2E8. Virtual targets must be executable in the supported game module; each step must advance exactly one level and retain unit/ability identity. Existing level is preserved for default/level1, matching legacy #sabi. Feedback reports actual capped level and distinguishes partial addition with a failed level update. Tests cover native argument forwarding, level cap, decrement, repeated additions, preserved existing level, no-progress rejection, failed creation and invalid syntax. Live ability creation and rendering remain unverified.

Selected unit death: #kill follows native KillUnit A1CA0 through CUnit virtual+124 = 268CB0, passing the engine's zero-life constant at D2D88C. Gate native method/vtable signatures and the selected CUnit's live identity. Keep the death callback as the last unit access because it may remove/recycle the selected object; notification says KillUnit was invoked, allowing maps that revive units. Reject extra arguments and nonzero engine constant, with local error reporting. ABI tests cover the exact unit/constant pointer, stale identity and agent, changed constant and death callback recycling selection. Live death animation and map-trigger behavior remain unverified.

Hero skill points: #sp [1..10000] (default 500) uses the 1.29 native UnitModifySkillPoints path at 297730 and GetHeroSkillPoints at 292A50. It checks the selected CUnit vtable, live agent identity, hero class and hero data before invoking the game method; then reports the actual increase and new total, because the game may cap available points. Signature guards match the 1.29.2.9231 executable. ABI/validation tests pass, but in-game map behavior is not yet verified. #lvl remains separate: native SetHeroLevel only raises levels and lowering requires the engine's strip-level path.

Hero attributes: #str, #agi, #int and #fullatt [1..10000] (default 500) call the 1.29 native CUnit SetHeroStr/Agi/Int methods with permanent=1, then query GetHeroStr/Agi/Int with bonus=1 and only report success when the requested total is observed. Commands require local mode, a selected live hero, exact CUnit vtable and guarded native signatures. Tests cover each attribute, all three together, native ABI, argument validation, nonhero and rejected updates. The tested build was installed to both 1.29 targets; map behavior remains to be checked in game.

### Hero level command

#lvl [1..100], default 10, now follows the native SetHeroLevel experience path and UnitStripHeroLevel when lowering. Maps CUnit GetHeroLevel 291920, XP threshold 291AE0, current XP 293780, native integer-to-real converter 5A1620, AddHeroXP 290910 (RET18; eye candy=1, remaining flags zero), and strip method 29EA60. Checks signatures, CUnit/hero/live agent/selection identity, validates XP delta and reads level back before success. Same-level requests are no-ops. Tests cover x86 ABI, raising/lowering, repeated requests, rejected updates, nonhero and stale selection. Build/package tests passed; live map-trigger behavior still requires in-game verification.
Hero level range correction: #lvl now accepts 1..10000 for maps with levels above 100. Added a level-400 mutation contract and upper-bound parser checks. Actual level remains governed by native map experience rules and readback.

Resource command parity: #gold/#lumber [amount] add resources (default 100000); #food [cap] sets player state4 (default200); #fullres sets gold/lumber1000000 and food cap300. Exact 1.29 GetLocalPlayer93790, GetPlayerState94A80 and SetPlayerStateA97A0 use native player handles already validated by the resource reader. Commands require local mode/active world UI and signature gates, check integer overflow before writing, verify local identity before each write and read values back. Failed or capped updates report an error; fullres may be partially applied if a later state is rejected. Tests cover parser/ABI/repeated additions/food/fullres/overflow/wrong player/rejected writes. Live map resource behavior remains unverified.

Remove selected unit: #udel follows native RemoveUnit A53E0: CUnit virtual+84 (2670D0) preparation, then thiscall29AFE0(unit,1,1). Gates entry signatures and vtable, validates selected live identity before preparation and re-resolves agent afterward. Preparation can clear selection; retain original unit identity rather than requiring the portrait to stay selected. Removal is the final unit access and reports invocation, not a promise against map recreation. Contracts cover x86 arguments, stale/inactive identity and preparation callbacks that invalidate the agent. Live map behavior remains unverified.

Selected-unit life reduction: #udmg [positive amount] defaults100, accepts finite values up to1e9, matching legacy command intent. Reads life through CUnit virtual12C=268E40 (output float pointer, RET4), then uses virtual124=268CB0 with max(0,current-amount). Signature/vtable/live agent/selection gates apply. The life setter is final unit access because death callbacks can recycle the object. Feedback reports target HP and native invocation rather than promising immunity/map-trigger behavior. Contracts cover float parsing, x86 forwarding, repeated reduction, clamping to0 and inactive/stale units. Live gameplay verification pending.

Base damage command: #udmgb [0..1000000000], default50, follows BlzSetUnitBaseDamageAB1D0 for weapon1. CUnit attack+3F4; getter720AF0(attack,index0,RET4), additive setter71E810(attack,requested-current,mask1,RET8), followed by readback. Signature/live-agent/selection/attack ownership checks and delta overflow guard. Reject malformed/fractional values; no direct legacy +AC write. Tests cover ABI, increasing/decreasing/zero/same-value requests, rejected updates and missing attack data. Live combat/UI behavior pending.

Camera focus: #focus centers the local camera at the selected unit's map coordinates using the guarded ReadUnitInfo129 and PanCamera129 paths. Requires a valid selection/camera and no arguments; reports native failures locally. Live camera movement remains to be checked.

Resource transfer parity: #ggl slot gold lumber and #gll slot lumber gold. Zero-based slots0..23, active target, local mode, native player-state signature gates. Requires sufficient balance, rejects self/overflow, preflights both currencies and verifies each write. Interrupted native updates report possible partial transfer. Contracts cover parser, balance conservation, insufficient funds/self/overflow. Live gameplay pending.

Single resource transfer: #gg slot gold and #gl slot lumber use the same native balance/overflow/readback checks as #ggl/#gll. Require exactly two integers; slots0..23 and positive amount. Contracts cover syntax, single-currency mapping and lumber-only transfer without touching gold.

Player numbering correction: #gg/#gl now accept displayed Player1..24 and convert to native slot0..23. Feedback displays Player number. #ggl/#gll retain their existing slot syntax. Contracts cover Player1, Player2, Player24 and invalid Player0/25 for both single-resource commands.

Movement speed parity: #ms [speed], default522, finite input clamped0..2000 as legacy. Native SetUnitMoveSpeed AB880 calls raw CUnit29CF90 (movement+3F8 ->769520); GetUnitMoveSpeed978B0 calls291A50 with float output pointer. Signature/selected live agent/movement ownership guards. Reports actual native speed and requested value so map caps remain visible. Contracts cover repeated changes, native cap readback, absent movement data and parsing. Live gameplay pending.

Unit armor: #armor [value], default50, finite range -1000000..1000000. BlzSetUnitArmor AB130 reads total armor+1E8, subtracts native5A00B0(requested,current), calls CUnit additive method280070(delta pointer, RET4), which refreshes UI. BlzGetUnitArmor97280 corroborates total armor+1E8. Signature/live agent/selection checks; readback must match. Contracts cover fractional/negative/repeated values, rejected native update and stale selection. Live combat/UI verification pending.

Attack cooldown: #attackcd seconds [weapon1|2], finite0.01..3600, primary by default. BlzSetUnitAttackCooldownAB180 calls raw attack722D80(float by value,index0/1,RET8); getter972C0 calls720990(output pointer,index,RET8). Exact signatures, selected/live agent and attack+3F4 ownership checks, readback. Contracts verify primary/secondary independence, argument ABI, malformed inputs, rejected update and absent attack data. Changes native base attack interval; agility/buffs/animation may affect observed attacks. Live gameplay pending.

Detailed damage/armor hover descriptions: Graphics / Detailed Stats Tooltip (default ON), #statstooltip [0|1]. Preserves native descriptions; adds legacy attack rate, reload time, attack speed bonus, map maximum attack rate, and numeric movement speed. Hooks cdecl four-argument description builders 2289B0/229740; attack data unit+3F4, base cooldown+15C, multiplier+1B4, engine maximum DB4C6C. Both hooks require matching entry signatures; bounded 512-byte append retains native text if insufficient space. Automated formatting/buffer contracts covered; in-game visual verification pending.

Attack speed multiplier: #as [factor], default2, finite input clamped0.1..50 matching legacy syntax. CAttack+1B0 bounded float property (constructor71AEBC/71AED1, vtableA6E2F0) uses native thiscall70130(property,&factor,1,RET8), value+1B4, followed by native722C90(attack,unit,RET4) animation/stat refresh. Exact signature/live agent/selection/attack ownership/readback checks. Reports requested multiplier and effective value clamped to map globals DB4CAC/DB4C6C, without changing base attack cooldown. Contracts cover repeated values, map cap, rejected writes, stale selection and property mismatch. Live combat behavior pending verification.

Counted chat repetition: #repeat <1..10000> <text>, #repeat stop, matching legacy syntax. UTF-8 text 1..128 bytes; hash commands, embedded NUL and line breaks rejected. Native quick-chat sender on game thread; decrements only on successful send. Shared 250ms cadence with #nhailai prevents simultaneous queue sends. Pauses for typing/tool menu/macros/focus loss and resets on match teardown. Parser, cadence, failed-send, completion/reset contracts covered; live chat verification pending.

Selected-unit cooldown reset: #nocd enables repeated native UnitResetCooldown (AF760 -> thiscall CUnit2D8A10, no arguments); #yescd stops. Validates signatures, current selection, exact CUnit class and live agent before each call. Operates on current selection every100ms, pauses during tool menu/chat/macros/focus loss and resets on match teardown. Separate from attack interval/attack speed. Tests cover repeated native calls, stale selection, cadence/pause/reset; live skill/item cooldown behavior pending verification.

Game speed: #speed [0|slow,1|normal,2|fast], defaultNormal. Native SetGameSpeedA8790 uses contextD3B6F4+30 and thiscallC2450(context,enum,RET4); GetGameSpeed927F0 confirms context+47 enum. Only actual engine levels0..2 supported; unlike legacy misleading1..70 input. Validates exact setter signature, respects map lock flag4000, verifies context and readback. Parser, repeated levels, lock/rejected writes and invalid input contracts covered; live simulation speed pending.

#speed parity correction: user requires numeric1..70, decimals accepted, default1. Uses the legacy scaled QPC/timeGetTime/GetTickCount approach with continuous clock rebasing under mutex, real original trampolines and unsigned32-bit tick wrap handling. All three hooks must install successfully with partial rollback; native Fast baseline/readback and map lock still required. Restores1x on match teardown. Tests cover bounds/decimal input, continuity across changes/reset and tick wrap. Does not hook GetTickCount64; live accelerated simulation remains pending.

Reselect selected unit: #selc with no arguments. Follows SelectUnit A67C0 and ClearSelection8A810: validates selected CUnit/live agent/selectable flag, obtains local player through777D0 and manager+34, clears native selection then selects unit via EFC40(manager,unit,slot,0,1,1,RET14), updates via F3070(manager,0). Revalidates identity and match after clearing and confirms resulting selection. Native signatures and SelectUnit relative call target checked. Mock ABI/guard tests covered; live group selection and command-panel refresh pending.

Create units: #create rawcode [count], default1, count1..50; four-character rawcode or 0xXXXXXXXX. Creates local-player units at projected cursor map coordinates, with40-unit X spacing and270-degree facing. Uses signature-checked cdecl JASS CreateUnit8EE00, GetLocalPlayer93790 and GetUnitTypeId97D30; float arguments passed by pointer. Requires match context, stops on failed creation or changed match, reports actual verified count. Unit types only; no item fallback. Local mode only. Parser, ABI arguments, partial failure and unavailable-context contracts pass; live creation pending verification.
`nCopy unit type: #copy [count], default1, count1..50. Matches legacy behavior: creates fresh units of the selected type for the local player, beside the source at X+50 then 50-unit spacing, facing270. Does not clone hero levels, attributes or inventory. Reads type from CUnit+34 and native position3FCC50(CUnit+26C), with live agent/current identity validation before and after reading. Reuses verified CreateUnit API and reports actual count. Local mode only. Tests cover repeated commands, spacing/owner/float ABI, partial creation, stale/absent selection, invalid coordinates/count and unavailable natives. Build/package contracts pass; live verification pending.

Drop selected inventory: #drop and #dropall, no arguments, matching legacy aliases. Local mode only. Snapshots up to6 inventory slots, validates CUnit/live agent/current identity before each drop, invokes thiscall29B610(unit,item,RET4) used by UnitRemoveItem AF650, and confirms item absent from inventory before counting it. Exact signatures and wrapper relative call target checked; stops on inventory/selection changes or rejected writes, reporting actual count and error. Uses slot291820 and capacity2960F0. Tests cover sparse/empty inventory, repeated use, rejected drop, retained item, stale/absent selection and invalid capacity. Build/package contracts pass; live in-game verification pending.

Drop All HUD icon: BTNDropItem.blp / BTNDropItemDow.blp, same .016 size and .012 bottom as All Skills, at right .543 (All Skills .563), leaving .004 gap. Independent UI host preserves visibility when tool menu opens; clicks only execute with menu closed and chat inactive through #dropall handler. General tab existing Event/Show Drop All checkbox now available, persisted defaultON; #showdropall [0|1] also supported. Hover help localized; reset frame pointers on match/UI teardown. Contracts verify checkbox toggle persistence/layout and actual Storm access to both drop icon textures. Live appearance/click verification pending.

Drop icon correction: use Unicode escapes for Vietnamese hover text to prevent source encoding corruption. Drop All now vertically above All Skills at right .563, bottom .032; All Skills remains bottom .012, same .016 size with .004 gap. Vertical alignment contracts pass; live visual verification pending.

Four inventory HUD icons: column right .551, bottom from top .072/.052/.032/.012, width/height .016. Order matches legacy: Auto Pick, Drop All, Sell All, All Skills. Existing General checkboxes Show Auto Pick/Show Drop All/Show Sell All/Show Ins All Skills are active; showautopick/showsellall commands persist visibility. Uses original up/down MPQ icons; localized help uses Unicode escapes. Action state reflected by pressed textures, resets on match/UI teardown.
Auto Pick (#autopick [0|1]): selected local-owned live unit, inventory capacity1..6, visible unowned items within777 map units; native spatial query3F4D30 with item mask4F7 (EnumItemsInRect8FF80), bounded256 collector, native handleE72C0 through world+1C, IssueTargetOrderById9B010 smartD0003, at most one order per5 seconds. Sell All (#sellall): nearby live unit with Purchase Item Apit, neutral24+ or allied owner, query unit mask14FD from1024D0, UnitDropItemTargetAF130; only pawnable items, serial order and inventory disappearance confirmation, selection-change/30sec timeout cancellation. Pauses during typing/menu/macros/focus loss; drop action cancels sell and delays auto pickup2 seconds. Signature gates and SEH; local mode only. Tests cover column ordering, persisted visibility, bounded collector/pause/reset, packaged texture availability. Live pickup/shop behavior remains pending verification.

Inventory scan correction: native rectangles are Ymin,Xmin,Ymax,Xmax, confirmed GetRectMinY950A0(+24), MinX95080(+28), MaxY95060(+2C), MaxX95040(+30) and EnumItemsInRect copies these fields directly. Previous X/Y layout searched the transposed map location, affecting both shop and item discovery. NearbyRect129 fixes both paths; asymmetric coordinate regression test added. Icon column now right .558, width .012, keeping bounds .546..558 between inventory edge .544 and skills edge .561 in supplied screenshot; vertical order unchanged. Build/package contracts pass. Live pickup/selling/appearance need verification.

Inventory cadence parity: Auto Pick checks every250ms, remembers target/selected identity and does not resend while target remains available (5sec timeout), proceeds as soon as target disappears instead of a fixed5sec delay after each pickup. Sell All polls inventory every25ms and immediately advances after removal; retries held items after350ms nearby or4000ms beyond600 units, matching legacy retry cadence. Shop scan radius1200 matches legacy. Build/package contracts pass; live throughput pending verification.

Icon spacing: vertical pitch .028 matches legacy toolbar (was .020); positions top-down .096/.068/.040/.012, keeping bottom All Skills and horizontal bounds unchanged. Gap .012 between .016-high icons. Build/package checks pass; live appearance pending.

Random seed command: #srand [unsigned32 seed], default0. Local mode only; invokes native SetRandomSeedA99A0 cdecl(seed), verifies signature/global D61D90 and call5892A0, requires match context and confirms RNG context+0 seed readback. Unlike legacy CRT srand, seeds actual game engine for subsequent draws; does not reverse past random results. Tests cover bounds/default, repeated calls, rejected native updates and missing context/function. Build/package checks pass; live map RNG behavior pending verification.

Shared control: #shareall [0|1], defaultON, Local mode only. Grants/revokes local player's ALLIANCE_SHARED_CONTROL6 toward other active player slots0..23, skipping local/empty/left slots; like legacy applies across players rather than filtering allies. Native PlayerA4240, GetPlayerSlotState94980, SetPlayerAllianceA94A0, GetPlayerAlliance944C0, GetLocalPlayer93790. Signature checks, skips unchanged flags, verifies each write/readback, reports changed count and partial failure. Tests cover player24, inactive/self exclusion, repeat no-op, revoke and rejected writes. Build/package contracts pass; live control behavior pending verification.

## Save game command
Added Local-only `#save [name]` (default `wht-save`). Names accept ASCII letters, digits, underscore and hyphen. Native SaveGame RVA A5C50 is signature gated; its synchronous String::GetCString reader receives a temporary string wrapper. Match and pending-save checks prevent invalid requests. Reports only queue acceptance via world+6D4, not completed file creation. Parser, ABI and rejected/pending request tests pass; live save/load validation remains pending.

## Single inventory item drop
Local-only `#dropitem <1..6>` drops one selected inventory slot, numbered left-to-right and top-to-bottom. Uses existing signature-gated raw item drop API; checks live selection, inventory capacity, empty slots, and item disappearance before success. Stops sell automation and pauses pickup for two seconds like drop-all. Tests cover last slot, other items retained, repeated empty slot, out-of-range slot, retained-item refusal, and invalid selection. Live gameplay validation pending.

## Shift-right-click quick drop
Added default-on Shift + right-click inventory drop using actual 1.29 CSimpleRegion button rectangles (+44..+50, Ymin/Xmin/Ymax/Xmax), without legacy fixed-coordinate fallback. Ctrl/Alt, chat, menus, hidden console, macros and background window disable interception. Matched clicks route through verified #dropitem and consume matching mouse-up even if Shift is released first. #quickdrop [0|1] toggles the session setting. Geometry and existing single-item drop contracts pass; live hit-position validation remains pending.

Quick-drop hit-test correction: inspected native RVA 473350 and replaced the incorrect +3C rectangle with +44 Ymin, +48 Xmin, +4C Ymax, +50 Xmax. The old miss let standard right-click target selection run. Live verification pending.

Quick-drop widescreen correction: convert client mouse X into native centered 4:3 coordinate space instead of overlay normalized 0..0.8. Native inventory rectangles use this space; tests cover 4:3 and 16:9 and distinct adjacent columns. Live verification pending.

## Create items
Added Local-only #item <rawcode> [1..50] at map cursor, using CreateItem RVA 8E430 and GetItemTypeId 92FC0, signature gated. Verifies returned item type and reports actual count, stops on failure/context change. Tests cover spacing, partial creation, limits and missing match/API. Live gameplay pending.
