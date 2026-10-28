from pathlib import Path
for p in Path('WHT-1.31/native').glob('*.hpp'):
 for line in p.read_text(encoding='utf-8-sig').splitlines():
  if any(t in line for t in ['struct InventoryProfile131','ItemCodeValue131(', 'bool ItemCodeGuard131']):print(p.name,line[:450])
