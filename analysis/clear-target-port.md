# Clear Set Id Target ? 2026-10-28

Actual WHT/native/runtime.cpp:2962?2968 clears g_setIdTargetHandle and reports
Clear Set Id Target. Reference hashes were recorded before edits in
clear-target-evidence.json. WHT-1.29 common gameplay handler has no #cs;
the user explicitly permits the actual WHT reference in this situation.

In 1.31 the Set Id Target hotkey fills SelfCastSource131 pinned131/pinned64.
ReadPinned131/64 validates it and compares its native unit, registry identity,
world/session and UI before ActivateTargetCast131 can submit an order. #cs
now resets this whole record to {}, preventing further use of the old pin.
Current selection and unrelated courier pins stay unchanged. Both architecture
handlers reject extra tokens before clearing. Repeated #cs is harmless.
Existing authorization/Local/UI command gates remain in effect. The shared
feedback helper publishes the original colored title once after success.
No new offset, game memory write, setting, hook, frame or thread is introduced.

The existing full x86/x64 suites are required. No implementation-mirroring test
was added for this direct state reset. Actual hotkey/command use in an in-match
client was not tested. Runtime state/config/logs are preserved and installed
program artifacts are compared against build SHA256 hashes.
