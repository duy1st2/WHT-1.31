from port3_research import *
import hashlib,datetime,shutil
specs=[(8740,12164,32,0xbb1ac0,0x3a25f0,0x927770),(32116,12173,32,0xb71060,0x380b00,0x8f4a30),(24984,12164,64,0xd10d80,0x40f370,0xa3ef60),(27336,12173,64,0xd4f820,0x477050,0xa8a560)]
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/no_cooldown129.hpp',root.parent/'WHT-1.29/native/version129.cpp']
evidence={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'profiles':[]}
backup=root/'analysis'/('before-no-cooldown-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for rel in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/rel,backup/Path(rel).name)
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct CooldownResetProfile131 {unsigned build;uintptr_t reset;const ResourceTextCode131* code;size_t count;};'];profiles=[]
for pid,build,bits,reset,ability,wrapper in specs:
 b,base,_=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 def instructions(a):
  ins=[]
  for i in d.disasm(b[a:a+1024],base+a):
   ins.append(i)
   if i.mnemonic=='ret':return ins
  raise RuntimeError(hex(a))
 funcs=[instructions(a) for a in [reset,ability,wrapper]]
 assert any(i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM and i.operands[0].imm==base+ability for i in funcs[0])
 assert any(i.mnemonic=='jmp' and i.operands[0].type==capstone.x86.X86_OP_IMM and i.operands[0].imm==base+reset for i in funcs[2])
 ranges=[(a,ins[-1].address+ins[-1].size-base-a) for a,ins in zip([reset,ability,wrapper],funcs)]
 relocs=[];pe=struct.unpack_from('<I',b,60)[0]
 if bits==32:
  rr,size=struct.unpack_from('<II',b,pe+24+96+40);a=rr
  while a<rr+size:
   page,n=struct.unpack_from('<II',b,a)
   if n<8:break
   for q in range(a+8,a+n,2):
    v=struct.unpack_from('<H',b,q)[0]
    if v>>12==3:relocs.append(page+(v&4095))
   a+=n
 tag=f'ncd{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[]
 for idx,(a,n) in enumerate(ranges):
  assert n<512
  data=bytearray(b[a:a+n]);rr=[r-a for r in relocs if a<=r and r+4<=a+n]
  for r in rr:struct.pack_into('<I',data,r,(struct.unpack_from('<I',data,r)[0]-base)&0xffffffff)
  name=f'{tag}_{idx}';out.append(f'inline constexpr unsigned char {name}[]={{'+','.join(hex(x) for x in data)+'};')
  if rr:out.append(f'inline constexpr unsigned {name}r[]={{'+','.join(str(x) for x in rr)+'};')
  entries.append('{'+f'0x{a:x},{name},sizeof({name}),'+(f'{name}r,{len(rr)}' if rr else 'nullptr,0')+'}')
 out.append(f'inline constexpr ResourceTextCode131 {tag}c[]={{'+','.join(entries)+'};');out.append('#endif')
 profiles.extend(['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+f'{build},0x{reset:x},{tag}c,sizeof({tag}c)/sizeof({tag}c[0])'+'},','#endif'])
 evidence['profiles'].append({'pid':pid,'build':build,'bits':bits,'ranges':ranges,'disassembly':[[f'{i.address-base:x} {i.mnemonic} {i.op_str}' for i in ins] for ins in funcs]})
out+=['inline constexpr CooldownResetProfile131 cooldownResetProfiles131[]={',*profiles,'};','}']
(root/'analysis/no-cooldown-native-evidence.json').write_text(json.dumps(evidence,indent=2),encoding='utf-8')
(root/'native/no_cooldown_profile131.hpp').write_text('\n'.join(out)+'\n',encoding='utf-8')
print([(x['build'],x['bits'],x['ranges']) for x in evidence['profiles']])
