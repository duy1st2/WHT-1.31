#pragma once
#include "common.hpp"
#include <cstring>
namespace wht {
struct MultiboardProfile131 {unsigned build;uintptr_t table,minimize,maximize;};
#ifdef _WIN64
inline constexpr MultiboardProfile131 multiboardProfiles131[]={
 {12164,0x1c05d20,0xc91cc0,0xc91bc0},
 {12173,0x1c34be0,0xcd4b40,0xcd4a30}
};
#else
inline constexpr MultiboardProfile131 multiboardProfiles131[]={
 {12164,0x18267c8,0xb41a70,0xb419b0},
 {12173,0x17db6a8,0xb01760,0xb016a0}
};
#endif
inline const MultiboardProfile131* MultiboardProfileFor131(unsigned build){for(const auto& p:multiboardProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool MultiboardCode131(uintptr_t base,const MultiboardProfile131& p){
#ifdef _WIN64
 const unsigned char prefix[]={0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x30,0x48,0x8b,0xf1,0x4c,0x8b,0xc1,0x48,0x8b,0x89,0xe8,2,0,0,0x41,0xb1,1,0xba};
 const unsigned char stateWrite[]={0xc6,0x86,8,3,0,0};
 constexpr unsigned eventOffset=32,writeOffset=0xa1;
#else
 const unsigned char prefix[]={0x56,0x57,0x8b,0xf9,0x6a,1,0x57,0x68};
 const unsigned char stateWrite[]={0xc6,0x87,0xc4,1,0,0};
 constexpr unsigned eventOffset=8,writeOffset=0x7a;
#endif
 __try{
  if(!base)return false;
  for(unsigned i=0;i<2;++i){const auto code=base+(i?p.maximize:p.minimize);
   if(memcmp(reinterpret_cast<void*>(code),prefix,sizeof(prefix))||*reinterpret_cast<unsigned*>(code+eventOffset)!=(i?0x1a0387u:0x1a0388u)||memcmp(reinterpret_cast<void*>(code+writeOffset),stateWrite,sizeof(stateWrite))||*reinterpret_cast<unsigned char*>(code+writeOffset+6)!=(i?0:1))return false;
  }
  return true;
 }__except(1){return false;}
}
inline int MultiboardState131(uintptr_t base,const MultiboardProfile131& p,uintptr_t board){
 constexpr bool wide=sizeof(uintptr_t)==8;
 __try{
  if(!base||!board||*reinterpret_cast<uintptr_t*>(board)!=base+p.table||!*reinterpret_cast<unsigned char*>(board+(wide?0xf1:0x9d)))return -1;
  // Refuse an absent map model or incomplete frame construction.
  for(unsigned off:{wide?0x288u:0x174u,wide?0x2d0u:0x1a4u,wide?0x2e8u:0x1b0u,wide?0x2f0u:0x1b4u})if(!*reinterpret_cast<uintptr_t*>(board+off))return -1;
  const auto state=*reinterpret_cast<unsigned char*>(board+(wide?0x308:0x1c4));return state<=1?int(state):-1;
 }__except(1){return -1;}
}
template<class Minimize,class Maximize> bool ToggleMultiboardFrame131(uintptr_t base,const MultiboardProfile131& p,uintptr_t board,Minimize minimize,Maximize maximize){
 const int state=MultiboardState131(base,p,board);if(state<0)return false;
 if(state)maximize(board);else minimize(board);return true;
}
inline bool ToggleMultiboard131(uintptr_t base,unsigned build,uintptr_t board){
 const auto* p=MultiboardProfileFor131(build);if(!p||!MultiboardCode131(base,*p))return false;
 using Callback=void(__thiscall*)(uintptr_t);
 __try{return ToggleMultiboardFrame131(base,*p,board,reinterpret_cast<Callback>(base+p->minimize),reinterpret_cast<Callback>(base+p->maximize));}__except(1){return false;}
}
}
