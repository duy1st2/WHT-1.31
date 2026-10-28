# FPS / stutter investigation ? 2026-10-28

User clarified the problem is low FPS / stutter, not network latency. Inspection
found no game initially; later user client 15028 appeared in runtime-test 12173
x64 and was left running. Existing log 20:49:45?20:51:35 shows UI refresh counts
slowing while bar Show/Bind counters rise sharply. This suggests overhead UI
load but does not establish the complete cause of low FPS. Existing Update and
Graphics timings excluded PresentBar; GetTickCount64 durations were quantized to
about 15/16 ms and stopped after twelve reports. No FPS before/after measurement
was obtained, and no claim is made that every cause of stutter is fixed.

## Removed duplicate work

NotifyStatPresentation131 was dispatching the full WriteBar path independently
from native Show, Bind and Update. Full work includes unit level/HP/mana reads,
color discovery, dimensions, owned-child validation and text presentation. These
callbacks can repeat for the same bar and unit in one CGameUI Update cycle.

The presentation record now remembers the native UI update serial plus actual
bound unit pointer and its native two-word agent identity (existing x86 c/10,
x64 18/1c fields). With an active matching UI-thread refresh hook, repeated work
for the same key is coalesced. Different pooled unit or recycled identity bypasses
immediately. Each native UI update serial permits fresh work. Timer discovery
invalidates the cache, preserving configuration refresh. Without a valid UI hook,
matching class, or readable non-sentinel identity, retain the prior presentation path.
Reentrancy remains bounded. No cached child pointer is dereferenced to skip work.

Original Bind/Update calls keep their exact arguments and return registers.
Existing native Show suppression for hidden invulnerable bars is unchanged;
coalescing does not otherwise suppress native calls. Cheap ApplyStatPaint enforcement runs every time,
including dynamic unit flags and native texture resets. Native unit/world state,
movement, rendering rate, FPS integer/cadence and feature settings are untouched.
The UI frame priority rule and existing owned-child guards are preserved.

## Measurement

QPC replaces coarse GetTickCount64 durations. Four timing buckets cover update,
graphics, MTH toolbar and the previously unmeasured full bar presentation path.
The report remains every five seconds and stops after 120 reports (ten minutes)
until allowed state changes. Completed diagnostics avoid further QPC sampling.
Added full/coalesced counters to the existing bounded graphics diagnostic line.
Timing is labeled QPC, without claiming immunity to third-party clock hooks.

## Validation and limits

Existing full suites: 49/49 x86, 48/48 x64. Added regression checks retain native
ABI/returns and all 120 native calls while reducing 120 presentation invocations
to 40 across 40 modeled UI cycles. Additional checks cover pooled rebinding,
same-address/new-identity reuse, next-cycle freshness, unknown/sentinel identity,
missing UI hook fallback, recursion, high-resolution unit conversion and bounded
sampling. This is a work-count test, not a measurement of live FPS or CPU speedup.
No offsets, hooks or native ABI profiles were added. No user process was stopped
or injected for testing. A running client receives its MIX on the next Run-WHT
launch; installer skips loaded directories. Runtime settings/logs/ban lists and
macro scripts are preserved (active logs may append naturally).

Read-only native FPS font samples from user process 15028 (old MIX) were
44.7, 45.9, 38.9, 41.4, 33.7, 39.5 FPS. This is a six-second baseline only,
without a controlled same-scene comparison. Asked user to relaunch through
Run-WHT-1.31-x64.cmd to load pending MIX and obtain after measurements.
