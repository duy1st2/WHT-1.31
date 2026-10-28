from pathlib import Path
import hashlib,json,re
r=Path(__file__).resolve().parents[1]
refs=[r.parent/'WHT/native/gameplay_commands.hpp',r.parent/'WHT-1.29/native/gameplay_commands.hpp',r.parent/'WHT/native/runtime.cpp']
def commands(p): return set(re.findall(r'"(#[a-zA-Z0-9_]+)"',p.read_text(encoding='utf-8-sig')))
a,b=map(commands,refs[:2]);active=commands(r/'native/version131.cpp')|commands(r/'native/version64.cpp')
for p in (r/'native').glob('*131.hpp'):active|=commands(p)
remaining=sorted((a&b)-active)
report={'date':'2026-10-28','scope':'Common literal gameplay commands; not a claim of complete behavioral or gameplay parity','remaining':remaining,'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'findings':{'#findCP':'Both return a fixed None detected message without detection.','#spam':'WHT has a runtime sender. WHT-1.29 only assigns/resets spamSlot and nextSpam; no execution consumer found.','#spamall':'WHT has a runtime sender. WHT-1.29 only toggles/resets spamAll and nextSpam; no execution consumer found.','#wide':'WHT has a runtime graphics policy. WHT-1.29 toggles an otherwise unused widescreen field; config wide has no rendering consumer found.'},'runtime_changed':False,'next_validation':['In-match #mh enable/disable and teardown','Existing native gameplay features with explicitly pending live validation'],'menu_inventory_evidence':'menu-disable-audit.json:239 interactive controls, zero capability-blocked after fix; parent dependencies can disable controls'}
(r/'analysis/remaining-common-commands.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'remaining':remaining,'runtime_changed':False},indent=2))
