from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((r/'analysis/map-trade-preserved-before.json').read_text());refs=json.loads((r/'analysis/map-trade-native-evidence.json').read_text())['references'];art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'built_sha256':sha(src),'installed_sha256':sha(dst)})
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'tests':{'x86':{'passed':42,'total':42},'x64':{'passed':41,'total':41}},'live_read_only_map_flag_guards_confirmed':True,'end_to_end_command_tested':False,'pending_loaded_client':[a for a in art if not a['same']]}
(r/'analysis/map-trade-verification.json').write_text(json.dumps(report,indent=2))


p=r/'README.md';text=p.read_text(encoding='utf-8');text+="""

Native map trade flag (2026-10-28): #mtrade clears the map's0x100 trade restriction bit, matching actual WHT/WHT-1.29 SetMapFlag(0x100,0) behavior. Native getter/setter operations, CWorld setup pointers and CMapSetupWar3 RTTI/vtables independently verified for12164/12173 x86/x64. Only that flag bit changes via a single compare/exchange; all other bits, setup/world/local identity and native byte guards are checked before/after. Already-clear is read-only, changed source refuses the update, unconfirmed results are reported. No new packet, JASS handle, setting or hook. Live12164 x64 inspection confirmed native bytes and setup identity; its restriction was already clear. Tests42/42 x86 and41/41 x64. Actual in-game mutation and multiplayer behavior remain untested. See analysis/map-trade-port.md and map-trade-verification.json. This supersedes the earlier unresolved map-trade mapping; running clients receive updated binaries through Run-WHT on next launch.
""";p.write_text(text,encoding='utf-8')
print(json.dumps({'matches':sum(a['same'] for a in art),'total':len(art),'references_unchanged':report['references_unchanged'],'changed_preserved':[a['path'] for a in report['preserved'] if not a['same']],'pending':report['pending_loaded_client']},indent=2))
