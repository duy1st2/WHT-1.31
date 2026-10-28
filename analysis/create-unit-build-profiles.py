from port3_research import *
import hashlib,datetime,shutil
specs=[(8740,12164,32,0xbbeb60,0xbbecaf,0x8d25a0,0x8d262a,0x6c5dd0,0x1806950),(32116,12173,32,0xb7df30,0xb7e06b,0x8a12c0,0x8a134a,0x6889a0,0x17bb980),(24984,12164,64,0xd20630,0,0x9e54d0,0x9e5585,0x782560,0x1bb2f48),(27336,12173,64,0xd5f160,0,0xa31400,0xa314b5,0x7dd220,0x1be1f20)]
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/create_unit129.hpp',root.parent/'WHT-1.29/native/copy_unit129.hpp',root.parent/'WHT-1.29/native/version129.cpp']
evidence={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'profiles':[]}
backup=root/'analysis'/('before-create-copy-unit-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for rel in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/rel,backup/Path(rel).name)
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct CreateUnitProfile131 {unsigned build;uintptr_t create,angleScale,ownerGetter;const ResourceTextCode131* code;size_t count;};'];profiles=[]
for pid,build,bits,start,end,wrapper,we,owner,table in specs:
 b,base,_=image(pid);pe=struct.unpack_from('<I',b,60)[0];d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 ins=list(d.disasm(b[wrapper:we],base+wrapper));assert any(i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM and i.operands[0].imm==base+start for i in ins)
 w=bits//8;assert int.from_bytes(b[table+(0x64 if bits==32 else 0xc8):table+(0x64 if bits==32 else 0xc8)+w],'little')==base+owner
 if bits==32:angle=struct.unpack_from('<I',b,wrapper+0x4d)[0]-base
 else:
  i=next(i for i in ins if i.mnemonic=='lea' and i.op_str.startswith('r8, [rip'))
  angle=i.address+i.size-base+i.disp
  pr,size=struct.unpack_from('<II',b,pe+24+112+24);pdata=[struct.unpack_from('<III',b,q) for q in range(pr,pr+size,12)]
  def rootentry(t):
   for _ in range(30):
    u=t[2]
    if not b[u]&32:return t
    t=struct.unpack_from('<III',b,u+4+((b[u+2]+1)&~1)*2)
   raise RuntimeError('unwind chain')
  end=max(t[1] for t in pdata if rootentry(t)[0]==start)
 scale=struct.unpack_from('<f',b,angle)[0];assert abs(scale-0.0174532925)<1e-8,(pid,hex(angle),scale)
 relocs=[];rr,size=struct.unpack_from('<II',b,pe+24+(96 if bits==32 else 112)+40);a=rr;dir64=[]
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0];r=page+(v&4095)
   if v>>12==3:relocs.append(r)
   if v>>12==10:dir64.append(r)
  a+=n
 ranges=[(start,end),(wrapper,we),(owner,owner+5),(angle,angle+4)];assert not any(s<=r<e for r in dir64 for s,e in ranges)
 tag=f'cu{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[];blocks=[]
 for begin,finish in ranges:
  a=begin
  while a<finish:
   stop=min(a+384,finish)
   for r in relocs:
    if a<=r<stop and r+4>stop:stop=r
   data=bytearray(b[a:stop]);rr=[r-a for r in relocs if a<=r and r+4<=stop]
   for r in rr:struct.pack_into('<I',data,r,(struct.unpack_from('<I',data,r)[0]-base)&0xffffffff)
   name=f'{tag}_{len(entries)}';out.append(f'inline constexpr unsigned char {name}[]={{'+','.join(hex(x) for x in data)+'};')
   if rr:out.append(f'inline constexpr unsigned {name}r[]={{'+','.join(str(x) for x in rr)+'};')
   entries.append('{'+f'0x{a:x},{name},sizeof({name}),'+(f'{name}r,{len(rr)}' if rr else 'nullptr,0')+'}');blocks.append((a,stop-a));a=stop
 out.append(f'inline constexpr ResourceTextCode131 {tag}c[]={{'+','.join(entries)+'};');out.append('#endif')
 profiles.extend(['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+f'{build},0x{start:x},0x{angle:x},0x{owner:x},{tag}c,sizeof({tag}c)/sizeof({tag}c[0])'+'},','#endif'])
 evidence['profiles'].append({'pid':pid,'build':build,'bits':bits,'entry':hex(start),'end':hex(end),'wrapper':hex(wrapper),'angle_scale':scale,'angle_rva':hex(angle),'owner_getter':hex(owner),'blocks':blocks})
out+=['inline constexpr CreateUnitProfile131 createUnitProfiles131[]={',*profiles,'};','}']
(root/'analysis/create-unit-native-evidence.json').write_text(json.dumps(evidence,indent=2),encoding='utf-8')
(root/'native/create_unit_profile131.hpp').write_text('\n'.join(out)+'\n',encoding='utf-8');print(evidence['profiles'])
