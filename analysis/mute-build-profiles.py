from port3_research import *
import hashlib,datetime,shutil
specs=[(8740,12164,32,0xb0a810,0xb0bdf7,0xafe680,0xafe6a4,0x18240a0),(32116,12173,32,0xac9f10,0xacb75f,0xabd580,0xabd5a4,0x17d8f80),(24984,12164,64,0xc57020,0xc584c8,0xc48c95,0xc48cb4,0x1c00ec0),(27336,12173,64,0xc9a530,0xc9b8d3,0xc8e9f5,0xc8ea14,0x1c2fd78)]
# x86 UI table is taken from the shipped independently verified native profile.
text=(root/'native/profile131.hpp').read_text(encoding='utf-8') if (root/'native/profile131.hpp').exists() else ''
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT/native/runtime.cpp',root.parent/'WHT-1.29/native/mute129.hpp',root.parent/'WHT-1.29/native/version129.cpp']
evidence={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'profiles':[]}
backup=root/'analysis'/('before-mute-chat-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for rel in ['native/version131.cpp','native/version64.cpp','native/repeat_chat131.hpp','CMakeLists.txt','README.md']:shutil.copy2(root/rel,backup/Path(rel).name)
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct MuteChatProfile131 {unsigned build;uintptr_t display,uiTable;const ResourceTextCode131* code;size_t count;};'];profiles=[]
for pid,build,bits,start,end,cs,ce,ui in specs:
 b,base,_=image(pid);pe=struct.unpack_from('<I',b,60)[0];relocs=[]
 if bits==32:
  rr,size=struct.unpack_from('<II',b,pe+24+96+40);a=rr
  while a<rr+size:
   page,n=struct.unpack_from('<II',b,a)
   if n<8:break
   for q in range(a+8,a+n,2):
    v=struct.unpack_from('<H',b,q)[0]
    if v>>12==3:relocs.append(page+(v&4095))
   a+=n
 tag=f'mute{bits}_{build}';out.append('#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'));entries=[];blocks=[]
 for begin,finish in [(start,end),(cs,ce)]:
  a=begin
  while a<finish:
   stop=min(a+384,finish)
   for r in relocs:
    if a<=r<stop and r+4>stop:stop=r
   n=stop-a;data=bytearray(b[a:stop]);rr=[r-a for r in relocs if a<=r and r+4<=stop]
   for r in rr:struct.pack_into('<I',data,r,(struct.unpack_from('<I',data,r)[0]-base)&0xffffffff)
   name=f'{tag}_{len(entries)}';out.append(f'inline constexpr unsigned char {name}[]={{'+','.join(hex(x) for x in data)+'};')
   if rr:out.append(f'inline constexpr unsigned {name}r[]={{'+','.join(str(x) for x in rr)+'};')
   entries.append('{'+f'0x{a:x},{name},sizeof({name}),'+(f'{name}r,{len(rr)}' if rr else 'nullptr,0')+'}');blocks.append((a,n));a=stop
 out.append(f'inline constexpr ResourceTextCode131 {tag}c[]={{'+','.join(entries)+'};');out.append('#endif')
 profiles.extend(['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),'{'+f'{build},0x{start:x},0x{ui:x},{tag}c,sizeof({tag}c)/sizeof({tag}c[0])'+'},','#endif'])
 evidence['profiles'].append({'pid':pid,'build':build,'bits':bits,'entry':hex(start),'end':hex(end),'caller':[cs,ce],'blocks':blocks})
out+=['inline constexpr MuteChatProfile131 muteChatProfiles131[]={',*profiles,'};','}']
(root/'analysis/mute-chat-native-evidence.json').write_text(json.dumps(evidence,indent=2),encoding='utf-8')
(root/'native/mute_chat_profile131.hpp').write_text('\n'.join(out)+'\n',encoding='utf-8')
print([(p['build'],p['bits'],p['entry']) for p in evidence['profiles']])
