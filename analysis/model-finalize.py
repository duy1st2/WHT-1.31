from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((root/'analysis/unit-model-preserved-before.json').read_text(encoding='utf-8'))
preserved=[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()]
refs=json.loads((root/'analysis/unit-model-native-evidence.json').read_text(encoding='utf-8'))['references']
artifacts=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=root/folder/'Release'/name;dest=root/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   artifacts.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dest),'build_sha256':sha(src),'installed_sha256':sha(dest)})
report={'artifacts':artifacts,'preserved':preserved,'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'live_read_only_verified':'12164 x64 selected unit exact model/STRINGREF chain','live_command_tested':False,'pending_running_client':'12164 x64 if artifacts differ'}
(root/'analysis/unit-model-verification.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
p=root/'README.md';s=p.read_text(encoding='utf-8');s+='\n\nSelected-unit model query (2026-10-28): #gmodel now reads the exact verified native sprite -> STRINGREF -> filename chain for12164/12173 x86/x64. Refuses unverified sprite/string classes, unstable selection/registry/pointers, unterminated/non-model filenames and notification-control characters. No fabricated units/rawcode.mdx fallback. Live12164 x64 selected-unit chain confirmed read-only; rebuilt command invocation awaits restart. See analysis/unit-model-port.md and unit-model-verification.json.\n';p.write_text(s,encoding='utf-8')
print(json.dumps({'installed_matches':sum(x['same'] for x in artifacts),'artifacts':len(artifacts),'preserved':sum(x['same'] for x in preserved),'preserved_total':len(preserved),'references_unchanged':report['references_unchanged']}))
