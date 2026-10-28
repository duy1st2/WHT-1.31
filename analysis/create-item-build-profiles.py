from port3_research import *
import hashlib,datetime,shutil
backup=root/'analysis'/('before-create-item-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/f,backup/Path(f).name)
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT/native/runtime.cpp'];e={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'profiles':[],'scope':'WHT #item rawcode [1..50] and #copyitem slot1..6 [1..50], cursor position +40*x spacing, default native charges.'}
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())];(root/'analysis/create-item-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct CreateItemProfile131 {unsigned build;uintptr_t create,getType,resolve;const ResourceTextCode131* code;size_t count;};'];rows=[]
for pid,livepid,build,bits,create,getter in [(8740,35632,12164,32,0x8d13f0,0x8dd8b0),(32116,29268,12173,32,0x8a0160,0x8ac300),(24984,34924,12164,64,0x9e3fb0,0x9f2320),(27336,19468,12173,64,0xa2ff60,0xa3d9a0)]:
 b,base,_=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 def targets(a,n):return [i.operands[0].imm-base for i in d.disasm(b[a:a+n],base+a) if i.mnemonic in ['call','jmp'] and i.operands[0].type==capstone.x86.X86_OP_IMM]
 worker,convert=targets(create,30 if bits==32 else 23)[:2];resolve=targets(getter,25)[0]
 resolved=json.loads((root/f'analysis/item-resolved-{livepid}.json').read_text())['bindings'];assert hex(create) in resolved['CreateItem'] and hex(getter) in resolved['GetItemTypeId']
 spans=[(create,30 if bits==32 else 23),(getter,25 if bits==32 else 27),(resolve,1024),(worker,1024)]
 p=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,p+24+(96 if bits==32 else 112)+40);a=rr;rel=[]
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0]
   if v>>12==3:rel.append(page+(v&4095))
   if v>>12==10:assert not any(s<=page+(v&4095)<s+n for s,n in spans)
  a+=n
 chunks=[]
 for start,n in spans:
  for a in range(start,start+n,384):chunks.append((a,min(384,start+n-a)))
 tag=f'ci{bits}_{build}';out.append('#if '+('defined' if bits==64 else '!defined')+'(_WIN64)');entries=[]
 for j,(a,n) in enumerate(chunks):
  raw=bytearray(b[a:a+n]);offsets=[rv-a for rv in rel if a<=rv and rv+4<=a+n]
  assert not any(a-3<=rv<a or a+n-3<=rv<a+n for rv in rel),(pid,a,'relocation split')
  for off in offsets:struct.pack_into('<I',raw,off,(struct.unpack_from('<I',raw,off)[0]-base)&0xffffffff)
  out.append(f'inline constexpr unsigned char {tag}_{j}[]={{'+','.join(hex(x) for x in raw)+'};')
  if offsets:out.append(f'inline constexpr unsigned {tag}r{j}[]={{'+','.join(map(str,offsets))+'};')
  entries.append('{'+f'0x{a:x},{tag}_{j},sizeof({tag}_{j}),'+(f'{tag}r{j},{len(offsets)}' if offsets else 'nullptr,0')+'}')
 out.append(f'inline constexpr ResourceTextCode131 {tag}[]={{'+','.join(entries)+'};');out.append('#endif');rows+=['#if '+('defined' if bits==64 else '!defined')+'(_WIN64)',f'{{{build},0x{create:x},0x{getter:x},0x{resolve:x},{tag},std::size({tag})}},','#endif'];e['profiles'].append({'build':build,'bits':bits,'pid':pid,'live_binding_pid':livepid,'create':hex(create),'getType':hex(getter),'resolve':hex(resolve),'worker':hex(worker),'engine_handle_conversion':hex(convert),'blocks':chunks})
out+=['inline constexpr CreateItemProfile131 createItemProfiles131[]={']+rows+['};','}'];(root/'native/create_item_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/create-item-evidence.json').write_text(json.dumps(e,indent=2))
