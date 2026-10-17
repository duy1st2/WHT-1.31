#pragma once
#include "common.hpp"
namespace wht {
struct RevealProfile131 {unsigned build;uintptr_t worldTable,flag,visibilityCheck;};
#ifdef _WIN64
inline constexpr RevealProfile131 revealProfiles131[]={
 {12164,0x1c03a08,0x5f3,0xc7ba9c},
 {12173,0x1c328c0,0x5e3,0xcbea9c}};
#else
inline constexpr RevealProfile131 revealProfiles131[]={
 {12164,0x1825828,0x3f3,0xb2c1a4},
 {12173,0x17da708,0x3df,0xaeb9d4}};
#endif
inline const RevealProfile131* RevealProfileFor131(unsigned build){for(const auto& p:revealProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool RevealCode131(uintptr_t base,const RevealProfile131& p){
 __try{const auto code=reinterpret_cast<const uint8_t*>(base+p.visibilityCheck);
  return base&&code[0]==0x80&&code[1]==0xbf&&*reinterpret_cast<const uint32_t*>(code+2)==p.flag&&code[6]==0;
 }__except(1){return false;}
}
struct Reveal131 {
 uintptr_t owner=0;uint8_t original=0;bool applied=false,reported=false;
 void Reset(){*this={};}
 // Only the current UI's exact native world frame can be written. Never keep
 // a previous match's frame alive, or restore through a stale frame pointer.
 bool Apply(uintptr_t base,const RevealProfile131& p,uintptr_t current,bool enabled){
  if(owner&&owner!=current)Reset();
  __try{
   if(!current||*reinterpret_cast<uintptr_t*>(current)!=base+p.worldTable){Reset();return false;}
   auto& flag=*reinterpret_cast<uint8_t*>(current+p.flag);
   if(flag>1){Reset();return false;}
   if(enabled){if(!applied){owner=current;original=flag;applied=true;}else if(flag!=0)original=flag;flag=0;return true;}
   // Restore only if the byte still contains our value. If the engine wrote
   // another value, preserve the engine's update instead of replacing it.
   if(applied&&owner==current&&flag==0)flag=original;
   Reset();return false;
  }__except(1){Reset();return false;}
 }
 bool Update(uintptr_t base,unsigned build,uintptr_t current,bool enabled){auto p=RevealProfileFor131(build);if(!p||!RevealCode131(base,*p)){Reset();return false;}return Apply(base,*p,current,enabled);}
};
}
