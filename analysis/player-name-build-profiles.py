from port3_research import *
import hashlib,datetime,shutil
backup=root/'analysis'/('before-player-name-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','native/command_feedback131.hpp','CMakeLists.txt','README.md']:shutil.copy2(root/f,backup/Path(f).name)
refs=[root.parent/'WHT/native/runtime.cpp',root.parent/'WHT-1.29/native/version129.cpp'];e={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'reference_lines':'WHT runtime.cpp:3355-3430 #n/#name,2690-2704 native network name;1.31 adapts to verified engine managed player name, match only, no global registry writes.','profiles':[]}
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())];(root/'analysis/player-name-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct PlayerNameProfile131 {unsigned build;uintptr_t publicSet,setString,nameOffset;const ResourceTextCode131* code;size_t count;};'];rows=[]
for pid,build,bits,public,worker,getter,offset,live in [(8740,12164,32,0x91bb50,0x2c08a0,0x2c0b90,0x2c,14320),(32116,12173,32,0x8e91c0,0x2aa6d0,0x2aa9c0,0x34,15612),(24984,12164,64,0xa32b20,0x316170,0x316560,0x58,35488),(27336,12173,64,0xa7e960,0x383bf0,0x383fe0,0x48,3680)]:
 b,base,_=image(pid);spans=[(public,44 if bits==32 else 57),(worker,177 if bits==32 else 245),(getter,14 if bits==32 else 15)]
 d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 ins=list(d.disasm(b[public:public+spans[0][1]],base+public));assert ins[-1].mnemonic=='ret'
 calls=[i.address+i.size+struct.unpack_from('<i',i.bytes,1)[0]-base for i in ins if i.bytes[0]==0xe8];assert calls[-1]==worker,(pid,calls)
 for a,n in spans:assert list(d.disasm(b[a:a+n],base+a))[-1].mnemonic=='ret',(pid,hex(a))
 p=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,p+24+(96 if bits==32 else 112)+40);a=rr;rel=[]
 while a<rr+size:
  page,length=struct.unpack_from('<II',b,a)
  if length<8:break
  for q in range(a+8,a+length,2):
   v=struct.unpack_from('<H',b,q)[0]
   if v>>12==3:rel.append(page+(v&4095))
   if v>>12==10:assert not any(s<=page+(v&4095)<s+n for s,n in spans)
  a+=length
 tag=f'pname{bits}_{build}';out.append('#if '+('defined' if bits==64 else '!defined')+'(_WIN64)');entries=[]
 for j,(a,n) in enumerate(spans):
  raw=bytearray(b[a:a+n]);offsets=[rv-a for rv in rel if a<=rv<a+n]
  for off in offsets:struct.pack_into('<I',raw,off,(struct.unpack_from('<I',raw,off)[0]-base)&0xffffffff)
  out.append(f'inline constexpr unsigned char {tag}_{j}[]={{'+','.join(hex(x) for x in raw)+'};')
  if offsets:out.append(f'inline constexpr unsigned {tag}r{j}[]={{'+','.join(map(str,offsets))+'};')
  entries.append('{'+f'0x{a:x},{tag}_{j},sizeof({tag}_{j}),'+(f'{tag}r{j},{len(offsets)}' if offsets else 'nullptr,0')+'}')
 out.append(f'inline constexpr ResourceTextCode131 {tag}[]={{'+','.join(entries)+'};');out.append('#endif');rows+=['#if '+('defined' if bits==64 else '!defined')+'(_WIN64)',f'{{{build},0x{public:x},0x{worker:x},0x{offset:x},{tag},std::size({tag})}},','#endif'];e['profiles'].append({'build':build,'bits':bits,'capture':pid,'live_binding_pid':live,'public':hex(public),'worker':hex(worker),'nameOffset':hex(offset),'blocks':spans,'signature':'void thiscall(managed string field, const char* utf8); standard Win64 RCX/RDX; public SetPlayerName resolves CPlayer then field+offset then worker'})
out+=['inline constexpr PlayerNameProfile131 playerNameProfiles131[]={']+rows+['};','}'];(root/'native/player_name_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/player-name-evidence.json').write_text(json.dumps(e,indent=2));print('All four public setter-to-managed-worker call edges verified; profiles generated')

