from pathlib import Path
for f in ['version131.cpp','version64.cpp']:
 for line in (Path('WHT-1.31/native')/f).read_text(encoding='utf-8-sig').splitlines():
  if 'ViewCommandFor131' in line:print(f,line)
