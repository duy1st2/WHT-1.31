import pathlib,sys,struct,json
sys.path.insert(0,str(pathlib.Path(__file__).parent/'python-tools'));import capstone
root=pathlib.Path(__file__).resolve().parents[1]
configs=[(14740,12164,32,[0xdfeca0,0xf00050,0xd58f50,0x14e8d20,0xd3a5c0,0xdfcee0]),(23884,12173,32,[0xdbdaa0,0xebf030,0xd13f50,0x14a89a0,0xcf55c0,0xdbbce0]),(6688,12164,64,[0xfb9b10,0x10dd450,0xeff2c0,0x17a8430,0xed9af0,0xfb7ba0]),(29928,12173,64,[0xff6680,0x1119eb0,0xf3b0b0,0x17e4020,0xf158e0,0xff4710])]
out=['#pragma once','#include "common.hpp"','namespace wht {','struct ResourceTextCode131 {uintptr_t rva;const unsigned char* bytes;size_t length;const unsigned* reloc;size_t relocCount;};','struct ResourceTextProfile131 {unsigned build;uintptr_t position,viewport,nx,ny,fontTable;const ResourceTextCode131* code;size_t count;};'];reports=[]
for pid,build,bits,addrs in configs:
 b=(root/f'analysis/memory-{pid}.bin').read_bytes();pe=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I' if bits==32 else '<Q',b,pe+24+(28 if bits==32 else 24))[0];d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 # Function sizes from complete disassembly, with known early-return position branch.
 lengths=[0x1a4 if bits==32 else 0x1cd,0x6d if bits==32 else 0x5c,0x124 if bits==32 else 0x108,0x35 if bits==32 else 0x25,0x22 if bits==32 else 0x37,0x80 if bits==32 else 0x7f]
 tag=f'p{bits}_{build}';out+=['#if '+('!defined(_WIN64)' if bits==32 else 'defined(_WIN64)')];records=[]
 for n,(at,length) in enumerate(zip(addrs,lengths)):
  code=bytearray(b[at:at+length]);rel=[]
  if bits==32:
   for i in d.disasm(code,at):
    for off,sz in [(i.imm_offset,i.imm_size),(i.disp_offset,i.disp_size)]:
     if sz!=4:continue
     if off==i.imm_offset and (i.group(capstone.CS_GRP_CALL) or i.group(capstone.CS_GRP_JUMP)):continue
     val=struct.unpack_from('<I',code,i.address-at+off)[0]
     if base<=val<base+len(b):struct.pack_into('<I',code,i.address-at+off,val-base);rel.append(i.address-at+off)
  out.append(f'inline constexpr unsigned char {tag}b{n}[]={{'+','.join(hex(x) for x in code)+'};')
  out.append(f'inline constexpr unsigned {tag}r{n}[]={{'+','.join(map(str,rel or [0]))+'};')
  records.append('{'+f'{hex(at)},{tag}b{n},sizeof({tag}b{n}),{tag}r{n},{len(rel)}'+'}')
 out.append(f'inline constexpr ResourceTextCode131 {tag}code[]={{'+','.join(records)+'};')
 conv=list(d.disasm(b[addrs[3]:addrs[3]+lengths[3]],addrs[3]));norm=[]
 for i in conv:
  if i.mnemonic=='divss':
   op=i.operands[1];norm.append((i.address+i.size+op.mem.disp) if bits==64 else op.mem.disp-base)
 ctor=list(d.disasm(b[addrs[5]:addrs[5]+lengths[5]],addrs[5]));table=0
 if bits==32:table=struct.unpack_from('<I',b,addrs[5]+0x1a)[0]-base
 else:
  i=next(i for i in ctor if i.mnemonic=='lea' and i.op_str.startswith('rax, [rip'));table=i.address+i.size+i.operands[1].mem.disp
 out.append(f'inline constexpr ResourceTextProfile131 {tag}={{{build},{hex(addrs[1])},{hex(addrs[4])},{hex(norm[0])},{hex(norm[1])},{hex(table)},{tag}code,6}};');out+=['#endif'];reports.append(dict(pid=pid,build=build,bits=bits,functions=addrs,lengths=lengths,norm=norm,fontTable=table))
out+=['inline const ResourceTextProfile131* ResourceTextProfileFor131(unsigned build){','#ifdef _WIN64','return build==12164?&p64_12164:build==12173?&p64_12173:nullptr;','#else','return build==12164?&p32_12164:build==12173?&p32_12173:nullptr;','#endif','}','inline bool ResourceTextGuard131(uintptr_t base,const ResourceTextProfile131& p){if(!base)return false;__try{for(size_t n=0;n<p.count;++n){const auto& c=p.code[n];unsigned char expected[512];if(c.length>sizeof(expected))return false;memcpy(expected,c.bytes,c.length);for(size_t j=0;j<c.relocCount;++j){auto at=c.reloc[j];if(at+4>c.length)return false;unsigned value=0;memcpy(&value,expected+at,4);value+=unsigned(base);memcpy(expected+at,&value,4);}if(memcmp((void*)(base+c.rva),expected,c.length))return false;}return true;}__except(1){return false;}}','}']
(root/'native/resource_text_profile131.hpp').write_text('\n'.join(out)+'\n',encoding='utf-8');(root/'analysis/resource-text-profiles.json').write_text(json.dumps(reports,indent=2))
