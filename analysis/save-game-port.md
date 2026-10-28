# Save game port

Both reference #save handlers and active1.29 save_game129.hpp were read/hashes recorded before source changes. Name defaults wht-save;64 ASCII alphanumeric/_/- only.

1.31 native SaveGame binding uses a script context plus managed string (x86 two stack words, x64 RCX/RDX), unlike1.29. Do not fabricate that ABI. Independently followed native SaveGame non-script branch: x86 caller8f4e64/8c2ea0 passes resolved char path and flag0 to worker6f3f50/6ba950; x64 callers a08397/a5305c use RCX=char path, EDX=0 to worker7ad840/7fc9e0. Complete workers guarded, including full x64 chained-unwind extent and x86 exception epilogues. This is the same actual simulation serialization worker used by native SaveGame; no JASS thread/context or fake string object.

Path evidence on12164x86: original worker argument at[ebp+8] saved[ebp-ad4], reloadedEDI at6f458d and passed unchanged at6f46f1 to archive writer a9ec40. That writer passes its filename to a9f3c0, which copies only to derive/create parent then passes original filename to f16b70/f173d0 file opening. Tool supplies an absolute path beneath the executing runtime-test client/WHT-Saves, rejects non-ASCII/long paths, links, existing destination and invalid executable placement. Native worker flag0 matches SaveGame callsite. No live save invoked during research.

Success requires native bool plus newly created nonempty destination file, with stable match identity. No claim of save reload validation without a live test.

Validation: combined x8635/35, x64 34/34 contracts passed, covering both builds per architecture. Full guarded blocks compare to captured profiles and reject tampering after relocation. Shared-control flow mocks cover all active peers, idempotence, OFF, unrelated bits retained, roster/callback changes, no-op and partial failure; no live alliance mutations executed. Save mocks/pure path tests cover defaults/name boundaries/reserved Windows names, malformed traversal/colon/NUL input before access, runtime-test placement/build/architecture, native length bounds, rejected preparation/existing destination, match changes, native failure/no retries and missing output confirmation. No save worker executed in tests, no actual archive/reload validation performed. Inspected x64 code guards for DIR64 relocations: none in guarded ranges.

Installed12164/12173 x86/x64 with backups,12 artifact SHA256 matches,12 preexisting INI/log/ban files and all reference hashes unchanged. Source ZIP refreshed. User game processes were never stopped and loaded binaries were not replaced.
