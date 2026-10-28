# Map trade flag research

Both actual reference gameplay_commands.hpp implementations of #mtrade take no arguments and call SetMapFlag(0x100,0). 1.29 mapped capture registration resolves real setter a9320 and getter99310: root global d3b6f4, config+30, flag word+38. Setter ORs/ANDs requested mask; getter tests mask. These offsets must not be reused for1.31.

1.31 x86 SetMapFlag strings found at19254ac/18da3ac and registrations153d52f/14ff5ec; current registration passes protected dispatch pointers172f4b0/1791530 rather than a verified direct setter. IsMapFlagSet names1925678/18da578 likewise. x64 names1d6ad98/1d99c98. Static complement/AND candidate search examined several unit flags, interpolation properties and other unrelated functions; no candidate accepted as map config. Do not enable these candidates.

Next evidence required: live runtime-test1.31 match to resolve actual native binding or verify map-configuration getter/setter call chain. No source/build/client artifacts changed in this investigation. Existing completed functions remain installed.
