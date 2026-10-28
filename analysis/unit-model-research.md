# Selected unit model query research

Read actual TryGetUnitModel and #gmodel in both WHT/native/gameplay_commands.hpp and WHT-1.29/native/gameplay_commands.hpp. Both scan unit/model +0x60 typeInfo candidates and offsets1c..2c heuristically, then fabricate units\rawcode.mdx as fallback. This fallback does not prove the asset exists and is not suitable as confirmed model path.

The existing independently verified 1.31 unit-scale profiles establish only CUnit model-object getter (+28 x86/+48 x64), not a filename field. No filename offset inferred from 1.29 has been enabled. No source/build/runtime artifacts changed for this research.

Next required evidence: running 1.31 runtime-test client in a match with a selected unit. Inspect model object and native filename/type-data accessor read-only; compare native caller and layout for both12164/12173 and architectures before enabling #gmodel. No Warcraft III processes found in latest inspection.
