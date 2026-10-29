# Incremental overhead discovery

Reference: WHT/native/overhead_bars.hpp ContinueSweep and UpdateBarsIncremental; overhead_cadence.hpp FrameLoadCadence. WHT source hashes are recorded in overhead-sweep-evidence.json. No WHT or WHT-1.29 files are modified. This continues the actual WHT overlay optimization port; no unrelated utility or command is introduced.

The previous 1.31 discovery passes synchronously validated every registered frame. Both StateBars131.Update and Gameplay64.Update now retain a handle-only queue between UI ticks. Each regular pump validates at most128 handles and cooperatively yields after approximately1/0.5/0.33 ms according to the existing FPS LOD budget divisor. At least one item progresses; failed QPC uses the item bound. The per-frame callback remains unchanged, including immediate placement/clipping and the previously ported hero/selected-unit sampling priority.

Handles are not captured native unit pointers. Each visit uses the existing verified SampleStateBar131/Hp64 to resolve the current frame class and current unit binding. Normal discovery only registers paint metadata; it does not show/hide a frame from a stale snapshot. Configuration/UI/permission transitions cancel pending work. Display changes and disable restoration retain the original synchronous pass. Once disabled cleanup completes, repeated discovery stops. Final valid native-frame count updates the crowd policy only after all pending handles are processed.

Registry snapshot collection and cache pruning remain synchronous once at the beginning of each pass. These operations and the explicit immediate settings/disable path are outside the incremental processing budget. This port therefore does not assert a hard bound on total UI-tick time, nor does it copy WHT's separate cleanup queue (1.31 cache pruning differs). No engine profile, address, frame level or native ABI is changed.

The new tests check exact128 limits, deterministic QPC budgets, guaranteed progress, failed-clock fallback, completion counts, canceled/reentrant work and context/config/permission transitions. Actual x86/x64 overlay-service fixtures own a300-frame registry; they verify128/128/rest discovery, change a not-yet-processed frame binding between ticks, check final crowd counts, immediate disable/settings processing and teardown. They do not execute game code or hook native classes. Existing native paint/sampling suites still verify the real writer path separately.

Actual large-battle FPS improvement is unmeasured.
