# Native selected-unit name search

Reference SHA256 evidence: unit-name-native-evidence.json. Both actual WHT/native/gameplay_commands.hpp:1498 and WHT-1.29/native/gameplay_commands.hpp:1311 accept #ufinds and read only the selected unit position. Neither handler reads or compares the unit name; it prints the supplied query as Found. This port retains selected-unit scope and compares the actual display name, preventing that false positive. No map enumeration is claimed.

Verified native chain:

| Build / architecture | GetUnitName wrapper | CUnit name getter | Cache initializer | Inline cache |
| --- | --- | --- | --- | --- |
| 12164 x86 | 8e3e60 | bac220 | bd7740 | CUnit+38 |
| 12173 x86 | 8b2570 | b6b690 | b971f0 | CUnit+38 |
| 12164 x64 | 9f9760 | d09a90 | d3e4c0 | CUnit+5c |
| 12173 x64 | a44dd0 | d48550 | d7cbc0 | CUnit+5c |

The live 12164 x64 native binding resolves GetUnitName to 9f9760. Its wrapper resolves a script unit, calls the CUnit worker and converts the returned name to a script string. The worker returns the inline cache, lazily calling its initializer if empty. Every initializer copies min(strlen(name),259) bytes to this same cache and appends NUL. Both x86 builds independently show the same chain and bounded copy. Full wrapper, worker and initializer bytes are guarded; relocation directories contain no relocations in these ranges. No old-version offsets or protected string-conversion helper are invoked.

Implementation reads an already initialized cache only, bounded to260 bytes, with strict UTF-8 validation. It makes no game-memory writes, does not initialize an empty cache and does not call JASS/native name functions. An empty/unterminated/invalid cache reports unavailable. Existing CUnit class, registry ID, world/local-player and selection checks are retained around reads; two snapshots must match name, type and source identity. Coordinates use the latest valid snapshot.

#ufinds accepts the trimmed remainder of the command as a name substring, including spaces and Unicode. Matching uses Windows ordinal case-insensitive comparison. Known Warcraft color codes are removed before comparison; markup in query is rejected and residual native pipes are sanitized in notifications. #ufind remains rawcode search. Both commands operate on active selection only, under existing Local/settings authorization.

Live evidence: unit-name-live-18240.json reads the selected unit's UTF-8 Vietnamese name and confirms the same portrait selection; unit-name-live-guard.json compares all three native blocks against the running12164 x64 process at its actual ASLR base. No remote invocation, unit mutation or client shutdown was performed. End-to-end command execution requires the new build on the next Run-WHT launch.

Tests cover both builds in each architecture using full captured blocks at a relocated fixture base, tamper rejection, native offsets/bounds, UTF-8 and malformed input, multiword/Unicode matching, color stripping, false-positive prevention, identity/name/type/world changes and current coordinates.

## All-player ability commands still pending

player-abilities-reference-evidence.json records both #addall/#raddall reference implementations before source changes. These genuinely enumerate all local-player units via GroupEnumUnitsOfPlayer with null filter, then add/remove the ability. The live12164 x64 binding resolves GroupEnumUnitsOfPlayer to9faee0, whose body remains protected in available module evidence. A world-frame render collection or current selection cannot establish complete enumeration. These two commands remain disabled until that native enumeration path is verified; no selection-only substitute was enabled.
