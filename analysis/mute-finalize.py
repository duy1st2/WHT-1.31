from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((root/'analysis/mute-chat-preserved-before.json').read_text(encoding='utf-8'))
refs=json.loads((root/'analysis/mute-chat-native-evidence.json').read_text(encoding='utf-8'))['references']
artifacts=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=root/folder/'Release'/name;dest=root/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   artifacts.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dest),'build_sha256':sha(src),'installed_sha256':sha(dest)})
report={'artifacts':artifacts,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'live_mute_tested':False}
(root/'analysis/mute-chat-verification.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
p=root/'README.md';s=p.read_text(encoding='utf-8');s+='''

Player chat mute (2026-10-28): #mute <1..24> / #unmute <1..24> now filter local onscreen chat and chat history on12164/12173 x86/x64. Self and system messages remain visible. The native screen function takes sender/text/channel/duration while history has no duration; independently verified native callers and full screen bodies are guarded before hook installation. Existing history observer still runs before filtering. Atomic per-match masks clear on world/local change, authorization/settings loss or UI teardown. No INI persistence, packet changes or remote moderation. Tests exercise full captured profile guards,24-player masks/context reset and actual detour/trampoline argument forwarding on owned mock functions only. Live game filtering not yet tested. See analysis/mute-chat-port.md and mute-chat-verification.json.
''';p.write_text(s,encoding='utf-8')
print(json.dumps({'matches':sum(x['same'] for x in artifacts),'total':len(artifacts),'preserved':sum(x['same'] for x in report['preserved']),'preserved_total':len(before),'references_unchanged':report['references_unchanged']}))
