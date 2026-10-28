from port3_research import *
import hashlib,datetime,shutil
specs=[(8740,12164,32,0x9c,0x38,0x1806b6c,0x91b40a,0x91b43d,0x8e7bd6,0x8e7bea),(32116,12173,32,0xdc,0x38,0x17bbb9c,0x8e8a96,0x8e8ac9,0x8b6142,0x8b6156),(24984,12164,64,0xe8,0x60,0x1bb3bc0,0xa322c0,0xa322f1,0x9fdf07,0x9fdf19),(27336,12173,64,0xd0,0x60,0x1be2b98,0xa7e115,0xa7e146,0xa4912c,0xa4913e)]
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp'];e={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'profiles':[]}
backup=root/'analysis'/('before-map-trade-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for f in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:shutil.copy2(root/f,backup/Path(f).name)
paths=[p for folder in (root/'runtime-test').glob('Warcraft3-*') for arch in ['x86','x86_64'] for p in (folder/arch).glob('*') if p.is_file() and (p.suffix.lower() in ['.ini','.log'] or 'ban' in p.name.lower())];(root/'analysis/map-trade-preserved-before.json').write_text(json.dumps({str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},indent=2))
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct MapTradeProfile131 {unsigned build;uintptr_t setupOffset,flagsOffset,setupTable;const ResourceTextCode131* code;size_t count;};'];rows=[]
for pid,build,bits,setup,flags,table,s,se,g,ge in specs:
 b,base,_=image(pid);w=bits//8;col=int.from_bytes(b[table-w:table],'little')-base;td=struct.unpack_from('<I',b,col+12)[0]-(base if bits==32 else 0);name=td+2*w;assert b[name:name+20]==b'.?AVCMapSetupWar3@@\0';assert struct.unpack_from('<I',b,col+4)[0]==0
 peoff=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,peoff+24+(96 if bits==32 else 112)+40);rel=[];r64=[];a=rr
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0];rv=page+(v&4095)
   if v>>12==3:rel.append(rv)
   if v>>12==10:r64.append(rv)
  a+=n
 ranges=[(s,se),(g,ge),(name,name+20)];assert not any(a<=rv<z for rv in r64 for a,z in ranges)
 tag=f'mt{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[];blocks=[]
 for i,(a,z) in enumerate(ranges):
  data=bytearray(b[a:z]);rs=[rv-a for rv in rel if a<=rv and rv+4<=z]
  for rv in rs:struct.pack_into('<I',data,rv,(struct.unpack_from('<I',data,rv)[0]-base)&0xffffffff)
  n=f'{tag}_{i}';out.append(f'inline constexpr unsigned char {n}[]={{'+','.join(hex(x) for x in data)+'};')
  if rs:out.append(f'inline constexpr unsigned {n}r[]={{'+','.join(str(x) for x in rs)+'};')
  entries.append('{'+f'0x{a:x},{n},sizeof({n}),'+(f'{n}r,{len(rs)}' if rs else 'nullptr,0')+'}');blocks.append((a,z-a))
 out.append(f'inline constexpr ResourceTextCode131 {tag}c[]={{'+','.join(entries)+'};');out.append('#endif');rows+=['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+f'{build},0x{setup:x},0x{flags:x},0x{table:x},{tag}c,sizeof({tag}c)/sizeof({tag}c[0])'+'},','#endif'];e['profiles'].append({'pid':pid,'bits':bits,'build':build,'setup_offset':setup,'flags_offset':flags,'setup_table':table,'blocks':blocks})
out+=['inline constexpr MapTradeProfile131 mapTradeProfiles131[]={',*rows,'};','}'];(root/'native/map_trade_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/map-trade-native-evidence.json').write_text(json.dumps(e,indent=2));print(e['profiles'])

