from port3_research import *
configs=[
 (14740,12164,32,0x182eeb0,0xc6a970,0x568,0x180fdf8,0x9d35a0,0xe00570,0xf02b26,0xc5ead0,0xa98a50,0xa9a310,0xc60a10,0xc60932,0x2fb0d0,0xc60b70),
 (23884,12173,32,0x17e3d90,0xc259f0,0x554,0x17c4cc0,0x9942e0,0xdbf340,0xec1b06,0xc19c70,0xa57a70,0xa59250,0xc1ba90,0xc1b9b2,0x2dfc60,0xc1bdc0),
 (6688,12164,64,0x1c16140,0xde2000,0x6a4,0x1bda5c0,0xaf6760,0xfbb7e0,0x10e07cc,0xdd6ea0,0xbcf800,0xbd1320,0xdd8fb4,0xdd8ea4,0x3547b0,0xdd7070),
 (29928,12173,64,0x1c45000,0xe1dee0,0x6ac,0x1c09440,0xb3f140,0xff8350,0x111d22c,0xe13230,0xc16320,0xc17df0,0xe14ea4,0xe14d94,0x3bf9a0,0xe133a0)]
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','inline bool MenuFogVertexControl131(const std::wstring& s,const std::wstring& k){return (s==L"Graphics"&&k==L"VertexColor")||(s==L"Event"&&(k==L"Fog Click"||k==L"Fast Menu"));}','struct MenuFogVertexProfile131 {unsigned build;uintptr_t unitTable,color,alpha,glueTable,glueUpdate,modelGet,frames,speed,reg,unreg,resolve;const ResourceTextCode131* code;size_t count;};'];report=[]
for pid,build,bits,unit,color,rgb,glue,update,get,speedRead,dispatch,reg,unreg,regCall,unregCall,resolve,agentProof in configs:
 b,base,actual=image(pid);assert bits==actual
 assert struct.unpack_from('<I' if bits==32 else '<Q',b,unit+43*(bits//8))[0]==base+color
 assert struct.unpack_from('<I' if bits==32 else '<Q',b,glue+12*(bits//8))[0]==base+update
 # The four independently identified color readers consume the profile field.
 assert struct.pack('<I',rgb) in b[color:color+512]
 assert struct.pack('<I',0x188 if bits==32 else 0x2c8) in b[speedRead:speedRead+8]
 ranges=[(color,512),(update,100),(get,21 if bits==32 else 23),(speedRead,40),(dispatch,48),(reg,48),(unreg,40),(regCall,25),(unregCall,20),(resolve,48)]
 alliance={14740:(0x97bf46,80),23884:(0x93f470,80),6688:(0xa941f3,84),29928:(0xade7af,84)}[pid];ranges.append(alliance)
 # Capture native packet field reads and typed-agent extraction independently.
 if bits==32:
  field=b.index(b'\x0f\xb6\x43\x15',dispatch,dispatch+600);ranges.append((field-3,48))
  if pid==14740:ranges.append((0xc5ec87,22));ranges.append((0xc60ba5,36))
  else:
   at=b.index(b'\x8b\x43\x20',dispatch,dispatch+1200);ranges.append((at,25));ranges.append((0xb15580,112))
 else:
  needle=b'DispatchUnitSelectionModify Start';string=b.index(needle)
  # Capture the field decoding around its RIP-relative string reference.
  for m in re.finditer(rb'[\x48\x4c]\x8d.',b[dispatch:dispatch+1000],re.S):
   at=dispatch+m.start()
   if b[at+2]&0xc7==5 and at+7+struct.unpack_from('<i',b,at+3)[0]==string:ranges.append((at-58,75));break
  else:raise ValueError('missing field reader')
  ranges.append((0xdd7070 if pid==6688 else 0xe133c8,128))
  if pid==29928:ranges.append((0xe15080,156))
 tag=f'mfv{bits}_{build}';out+=['#if '+('!defined(_WIN64)' if bits==32 else 'defined(_WIN64)')];records=[]
 fixups=[]
 if bits==32:
  peoff=struct.unpack_from('<I',b,60)[0];ra,rs=struct.unpack_from('<II',b,peoff+24+96+5*8);fixups=[];pos=ra
  while pos<ra+rs:
   page,sz=struct.unpack_from('<II',b,pos)
   if sz<8:break
   for q in range(pos+8,pos+sz,2):
    value=struct.unpack_from('<H',b,q)[0]
    if value>>12==3:fixups.append(page+(value&4095))
   pos+=sz

 for n,(at,length) in enumerate(ranges):
  code=bytearray(b[at:at+length]);rel=[];d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
  if bits==32:
   ending=at+length
   for off in sorted(fixups):
    if at<=off<ending and off+4>ending:ending=off+4
   length=ending-at;code=bytearray(b[at:ending])
   for off in fixups:
    if at<=off and off+4<=ending:
     val=struct.unpack_from('<I',code,off-at)[0];struct.pack_into('<I',code,off-at,(val-base)&0xffffffff);rel.append(off-at)
  name=tag+'b'+str(n);out.append('inline constexpr unsigned char '+name+'[]={'+','.join(hex(x) for x in code)+'};')
  if rel:out.append('inline constexpr unsigned '+name+'r[]={'+','.join(map(str,rel))+'};')
  records.append('{'+hex(at)+','+name+',sizeof('+name+'),'+(name+'r,sizeof('+name+'r)/sizeof(unsigned)' if rel else 'nullptr,0')+'}')
 out.append('inline constexpr ResourceTextCode131 '+tag+'code[]={'+','.join(records)+'};')
 vals=[build,unit,color,rgb+3,glue,update,get,0x1a0 if bits==32 else 0x2d8,0x188 if bits==32 else 0x2c8,reg,unreg,resolve]
 out.append('inline constexpr MenuFogVertexProfile131 '+tag+'={'+','.join(hex(v) for v in vals)+','+tag+'code,sizeof('+tag+'code)/sizeof(ResourceTextCode131)};');out.append('#endif')
 report.append(dict(pid=pid,build=build,bits=bits,color=hex(color),alpha=hex(rgb+3),glue=hex(glue),update=hex(update),getter=hex(get),speed=hex(vals[8]),register=hex(reg),unregister=hex(unreg),dispatch=hex(dispatch),ranges=ranges))
out+=['inline const MenuFogVertexProfile131* MenuFogVertexProfileFor131(unsigned build){','#ifdef _WIN64','return build==12164?&mfv64_12164:build==12173?&mfv64_12173:nullptr;','#else','return build==12164?&mfv32_12164:build==12173?&mfv32_12173:nullptr;','#endif','}','inline bool MenuFogVertexGuard131(uintptr_t base,const MenuFogVertexProfile131& p){ResourceTextProfile131 guard{};guard.code=p.code;guard.count=p.count;return ResourceTextGuard131(base,guard);}','}']
(root/'native/menu_fog_vertex_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/menu-fog-vertex-profiles.json').write_text(json.dumps(report,indent=2));print('Generated 4 verified profiles')
