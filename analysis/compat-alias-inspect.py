from pathlib import Path
for f in ['native/version131.cpp','native/version64.cpp','native/command_feedback131.hpp']:
 p=Path('WHT-1.31')/f
 for i,line in enumerate(p.read_text(encoding='utf-8-sig').splitlines(),1):
  if any(t in line for t in ['CanonicalCommand131','#showregen','#regen"']): print(f,i,line[:220])
for p in Path('WHT-1.31/tests').glob('*'):
 if p.suffix in ['.cpp','.hpp']:
  for i,line in enumerate(p.read_text(encoding='utf-8-sig').splitlines(),1):
   if 'CanonicalCommand131' in line:print(p.name,i,line[:220])
