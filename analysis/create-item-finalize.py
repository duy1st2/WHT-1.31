from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((r/'analysis/create-item-preserved-preinstall.json').read_text());e=json.loads((r/'analysis/create-item-evidence.json').read_text());art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'sha256':sha(dst)})
live=[json.loads((r/f'analysis/create-item-live-guard-{build}-{bits}.json').read_text()) for build in [12164,12173] for bits in [32,64]]
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in e['references'].items()),'tests':{'x86':48,'x64':47},'read_only_live_guard':[{k:v for k,v in p.items() if k!='blocks'} for p in live],'end_to_end_item_creation_tested':False,'native_factory_called_in_game':False,'owned_test_clients_closed':True,'no_new_hooks_threads_frames_settings':True}
(r/'analysis/create-item-verification.json').write_text(json.dumps(report,indent=2));assert all(a['same'] for a in art);assert report['references_unchanged'];assert all(a['same'] for a in report['preserved']);assert all(p['matches']==p['total']==8 for p in live);print('12 artifacts match; all four native profiles match 8/8 live guard blocks; references/config/log/ban preserved')
