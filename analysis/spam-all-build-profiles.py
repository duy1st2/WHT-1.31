from port3_research import *
import hashlib,datetime,shutil
backup=root/'analysis'/('before-spam-all-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/f,backup/Path(f).name)
refs=[root.parent/'WHT/native/runtime.cpp',root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp']
e={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'profiles':[],'scope':'WHT SendAllianceSpam(includePings=true), ProcessChatAutomation; user permits WHT-only implementations.'}
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())];(root/'analysis/spam-all-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {'];rows=[]
for pid,build,bits,ctor,wrapper,codec in [(8740,12164,32,0x9c9f00,0x9c33d0,0xaa0cd0),(32116,12173,32,0x98aa90,0x984360,0xa5fc90),(24984,12164,64,0xaec0c0,0xae56f0,0xbd8570),(27336,12173,64,0xb34bc0,0xb2e7d0,0xc1f040)]:
 b,base,_=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 def instructions(a,n):return list(d.disasm(b[a:a+n],base+a))
 def calls(a,n):return [i.operands[0].imm-base for i in instructions(a,n) if i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM]
 helper=calls(codec,50 if bits==32 else 63);assert len(helper)==3 and len(set(helper))==1;helper=helper[0]
 assert codec in calls(wrapper,220)
 spans=[(ctor+(0x2c if bits==32 else 0x13),27 if bits==32 else 28),(wrapper+(0x96 if bits==32 else 0x7f),65 if bits==32 else 39),(codec,50 if bits==32 else 63)]
 # Include the float-bit serializer through its first return; no invented managed ABI.
 code=[]
 for i in instructions(helper,80):
  code.append(i)
  if i.mnemonic=='ret':break
 assert code[-1].mnemonic=='ret';spans.append((helper,sum(i.size for i in code)))
 # No relocations are allowed in these spans. Constructor excludes its RTTI pointer.
 peoff=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,peoff+24+(96 if bits==32 else 112)+40);a=rr;rel=[]
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0]
   if v>>12 in [3,10]:rel.append(page+(v&4095))
  a+=n
 assert not any(s<=rv<s+n for s,n in spans for rv in rel)
 name=f'pingAction{bits}_{build}';out.append('#if '+('defined' if bits==64 else '!defined')+'(_WIN64)')
 for j,(a,n) in enumerate(spans):out.append(f'inline constexpr unsigned char {name}_{j}[]={{'+','.join(hex(x) for x in b[a:a+n])+'};')
 out.append(f'inline constexpr ResourceTextCode131 {name}[]={{'+','.join('{'+f'0x{a:x},{name}_{j},sizeof({name}_{j}),nullptr,0'+'}' for j,(a,n) in enumerate(spans))+'};');out.append('#endif');rows+=['#if '+('defined' if bits==64 else '!defined')+'(_WIN64)',f'{{{build},0,0,0,0,0,{name},4}},','#endif'];e['profiles'].append({'build':build,'bits':bits,'capture':pid,'producer':hex(ctor),'native_sender_wrapper':hex(wrapper),'codec':hex(codec),'float_bit_writer':hex(helper),'guard_spans':[(hex(a),n) for a,n in spans]})
out+=['inline constexpr ResourceTextProfile131 pingActionProfiles131[]={']+rows+['};','}'];(root/'native/ping_action_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/spam-all-evidence.json').write_text(json.dumps(e,indent=2))
