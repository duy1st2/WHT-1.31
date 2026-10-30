# Match timer synchronization (2026-10-30)

Reference: WHT/native/match_timer.hpp and WHT/analysis/match-timer-cycles-20261030.md. Source evidence: the legacy native scheduler subtracts period from queued deadlines and increments its cycle counter; complete elapsed time is current + cycles * period. This is the same simulation-time semantics required by the user, without a tool-start or wall-clock origin.

1.29 independent reader ReadClock129 uses manager D5E9EC -> +44 scheduler. It now reads current+40, cycles+44 and period+48, validates the fields, computes the total in double precision and preserves its existing float API. Timer, external HUD and Time Boss all use this reader. The timer keeps its last displayed time if a native read fails. Match UI recreation and late enable read the existing native total.

1.31 already implements the complete native counter in SimulationSeconds131, with separate verified profiles for 12164/12173 and x86/x64. Keep those addresses and semantics. ClockOverlay131 no longer hides its valid last text on a transient read failure; disabling the feature still hides the frame. No wall-clock fallback or speed multiplier is introduced.

Regression cases: rollover boundary, many completed cycles/late reads, paused native values, simulation increments corresponding to increased game speed, invalid periods/nonfinite values. No live game speed/pause check is claimed from the contract tests alone. Only the independent 1.29 and 1.31 projects are changed in this request.
