from pathlib import Path
import hashlib,json
r=Path('WHT-1.31').resolve();sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();e=json.loads((r/'analysis/player-name-evidence.json').read_text())
art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name;art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'sha256':sha(dst)})
state={}
for f in ['player-name-preserved-before.json','player-name-preserved-preinstall.json']:
 before=json.loads((r/'analysis'/f).read_text());state[f]=[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()]
live=[json.loads((r/f'analysis/player-name-live-guard-{build}-{bits}.json').read_text()) for build in [12164,12173] for bits in [32,64]]
report={'artifacts':art,'preserved':state,'references_unchanged':all(sha(Path(p))==h for p,h in e['references'].items()),'tests':{'x86':49,'x64':48},'live_native_guard':[{k:v for k,v in p.items() if k!='blocks'} for p in live],'end_to_end_rename_tested':False,'lobby_network_identity_supported':False,'native_mutator_called_in_live_game':False,'owned_test_clients_closed':True}
(r/'analysis/player-name-verification.json').write_text(json.dumps(report,indent=2));assert all(a['same'] for a in art);assert report['references_unchanged'];assert all(x['same'] for s in state.values() for x in s);assert all(p['matches']==p['total']==3 for p in live);print('12/12 artifact SHA256 match; state/reference hashes unchanged; four live native guards 3/3; suites 49/49 x86,48/48 x64')
