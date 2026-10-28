import sys,struct,json
from pathlib import Path
sys.path.insert(0,'WHT-1.31/analysis/python-tools');import capstone as c
specs=[(32,12164,'14740',0x1d0000,[0xbabfa0,0xbac0c0,0xbac240,0xf46790,0xbabb90,0xbacb80]),(32,12173,'23884',0x270000,[0xb6b410,0xb6b530,0xb6b6b0,0xf05750,0xb6b000,0xb6bfe0]),(64,12164,'6688',0x7ff782f60000,[0xd096e0,0xd09860,0xd09ac0,0x112c110,0xd09100,0xd0a730]),(64,12173,'29928',0x7ff626470000,[0xd481a0,0xd48320,0xd48580,0x1168b70,0xd47bc0,0xd491b0])]
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct HeroLevelProfile131 {unsigned build;uintptr_t level,required,experience,real,add,strip;const ResourceTextCode131* code;size_t count;};'];meta=[];logs=[]
for bits,build,pid,base,rv in specs:
 d=Path('WHT-1.31/analysis/memory-'+pid+'.bin').read_bytes();m=c.Cs(c.CS_ARCH_X86,c.CS_MODE_64 if bits==64 else c.CS_MODE_32);m.detail=True
 def cfg(a):
  todo=[a];seen={};ends=[]
  while todo:
   q=todo.pop()
   while q not in seen:
    assert a<=q<a+0x4000,(hex(a),hex(q))
    i=next(m.disasm(d[q:q+16],base+q,count=1));seen[q]=i;q+=i.size
    if i.group(c.CS_GRP_JUMP):
     if i.operands[0].type==c.x86.X86_OP_IMM:
      t=i.operands[0].imm-base
      if a<=t<a+0x4000:todo.append(t)
     if i.mnemonic=='jmp':break
    if i.group(c.CS_GRP_RET):break
  return max(q+i.size for q,i in seen.items())-a
 funcs=list(rv)
 # Wrapper tail targets are the hero implementations.
 for a in [rv[1],rv[4],rv[5]]:
  for i in m.disasm(d[a:a+cfg(a)],base+a):
   if i.mnemonic=='jmp' and i.operands[0].type==c.x86.X86_OP_IMM:
    t=i.operands[0].imm-base
    if t<a or t>a+0x4000:funcs.append(t)
 name=f'hl{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[];info=[]
 for a in funcs:
  n=cfg(a);info.append([hex(a),n]);logs.append(f'\n{bits} {build} {a:x} length {n:x}')
  ins=list(m.disasm(d[a:a+n],base+a));logs.extend(f'{i.address-base:x} {i.mnemonic} {i.op_str}' for i in ins)
  # Chunk contiguous complete instructions, including non-executed padding within CFG bounds.
  for off in range(0,n,384):
   data=bytearray(d[a+off:a+min(off+384,n)]);rel=[]
   if bits==32:
    # PE base relocation directory gives authoritative absolute relocations, including operands crossing chunk edges.
    pe=struct.unpack_from('<I',d,0x3c)[0];rr,sz=struct.unpack_from('<II',d,pe+24+96+5*8);pos=rr
    while pos<rr+sz:
     page,size=struct.unpack_from('<II',d,pos)
     if size<8:break
     for k in range(pos+8,pos+size,2):
      v=struct.unpack_from('<H',d,k)[0];r=page+(v&4095)
      if v>>12==3 and a+off<=r<a+off+len(data):
       assert r+4<=a+off+len(data),'split relocation'
       at=r-a-off;val=struct.unpack_from('<I',data,at)[0];struct.pack_into('<I',data,at,(val-base)&0xffffffff);rel.append(at)
     pos+=size
   idx=len(entries);bn=f'{name}b{idx}';rn=f'{name}r{idx}';out.append('inline constexpr unsigned char '+bn+'[]={'+','.join(hex(x) for x in data)+'};')
   if rel:out.append('inline constexpr unsigned '+rn+'[]={'+','.join(map(str,rel))+'};')
   entries.append('{'+hex(a+off)+','+bn+',sizeof('+bn+'),'+(rn+','+str(len(rel)) if rel else 'nullptr,0')+'}')
 out.append('inline constexpr ResourceTextCode131 '+name+'[]={'+','.join(entries)+'};');out.append('#endif');meta.append((bits,build,rv,name,info))
out.append('inline constexpr HeroLevelProfile131 heroLevelProfiles131[]={')
for bits,build,rv,name,info in meta:out.extend(['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+str(build)+','+','.join(hex(a) for a in rv)+','+name+',sizeof('+name+')/sizeof(*'+name+')},','#endif'])
out+=['};','}'];Path('WHT-1.31/native/hero_level_profile131.hpp').write_text('\n'.join(out));Path('WHT-1.31/analysis/hero-level-disassembly.txt').write_text('\n'.join(logs));Path('WHT-1.31/analysis/hero-level-profile-evidence.json').write_text(json.dumps(meta,indent=2));print([(b,v,info) for b,v,rv,name,info in meta])
