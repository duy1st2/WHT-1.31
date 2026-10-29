# #csave save-event notifications

Reference implementation: WHT/native/runtime.cpp DetourSaveGameHandler and #csave (records source hashes before modifications in save-notices-evidence.json). WHT-1.29 lacks the native save-event notification handler; user scope permits using actual WHT. Only WHT-1.31 changed.

Independent profiles:
- 12164 x86 handler 9c7c80, CNetCommandSave table 180ea24, slot +15; 12173 handler 988ab0, table 17c38ec.
- 12164 x64 handler ae9a80, table 1bd80f8, slot +29; 12173 handler b32ad0, table 1c06f78.
- Native handler registrations explicitly select event 6 on each build. Handler bodies validate the same slot/sequence header and call the verified save worker with the engine-save flag. x86 handler takes the packet on stack and returns bool with plain ret; x64 packet is RCX. Observer uses the already verified generic event registry shared with Fog Click (cdecl event/callback/userdata/priority, unregister event/callback/userdata).

Implementation registers an additive event-6 observer, leaving stock save handler installed. Callback returns continue=1 even for invalid packets/exceptions. Packet class and slot are validated; only bounded event metadata is queued. UI drain resolves the exact native player slot, without guessing slot minus one or blaming local player on failure. Unknown/invalid names use Player N, and control characters/invalid UTF-8 cannot inject log lines. Current root/world plus epoch prevent stale in-flight events crossing match or toggle transitions. Bounded64 events, age2seconds. OFF/teardown disables collection before unregister; failed unregister remains retryable with collection disabled.

#csave (no arguments) toggles existing Event / Noti/Log Info Players Save Game. Checks save plus reload; WHT color feedback. Native profile must validate before enabling. Incoming save events show [Save Game]: name and append Save Game: name to TS-SaveGame.log beside the tool. Direct #save worker calls and file completion are separate, as documented in README. No new frames, stock frame changes, threads or MinHook detours.

Verification: full suites52/52 x86 and51/51 x64. New contracts cover callback continuation, owned packet layout/class/slot, bounded queue, once-only drain, expiration/clock rollback, epochs, disable/remove retry, toggle persistence errors, name/log sanitization, and normalized capture guards. Live read-only12/12 handler/registration/ABI spans matched on four owned runtime-test clients; each closed only after exact recorded path/StartTime check. No save command or file output invoked in those clients. In-match native observer delivery and actual UI/log output after a real save remain untested.
