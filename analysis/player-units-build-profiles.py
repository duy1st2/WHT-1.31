from port3_research import *
import hashlib,datetime,shutil
specs=[(8740,12164,32,0xd60d80,0xd60e24,0xd60e30,0xd60fb0,0xba40e0,0xba4113,0xb20a10,0xb20a4a),(32116,12173,32,0xd1bd30,0xd1bdac,0xd1bdb0,0xd1bf50,0xb63730,0xb63753,0xae0220,0xae025a),(24984,12164,64,0xf08610,0xf086e8,0xf086f0,0xf088f0,0xd011a0,0xd011bf,0xc6ecd0,0xc6ed2c),(27336,12173,64,0xf44400,0xf444db,0xf444e0,0xf44700,0xd3fe10,0xd3fe2f,0xcb1c40,0xcb1c9c)]
e=json.loads((root/'analysis/player-abilities-reference-evidence.json').read_text());e['profiles']=[]
backup=root/'analysis'/('before-player-abilities-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/f,backup/Path(f).name)
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())]
(root/'analysis/player-abilities-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct PlayerUnitsProfile131 {unsigned build;uintptr_t enumerate;const ResourceTextCode131* code;size_t count;};'];rows=[]
for pid,build,bits,fn,end,helper,he,tag,te,caller,ce in specs:
 b,base,_=image(pid);peoff=struct.unpack_from('<I',b,60)[0];d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 calls=[i.operands[0].imm-base for i in d.disasm(b[caller:ce],base+caller) if i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM];assert fn in calls and tag in calls
 if bits==64:
  pr,ps=struct.unpack_from('<II',b,peoff+24+112+24);pdata=[struct.unpack_from('<III',b,a) for a in range(pr,pr+ps,12)]
  def r(t):
   for _ in range(30):
    u=t[2]
    if not b[u]&32:return t
    t=struct.unpack_from('<III',b,u+4+((b[u+2]+1)&~1)*2)
   raise ValueError('unwind')
  end=max(t[1] for t in pdata if r(t)[0]==fn);he=max(t[1] for t in pdata if r(t)[0]==helper)
 ranges=[(fn,end),(helper,he),(tag,te),(caller,ce)];rr,size=struct.unpack_from('<II',b,peoff+24+(96 if bits==32 else 112)+40);a=rr;rel=[];r64=[]
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0];rv=page+(v&4095)
   if v>>12==3:rel.append(rv)
   if v>>12==10:r64.append(rv)
  a+=n
 assert not any(s<=rv<e for rv in r64 for s,e in ranges)
 name=f'pu{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[];blocks=[]
 for start,finish in ranges:
  a=start
  while a<finish:
   stop=min(a+384,finish)
   for rv in rel:
    if a<=rv<stop and rv+4>stop:stop=rv
   data=bytearray(b[a:stop]);rs=[rv-a for rv in rel if a<=rv and rv+4<=stop]
   for rv in rs:struct.pack_into('<I',data,rv,(struct.unpack_from('<I',data,rv)[0]-base)&0xffffffff)
   n=f'{name}_{len(entries)}';out.append(f'inline constexpr unsigned char {n}[]={{'+','.join(hex(x) for x in data)+'};')
   if rs:out.append(f'inline constexpr unsigned {n}r[]={{'+','.join(str(x) for x in rs)+'};')
   entries.append('{'+f'0x{a:x},{n},sizeof({n}),'+(f'{n}r,{len(rs)}' if rs else 'nullptr,0')+'}');blocks.append((a,stop-a));a=stop
 out.append(f'inline constexpr ResourceTextCode131 {name}c[]={{'+','.join(entries)+'};');out.append('#endif');rows+=['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+f'{build},0x{fn:x},{name}c,sizeof({name}c)/sizeof({name}c[0])'+'},','#endif'];e['profiles'].append({'pid':pid,'bits':bits,'build':build,'enumerate':fn,'ranges':ranges,'blocks':blocks})
out+=['inline constexpr PlayerUnitsProfile131 playerUnitsProfiles131[]={',*rows,'};','}'];(root/'native/player_units_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/player-abilities-native-evidence.json').write_text(json.dumps(e,indent=2));print(e['profiles'])

