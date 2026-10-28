from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
before=json.loads((root/'analysis/courier-aliases-preserved-before.json').read_text(encoding='utf-8'))
refs=json.loads((root/'analysis/courier-aliases-reference.json').read_text(encoding='utf-8'))['references']
artifacts=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=root/folder/'Release'/name;dest=root/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name
   artifacts.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dest),'build_sha256':sha(src),'installed_sha256':sha(dest)})
report={'artifacts':artifacts,'preserved':[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()],'references_unchanged':all(sha(Path(p))==h for p,h in refs.items()),'live_route_tested':False}
(root/'analysis/courier-aliases-verification.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
text='''

Courier route command aliases (2026-10-28): added #chayga [1..3], #chaysoi [1..3] and #caysoi [1..3] from both actual reference gameplay_commands.hpp handlers. These are additional names for existing chicken/wolf route workers, not new movement behavior. Default index1; parsed out-of-range index falls back1 like references. Malformed/extra/NUL/line-break arguments are refused before worker access. Existing canonical commands retain their no-argument grammar. MTH master, authorization/settings, native point-order profile, living owned movable courier, concurrent action and original identity checks remain on the same path. Route count comes from the corresponding saved MTH slot. Evidence and backups recorded before edits. Parser mappings/invalid inputs plus existing route worker/native contracts tested on both architectures; no new native mapping, packet, hook or config write. Live courier behavior not tested. See analysis/courier-aliases-verification.json for installation/config/reference hashes.
'''
p=root/'README.md';p.write_text(p.read_text(encoding='utf-8')+text,encoding='utf-8')
(root/'analysis/courier-aliases-port.md').write_text('# Courier route aliases\n'+text,encoding='utf-8')
print(json.dumps({'matches':sum(x['same'] for x in artifacts),'total':len(artifacts),'preserved':sum(x['same'] for x in report['preserved']),'preserved_total':len(before),'references_unchanged':report['references_unchanged']}))
