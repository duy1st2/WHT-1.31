# Version 1.1.7 - 2026-10-29

Updated native version definitions, embedded helper assembly version and MIX metadata for WHT 1.24/1.26, WHT 1.29, and WHT 1.31 x86/x64. Updated the 1.29 test expected version/export value. Other source changes preserved.

Tests passed: WHT 21/21; WHT 1.29 10/10; WHT 1.31 x86 57/57. See analysis/version-117-x64-build.log for the x64 suite result. All four MIX product versions verified as 1.1.7; all three helper file versions verified as 1.1.7.0. Existing compiler/CMake warnings remain.

Installed with backups and hash checks into configured 1.24/1.26 runtime clients, independent 1.29 clients, and both 1.31 profiles within runtime-test only. INI settings preserved; no user game stopped.

Updated WHT/OK, WHT-1.29/OK, WHT-1.29/dist/update/WHT-1.29.mix, WHT/dist/singlefile/WH.mix and WHT-1.31/dist/client-x86 and client-x64. Logs: analysis/version-117*.log. No Drive upload or Mongo toolinfo publication performed.


Project  Architecture Version SHA256                                                          
-------  ------------ ------- ------                                                          
WHT      x86          1.1.7   BA0B0CBE11FBBE51B837864B8D9AE615F05F9446C235CC66214AEBAECB860E38
WHT-1.29 x86          1.1.7   0000149D2094FFDD0706C5C040E26F65B860FF66790C5ACEE35E2C2AFEA8B497
WHT-1.31 x86          1.1.7   9B12F3E51807D632C32945D9779B24A958445B297866C7EDAE88BF04FAE88DE9
WHT-1.31 x64          1.1.7   90C096D637908A6037729A2384811D3BBE6F915D860247BD5AD98CFF1627F1B0



