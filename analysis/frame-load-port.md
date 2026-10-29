# FPS-based overhead sampling

Reference: WHT/native/overhead_cadence.hpp FrameLoadCadence, overhead_bars.hpp SampleIntervalFor/CollectSweep and runtime.cpp native FPS sampling. WHT-1.29 has no corresponding adaptive frame-load consumer. Source hashes in frame-load-evidence.json establish the actual WHT implementation. FrameLoadCadence was already copied into 1.31 but unused; this change connects it to the existing actual overhead writers.

Implementation: frame_load131.hpp polls the stock FPS sampler at250 ms with UI identity, foreground, enable and clock rollback guards. Both version131.cpp and version64.cpp use the previously independently verified EffectsRead131 native renderer counter, without enabling particle emission changes. OverheadSampling131 takes the maximum of crowd and FPS data intervals and adjusts discovery sweeps to300/400/600 ms. Heroes and selected units retain200 ms. Existing native placement/clipping and cheap visibility/color paths are not throttled.

The WHT policy remains unchanged: below50 FPS for1 s reaches level1, below30 reaches level2, at58+ FPS recovery steps every5 s. Ordinary data intervals have floors600/1200 ms. Background and missing samples suspend dwell, leaving any existing level until valid recovery or context reset; disable/absent/new UI clears it. Settings change immediately invalidates cached data and sweep timing. No new command or configuration; this is the remaining automatic overlay optimization from WHT, independent of #autoeffects and Show FPS.

Validation expands actual overhead writer fixtures on both architectures to count native float reads at the1200 ms boundary and ensure selected units still sample at200 ms. Pure tests exercise polling frequency, dwell/recovery boundaries, background/missing reads, clock rollback, disable/context lifecycle, priority counts, crowd interaction and immediate settings invalidation. Existing native particle/FPS profile contracts still cover all four build images.

This does not port WHT incremental/time-budgeted full sweep queues. A full discovery pass still processes its current registered frames. Actual large-battle FPS benefit and UI responsiveness have not been measured live.

Caller review: x86 overhead sampling is keyed by menu131.independentHost; x64 is keyed by CGameUI. Polling uses the exact respective owner passed into the actual writer, rather than assuming both architectures use the same root. UI teardown already resets both samplers.
