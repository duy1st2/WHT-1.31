# Player name command ? 2026-10-28

Actual reference WHT/native/runtime.cpp:3355?3430 implements #n/#name:
query, rename, restore/default/reset/off, retain original and persist desired
name in Event OLDNames[_Ex]/NewNames[_Ex]. Its main tick reapplies configured
name. WHT-1.29/version129.cpp has no matching execution branch. Reference
hashes were recorded before production edits in player-name-evidence.json.

Important adaptation: WHT writes a 64-byte network-player buffer (+21c) and
has a separate lobby-name path. Neither offset is reused here. This 1.31 port
updates the actual managed CPlayerWar3 player name through the engine string
assignment worker called by public SetPlayerName. It affects native player-name
queries in an active match, including the existing resource board. It does not
rewrite CNetData name buffers, lobby/channel names, outgoing network identities
or global Warcraft registry values. Cross-client rename visibility is not
established. Lobby/channel support is outside this implemented path.

## Native evidence

| Build | Bits | Public SetPlayerName | String worker | Field | String getter |
|---|---:|---:|---:|---:|---:|
| 12164 | 32 | 91bb50 | 2c08a0 | 2c | 2c0b90 |
| 12173 | 32 | 8e91c0 | 2aa6d0 | 34 | 2aa9c0 |
| 12164 | 64 | a32b20 | 316170 | 58 | 316560 |
| 12173 | 64 | a7e960 | 383bf0 | 48 | 383fe0 |

Addresses are RVAs. Live initialized JASS native-cache name/function bindings
were read through OpenProcess(0x410), without writes. Public setters resolve a
real CPlayer handle, obtain the string, address the indicated player field and
call the worker. Production uses the existing verified native local-player
resolver and passes that exact field plus a bounded UTF-8 C string to the
worker. It never fabricates a JASS string or player handle. x86 ABI is thiscall
(ECX field, stack const char*, callee ret 4); x64 uses RCX/RDX. Refcounted string
interning/allocation/release is performed by the engine, not raw pointer edits.

Three complete guard spans per profile cover public setter, assignment worker
and getter. x86 HIGHLOW fields are rebased. All four profiles match 3/3 blocks
in freshly launched, owned runtime-test clients; each was closed only after
path and recorded StartTime checks. Native write functions were not called in
these live clients. These guards establish code identity, not end-to-end rename
behavior. Tests also exercise ABI arguments with an owned mock function.

## Command/state behavior

Reject embedded NUL, CR/LF, controls, invalid surrogate sequences and names over
63 UTF-8 bytes. Trim outside spaces/tabs, preserve interior spaces, reject
oversize instead of cutting a UTF-8 character. A bare command queries native
state without configuration writes. Before first rename, save the valid original
name; if saving fails, do not invoke native assignment. Re-read context/name
before writing and confirm world/player/local/name after the write. Only then
persist desired name (or clear it on restore). Failures report unconfirmed or
partial persistence, without claiming success or blindly rolling back/retrying.
A saved old name remains available for restore. #name retains WHT's [64] label.
The shared feedback suppressor avoids a second generic success notification.

The configured name policy runs at most once per second while authorized,
settings enabled, and in Local mode with an active world/player. Failed native
attempts are quarantined until the world/player or desired name changes; UI
teardown resets policy state. Empty desired name disables reapplication. No new
hook, frame, priority change, thread, registry setting or unrelated command.

Validation: full architecture suites (49 x86 / 48 x64), strict text boundaries,
rename/query/restore, original retention, persistence failures, context change,
readback mismatch, policy cadence/quarantine, ABI pointer width and all four
rebased fixture profiles. Actual in-match commands and multiplayer name
propagation were not tested. Existing reference trees, configuration/log/ban
files and macro scripts are preserved; final artifact SHA256 report is separate.
