from pathlib import Path
import hashlib,json,shutil,datetime
r=Path('WHT-1.31').resolve();sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
refs=[r.parent/'WHT/native/runtime.cpp',r.parent/'WHT/native/show_unit_skills.hpp',r.parent/'WHT/native/common.hpp',r.parent/'WHT-1.29/native/gameplay_commands.hpp']
for p in refs:
 if not p.exists():continue
 if '1.29' in str(p):print('129 cs matches',[i for i,l in enumerate(p.read_text(encoding='utf-8-sig').splitlines(),1) if '#cs' in l])
e={'references':{str(p):sha(p) for p in refs if p.exists()},'source':'WHT/native/runtime.cpp:2962-2968 clears g_setIdTargetHandle; 1.31 target cast reads pinned131/pinned64 via ReadPinned and SameSelfCastSource131. Reset the entire native pin record; no game memory write required.','ssp_status':'Not enabled; x86 enumerators mapped but selection/display gate and x64 callbacks not verified.'}
(r/'analysis/clear-target-evidence.json').write_text(json.dumps(e,indent=2))
b=r/'analysis'/('before-clear-target-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));b.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','native/command_feedback131.hpp','README.md']:
 d=b/f;d.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(r/f,d)
preserved={str(p):sha(p) for p in (r/'runtime-test').rglob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'macro' in p.name.lower() and p.suffix.lower() in ['.txt','.json','.lua','.wht'])}
(r/'analysis/clear-target-preserved.json').write_text(json.dumps(preserved,indent=2));print('Evidence and backups saved; preserved',len(preserved),'state files')
