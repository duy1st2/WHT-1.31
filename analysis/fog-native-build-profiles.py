from port3_research import *
import hashlib,datetime,shutil
# All entries are resolved native bindings, except12164 x64 RemoveRect,
# independently identified by its typed rectangle resolver/destruction wrapper.
specs=[(8740,12164,32,[0x8ddd50,0x8e4d30,0x8d0c10,0x8d67d0,0x8d67f0,0x8d31a0,0x8f1470],0x18240a0,0x1806950,0x6c5dd0),(32116,12173,32,[0x8ac7a0,0x8b3430,0x89f9a0,0x8a5210,0x8a5230,0x8a1e30,0x8bf640],0x17d8f80,0x17bb980,0x6889a0),(24984,12164,64,[0x9f2870,0x9fa950,0x9e3680,0x9ea800,0x9ea820,0x9e63a0,0xa065a0],0x1c00ec0,0x1bb2f48,0x782560),(27336,12173,64,[0xa3def0,0xa45fe0,0xa2f690,0xa36050,0xa36070,0xa32160,0xa514a0],0x1c2fd78,0x1be1f20,0x7dd220)]
e=json.loads((root/'analysis/fog-modifier-reference-evidence.json').read_text());e['profiles']=[]
backup=root/'analysis'/('before-fog-native-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md','native/fog_modifier_lifecycle.hpp']:shutil.copy2(root/f,backup/Path(f).name)
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())];(root/'analysis/fog-native-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct FogNativeProfile131 {unsigned build;uintptr_t local,bounds,create,start,stop,destroy,removeRect,player,rect,fog,uiTable,fogTable,rectTable,configure,fogDelete,rectDelete,ownerGetter;const ResourceTextCode131* code;size_t count;};'];rows=[]
for pid,build,bits,api,ui,playerTable,getter in specs:
 b,base,_=image(pid);w=bits//8;d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 def calls(a,n):return [i.operands[0].imm-base for i in d.disasm(b[a:a+n],base+a) if i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM]
 player,rect=calls(api[2],130)[:2];fog=calls(api[3],30)[0];assert fog==calls(api[4],30)[0]==calls(api[5],30)[0];assert rect==calls(api[6],30)[0]
 def table(name):
  td=b.index(name)-2*w;v=[]
  for m in re.finditer(re.escape(struct.pack('<I',base+td if bits==32 else td)),b):
   col=m.start()-12
   if struct.unpack_from('<I',b,col)[0]!=(0 if bits==32 else 1) or struct.unpack_from('<I',b,col+4)[0]!=0:continue
   v += [m.start()+w for m in re.finditer(re.escape(struct.pack('<I' if bits==32 else '<Q',base+col)),b)]
  assert len(v)==1,(pid,name,v)
  return v[0]
 ft=table(b'.?AVCFogModifier@@\0');rt=table(b'.?AVCRectWar3@@\0');method=lambda t,n:int.from_bytes(b[t+n*w:t+(n+1)*w],'little')-base
 config=method(ft,24);fd=method(ft,23);rd=method(rt,23);assert method(playerTable,25)==getter
 funcs=sorted(set(api+[player,rect,fog,config,fd,rd]));ranges=[]
 for a in funcs:
  if ranges and a<=ranges[-1][1]:ranges[-1]=(ranges[-1][0],max(ranges[-1][1],a+1024))
  else:ranges.append((a,a+1024))
 ranges.append((getter,getter+5));peoff=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,peoff+24+(96 if bits==32 else 112)+40);a=rr;rel=[];r64=[]
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0];rv=page+(v&4095)
   if v>>12==3:rel.append(rv)
   if v>>12==10:r64.append(rv)
  a+=n
 assert not any(s<=rv<z for rv in r64 for s,z in ranges)
 tag=f'fg{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[];blocks=[]
 for s,z in ranges:
  a=s
  while a<z:
   end=min(a+384,z)
   for rv in rel:
    if a<=rv<end and rv+4>end:end=rv
   data=bytearray(b[a:end]);rs=[rv-a for rv in rel if a<=rv and rv+4<=end]
   for rv in rs:struct.pack_into('<I',data,rv,(struct.unpack_from('<I',data,rv)[0]-base)&0xffffffff)
   n=f'{tag}_{len(entries)}';out.append(f'inline constexpr unsigned char {n}[]={{'+','.join(hex(x) for x in data)+'};')
   if rs:out.append(f'inline constexpr unsigned {n}r[]={{'+','.join(str(x) for x in rs)+'};')
   entries.append('{'+f'0x{a:x},{n},sizeof({n}),'+(f'{n}r,{len(rs)}' if rs else 'nullptr,0')+'}');blocks.append((a,end-a));a=end
 out.append(f'inline constexpr ResourceTextCode131 {tag}c[]={{'+','.join(entries)+'};');out.append('#endif');values=api+[player,rect,fog,ui,ft,rt,config,fd,rd,getter];rows+=['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+str(build)+','+','.join(hex(x) for x in values)+f',{tag}c,sizeof({tag}c)/sizeof({tag}c[0])'+'},','#endif'];e['profiles'].append({'pid':pid,'bits':bits,'build':build,'api':[hex(x) for x in api],'player':hex(player),'rect':hex(rect),'fog':hex(fog),'fog_table':hex(ft),'rect_table':hex(rt),'blocks':blocks})
out+=['inline constexpr FogNativeProfile131 fogNativeProfiles131[]={',*rows,'};','}'];(root/'native/fog_native_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/fog-native-evidence.json').write_text(json.dumps(e,indent=2));print([(p['build'],p['bits'],p['player'],p['rect'],p['fog']) for p in e['profiles']])
