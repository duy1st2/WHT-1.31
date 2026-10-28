from port3_research import *
import hashlib,datetime,shutil
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct UnitModelProfile131 {unsigned build;uintptr_t modelTable,stringTable,modelOffset,nameOffset,stringOffset;const ResourceTextCode131* code;size_t count;};']
specs=[(8740,12164,32,0x18614bc,0x186197c,0xf1d1b0,0xf0782a,0xf1dd10,0xbc0f20),(32116,12173,32,0x18163dc,0x181689c,0xedc180,0xec680a,0xedcce0,0xb80210),(24984,12164,64,0x1c720a0,0x1c736d8,0x10fe480,0x10e5a9d,0x10ff3a0,0xd22f70),(27336,12173,64,0x1ca0fb0,0x1ca25e8,0x113aee0,0,0,0xd618c0)]
records=[]
for pid,build,bits,mt,st,get,call,factory,modelget in specs:
 b,base,_=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 if not call:
  hits=[]
  for m in re.finditer(b'\x48\x89\x86\x98\x00\x00\x00',b[0x1110000:0x1130000]):
   at=0x1110000+m.start()
   if b[at-5]==0xe8:hits.append(at-8)
  assert len(hits)==1,hits
  call=hits[0];factory=call+8+struct.unpack_from('<i',b,call+4)[0]
 def full(a):
  for i in d.disasm(b[a:a+512],base+a):
   if i.mnemonic=='ret':return i.address+i.size-base-a
  raise RuntimeError(hex(a))
 ranges=[(get,full(get)),(modelget,full(modelget)),(call,9 if bits==32 else 15),(factory,full(factory))]
 relocs=[]
 if bits==32:
  pe=struct.unpack_from('<I',b,60)[0];rva,size=struct.unpack_from('<II',b,pe+24+96+40);at=rva
  while at<rva+size:
   page,n=struct.unpack_from('<II',b,at)
   if n<8:break
   for q in range(at+8,at+n,2):
    v=struct.unpack_from('<H',b,q)[0]
    if v>>12==3:relocs.append(page+(v&4095))
   at+=n
 tag=f'um{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[]
 for idx,(a,n) in enumerate(ranges):
  data=bytearray(b[a:a+n]);rr=[r-a for r in relocs if a<=r and r+4<=a+n]
  for r in rr:struct.pack_into('<I',data,r,(struct.unpack_from('<I',data,r)[0]-base)&0xffffffff)
  name=f'{tag}_{idx}';out.append(f'inline constexpr unsigned char {name}[]={{'+','.join(hex(x) for x in data)+'};')
  if rr:out.append(f'inline constexpr unsigned {name}r[]={{'+','.join(str(x) for x in rr)+'};')
  entries.append('{'+f'0x{a:x},{name},sizeof({name}),'+(f'{name}r,{len(rr)}' if rr else 'nullptr,0')+'}')
 out.append(f'inline constexpr ResourceTextCode131 {tag}c[]={{'+','.join(entries)+'};');out.append('#endif')
 records.append(dict(pid=pid,build=build,bits=bits,model_table=hex(mt),string_table=hex(st),factory=hex(factory),ranges=ranges))
out.append('inline constexpr UnitModelProfile131 unitModelProfiles131[]={')
for r,s in zip(records,specs):
 pid,build,bits,mt,st,get,call,factory,modelget=s;tag=f'um{bits}_{build}'
 out.extend(['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+f'{build},0x{mt:x},0x{st:x},'+('0x48,0x98,0x38' if bits==64 else '0x28,0x58,0x1c')+f',{tag}c,sizeof({tag}c)/sizeof({tag}c[0])'+'},','#endif'])
out.extend(['};','}'])
(root/'analysis/unit-model-native-evidence.json').write_text(json.dumps({'profiles':records,'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp']}},indent=2),encoding='utf-8')
(root/'native/unit_model_profile131.hpp').write_text('\n'.join(out)+'\n',encoding='utf-8')
print(json.dumps(records,indent=2))
