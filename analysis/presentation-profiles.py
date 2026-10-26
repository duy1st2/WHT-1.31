from pathlib import Path
import sys,struct,json
sys.path.insert(0,str(Path(__file__).parent/'python-tools'))
import capstone
root=Path(__file__).resolve().parents[1]
configs=[(14740,12164,32,0x274ca0,0x274d50,0x2743b0),(23884,12173,32,0x2614e0,0x261590,0x260b80),(6688,12164,64,0x2be170,0x2be2a0,0x2bd780),(29928,12173,64,0x333920,0x333a50,0x332e70)]
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct PresentationProfile131 {unsigned build;uintptr_t settings,read,apply,unlock,renderer;const ResourceTextCode131* code;size_t count;};'];report=[]
for pid,build,bits,single,read,apply in configs:
 b=(root/f'analysis/memory-{pid}.bin').read_bytes();pe=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I' if bits==32 else '<Q',b,pe+24+(28 if bits==32 else 24))[0];d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 ins=list(d.disasm(b[apply:apply+72],apply+base))
 if bits==32:
  settings=struct.unpack_from('<I',b,single+1)[0]-base
  table=next(i.disp-base for i in ins if i.mnemonic=='jmp');index=next(i.disp-base for i in ins if i.mnemonic=='movzx')
 else:
  settings=single+11+struct.unpack_from('<i',b,single+7)[0]
  index=next(i.disp for i in ins if i.mnemonic=='movzx');table=next(i.disp for i in ins if i.mnemonic=='mov' and i.disp>0x10000)
 cases={k:struct.unpack_from('<I',b,table+b[index+k-5]*4)[0]-(base if bits==32 else 0) for k in [17,19]}
 ranges=[(single,5 if bits==32 else 11),(read,0x67 if bits==32 else 0x74),(apply,0x2e if bits==32 else 0x43),(index,58),(table,120)]
 targets={}
 for key,a in cases.items():
  decoded=[]
  for i in d.disasm(b[a:a+150],a+base):
   decoded.append(i)
   if i.mnemonic=='ret':break
  assert decoded[-1].mnemonic=='ret'
  ranges.append((a,decoded[-1].address+decoded[-1].size-base-a))
  calls=[i for i in decoded if i.mnemonic=='call'];target=calls[-1].operands[0].imm-base;targets[key]=target
  tail=[]
  for i in d.disasm(b[target:target+400],target+base):
   tail.append(i)
   if i.mnemonic=='ret' or (i.mnemonic=='jmp' and i.operands[0].type!=capstone.x86.X86_OP_IMM):break
  assert tail[-1].mnemonic in ('ret','jmp'),(pid,key,hex(target))
  ranges.append((target,tail[-1].address+tail[-1].size-base-target))
 renderer=(struct.unpack_from('<I',b,targets[19]+5)[0]-base) if bits==32 else (targets[19]+10+struct.unpack_from('<i',b,targets[19]+6)[0])
 tag=f'pr{bits}_{build}';out+=['#if '+('!defined(_WIN64)' if bits==32 else 'defined(_WIN64)')];records=[]
 for n,(at,length) in enumerate(ranges):
  code=bytearray(b[at:at+length]);rel=[]
  if bits==32:
   # Table DWORDs are pointers; code operands are normalized independently.
   if at==table:offsets=range(0,length,4)
   elif at==index:offsets=[]
   else:
    offsets=[]
    for i in d.disasm(code,at):
     for off,size in [(i.imm_offset,i.imm_size),(i.disp_offset,i.disp_size)]:
      if size==4 and not(off==i.imm_offset and (i.group(capstone.CS_GRP_CALL) or i.group(capstone.CS_GRP_JUMP))):offsets.append(i.address-at+off)
   for off in offsets:
    val=struct.unpack_from('<I',code,off)[0]
    if base<=val<base+len(b):struct.pack_into('<I',code,off,val-base);rel.append(off)
  name=tag+'b'+str(n);out.append('inline constexpr unsigned char '+name+'[]={'+','.join(hex(x) for x in code)+'};')
  if rel:out.append('inline constexpr unsigned '+name+'r[]={'+','.join(map(str,rel))+'};')
  records.append('{'+hex(at)+','+name+',sizeof('+name+'),'+(name+'r,sizeof('+name+'r)/sizeof(unsigned)' if rel else 'nullptr,0')+'}')
 out.append('inline constexpr ResourceTextCode131 '+tag+'code[]={'+','.join(records)+'};');out.append('inline constexpr PresentationProfile131 '+tag+'={'+str(build)+','+','.join(hex(v) for v in [settings,read,apply,targets[17],renderer])+','+tag+'code,sizeof('+tag+'code)/sizeof(ResourceTextCode131)};');out.append('#endif')
 report.append(dict(pid=pid,build=build,bits=bits,settings=hex(settings),read=hex(read),apply=hex(apply),cases={k:hex(v) for k,v in cases.items()},targets={k:hex(v) for k,v in targets.items()},ranges=ranges))
out+=['inline const PresentationProfile131* PresentationProfileFor131(unsigned build){','#ifdef _WIN64','return build==12164?&pr64_12164:build==12173?&pr64_12173:nullptr;','#else','return build==12164?&pr32_12164:build==12173?&pr32_12173:nullptr;','#endif','}','inline bool PresentationGuard131(uintptr_t base,const PresentationProfile131& p){ResourceTextProfile131 guard{};guard.code=p.code;guard.count=p.count;return ResourceTextGuard131(base,guard);}','}']
(root/'native/presentation_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/presentation-profiles.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
