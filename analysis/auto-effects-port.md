# Adaptive effects port

Reference: WHT/native/adaptive_effects.hpp, adaptive_effects_native.hpp and runtime.cpp. The policy/state header is copied byte-for-byte. WHT-1.29 has no native adaptive particle consumer. Scope permission allows using the actual WHT feature when 1.29 lacks it.

For each supplied 1.31 profile, independently locate the MSVC RTTI .?AVCParticleEmitter2@@ table, its first method, and both calls to a global emission-scale getter. That getter returns the same global written by a 0..1 clamping setter. x86 uses x87 float return and cdecl bool(float); x64 SSE float and bool AL return. The 1.24 stdcall int(float) signature is deliberately not reused. Setter is invoked only with finite 0..1 and read back after the call. No native engine instruction/table patch is added.

Native FPS is traced from the stock FPS formatter to its counter wrapper (index7), then to the renderer counter array (+0x34 x86 / +0x68 x64). Reading counter7 gives offsets0x50/0x84; divide by100 as the stock formatter does. This works when Show FPS is off. Guards cover the entire setter/getter/counter wrapper and worker, the emitter consumer call window and stock formatter, normalized x86 HIGHLOW relocations, type name and table identity. The setter upper-bound float constant is checked to remain1.

Runtime uses the same WHT reduction/recovery thresholds and 250 ms sampling. Default OFF in 1.31, explicitly enabled with #autoeffects. Persists Graphics/Auto Reduce Effects with checked save/reload. Only authorized Local mode in an active world enables adaptation. Background periods suspend dwell. Disabling and UI teardown restore only the setting owned by the controller; an external change is preserved. A native guard/read/write failure is reported once until recovered. Strong reductions can remove particle visual cues. No simulation/unit or time scale is changed.

Tests use owned copies of all four captured images with relocated code/data pointers and mock renderer counters. They never execute captured engine code. The native bool-float call ABI is checked using a real C++ test function. Policy/controller scenarios cover thresholds, recovery, zero emission, original restoration, external writes, world changes, background dwell, failed setter, invalid commands and persistence failure.

Actual in-game visual effect and controlled FPS improvement have not yet been measured. This is a command-only port; no additional menu control.
