from pathlib import Path
import shutil,json,hashlib
r=Path('WHT-1.31').resolve();out=r/'analysis/create-item-before-live-guard-files';out.mkdir(exist_ok=True);report=[]
for folder in (r/'runtime-test').glob('Warcraft3-*'):
 for arch in ['x86','x86_64']:
  for p in (folder/arch).glob('*'):
   if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower()):
    dest=out/(folder.name+'-'+arch+'-'+p.name);shutil.copy2(p,dest);report.append({'path':str(p),'backup':str(dest),'sha256':hashlib.sha256(p.read_bytes()).hexdigest()})
(r/'analysis/create-item-live-guard-file-backups.json').write_text(json.dumps(report,indent=2))
