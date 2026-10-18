#pragma once
inline void CheckConsoleUi131(){
 check(!wht::ConsoleProfileFor131(129),"unprofiled console refused");
 for(const auto& p:wht::consoleProfiles131){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char frame[0x200]{};*reinterpret_cast<uintptr_t*>(frame)=base+p.table;
  const auto f=reinterpret_cast<uintptr_t>(frame);auto flags=frame+(sizeof(uintptr_t)==8?0xf0:0x9c);flags[0]=flags[1]=1;flags[2]=0xab;flags[3]=0xcd;
  bool requested=false,attached=false;check(wht::ReadConsole131(base,p,f,requested,attached)&&requested&&attached,"read byte console visibility");
  wht::ConsoleUi131 state;unsigned shown=0,hidden=0;
  auto write=[&](bool v){return wht::WriteConsoleRequested131(base,p,f,v);};
  auto show=[&](uintptr_t owner){check(owner==f&&flags[0]==1,"show after request restored");++shown;flags[1]=1;};auto hide=[&](uintptr_t owner){check(owner==f&&flags[0]==0,"detach after request cleared");++hidden;flags[1]=0;};
  auto apply=[&](bool enabled){bool a=false,b=false;const bool valid=wht::ReadConsole131(base,p,f,a,b);return state.Apply(f,valid,a,b,enabled,write,show,hide);};
  check(apply(true)&&flags[0]==0&&flags[1]==0&&hidden==1,"console hides natively");apply(true);check(hidden==1,"no repeat detach when already hidden");
  flags[0]=flags[1]=1;apply(true);check(hidden==2&&flags[0]==0,"engine reattach is suppressed");
  apply(false);check(shown==1&&flags[0]==1&&flags[1]==1,"disable restores original visibility");apply(false);check(shown==1,"inactive no forced show");
  check(flags[2]==0xab&&flags[3]==0xcd,"byte write preserves adjacent flags");
  flags[0]=flags[1]=0;apply(true);apply(false);check(shown==1&&flags[0]==0,"map-hidden console remains hidden");
  flags[0]=1;flags[1]=0;apply(true);apply(false);check(shown==2&&flags[0]==1,"requested shown but detached restored via native show");
  apply(true);state.Apply(f+8,true,false,false,false,write,show,hide);check(shown==2,"changed console never writes old pointer");
  state.Apply(f,false,false,false,true,write,show,hide);check(!state.owner&&!state.active,"invalid frame clears capture");
  flags[0]=2;check(!wht::ReadConsole131(base,p,f,requested,attached)&&!write(false)&&flags[0]==2,"nonboolean field rejected without write");flags[0]=0;
  ++*reinterpret_cast<uintptr_t*>(frame);check(!write(true)&&!wht::ReadConsole131(base,p,f,requested,attached),"foreign frame refused");
  check(!wht::ConsoleCode131(0,p),"missing native module refused");
 }
}
