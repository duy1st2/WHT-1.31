from pathlib import Path
import json,hashlib
r=Path('WHT-1.31').resolve();p=r/'analysis/create-item-preserved-preinstall.json';before=json.loads(p.read_text())
for folder in (r/'runtime-test').glob('Warcraft3-*'):
 for arch in ['x86','x86_64']:
  for f in (folder/arch).rglob('*'):
   if f.is_file() and ('macro' in f.name.lower() or any('macro' in part.lower() for part in f.relative_to(folder/arch).parts[:-1])):before[str(f)]=hashlib.sha256(f.read_bytes()).hexdigest()
p.write_text(json.dumps(before,indent=2));print('Preserved state files:',len(before))
