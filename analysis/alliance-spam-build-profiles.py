from port3_research import *
import hashlib,datetime,shutil
backup=root/'analysis'/('before-alliance-spam-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md','AGENTS.md']:shutil.copy2(root/f,backup/Path(f).name)
refs=[root.parent/'WHT/native/runtime.cpp',root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp']
e={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'scope':'User permits WHT-only functionality when WHT-1.29 has no implementation. Port #spam single target; #spamall pings and #wide separate.'}
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())]
(root/'analysis/alliance-spam-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {'];rows=[];profiles=[]
for pid,build,bits,ctor,serializer,n in [(8740,12164,32,0xa8099d,0xaa1b00,39),(32116,12173,32,0xa4112d,0xa60ac0,39),(24984,12164,64,0xbb52c0,0xbd9610,50),(27336,12173,64,0xbfd290,0xc200e0,50)]:
 b,base,_=image(pid);name=f'allianceSpam{bits}_{build}';out.append('#if '+('defined' if bits==64 else '!defined')+'(_WIN64)')
 spans=[(ctor,18),(serializer,n)]
 for j,(a,size) in enumerate(spans):out.append(f'inline constexpr unsigned char {name}_{j}[]={{'+','.join(hex(x) for x in b[a:a+size])+'};')
 out.append(f'inline constexpr ResourceTextCode131 {name}[]={{'+','.join('{'+f'0x{a:x},{name}_{j},sizeof({name}_{j}),nullptr,0'+'}' for j,(a,size) in enumerate(spans))+'};');out.append('#endif')
 rows+=['#if '+('defined' if bits==64 else '!defined')+'(_WIN64)',f'{{{build},0,0,0,0,0,{name},2}},','#endif']
 profiles.append({'build':build,'bits':bits,'constructor':hex(ctor),'serializer':hex(serializer),'capture_pid':pid,'wire':'50, uint8 target, uint32 LE flags; 6 bytes'})
out+=['inline constexpr ResourceTextProfile131 allianceSpamProfiles131[]={']+rows+['};','}'];(root/'native/alliance_spam_profile131.hpp').write_text('\n'.join(out)+'\n');e['profiles']=profiles;(root/'analysis/alliance-spam-evidence.json').write_text(json.dumps(e,indent=2))
