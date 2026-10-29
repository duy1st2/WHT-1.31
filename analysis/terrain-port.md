# Native terrain background (#codenen/#code_nen/#nen)

Reference: actual WHT/native/runtime.cpp ApplyCodeNen and command handlers; reference hashes/pre-edit backups recorded in terrain-evidence.json. WHT-1.29 only stores code_nen; no applying consumer found. User permits actual WHT features when 1.29 lacks them. Only WHT-1.31 modified.

Four independently recovered native bindings:
- 12164 x86 SetTerrainType91f220 / GetTerrainType8e1980; 12173 x86 8ec830 / 8b01e0.
- 12164 x64 a35420 / 9f6d80; 12173 x64 a810a0 / a42770.
Full public wrappers establish pointer-to-float coordinates, canonical rawcode byte swap, six setter arguments, cdecl x86/Win64 ABI. Getter-worker and getter-method code establishes index/out-of-range checks returning0. Native bodies are guarded against independent captures; read-only live16/16 spans matched in all four owned runtime-test clients. No terrain function was invoked in these clients. Each was closed only after exact saved path and StartTime match.

WHT grid preserved: x=-15000; x<15000; x+=256 and y=15000; y>-15000; y-=256 =118x118=13924 points, final14952/-14952. Setter variation-1, area7, shape1. Native getter returns0 outside supported terrain and those points are skipped. Every valid point is still painted (including points already showing the rawcode, preserving brush coverage) and read back; mismatches/failures stop rather than falsely announcing completion.

Work runs on UI updates: at most256 points inspected and8 writes per update, with2ms QPC budget checked between grid-point operations. This is cooperative, not a hard upper bound on a native operation or its before/set/readback sequence. No per-frame work when setting is empty. Revision/reentry guard prevents stale commands advancing a replacement job. Root/world/player/local and ready native session are checked before/after each native getter/setter; no mutation during loading, after changed match/UI or disabled settings. Failed jobs are not retried every update. Saved valid code is queued once per new match/config context. No engine-owned handles or frames created, no new hooks/threads, old offsets, or ManaBar restoration.

Commands query with no argument; exactly four printable ASCII characters configure code. off/none/clear/0 clears persisted setting and cancels further painting. This does not restore already changed tiles; map reload required, matching WHT behavior and explicitly reported. Persistence is checked and reloaded before resetting/queuing job. No immediate game mutation inside the chat callback. Invalid native profiles cannot persist an application request.

Tests: typed six-argument setter/getter ABI; parser, rawcode order; whole exact grid and endpoints; hard write/query limits and cooperative budget; readback rejection and no-retry; empty/no-terrain; cancellation, reentrant replacement, recursive tick; context/session changes; capture guards and mutation rejection; deferred command flow and failed persistence. No real map repaint or live FPS impact measured. Native context/session readiness uses existing verified Resource/DropSession helpers.
