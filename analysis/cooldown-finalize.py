from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((root/'analysis/no-cooldown-preserved-before.json').read_text(encoding='utf-8'))
refs=json.loads((root/'analysis/no-cooldown-native-evidence.json').read_text(encoding='utf-8'))['references']
artifacts=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=root/folder/'Release'/name;dest=root/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   artifacts.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dest),'build_sha256':sha(src),'installed_sha256':sha(dest)})
report={'artifacts':artifacts,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'live_cooldown_tested':False}
(root/'analysis/no-cooldown-verification.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
p=root/'README.md';s=p.read_text(encoding='utf-8');s+='''

Selected-unit native cooldown reset (2026-10-28): #nocd invokes verified native CUnit reset once, then repeats every100ms for the current selection; #yescd disables without invoking game code. 12164/12173 x86/x64 profiles independently mapped and full unit/ability/wrapper blocks guarded. Registry/world/local/unit serial checked before/after native callbacks. Existing Local authorization/settings retained; menu/chat/focus/macro/competing feature states pause. Authorization/UI loss disables, native/selection failure stops once without repeated retries. No new hooks, packets, raw cooldown writes or INI settings. Tests cover captured profile guards, identity changes,100ms cadence, pause/resume/no catch-up burst, disable, failure/no retry and reentrant teardown. Direct in-game cooldown behavior remains untested. See analysis/no-cooldown-port.md and no-cooldown-verification.json.
''';p.write_text(s,encoding='utf-8')
print(json.dumps({'matches':sum(x['same'] for x in artifacts),'total':len(artifacts),'preserved':sum(x['same'] for x in report['preserved']),'preserved_total':len(before),'references_unchanged':report['references_unchanged']}))
