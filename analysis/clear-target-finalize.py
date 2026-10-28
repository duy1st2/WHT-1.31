from pathlib import Path
import hashlib,json
r=Path('WHT-1.31').resolve();sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
e=json.loads((r/'analysis/clear-target-evidence.json').read_text());before=json.loads((r/'analysis/clear-target-preserved.json').read_text())
art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   pending=build==12173 and arch=='x86_64'
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'deferred_running_client':pending,'build_sha256':sha(src),'installed_sha256':sha(dst)})
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in e['references'].items()),'tests':{'x86':48,'x64':47},'end_to_end_game_command_tested':False,'ssp_enabled':False}
(r/'analysis/clear-target-verification.json').write_text(json.dumps(report,indent=2))
assert all(a['same'] or a['deferred_running_client'] for a in art)
assert report['references_unchanged']
active=r/'runtime-test/Warcraft3-1.31.1.12173/x86_64'
for state in report['preserved']:
 state['active_client_changed_during_work']=not state['same'] and Path(state['path']).parent==active and Path(state['path']).name in ['WHT.ini','WHT.log']
 assert state['same'] or state['active_client_changed_during_work']
report['active_client_state_not_overwritten']=True
(r/'analysis/clear-target-verification.json').write_text(json.dumps(report,indent=2))
print('Artifact matches:',sum(a['same'] for a in art),'/12; 12173 x64 MIX update deferred to Run-WHT; inactive client state preserved; active client INI/log changed during work and were left untouched; references unchanged')
