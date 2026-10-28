# WHT HUD optimizations port ? 2026-10-28

Actual sources read before porting; SHA256 recorded in
wht-optimization-evidence.json. Copied the pure overhead_cadence.hpp policy
header byte-for-byte. Native 1.24/1.26 offsets, engine-specific overhead managers,
removed ManaBar, renderer setters and native frame APIs were not copied.

## Implemented

| WHT source | 1.31 application |
|---|---|
| overhead_cadence.hpp / OverheadCadence.Full | 300 ms registration/pruning sweeps in StateBars131 and Gameplay64 |
| overhead_bars.hpp / sampleCadence.Unit, SampleIntervalFor, crowdCadence.Update | Shared 200/400/800 ms overhead data policy using the port's validated native bar and unit IDs; selected/hero priority at 200 ms |
| regen_overlay.hpp / ButtonUpdateCadence | 100 ms HP/s and MP/s native queries; selection/enable/root changes bypass |
| runtime.cpp / buttonCadence, cooldown_hotkey_overlay.hpp / SetTextCached | 100 ms cooldown queries; do not collect disabled button arrays; avoid repeat empty-text writes |
| memory_overlay.hpp / 500 ms lastUpdateTick and applied text cache | Shared StatusCache131: no per-tick process-memory query or unchanged status font write |

The imported crowd hysteresis is preserved: level 1 enters at 50 and stays
while at least 40; level 2 enters at 120 and stays while at least 100. Counts
are validated native HP roots from the registry, not all GUI/font frames.
Each sample key contains root, actual unit pointer, native agent identity and
native frame pointer. Roots, permissions or display settings invalidate cached
sampling. A child identity/parent mismatch forces a full guarded refresh.
No cached page permissions or stale font pointers are used to skip validation.

Sampling gates only expensive data reads and associated label/owner-color
sampling. Custom width/height and invulnerability handling run before the gate.
A fresh-data fast path validates the owned child, maintains native clipping and
hides supplemental text immediately for ineligible units. Native Show/Bind/Update
forwarding and existing color/hide enforcement remain unchanged. Frame movement
and anchors continue following the game normally. New label discovery can wait
up to the 300 ms sweep, and non-priority values in a crowd can lag up to 800 ms,
as with the WHT sampling policy. No label-count reduction or removed ManaBar.

The x64 priority check uses the already verified hero ability identity, but full
presentation retains the original level>0 hero classification. State/movement
and native getters remain the existing independently verified 1.31 code paths.
The sample-clock injection mirrors WHT's testable sampleClock field.

Status-cache owner/font changes reset its lifetime. Authorization, memory-toggle
or external-HUD changes update the label policy immediately. Failed memory
queries retain the last valid sample; failed text writes remain retryable.
The version/authentication labels and memory format are preserved. This cache
only controls the native status overlay; it does not promise to pace every
memory query made by other services. No INI keys are added or rewritten.

## Audit limits

worker_placement.hpp and fps_text_cadence.hpp already match WHT byte-for-byte;
no duplicate replacement needed. The copied FrameLoadCadence declaration is
unused: adaptive effects and FPS-driven LOD require separately verified 1.31
renderer/FPS bindings. The incremental 128-item/time-budget sweep scheduler is
not ported in this change; the port retains its bounded native-frame registry
walk, now paced at 300 ms. This is the listed HUD optimization group, not a claim
that every WHT optimization or old renderer backend is implemented.

## Validation

Full suites: 50/50 x86, 49/49 x64. Policy tests cover base/crowd cadence,
selected/hero priority, hysteresis, pooled frame and unit identity reuse,
configuration/permission/root invalidation, missing-root fallback and clock
rollback. RAM/text tests verify 2 queries and 1 stable text write across 100
callbacks in one second; panel queries 10/100; sweeps 4/100; normal samples 5/100.

Owned-memory integration tests execute the actual StateBars131/Gameplay64
WriteBar functions with mock native float getters and owned frames. One hundred
fresh callbacks cause exactly two float getter calls (current and maximum) and
one label write. A 200 ms deadline, recycled identity, format change and root
change force new native queries; unchanged text is not rewritten. The x86
cooldown integration test clears both layers on disable, then suppresses repeat
empty writes. No native game mutator is invoked by these tests.

These counts prove eliminated work in deterministic tests, not a measured live
FPS improvement. No live before/after scene comparison was available. Build and
runtime-test artifact SHA256/state/reference verification is recorded separately.
