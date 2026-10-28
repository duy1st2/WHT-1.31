from port3_research import *
import hashlib,datetime,shutil
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp']
e={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'scope':'#ufinds reads active selected unit position in both references; those handlers never compare real name. Port compares native cached display name; no map enumeration, no success on arbitrary query.'}
backup=root/'analysis'/('before-unit-name-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/f,backup/Path(f).name)
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())]
(root/'analysis/unit-name-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
specs=[(8740,12164,32,0xbac220,0xbac236,0xbd7740,0xbd7785,0x8e3e60,0x8e3e84,0x38),(32116,12173,32,0xb6b690,0xb6b6a6,0xb971f0,0xb97235,0x8b2570,0x8b2594,0x38),(24984,12164,64,0xd09a90,0xd09ab2,0xd3e4c0,0xd3e514,0x9f9760,0x9f9787,0x5c),(27336,12173,64,0xd48550,0xd48572,0xd7cbc0,0xd7cc14,0xa44dd0,0xa44df7,0x5c)]
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct UnitNameProfile131 {unsigned build;uintptr_t nameOffset;const ResourceTextCode131* code;size_t count;};'];rows=[];e['profiles']=[]
for pid,build,bits,g,ge,init,ie,wr,we,offset in specs:
 b,base,_=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 def callto(s,t,end):return any(i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM and i.operands[0].imm==base+t for i in d.disasm(b[s:end],base+s))
 assert callto(wr,g,we) and callto(g,init,ge)
 for start,end in [(g,ge),(init,ie)]:
  ins=list(d.disasm(b[start:end],base+start));assert ins[-1].mnemonic=='ret';assert any(i.mnemonic=='lea' and i.operands[-1].type==capstone.x86.X86_OP_MEM and i.disp==offset for i in ins)
 assert b'\x03\x01\x00\x00' in b[init:ie] # cap259 + NUL
 # Ranges have no relocatable absolute operands; verify PE relocation directory.
 peoff=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,peoff+24+(96 if bits==32 else 112)+40);a=rr;relocs=[]
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0]
   if v>>12 in [3,10]:relocs.append(page+(v&4095))
  a+=n
 ranges=[(g,ge),(init,ie),(wr,we)];assert not any(s<=r<end for r in relocs for s,end in ranges)
 tag=f'un{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[]
 for n,(s,end) in enumerate(ranges):
  name=f'{tag}_{n}';out.append(f'inline constexpr unsigned char {name}[]={{'+','.join(hex(x) for x in b[s:end])+'};');entries.append('{'+f'0x{s:x},{name},sizeof({name}),nullptr,0'+'}')
 out.append(f'inline constexpr ResourceTextCode131 {tag}c[]={{'+','.join(entries)+'};');out.append('#endif');rows.extend(['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+f'{build},0x{offset:x},{tag}c,sizeof({tag}c)/sizeof({tag}c[0])'+'},','#endif'])
 e['profiles'].append({'pid':pid,'bits':bits,'build':build,'nameOffset':offset,'ranges':ranges,'cache_capacity':260})
out+=['inline constexpr UnitNameProfile131 unitNameProfiles131[]={',*rows,'};','}'];(root/'analysis/unit-name-native-evidence.json').write_text(json.dumps(e,indent=2));(root/'native/unit_name_profile131.hpp').write_text('\n'.join(out)+'\n');print(e['profiles'])
