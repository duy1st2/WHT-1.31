#pragma once
inline void CheckMultiboard131(){
 check(!wht::MultiboardProfileFor131(129),"unprofiled multiboard build rejected");
 for(const auto& p:wht::multiboardProfiles131){
  constexpr bool wide=sizeof(uintptr_t)==8;const uintptr_t base=0x10000000;
  alignas(uintptr_t) unsigned char frame[0x320]{};
  *reinterpret_cast<uintptr_t*>(frame)=base+p.table;frame[wide?0xf1:0x9d]=1;
  for(unsigned off:{wide?0x288u:0x174u,wide?0x2d0u:0x1a4u,wide?0x2e8u:0x1b0u,wide?0x2f0u:0x1b4u})*reinterpret_cast<uintptr_t*>(frame+off)=1;
  const auto board=reinterpret_cast<uintptr_t>(frame);const unsigned state=wide?0x308:0x1c4;
  frame[state+1]=0xab;frame[state+2]=0xcd;frame[state+3]=0xef;
  unsigned small=0,large=0;auto minimize=[&](uintptr_t f){check(f==board,"native board passed to minimize");++small;frame[state]=1;};auto maximize=[&](uintptr_t f){check(f==board,"native board passed to maximize");++large;frame[state]=0;};
  check(wht::MultiboardState131(base,p,board)==0,"state is one byte, adjacent flags ignored");
  check(wht::ToggleMultiboardFrame131(base,p,board,minimize,maximize)&&small==1&&!large,"expanded board minimizes");
  check(wht::ToggleMultiboardFrame131(base,p,board,minimize,maximize)&&large==1&&frame[state]==0,"minimized board maximizes");
  frame[state]=2;check(!wht::ToggleMultiboardFrame131(base,p,board,minimize,maximize)&&small==1&&large==1,"invalid state submits no callback");frame[state]=0;
  frame[wide?0xf1:0x9d]=0;check(wht::MultiboardState131(base,p,board)<0,"hidden map board refused");frame[wide?0xf1:0x9d]=1;
  for(unsigned off:{wide?0x288u:0x174u,wide?0x2d0u:0x1a4u,wide?0x2e8u:0x1b0u,wide?0x2f0u:0x1b4u}){*reinterpret_cast<uintptr_t*>(frame+off)=0;check(wht::MultiboardState131(base,p,board)<0,"incomplete native board refused");*reinterpret_cast<uintptr_t*>(frame+off)=1;}
  ++*reinterpret_cast<uintptr_t*>(frame);check(wht::MultiboardState131(base,p,board)<0,"unrelated frame refused");
  check(wht::MultiboardState131(base,p,0)<0&&!wht::MultiboardCode131(0,p),"absent board/base refused");
 }
}
