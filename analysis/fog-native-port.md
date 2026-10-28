# Native #mh port — 2026-10-28

Implemented in version131.cpp and version64.cpp for builds 12164 and 12173,
x86 and x64. This supersedes the earlier preparation-only fog audit.
Reference source paths and pre-edit SHA256 values are in fog-native-evidence.json.
Both reference implementations use visible fog state 4, local player,
GetWorldBounds, shared vision true and after-shared vision false.

`#mh` toggles; `#mh 1` enables; `#mh 0` disables. Existing local-game/settings
gates apply. No network packets, new hooks, INI settings or overlay were added.

The backend obtains real JASS handles through GetLocalPlayer/GetWorldBounds,
creates through CreateFogModifierRect and calls native Start/Stop/Destroy.
It validates typed objects, registry serials, local player ownership, modifier
flags and copied rectangle geometry. The temporary rectangle is removed before
activation. Deletion acceptance uses the engine registry's deferred-removal
marker, not immediate deallocation or a raw object delete.

| Build / architecture | Local | Bounds | Create | Start | Stop | Destroy | RemoveRect |
|---|---|---|---|---|---|---|---|
| 12164 x86 | 8ddd50 | 8e4d30 | 8d0c10 | 8d67d0 | 8d67f0 | 8d31a0 | 8f1470 |
| 12173 x86 | 8ac7a0 | 8b3430 | 89f9a0 | 8a5210 | 8a5230 | 8a1e30 | 8bf640 |
| 12164 x64 | 9f2870 | 9fa950 | 9e3680 | 9ea800 | 9ea820 | 9e63a0 | a065a0 |
| 12173 x64 | a3def0 | a45fe0 | a2f690 | a36050 | a36070 | a32160 | a514a0 |

All values are hexadecimal module RVAs. Typed player/rectangle/fog resolvers:
12164 x86 700380/700980/6fed80; 12173 x86 6c6bc0/6c7190/6c5670;
12164 x64 7b9880/7b9ee0/7b8130; 12173 x64 8085a0/808b70/807060.

Guards compare conservative 1024-byte code windows and required vtable cells;
these windows are not claimed to be exact function extents. x86 HIGHLOW
relocations are normalized. Unexpected code rejects activation. Rectangle
coordinates must be finite, within +/-1,000,000 and have increasing bounds.

Partial failures retain owned handles for cleanup. Unconfirmed cleanup blocks
another allocation; explicit off can retry. Already-on requires a fresh native
state check. UI/world generation changes invalidate old handles after best-effort
cleanup; the old engine world owns remaining registered agents during teardown.

Validation: x86 44 tests, x64 43 tests, including lifecycle failure cases,
profile fixtures, ABI mock arguments, relocation/tamper rejection and geometry.
Read-only live code guard comparisons passed 34/34 blocks on 12173 x86 and
12164 x64. The other profiles have captured-image fixture verification.

End-to-end reveal and disable in a running match remain unverified. Both UI
automation channels failed with native pipe unavailable (os error 2); no chat
command or fog mutation was sent through them. Test clients owned by this
project were closed or had exited; user processes were not stopped.
