# WHT #oldname port

Reference: WHT/native/runtime.cpp:3429-3452; SHA256 and pre-edit backup recorded in oldname-evidence.json before modification. User allows actual WHT implementations where WHT-1.29 lacks them.

Both native dispatchers support #oldname (query) and #oldname Name (save Event OLDNames and OLDNames_Ex). Feedback uses WHT Old Name color. The saved value feeds the existing #n restore implementation; no native offsets, lobby writes, hooks, frames or polling added. Uses the existing 63-byte UTF-8 restore name constraint. Unlike the reference unchecked saves, failed or incomplete persistence is reported as failure. No active match or native read required to edit the setting.

Tests extend the player-name contracts with the real restore command flow after setting the original name, query without writes, both keys, invalid names, and partial-save failure. Native in-match rename has not been end-to-end exercised in this turn.
