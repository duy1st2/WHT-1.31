# Single-player-target alliance automation — 2026-10-28

User expanded reference scope: use WHT when WHT-1.29 lacks the execution path.
Source hashes were recorded before edits in alliance-spam-evidence.json.
WHT/runtime.cpp SendAllianceSpam and ProcessChatAutomation implement the real
sender; WHT-1.29 only assigns spamSlot/nextSpam without an execution consumer.

Implemented `#spam 1..16`, with bare `#spam` stopping it. Matches WHT's single
target behavior: once per second submit flags FF0000FF then 0 using action 50.
Single-target WHT does not include minimap pings. Neither #spamall nor #wide is
enabled by this change. #findCP remains a fixed response in both references,
so there is no detector to port.

Verified captured native 1.31 action constructors identify action 50. Its
serializer writes one target byte then a 32-bit flags word. Together with the
opcode the wire payload is six bytes. Constructor identity windows and complete
serializer instruction bytes guard all four profiles. No 1.24 offsets are used.
Uses the already guarded native sender and CDataStore ABI from resource transfer.

| Build | Bits | Constructor guard RVA | Serializer RVA |
|---|---|---|---|
| 12164 | 32 | a8099d | aa1b00 |
| 12173 | 32 | a4112d | a60ac0 |
| 12164 | 64 | bb52c0 | bd9610 |
| 12173 | 64 | bfd290 | c200e0 |

Before arming, resolve a stable active remote player and session twice. Before
each action, recheck the source world/local player, target pointer/physical slot
and session. Self-target, unavailable player or profile mismatch refuses.
Changed context or uncertain submission stops the sequence without retry.
The engine provides no acknowledgement here: successful submission does not
claim remote processing or restoration of an original alliance mask.

Existing authorization, Local mode and settings gates remain. Chat, menu,
macro/skill/craft activity and background focus pause sends. Late ticks submit
one pair without replaying missed periods. UI replacement resets the state.
No new hook, thread, INI setting or menu frame is added. No messages or packets
were sent to a running game during verification.

Contract tests cover strict arguments, packet bytes, borrowed-buffer ABI,
cadence, pause/resume, session replacement, partial submission/no retry,
authorization reset, captured profile guards and modified-code refusal.
In-match effects and multiplayer compatibility remain unverified.

The #spamall investigation found multiple native action objects associated
with identifier 68 with differing layouts. Its exact minimap wire path has not
been established; raw 1.24 ping bytes must not be assumed valid for 1.31.
The #wide perspective hook also needs independent 1.31 mapping, particularly
because the engine's existing aspect-ratio behavior differs from older clients.
