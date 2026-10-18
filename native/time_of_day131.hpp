#pragma once
#include "common.hpp"
#include <cstring>
namespace wht {
struct TimeOfDayProfile131 {unsigned build;uintptr_t table,show,hide;};
#ifdef _WIN64
inline constexpr TimeOfDayProfile131 timeOfDayProfiles131[]={
 {12164,0x1c14048,0xfb0bd0,0xfad510},
 {12173,0x1c42f08,0xfed740,0xfea080}
};
#else
inline constexpr TimeOfDayProfile131 timeOfDayProfiles131[]={
 {12164,0x182ddb0,0xdf73a0,0xdf42f0},
 {12173,0x17e2c90,0xdb61a0,0xdb30f0}
};
#endif
inline const TimeOfDayProfile131* TimeOfDayProfileFor131(unsigned build){for(const auto& p:timeOfDayProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool TimeOfDayCode131(uintptr_t base,const TimeOfDayProfile131& p){
#ifdef _WIN64
 const unsigned char show[]={0x40,0x57,0x48,0x83,0xec,0x20,0x8b,0x81,0x50,1,0,0,0x48,0x8b,0xf9,0xa8,1,0x74,0x76,0x83,0xe0,0xfe,0x89,0x81,0x50,1,0,0};
 const unsigned char hide[]={0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0x81,0x50,1,0,0,0x48,0x8b,0xd9,0xa8,1,0x75,0x7e,0x83,0xc8,1,0x89,0x81,0x50,1,0,0};
#else
 const unsigned char show[]={0x56,0x8b,0xf1,0x8b,0x86,0xb8,0,0,0,0xa8,1,0x74,0x64,0x83,0xe0,0xfe,0x89,0x86,0xb8,0,0,0};
 const unsigned char hide[]={0x56,0x8b,0xf1,0x8b,0x86,0xb8,0,0,0,0xa8,1,0x75,0x5b,0x83,0xc8,1,0x89,0x86,0xb8,0,0,0};
#endif
 __try{return base&&!memcmp(reinterpret_cast<void*>(base+p.show),show,sizeof(show))&&!memcmp(reinterpret_cast<void*>(base+p.hide),hide,sizeof(hide));}__except(1){return false;}
}
inline bool ReadTimeOfDayFlags131(uintptr_t base,const TimeOfDayProfile131& p,uintptr_t frame,unsigned& flags){
 __try{if(!base||!frame||*reinterpret_cast<uintptr_t*>(frame)!=base+p.table)return false;flags=*reinterpret_cast<unsigned*>(frame+(sizeof(uintptr_t)==8?0x150:0xb8));return true;}__except(1){return false;}
}
struct TimeOfDay131 {
 uintptr_t owner=0;bool active=false,wasLocalShown=false;
 void Reset(){*this={};}
 template<class Show,class Hide> bool Apply(uintptr_t frame,bool valid,unsigned flags,bool enabled,Show show,Hide hide){
  // Never restore through the old pointer after the current frame changes.
  if(owner!=frame){Reset();owner=frame;}
  if(!valid){Reset();return false;}
  const bool localShown=(flags&1)==0;
  if(enabled){if(!active){active=true;wasLocalShown=localShown;}if(localShown)hide(frame);}
  else if(active){if(wasLocalShown&&!localShown)show(frame);active=false;}
  return true;
 }
 bool Update(uintptr_t base,unsigned build,uintptr_t frame,bool enabled){
  const auto* p=TimeOfDayProfileFor131(build);unsigned flags=0;
  if(!p||!TimeOfDayCode131(base,*p)||!ReadTimeOfDayFlags131(base,*p,frame,flags)){Reset();return false;}
  using Callback=void(__thiscall*)(uintptr_t);
  __try{return Apply(frame,true,flags,enabled,reinterpret_cast<Callback>(base+p->show),reinterpret_cast<Callback>(base+p->hide));}__except(1){Reset();return false;}
 }
};
}
