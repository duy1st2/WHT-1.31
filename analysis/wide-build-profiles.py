from port3_research import *
import hashlib,datetime,shutil
backup=root/'analysis'/('before-wide-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/f,backup/Path(f).name)
refs=[root.parent/'WHT/native/runtime.cpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp'];e={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'profiles':[],'scope':'WHT PerspectiveHook: call original, divide matrix[0] by (client width/client height)/(4/3). WHT-1.29 only toggles unused state.'}
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())];(root/'analysis/wide-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct WideScreenProfile131 {unsigned build;uintptr_t projection;const ResourceTextCode131* code;size_t count;};'];rows=[]
for pid,build,bits,fn in [(8740,12164,32,0xd53360),(32116,12173,32,0xd0e360),(24984,12164,64,0xef7e50),(27336,12173,64,0xf33c40)]:
 b,base,_=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 # Full body: x86 has a second invalid-input return after its success return.
 n=327 if bits==32 else 0x13e
 ins=list(d.disasm(b[fn:fn+n],base+fn));assert ins[-1].mnemonic=='ret',(pid,ins[-1].mnemonic,hex(ins[-1].address-base))
 callers=[m.start() for m in re.finditer(b'\xe8',b[:0x1900000]) if m.start()+5+struct.unpack_from('<i',b,m.start()+1)[0]==fn];assert len(callers)==1,(pid,callers)
 constants={}
 for i in ins:
  for op in i.operands:
   if op.type==capstone.x86.X86_OP_MEM and op.mem.disp and (not op.mem.base or op.mem.base==capstone.x86.X86_REG_RIP):
    rv=op.mem.disp-base if not op.mem.base else i.address+i.size+op.mem.disp-base
    if 0<rv<len(b)-4:constants[rv]=4
 # caller window is instruction-aligned from call itself; function body proves native signature.
 spans=[(fn,n),(callers[0],5)]+sorted(constants.items())
 p=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,p+24+(96 if bits==32 else 112)+40);a=rr;rel=[]
 while a<rr+size:
  page,length=struct.unpack_from('<II',b,a)
  if length<8:break
  for q in range(a+8,a+length,2):
   v=struct.unpack_from('<H',b,q)[0]
   if v>>12==3:rel.append(page+(v&4095))
   if v>>12==10:assert not any(s<=page+(v&4095)<s+n for s,n in spans)
  a+=length
 tag=f'wide{bits}_{build}';out.append('#if '+('defined' if bits==64 else '!defined')+'(_WIN64)');entries=[]
 for j,(a,n) in enumerate(spans):
  raw=bytearray(b[a:a+n]);offsets=[rv-a for rv in rel if a<=rv<a+n]
  for off in offsets:struct.pack_into('<I',raw,off,(struct.unpack_from('<I',raw,off)[0]-base)&0xffffffff)
  out.append(f'inline constexpr unsigned char {tag}_{j}[]={{'+','.join(hex(x) for x in raw)+'};')
  if offsets:out.append(f'inline constexpr unsigned {tag}r{j}[]={{'+','.join(map(str,offsets))+'};')
  entries.append('{'+f'0x{a:x},{tag}_{j},sizeof({tag}_{j}),'+(f'{tag}r{j},{len(offsets)}' if offsets else 'nullptr,0')+'}')
 out.append(f'inline constexpr ResourceTextCode131 {tag}[]={{'+','.join(entries)+'};');out.append('#endif');rows+=['#if '+('defined' if bits==64 else '!defined')+'(_WIN64)',f'{{{build},0x{fn:x},{tag},std::size({tag})}},','#endif'];e['profiles'].append({'build':build,'bits':bits,'capture':pid,'projection':hex(fn),'length':spans[0][1],'caller':hex(callers[0]),'constants':{hex(a):b[a:a+n].hex() for a,n in constants.items()},'signature':'cdecl(float fov,float aspect,float near,float far,float* output); Win64 standard five arguments'})
out+=['inline constexpr WideScreenProfile131 wideScreenProfiles131[]={']+rows+['};','}'];(root/'native/wide_screen_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/wide-native-evidence.json').write_text(json.dumps(e,indent=2))

