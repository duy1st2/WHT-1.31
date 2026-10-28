from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((root/'analysis/create-copy-unit-preserved-before.json').read_text(encoding='utf-8'))
refs=json.loads((root/'analysis/create-unit-native-evidence.json').read_text(encoding='utf-8'))['references']
artifacts=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=root/folder/'Release'/name;dest=root/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   artifacts.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dest),'build_sha256':sha(src),'installed_sha256':sha(dest)})
report={'artifacts':artifacts,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'live_spawn_tested':False,'pending_loaded_client':[x for x in artifacts if not x['same']]}
(root/'analysis/create-copy-unit-verification.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
p=root/'README.md';s=p.read_text(encoding='utf-8');s+='''

Native create/copy units (2026-10-28): #create rawcode [1..50] creates fresh local-player units at cursor,40 units apart. #copy [1..50] creates fresh units of the selected type beside source,50 units apart. Default count1; no level, inventory or current-state duplication. 12164/12173 x86/x64 raw native workers independently mapped through actual CreateUnit wrappers; physical owner getter and native degree-to-radian scalar guarded. Local/settings authorization, stable world/player/source, native registry/type/owner/position confirmation and batch reentry limit retained. No item fallback, fabricated counts, rollback/delete, new hook, packet or INI setting. Failed batches stop and report confirmed fraction; latest native invocation may already have created a unit. Full profile/ASLR fixture and mock ABI/flow tests completed; actual game creation and multiplayer synchronization remain untested. See analysis/create-copy-unit-port.md and create-copy-unit-verification.json. Running clients receive the new build through Run-WHT on next launch.
''';p.write_text(s,encoding='utf-8')
print(json.dumps({'matches':sum(x['same'] for x in artifacts),'total':len(artifacts),'preserved':sum(x['same'] for x in report['preserved']),'preserved_total':len(before),'references_unchanged':report['references_unchanged'],'pending':[{'build':x['build'],'arch':x['arch'],'file':x['file']} for x in report['pending_loaded_client']]}))
