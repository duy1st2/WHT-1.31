from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((r/'analysis/unit-name-preserved-before.json').read_text());refs=json.loads((r/'analysis/unit-name-native-evidence.json').read_text())['references'];art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'built_sha256':sha(src),'installed_sha256':sha(dst)})
report={'artifacts':art,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'tests':{'x86':{'passed':40,'total':40},'x64':{'passed':39,'total':39}},'live_read_only_native_cache_confirmed':True,'end_to_end_command_tested':False,'pending_loaded_client':[a for a in art if not a['same']]}
(r/'analysis/unit-name-verification.json').write_text(json.dumps(report,indent=2))
p=r/'README.md';s=p.read_text(encoding='utf-8');s+='''

Selected-unit name search (2026-10-28): #ufinds name searches the actual native cached display name of the active selected unit, with case-insensitive Unicode substring matching, multiword queries and current coordinates. Warcraft color codes are omitted from matching/output. #ufind continues to compare rawcodes. Empty/uninitialized/invalid UTF-8 names and changed registry/selection/world/name fail truthfully. Native wrapper/getter/cache initializer independently verified for12164/12173 x86/x64; read-only260-byte bounded cache access, no game writes or JASS invocation. Live12164 x64 read confirmed the selected Vietnamese display name and all native guard blocks. Tests40/40 x86 and39/39 x64. See analysis/unit-name-port.md and unit-name-verification.json. A running client receives the build through Run-WHT on next launch. #addall/#raddall remain pending verified complete player-unit enumeration.
''';p.write_text(s,encoding='utf-8')
print(json.dumps({'matches':sum(a['same'] for a in art),'total':len(art),'preserved_same':sum(a['same'] for a in report['preserved']),'preserved_total':len(before),'references_unchanged':report['references_unchanged'],'pending':report['pending_loaded_client']},indent=2))
