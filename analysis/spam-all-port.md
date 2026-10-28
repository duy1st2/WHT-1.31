# WHT #spamall port — 2026-10-28

User permits actual WHT implementations where WHT-1.29 lacks execution.
Source hashes, native captures and RTTI proof are in spam-all-evidence.json.
WHT/runtime.cpp SendAllianceSpam and ProcessChatAutomation are the source.

Bare `#spamall` toggles the all-player sequence. Every second, visit active
remote slots 1..16 in order, send action 50 with FF0000FF then 0, and send two
random minimap pings after the first successful pair. No peers means no ping.
The PRNG, coordinate range +/-10000 and duration -1 match WHT. Single-target
#spam is paused while all-player mode is active, avoiding duplicate sends to
its selected slot; its armed target can resume when #spamall is toggled off.
Bare #spam stops only the single-target mode, as in WHT.

Earlier identifier-68 ambiguity is now resolved: CNetCommandAllyPing RTTI
matches the producer's vtable on all four profiles. Native producers set type
68 and initialize three float fields. Their sender wrapper writes the opcode,
calls the three-float codec and calls the same native sender used by resource
transfer. Each float writer copies its raw 32-bit representation. Thus the
wire payload is 68 + float x + float y + float duration, 13 bytes total.
The normal engine producer uses duration 5; this feature deliberately follows
WHT's -1 value. Captured serialization proves the bit format, not game-side
processing or how long a remote client displays a negative-duration ping.

| Build | Bits | Producer | Sender wrapper | Float codec |
|---|---|---|---|---|
| 12164 | 32 | 9c9f00 | 9c33d0 | aa0cd0 |
| 12173 | 32 | 98aa90 | 984360 | a5fc90 |
| 12164 | 64 | aec0c0 | ae56f0 | bd8570 |
| 12173 | 64 | b34bc0 | b2e7d0 | c1f040 |

All numbers are hexadecimal module RVAs. Generated runtime byte guards cover
the producer's type/opcode identity window, wrapper opcode/codec/send sequence,
complete codec and float-bit writer. Selected spans contain no PE relocations.
Existing guarded CDataStore sender and session validation are reused.

Arming requires two matching source/roster/session snapshots. Before every
action, compare the active local player, world, alliance owner, session and
complete roster again. Slots with no active remote player are skipped. A
noncanonical physical slot, self alias, changed context or uncertain send
stops all-player automation. Successful submission does not acknowledge remote
execution, and stopping does not restore an earlier alliance mask.

Existing authorization, Local-mode/settings and focus/chat/menu/macro/skill/
craft gates remain. Late updates send one batch without replaying old periods.
UI teardown, lost authorization and disabled settings reset both automation
states. No new thread, hook, persisted setting or overlay is added.

Tests cover exact LE packet bytes and borrowed-buffer ABI, argument rejection,
action ordering, timing/pause/no catch-up, zero peers, context changes,
partial pair/ping failure, no automatic retries and modified native guards.
The production integration gate that pauses single-target mode was inspected
in both version handlers. Both builds and architectures use captured profile fixtures. Actual commands
and packets were not sent to any running game during validation; in-match
effects and multiplayer compatibility remain unverified.

This supersedes the unresolved minimap-path notes in alliance-spam-port.md.
#wide still needs independent 1.31 perspective mapping; #findCP has no real
detector in either source reference.
