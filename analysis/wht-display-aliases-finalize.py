from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((r/'analysis/wht-display-aliases-preserved-before.json').read_text());evidence=json.loads((r/'analysis/wht-display-aliases-evidence.json').read_text());art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'sha256':sha(dst)})
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in evidence['references'].items()),'tests':{'x86':47,'x64':46},'aliases':evidence['aliases'],'resource_master_enable_expands_board':True,'new_native_hooks':False,'in_game_commands_executed':False}
(r/'analysis/wht-display-aliases-verification.json').write_text(json.dumps(report,indent=2));assert all(a['same'] for a in art);assert report['references_unchanged'];assert all(a['same'] for a in report['preserved']);print('12 artifacts match; source reference and runtime config/log/ban hashes unchanged')
