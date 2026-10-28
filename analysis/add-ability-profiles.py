from pathlib import Path
import sys,struct,json,shutil,datetime
sys.path.insert(0,'WHT-1.31/analysis/python-tools');import capstone as c
r=Path('WHT-1.31');b=r/'analysis'/('before-add-ability-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));b.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(r/f,b/Path(f).name)
spec=[(32,12164,'8740',0x460000,[0xbbe610,0x4612b0,0xbbb970,0xb17190,0x2f9d60,0x926550,0x920160]),(32,12173,'32116',0xe0000,[0xb7d9e0,0x43ab80,0xb7ad70,0xad6a80,0x2de860,0x8f39c0,0x8ed6a0]),(64,12164,'6688',0x7ff782f60000,[0xd1fff0,0x4dbce0,0xd1ca50,0xc64b00,0x352ed0,0xa3d7f0,0xa36560]),(64,12173,'29928',0x7ff626470000,[0xd5eb20,0x5416c0,0xd5b530,0xca7b90,0x3be090,0xa88f10,0xa81f00])]
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct AddAbilityProfile131 {unsigned build;uintptr_t before,add,after,refresh,maximum;unsigned level,up,down;const ResourceTextCode131* code;size_t count;};'];profiles=[];evidence=[]
for bits,build,pid,base,rv in spec:
 d=(r/'analysis'/('memory-'+pid+'.bin')).read_bytes();m=c.Cs(c.CS_ARCH_X86,c.CS_MODE_64 if bits==64 else c.CS_MODE_32);m.detail=True;pe=struct.unpack_from('<I',d,60)[0];relocs=[]
 if bits==64:
  pr,sz=struct.unpack_from('<II',d,pe+24+112+24);pdata=[struct.unpack_from('<III',d,a) for a in range(pr,pr+sz,12)]

 else:
  rr,sz=struct.unpack_from('<II',d,pe+24+96+40);a=rr
  while a<rr+sz:
   page,n=struct.unpack_from('<II',d,a)
   if n<8:break
   for p in range(a+8,a+n,2):
    v=struct.unpack_from('<H',d,p)[0]
    if v>>12==3:relocs.append(page+(v&4095))
   a+=n
 def length(start):
  fixed={0xa3d7f0:0xa3d892,0xa88f10:0xa88fb2,0xa36560:0xa3665e,0xa81f00:0xa81ffe}
  if start in fixed:return fixed[start]-start
  if bits==64:
   hits=[e-start for s,e,u in pdata if s==start]
   if hits:return hits[0]
  seen={};todo=[start]
  while todo:
   q=todo.pop()
   while q not in seen:
    assert start<=q<start+0x10000,(hex(start),hex(q));i=next(m.disasm(d[q:q+16],base+q,count=1));seen[q]=i;q+=i.size
    if i.group(c.CS_GRP_JUMP):
     if i.operands[0].type==c.x86.X86_OP_IMM:
      target=i.operands[0].imm-base
      if start<=target<start+0x10000:todo.append(target)
     if i.mnemonic=='jmp':break
    if i.group(c.CS_GRP_RET):break
  return max(a+i.size for a,i in seen.items())-start
 name=f'aa{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[];info=[]
 for start in rv:
  n=length(start);info.append((hex(start),n));off=0
  while off<n:
   end=min(n,off+384)
   for pos in relocs:
    if start+off<=pos<start+end and pos+4>start+end:end=pos-start
   assert end>off
   a=start+off;data=bytearray(d[a:start+end]);rel=[]
   for pos in relocs:
    if a<=pos<start+end:
     at=pos-a;v=struct.unpack_from('<I',data,at)[0];struct.pack_into('<I',data,at,(v-base)&0xffffffff);rel.append(at)
   i=len(entries);bn=name+'b'+str(i);rn=name+'r'+str(i);out.append('inline constexpr unsigned char '+bn+'[]={'+','.join(hex(v) for v in data)+'};')
   if rel:out.append('inline constexpr unsigned '+rn+'[]={'+','.join(map(str,rel))+'};')
   entries.append('{'+hex(a)+','+bn+',sizeof('+bn+'),'+(rn+','+str(len(rel)) if rel else 'nullptr,0')+'}');off=end
 out.append('inline constexpr ResourceTextCode131 '+name+'[]={'+','.join(entries)+'};');out.append('#endif');profiles.extend(['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+str(build)+','+','.join(hex(a) for a in rv[:5])+','+('0x74,0x5d0,0x5d8' if bits==64 else '0x50,0x2e8,0x2ec')+','+name+',sizeof('+name+')/sizeof(*'+name+')},','#endif']);evidence.append((bits,build,info));print(bits,build,info)
out+=['inline constexpr AddAbilityProfile131 addAbilityProfiles131[]={',*profiles,'};','}'];(r/'native/add_ability_profile131.hpp').write_text('\n'.join(out));(r/'analysis/add-ability-native-evidence.json').write_text(json.dumps(evidence,indent=2))
