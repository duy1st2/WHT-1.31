from pathlib import Path
r=Path('WHT-1.31/analysis');s=(r/'fog-native-live-guard.py').read_text().replace('fog-native-evidence.json','create-item-evidence.json').replace('fog-native-live-guard-','create-item-live-guard-');(r/'create-item-live-guard.py').write_text(s)
p=Path('WHT-1.31/tests/create-item-tests.cpp');s=p.read_text(encoding='utf-8-sig');s=s.replace('uintptr_t(0x12345678)',"(sizeof(uintptr_t)==8?uintptr_t(0x1234567812345678ULL):uintptr_t(0x12345678))");p.write_text(s,encoding='utf-8-sig')
