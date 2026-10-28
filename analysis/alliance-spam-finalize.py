from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
p=r/'README.md';s=p.read_text(encoding='utf-8-sig');s=s.replace('`r`nAlliance spam single target','\n\nAlliance spam single target');p.write_text(s,encoding='utf-8-sig')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((r/'analysis/alliance-spam-preserved-before.json').read_text());refs=json.loads((r/'analysis/alliance-spam-evidence.json').read_text())['references'];art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'sha256':sha(dst)})
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'tests':{'x86':45,'x64':44},'abi_tests_after_final_test_edit':{'x86':'passed','x64':'passed'},'runtime_enabled':['#spam'],'end_to_end_tested':False,'packets_sent_to_game':False,'remaining':['#spamall: minimap action mapping','#wide: native perspective mapping','#findCP: no real detector in WHT references']}
(r/'analysis/alliance-spam-verification.json').write_text(json.dumps(report,indent=2));assert all(a['same'] for a in art);assert report['references_unchanged'];assert all(a['same'] for a in report['preserved']);print('12 artifacts match; all reference and preserved file hashes unchanged')
