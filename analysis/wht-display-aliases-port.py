from pathlib import Path
import hashlib,json,datetime,shutil
r=Path('WHT-1.31').resolve();stamp=datetime.datetime.now().strftime('%Y%m%d-%H%M%S');backup=r/'analysis'/('before-wht-display-aliases-'+stamp);backup.mkdir()
for f in ['native/command_aliases131.hpp','README.md']:shutil.copy2(r/f,backup/Path(f).name)
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
ref=r.parent/'WHT/native/runtime.cpp';text=ref.read_text(encoding='utf-8-sig').splitlines();evidence={'references':{str(ref):sha(ref)},'source_lines':{str(i+1):line.strip() for i,line in enumerate(text) if ('overlayKey=' in line and any(t in line for t in ['#trade','#mem','#showregen']))},'aliases':{'#mem':'#showmem','#memory':'#showmem','#ram':'#showmem','#usemem':'#showmem','#trade':'#res','#showregen':'#regenhp','#regen':'#regenhp'},'scope':'Alternate WHT command names for already implemented features; no new native implementation.'};(r/'analysis/wht-display-aliases-evidence.json').write_text(json.dumps(evidence,indent=2))
paths=[p for folder in (r/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())];(r/'analysis/wht-display-aliases-preserved-before.json').write_text(json.dumps({str(p):sha(p) for p in paths},indent=2))
p=r/'native/command_aliases131.hpp';s=p.read_text(encoding='utf-8-sig');marker='  {L"#showcooldown",L"#cooldown"}';assert marker in s;s=s.replace(marker,'  {L"#mem",L"#showmem"},{L"#memory",L"#showmem"},\n  {L"#ram",L"#showmem"},{L"#usemem",L"#showmem"},\n  {L"#trade",L"#res"},{L"#showregen",L"#regenhp"},{L"#regen",L"#regenhp"},\n'+marker,1);p.write_text(s,encoding='utf-8-sig')
p=r/'README.md';s=p.read_text(encoding='utf-8-sig');s+='''

WHT display command compatibility (2026-10-28): #mem/#memory/#ram/#usemem now use the existing #showmem handler, #trade uses #res, and #showregen/#regen use #regenhp. Bare commands toggle; 0 disables and 1 enables. Both dispatch and feedback resolve the same canonical name on all four 1.31 profiles, retaining existing authorization, parsing, persistence and display code. These are alternate WHT command names for existing features, not new native implementations. No native offsets, hooks or frames were changed. Source evidence: analysis/wht-display-aliases-evidence.json.
''';p.write_text(s,encoding='utf-8-sig')
print(json.dumps(evidence['aliases'],indent=2))
