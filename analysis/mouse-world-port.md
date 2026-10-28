# Native map cursor coordinates and camera positioning

Reference evidence:
- WHT/native/gameplay_commands.hpp:1342 implements #mpos world cursor reporting; :1371 implements bare #ppos cursor reporting and #ppos X Y camera pan.
- WHT-1.29/native/version129.cpp:1150 provides the same commands; native/mouse_position129.hpp uses the native world-frame projection with engine cursor coordinates, returns actual X/Y/Z, and reports client resolution. native/pan_camera129.hpp bounds finite pan coordinates to +/-65536.
- The 1.31 display format follows 1.29 (actual native Z and client resolution), rather than WHT's placeholder Z and optional selected-unit suffix.
- Hash evidence: mouse-world-reference.json.

Independently verified 1.31 query profiles:
| Build / architecture | Current-cursor query | Engine cursor XY | World-frame vtable |
| --- | --- | --- | --- |
| 12164 x86 | 0xb208c0 | 0x1b6552c | 0x1825828 |
| 12173 x86 | 0xae00d0 | 0x1b1d524 | 0x17da708 |
| 12164 x64 | 0xc6ebb0 | 0x2191098 | 0x1c03a08 |
| 12173 x64 | 0xcb1b20 | 0x21be098 | 0x1c328c0 |

Native verification:
- Used existing unpacked captures memory-14740/23884/6688/29928.bin.
- Located the 1.31 counterpart of the reference explicit XY projection at x86 0xb20c10 / 0xae0420 and x64 0xc6eee0 (same initialized ray-query structure, projection call, success branch and copying three floats to caller output).
- The current-cursor query initializes that same ray-query structure but obtains XY from the engine's cursor pair directly. It invokes the same lower projection used by the explicit XY method. This avoids reimplementing viewport conversion or estimating coordinates from Win32 pixels.
- x86 reads the cursor pair, accepts ECX world-frame / output pointer / mode, and returns ret 8 with AL boolean success. x64 accepts RCX world-frame / RDX output / R8 mode and returns AL success. The typed adapters use bool, including when other EAX bits are unspecified.
- Disassembly evidence: disasm-14740-b208c0.txt, disasm-14740-b20c10.txt, disasm-6688-c6eb50.txt, disasm-6688-c6eec0.txt, mouse-world-12173-x86.txt and mouse-world-12173-x64.txt. Derived analysis PE containers were never executed.
- Each query has a 96-byte native code guard. x86 skips the relocated cursor pointer bytes and verifies their relocated value separately; x64 RIP-relative code is verified directly. World-frame class identity and finite/bounded engine cursor input are checked before querying.

Behavior:
- #mpos and bare #ppos return finite bounded native X/Y/Z plus game client resolution. Missing projection never fabricates zero coordinates.
- #ppos X Y uses the previously independently verified native camera pan profile, with strict finite +/-65536 arguments.
- Uses existing authorized Local match dispatch with settings enabled. No unit, settings, cursor or map-state writes.

Validation: all four captured 96-byte guards, ASLR cursor address and altered-byte rejection, native query ABI stub, finite/bounded inputs/results, Z, text/resolution formatting, strict arguments, read-versus-pan command routing, missing projection/camera and refused pan. Native world projection and camera movement in a live match remain untested.
