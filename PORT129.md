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
