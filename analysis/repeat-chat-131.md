# Repeat incoming player chat: #nhailai

Scope evidence read: WHT/native/gameplay_commands.hpp ObserveChat (766), command (1677); WHT/native/runtime.cpp ChatDisplayHook (1920), self-slot rejection (3666). WHT-1.29/native/repeat_chat129.hpp and native/version129.cpp Observe/command use same bounded queue, 250ms cadence, own sender exclusion, all/one player/off. Reference hashes recorded separately. No reference project changed.

1.31 independent native history entry profiles:
| Build | x86 | x64 |
|---|---|---|
|12164|b6a540|cc0c20|
|12173|b2a260|d00b40|

Read unpacked module captures memory-14740/23884/6688/29928.bin. CHAT_RECIPIENT_ALL and CHAT_RECIPIENT_ALLIES references and player-colored message formatting identify chat history path. x86 incoming-event caller b6aa60 pushes channel [event+98], text event+18, sender byte event+15; entry returns ret 0c. x64 incoming caller cc1180 uses R9d=[event+b0], R8=event+30, EDX=byte[event+29]. Signature: void(this,sender,text,channel), unlike 1.29 five-argument chat display. Other outgoing-chat callers also reach this entry; own sender excluded. System sender -1 excluded. This observer does not suppress original rendering/history and is not a mute implementation.

All four entries guarded by captured 48-byte prologue; x86 security cookie absolute operand relocated and checked separately. Hook lazily installed when command used. Original called exactly once, unchanged arguments. Callback bounded-copies text under SEH then queues under mutex; game player reads/sends stay on UI thread. Modes are session-only and default OFF. Clear on session/settings invalidation, UI destruction or local-player replacement. Queue <=64, UTF-8 <=128 bytes, excludes commands/control characters; 250ms backoff also on refused send. Native existing chat path sends only when foreground, not typing, menu closed and automation idle.

Usage: #nhailai 2, #nhailai all (toggle), #nhailai (OFF). 24-player range follows 1.29/newer game capacity. No live multiplayer messages sent during testing. Contract tests cover captured profiles, relocation/mutation, filtering, bounds, lifecycle, send cadence/refusal and original-call ABI. Real multiplayer receipt/delivery remains to be verified in game.

Validation complete: x86 10/10 and x64 9/9 CTests passed; both builds installed for 12164 and 12173 with backups. All 12 installed artifacts match built SHA256; all 6 preexisting INI/macro/ban files present in snapshot unchanged. Source ZIP refreshed. No game process stopped and no live chat sent.
