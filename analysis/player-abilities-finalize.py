from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((r/'analysis/player-abilities-preserved-before.json').read_text());refs=json.loads((r/'analysis/player-abilities-native-evidence.json').read_text())['references'];art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'built_sha256':sha(src),'installed_sha256':sha(dst)})
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'tests':{'x86':{'passed':41,'total':41},'x64':{'passed':40,'total':40}},'live_read_only_iterator_guards_confirmed':True,'end_to_end_command_tested':False,'pending_loaded_client':[a for a in art if not a['same']]}
(r/'analysis/player-abilities-verification.json').write_text(json.dumps(report,indent=2))

p=r/'README.md';text=p.read_text(encoding='utf-8');text+="""

All owned-unit abilities (2026-10-28): #addall rawcode adds an absent ability at level1 to the local player's snapshotted units; existing levels stay unchanged. #raddall rawcode removes it, accepting already absent abilities. Both accept four-byte ASCII or0xXXXXXXXX rawcodes. Native global CUnit registry enumeration is independently mapped for12164/12173 x86/x64 through actual engine call sites; no selected-unit fallback, render list, JASS group/handle or new network packet. Collection and complete preflight precede all mutations. Registry/world/player/physical ownership checks and confirmed partial-result reporting are retained before/after each update; failure stops without retry or rollback. Maximum8192 owned units and65536 visited objects; new units after snapshot are excluded. Runtime Local/settings authorization applies. Tests41/41 x86 and40/40 x64; current12164 x64 iterator guards matched live memory. Actual all-unit ability updates and multiplayer synchronization remain untested. See analysis/player-abilities-port.md and player-abilities-verification.json. Running clients receive the update through Run-WHT on next launch. This supersedes earlier pending #addall/#raddall notes.
""";p.write_text(text,encoding='utf-8')
print(json.dumps({'matches':sum(a['same'] for a in art),'total':len(art),'references_unchanged':report['references_unchanged'],'changed_preserved':[a['path'] for a in report['preserved'] if not a['same']],'pending':report['pending_loaded_client']},indent=2))
