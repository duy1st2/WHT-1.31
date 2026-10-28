from pathlib import Path
import json,hashlib
r=Path('WHT-1.31');e=json.loads((r/'analysis/create-item-evidence.json').read_text())
for p in e['profiles']:print(p['build'],p['bits'],p['create'],p['getType'],p['resolve'])
backups=json.loads((r/'analysis/create-item-live-guard-file-backups.json').read_text());report=[]
for row in backups:
 old=Path(row['backup']).read_bytes();now=Path(row['path']).read_bytes();report.append({'path':row['path'],'unchanged':old==now,'append_only':now.startswith(old),'backup_matches_original_hash':hashlib.sha256(old).hexdigest()==row['sha256']})
(r/'analysis/create-item-owned-launch-file-effects.json').write_text(json.dumps(report,indent=2));print('owned launch changed files', [p for p in report if not p['unchanged']])
paths={row['path']:hashlib.sha256(Path(row['path']).read_bytes()).hexdigest() for row in backups};(r/'analysis/create-item-preserved-preinstall.json').write_text(json.dumps(paths,indent=2))
