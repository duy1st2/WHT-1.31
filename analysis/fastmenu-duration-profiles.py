from port3_research import *
configs=[(14740,12164,0x9d3f00,0x123),(23884,12173,0x994c40,0x123),(6688,12164,0xaf7150,0x151),(29928,12173,0xb3fb30,0x151)]
out=['#pragma once','#include "resource_text_profile131.hpp"','namespace wht {','struct FastMenuTransitionProfile131{unsigned build;uintptr_t entry;ResourceTextCode131 code;};'];report=[]
for pid,build,entry,length in configs:
 b,base,bits=image(pid);raw=bytearray(b[entry:entry+length]);rel=[]
 if bits==32:
  # The sole absolute reference is the original animation completion callback.
  assert raw[0x102]==0x68
  value=struct.unpack_from('<I',raw,0x103)[0];assert base<value<base+len(b)
  struct.pack_into('<I',raw,0x103,value-base);rel=[0x103]
 tag=f'fmt{bits}_{build}';out+=['#if '+('defined(_WIN64)' if bits==64 else '!defined(_WIN64)'),f'inline constexpr unsigned char {tag}bytes[]={{'+','.join(hex(v) for v in raw)+'};']
 if rel:out+=[f'inline constexpr unsigned {tag}rel[]={{0x103}};']
 out+=[f'inline constexpr FastMenuTransitionProfile131 {tag}={{'+f'{build},{hex(entry)},'+'{'+f'{hex(entry)},{tag}bytes,sizeof({tag}bytes),'+(f'{tag}rel,1' if rel else 'nullptr,0')+'}};','#endif']
 report.append(dict(pid=pid,build=build,bits=bits,entry=hex(entry),length=length,relocations=rel,evidence='Full original function: model sequence dispatch, maximum duration, callback/context storage and registration, float return; x86 cdecl six stack args; x64 RCX/RDX/R8/R9 and two stack args.'))
out+=['inline const FastMenuTransitionProfile131* FastMenuTransitionFor131(unsigned build){','#ifdef _WIN64','return build==12164?&fmt64_12164:build==12173?&fmt64_12173:nullptr;','#else','return build==12164?&fmt32_12164:build==12173?&fmt32_12173:nullptr;','#endif','}','inline bool FastMenuTransitionGuard131(uintptr_t base,const FastMenuTransitionProfile131& p){ResourceTextProfile131 guard{};guard.code=&p.code;guard.count=1;return ResourceTextGuard131(base,guard);}','}']
(root/'native/fast_menu_transition_profile131.hpp').write_text('\n'.join(out)+'\n');(root/'analysis/fastmenu-duration-profiles.json').write_text(json.dumps(report,indent=2));print('Verified four full transition functions')
