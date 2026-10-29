# #q native verification

Reference WHT/native/quit_match.hpp queues EndGame(false) with original root/world, consumes request before invoking native, rejects context changes. WHT/native/runtime.cpp:2759 validates its old native wrapper; #q dispatch at 2879 rejects arguments and requires active match. Reference SHA256 recorded in quit-match-evidence.json.

Saved 1.31 images contain EndGame registration name strings at 192971c/18de61c (x86 12164/12173), 1d6f720/1d9e620 (x64). The x86 registration pushes function RVAs 155f4b0/1521530, both xor eax,eax;ret stubs. Those targets must never be used as successful native EndGame callbacks. x64 registration refs at 1820f1d/1857d99 are in protected registration code, not proof of a callable implementation.

Pending: live EndGame runtime binding, four independently verified bodies/ABI, valid match/session transition, consume-once deferred dispatch, and tests for changed world/root and rejected arguments. No production command or unverified offset was added. Asked user to open a runtime-test match, starting with 12164 x64. No game launched or stopped during this audit.

## Verified runtime binding follow-up

Read-only live scans recovered actual binding RVAs: 12164 x86 8d4970, 12173 x86 8a35d0, 12164 x64 9e8620, 12173 x64 a34330. Live entry bytes (48) match the independent saved captures on all four. These are protected game wrappers, not the inert registration stubs. x64 begins sub rsp,28;movzx edx,cl, confirming boolean first argument. Call uses cdecl(uint32_t) x86 and Win64 first-register argument x64, as the native EndGame(boolean) binding. No engine call made during binding verification. Three owned verification clients were launched inside runtime-test and closed only after matching path and recorded StartTime. User 12173 x64 PID15220 left running.

Implemented #q using queued EndGame(false) on the UI update thread. Queue is consumed before invocation; duplicate request refused; canceled on root/world change, authorization/settings loss or teardown. Entry guard rechecked at execution. No stock-frame changes, configuration additions, or new hooks/threads. Tests cover pending lifecycle, cancellation, reentry consumption, native argument ABI, failure/no-retry, unknown builds, and each guarded-byte mutation in independent captures. Actual return-to-menu behavior remains untested in-match.
