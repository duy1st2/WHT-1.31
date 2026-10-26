from pathlib import Path
import sys,struct,json
sys.path.insert(0,'analysis/python-tools');import capstone
profiles=[(14740,12164,32,0xb46850,0x298,0x1826a88,[(0xb46850,0x4c),(0xb30b77,0xd),(0xb30b8f,0x15)]),(23884,12173,32,0xb064c0,0x2c4,0x17db968,[(0xb064c0,0x4c),(0xaf0339,0xd),(0xaf0351,0x15)]),(6688,12164,64,0xc972c0,0x468,0x1c06408,[(0xc972c0,0x4c),(0xc80c17,0xf),(0xc80c28,0x27)]),(29928,12173,64,0xcda0c0,0x480,0x1c352c8,[(0xcda0c0,0x4c),(0xcc3a77,0xf),(0xcc3a88,0x27)])]
out=['#pragma once','#include "resource_text_profile131.hpp"','#include "courier_route131.hpp"','namespace wht {','struct CraftSelectionProfile131 {unsigned build;uintptr_t select,member,table;const ResourceTextCode131* code;size_t count;};'];report=[]
for pid,build,bits,fn,member,table,ranges in profiles:
 b=Path(f'analysis/memory-{pid}.bin').read_bytes();pe=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I' if bits==32 else '<Q',b,pe+24+(28 if bits==32 else 24))[0];d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 tag=f'cs{bits}_{build}';out+=['#if '+('!defined(_WIN64)' if bits==32 else 'defined(_WIN64)')];records=[]
 for index,(at,length) in enumerate(ranges):
  data=bytearray(b[at:at+length]);rel=[]
  if bits==32:
   for i in d.disasm(data,at):
    for off,size in [(i.imm_offset,i.imm_size),(i.disp_offset,i.disp_size)]:
     if size!=4 or (off==i.imm_offset and (i.group(capstone.CS_GRP_CALL) or i.group(capstone.CS_GRP_JUMP))):continue
     value=struct.unpack_from('<I',data,i.address-at+off)[0]
     if base<=value<base+len(b):struct.pack_into('<I',data,i.address-at+off,value-base);rel.append(i.address-at+off)
  name=tag+'b'+str(index);out.append('inline constexpr unsigned char '+name+'[]={'+','.join(hex(x) for x in data)+'};')
  if rel:out.append('inline constexpr unsigned '+name+'r[]={'+','.join(map(str,rel))+'};')
  records.append('{'+hex(at)+','+name+',sizeof('+name+'),'+(name+'r,sizeof('+name+'r)/sizeof(unsigned)' if rel else 'nullptr,0')+'}')
 out.append('inline constexpr ResourceTextCode131 '+tag+'[]={'+','.join(records)+'};');out.append('#endif');report.append(dict(pid=pid,build=build,bits=bits,select=hex(fn),uiMember=hex(member),table=hex(table),ranges=ranges))
out+=['#ifdef _WIN64','inline constexpr CraftSelectionProfile131 craftSelectionProfiles131[]={'+','.join('{'+str(build)+','+','.join(hex(x) for x in (fn,member,table))+f',cs{bits}_{build},3'+'}' for pid,build,bits,fn,member,table,ranges in profiles if bits==64)+'};','#else','inline constexpr CraftSelectionProfile131 craftSelectionProfiles131[]={'+','.join('{'+str(build)+','+','.join(hex(x) for x in (fn,member,table))+f',cs{bits}_{build},3'+'}' for pid,build,bits,fn,member,table,ranges in profiles if bits==32)+'};','#endif','}']
Path('native/craft_selection_profile131.hpp').write_text('\n'.join(out)+'\n');Path('analysis/craft-selection-profiles.json').write_text(json.dumps(report,indent=2))
