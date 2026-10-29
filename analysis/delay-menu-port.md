# Delay reducer menu completion

Actual reference command implementations: WHT/native/runtime.cpp (#dr/#drn range, integer validation, Local+LAN pairing) and WHT-1.29/native/parity_commands129.inl (checked delay controller). Source excerpts and SHA256 in delay-menu-evidence.json. The existing independently verified 1.31 provider callbacks remain unchanged; no new native address or instruction patch.

F8 / Other Hotkeys now exposes Local/LAN and Battle.net number fields in free rows515/560. Appending preserves all241 prior control IDs. Both UI and chat route through the same DelaySetting helper, including authorization, Command Settings, native availability, checked persistence, rollback and immediate apply. Integer1..1000 only;100 restores native defaults. Inactive scopes show100 rather than unused legacy Net250. Vietnamese/English labels and tooltips distinguish configured command delay from measured network ping.

Tests extend the existing captured-profile callback and real-INI suite with inactive/active display and independent scope routing. Static check confirms243 controls and unchanged first241 rows. No live visual or network latency measurement performed.
