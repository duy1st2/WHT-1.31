from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((r/'analysis/spam-all-preserved-before.json').read_text());refs=json.loads((r/'analysis/spam-all-evidence.json').read_text())['references'];art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'sha256':sha(dst)})
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'tests':{'x86':46,'x64':45},'runtime_enabled':['#spamall'],'end_to_end_tested':False,'packets_sent_to_game':False,'remaining':['#wide: native perspective mapping','#findCP: no real detector in WHT references']}
(r/'analysis/spam-all-verification.json').write_text(json.dumps(report,indent=2));assert all(a['same'] for a in art);assert report['references_unchanged'];assert all(a['same'] for a in report['preserved']);print('12 artifacts match; reference and preserved file hashes unchanged')
