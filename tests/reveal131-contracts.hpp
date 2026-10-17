#pragma once
#include "reveal131.hpp"
inline void CheckReveal131(){
 const uintptr_t base=0x10000000;
 for(const auto& p:wht::revealProfiles131){
  alignas(uintptr_t) unsigned char frame[0x800]{},other[0x800]{};
  auto object=reinterpret_cast<uintptr_t>(frame),replacement=reinterpret_cast<uintptr_t>(other);
  *reinterpret_cast<uintptr_t*>(frame)=base+p.worldTable;*reinterpret_cast<uintptr_t*>(other)=base+p.worldTable;
  // Independently observed native byte fields. Adjacent bytes deliberately
  // contain non-boolean data to catch the inherited DWORD-write regression.
#ifdef _WIN64
  const unsigned offset=p.build==12164?0x5f3:0x5e3;
#else
  const unsigned offset=p.build==12164?0x3f3:0x3df;
#endif
  frame[offset-1]=0x6b;frame[offset]=1;frame[offset+1]=0x7c;frame[offset+2]=0x8d;
  wht::Reveal131 state;
  check(state.Apply(base,p,object,true)&&!frame[offset]&&state.applied,"native invisible display overrides the verified byte");
  check(frame[offset-1]==0x6b&&frame[offset+1]==0x7c&&frame[offset+2]==0x8d,"reveal never writes neighboring native fields");
  check(state.Apply(base,p,object,true)&&state.original==1,"repeated updates preserve original state");
  state.Apply(base,p,object,false);check(frame[offset]==1&&!state.applied,"disable restores the original native state");
  frame[offset]=0;state.Apply(base,p,object,true);state.Apply(base,p,object,false);check(!frame[offset],"pre-existing native reveal state is preserved");
  state.Apply(base,p,object,true);frame[offset]=1;state.Apply(base,p,object,false);check(frame[offset]==1,"engine-owned update wins over stale captured value");
  state.Apply(base,p,object,true);other[offset]=1;state.Apply(base,p,replacement,true);state.Apply(base,p,replacement,false);
  check(!frame[offset]&&other[offset]==1,"new match restores only its current native frame and never writes stale owner");
  frame[offset]=2;check(!state.Apply(base,p,object,true)&&frame[offset]==2,"unexpected native flag value is refused");
  *reinterpret_cast<uintptr_t*>(frame)=base+p.worldTable+4;frame[offset]=1;check(!state.Apply(base,p,object,true)&&frame[offset]==1,"unrelated native frame is never changed");
 }
 alignas(uintptr_t) unsigned char code[128]{};wht::RevealProfile131 profile{12164,0,0x3f3,64};
 const unsigned char signature[]={0x80,0xbf,0xf3,0x03,0,0,0};memcpy(code+64,signature,7);
 auto module=reinterpret_cast<uintptr_t>(code);check(wht::RevealCode131(module,profile),"verified native visibility instruction accepted");
 code[66]=0xf4;check(!wht::RevealCode131(module,profile),"mismatched native flag displacement refused");
}
