# WHT display command compatibility — 2026-10-28

Read actual WHT/native/runtime.cpp overlay command dispatch. Source SHA256 and
line evidence were recorded before editing in wht-display-aliases-evidence.json.
User permits WHT implementations where WHT-1.29 does not implement execution.

Added alternate command names for existing 1.31 features:

| WHT names | Existing 1.31 handler | Behavior |
|---|---|---|
| #mem, #memory, #ram, #usemem | #showmem | Process memory overlay |
| #trade | #res | Resource board |
| #regen, #showregen | #regenhp | Selected-unit HP regeneration display |

Bare commands toggle. Argument 1 enables; 0 disables. Arguments are not rewritten:
existing native handler parsing, authorization, configuration writes and feedback
remain in effect. Both architecture dispatchers and the feedback helper already
use CanonicalCommand131, so this shared mapping fixes all three consumers.

WHT expands the resource board when its master display command enables it.
The 1.31 view-command path now sets expanded=true and next=0 after a successful
enable, including explicit 1 when already enabled. Invalid arguments, a failed
save, other display commands and disabling do not expand it. No new frame is
created for this behavior; the existing ToolMenuOverlay resource board refreshes.

This is command compatibility for implemented features, not seven new native
backends. No native offset, hook, removed mana bar or menu priority is changed.
All four runtime-test variants retain existing config/log/ban files and backups.
Existing full architecture suites are run; no new implementation-mirroring tests
were added for this small reversible mapping change. Actual commands were not
sent to an open game during verification.

The earlier command-gap audit considered only the common gameplay header.
It cannot establish complete parity with WHT runtime.cpp and its included
feature headers. Additional WHT-only native paths such as scene light, inspection
of other units' skill cards, and item creation/copy require separate source and
1.31 native mapping audits. Placeholder #findCP remains excluded.
