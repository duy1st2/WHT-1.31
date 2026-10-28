# WHT item creation / copy — 2026-10-28

Actual WHT/native/gameplay_commands.hpp implements #item and #copyitem; source
hashes recorded before edits in create-item-evidence.json. User permits WHT-only
functionality where WHT-1.29 lacks execution.

* `#item I000 [count]` or `#item 0x49303030 [count]`
* `#copyitem 1 [count]` through slot 6 on the selected unit

Count defaults to 1 and accepts 1..50. Both commands create at the initial map
cursor position, with each next item 40 units farther along X, matching WHT.
Copying copies the item type; native default charges are used. The source item
is never removed, edited or given manufactured handles. No automatic pickup.

| Build | Bits | CreateItem | GetItemTypeId | Typed item resolver |
|---|---|---|---|---|
| 12164 | 32 | 8d13f0 | 8dd8b0 | 6ff780 |
| 12173 | 32 | 8a0160 | 8ac300 | 6c6020 |
| 12164 | 64 | 9e3fb0 | 9f2320 | 7b8bd0 |
| 12173 | 64 | a2ff60 | a3d9a0 | 807a10 |

All addresses are module RVAs. Names/functions were recovered from the initialized
native binding cache of owned runtime-test clients. Candidate pointers were
checked against captured wrapper code; nearby heap values are not bindings.
CreateItem accepts a 32-bit rawcode and two borrowed real pointers, returning a
32-bit real JASS handle even on x64. Its native factory and engine handle
conversion run normally. The tool does not synthesize script handles, bypass
their conversion, manually adjust references or free native objects.

GetItemTypeId resolves a typed CItem then reads rawcode +34 x86 / +58 x64,
matching the existing independently guarded CItem constructor. Confirmation
checks item vtable, rawcode, global registry serial/node, deletion marker, typed
handle resolution and native type getter. Duplicate handles or serials within
a batch stop it. Engine objects/handles retain normal world-managed lifetime.

Byte guards cover complete public CreateItem/GetItemTypeId wrappers and
conservative 1024-byte windows at the native factory and typed resolver. These
windows are not claimed to be exact function extents. Protected internal handle
conversion is called only through the real native API, not independently invoked
or claimed to have been reverse engineered. x86 relocations are normalized.
Read-only live ASLR verification matched all 8 guard blocks on each of the four
profiles, before any feature activation. No native CreateItem call was made in
a game during this verification.

The existing authorized Local/settings gates are checked again between actions.
World/local player identity is re-read before and after creation, without
depending on resource balances. Copy additionally retains unit registry serial,
selection, inventory pointer/capacity, item slot/pointer/serial and type; any
change stops the batch. Map cursor and full batch coordinate extent must be
finite and bounded. Reentrant item batches refuse. Unconfirmed or partial
creation stops without retry or rollback, reporting confirmed count and that
the latest native call may have created an item. No success is inferred from
a nonzero handle alone. Creation handlers publish their own confirmed-count
report; generic command feedback is suppressed for item/unit creation and copy
to prevent a second misleading OK line.

Tests: x86 48/48, x64 47/47, including borrowed-real-pointer ABI, x64 resolver
pointer width, count/slot/rawcode parsing, spacing, failed preflight, partial
confirmation, changed context/source, duplicate serials and rebased/tampered
capture profiles. Existing native item readers and guarded mouse-world query
are reused. No new thread, hook, frame or INI setting is added.

Owned test client path/start identity was checked before each close; user
processes were not stopped. Config/log/ban files were backed up before live
guard launches; the comparison in create-item-owned-launch-file-effects.json
records their effects. Installation separately verifies preserved files and all
12 program artifacts. Actual ground-item appearance, default charges in custom
maps and multiplayer behavior remain untested.
