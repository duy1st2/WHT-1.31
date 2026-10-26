from pathlib import Path
import sys,struct,json
sys.path.insert(0,'analysis/python-tools');import capstone
configs=[(14740,12164,32,0x6d3f30,0x188,0x240,0x23c,0x1a4,[(0x6d3f30,0x136),(0xb2c154,0x9f)]),(23884,12173,32,0x697830,0x188,0x22c,0x228,0x1a4,[(0x697830,0x136),(0xaeb984,0x9f)]),(6688,12164,64,0x78f980,0x2a8,0x398,0x394,0x2d4,[(0x78f980,0xf8),(0xc7ba45,0xa5)]),(29928,12173,64,0x7e7f20,0x2a8,0x388,0x384,0x2d4,[(0x7e7f20,0xf8),(0xcbea45,0xa5)])]
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct AutoVisibilityProfile131 {unsigned build;uintptr_t query;unsigned context,vision,mask,local;const ResourceTextCode131* code;size_t count;};'];report=[]
for pid,build,bits,fn,ctx,vision,mask,local,ranges in configs:
 b=Path(f'analysis/memory-{pid}.bin').read_bytes();pe=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I' if bits==32 else '<Q',b,pe+24+(28 if bits==32 else 24))[0];d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 tag=f'av{bits}_{build}';out+=['#if '+('!defined(_WIN64)' if bits==32 else 'defined(_WIN64)')];records=[]
 for n,(at,length) in enumerate(ranges):
  code=bytearray(b[at:at+length]);rel=[]
  if bits==32:
   for i in d.disasm(code,at):
    for off,size in [(i.imm_offset,i.imm_size),(i.disp_offset,i.disp_size)]:
     if size!=4 or (off==i.imm_offset and (i.group(capstone.CS_GRP_CALL) or i.group(capstone.CS_GRP_JUMP))):continue
     val=struct.unpack_from('<I',code,i.address-at+off)[0]
     if base<=val<base+len(b):struct.pack_into('<I',code,i.address-at+off,val-base);rel.append(i.address-at+off)
  name=tag+'b'+str(n);out.append('inline constexpr unsigned char '+name+'[]={'+','.join(hex(x) for x in code)+'};')
  if rel:out.append('inline constexpr unsigned '+name+'r[]={'+','.join(map(str,rel))+'};')
  records.append('{'+hex(at)+','+name+',sizeof('+name+'),'+(name+'r,sizeof('+name+'r)/sizeof(unsigned)' if rel else 'nullptr,0')+'}')
 out.append('inline constexpr ResourceTextCode131 '+tag+'[]={'+','.join(records)+'};');out.append('#endif');report.append(dict(pid=pid,build=build,bits=bits,query=hex(fn),context=hex(ctx),vision=hex(vision),mask=hex(mask),local=hex(local),ranges=ranges))
for bits,cond in [(64,'#ifdef _WIN64'),(32,'#else')]:
 out.append(cond);out.append('inline constexpr AutoVisibilityProfile131 autoVisibilityProfiles131[]={'+','.join('{'+str(build)+','+','.join(hex(x) for x in (fn,ctx,vision,mask,local))+f',av{bits}_{build},2'+'}' for pid,build,b,fn,ctx,vision,mask,local,ranges in configs if b==bits)+'};')
out+=['#endif','}'];Path('native/auto_visibility_profile131.hpp').write_text('\n'.join(out)+'\n');Path('analysis/auto-visibility-profiles.json').write_text(json.dumps(report,indent=2))
