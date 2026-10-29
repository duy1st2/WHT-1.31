# Resource transfer player numbering

WHT: #ggl/#gll validate 1..16 and pass slot-1 to SafePlayer in native/gameplay_commands.hpp.
WHT-1.29: active dispatcher uses ParseResourceTransfer129 from native/resource_commands129.hpp, validates 1..24 and maps s-1. Its inherited gameplay_commands.hpp uses 1..16 and slot-1 as well. No changes needed in these projects.
WHT-1.31: active native/resource_transfer131.hpp previously parsed #gg/#gl as 1..24 but #ggl/#gll as 0..23. Unified all four to 1..24, converting once to native zero-based slot. Updated both local/network usage strings and README, plus inherited gameplay handler for consistent fallback behavior.
Tests compare all four commands for all 24 player numbers; reject 0/25; retain gold/lumber order, native transfer safety and synchronized packet tests. Existing flow fixtures now use player2 to target native slot1. No game offsets, native serializer or multiplayer transport changes.
