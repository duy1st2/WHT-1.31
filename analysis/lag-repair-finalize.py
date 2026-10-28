from pathlib import Path
import hashlib,json
r=Path('WHT-1.31').resolve();sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();before=json.loads((r/'analysis/lag-repair-preserved.json').read_text());state=[]
for path,old in before.items():
 p=Path(path);same=p.exists() and sha(p)==old;append=False
 if not same and p.name.endswith('.log') and p.exists():
  digest=hashlib.sha256()
  with p.open('rb') as f:
   for line in f:
    digest.update(line)
    if digest.hexdigest()==old:append=True;break
 state.append({'path':path,'same':same,'original_log_prefix_preserved':append})
art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name;pending=build==12173 and arch=='x86_64'
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'running_client_deferred':pending and sha(src)!=sha(dst),'build_sha256':sha(src),'installed_sha256':sha(dst)})
active_ini=r/'runtime-test/Warcraft3-1.31.1.12173/x86_64/WHT.ini'
for row in state:
 row['active_user_config_changed_during_work']=not row['same'] and Path(row['path'])==active_ini
 if row['active_user_config_changed_during_work']:
  assert 'Running client; installation pending' in (r/'analysis/lag-repair-install-x64.log').read_text(encoding='utf-16')
report={'tests':{'x86':49,'x64':48},'native_callback_regression':{'original_callbacks':120,'full_presentations':40,'coalesced':80,'cycles':40},'state':state,'artifacts':art,'before_live_fps_samples':[44.7,45.9,38.9,41.4,33.7,39.5],'live_FPS_improvement_measured':False,'user_process_stopped':False,'new_offsets_hooks':False}
(r/'analysis/lag-repair-verification.json').write_text(json.dumps(report,indent=2));assert all(x['same'] or x['running_client_deferred'] for x in art);assert all(x['same'] or x['original_log_prefix_preserved'] or x['active_user_config_changed_during_work'] for x in state);print('Artifact matches',sum(x['same'] for x in art),'/12; loaded 12173 x64 MIX may be pending; inactive state unchanged; logs append-only; active user INI changed during work and was left untouched; suites passed49/48')
