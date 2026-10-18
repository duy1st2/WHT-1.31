#pragma once
#include "all_skills131.hpp"
namespace wht {
struct HeroProfile131 {unsigned build;uintptr_t bar,button,listener,lookup;};
#ifdef _WIN64
inline constexpr HeroProfile131 heroProfiles131[]={
 {12164,0x1c04558,0x1c04428,0x1c04648,0xc8033e},
 {12173,0x1c33418,0x1c332e8,0x1c33508,0xcc31be}
};
#else
inline constexpr HeroProfile131 heroProfiles131[]={
 {12164,0x1825ca0,0x1825c08,0x1825d18,0xb30481},
 {12173,0x17dab80,0x17daae8,0x17dabf8,0xaefc61}
};
#endif
inline const HeroProfile131* HeroProfileFor131(unsigned build){for(const auto& p:heroProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool HeroCode131(uintptr_t base,const HeroProfile131& p){
#ifdef _WIN64
 const unsigned char bytes[]={0x48,0x8b,0x85,0,2,0,0,0x48,0x8b,0x4c,1,8,0x4c,0x8b,0x31,0x49,0x8b,0xb6,0xd8,2,0,0};
#else
 const unsigned char bytes[]={0x8b,0x86,0x4c,1,0,0,3,0xc9,0x8b,0x44,0xc8,4,0x8d,0x4d,8,0x8b,0x38,0xff,0xb7,0xc4,1,0,0};
#endif
 __try{return base&&!memcmp(reinterpret_cast<void*>(base+p.lookup),bytes,sizeof(bytes));}__except(1){return false;}
}
inline int HeroKeySlot131(unsigned key,const Config& c){if(key)for(unsigned slot=0;slot<c.heroes.size();++slot)if(c.heroes[slot]==key)return int(slot);return -1;}
inline uintptr_t HeroBarButton131(uintptr_t base,const HeroProfile131& p,uintptr_t bar,unsigned slot){
 if(slot>=7||!base||!bar)return 0;
 constexpr bool wide=sizeof(uintptr_t)==8;
 constexpr unsigned dimensions=wide?0x1d0:0x120,grid=wide?0x1f8:0x148,listener=wide?0x218:0x158,buttonListener=wide?0x1d0:0x120,unit=wide?0x2d8:0x1c4,visible=wide?0xf1:0x9d;
 __try{
  if(*reinterpret_cast<uintptr_t*>(bar)!=base+p.bar||!*reinterpret_cast<unsigned char*>(bar+visible))return 0;
  if(*reinterpret_cast<unsigned*>(bar+dimensions)!=7||*reinterpret_cast<unsigned*>(bar+dimensions+4)!=1||*reinterpret_cast<unsigned*>(bar+grid)!=7)return 0;
  if(*reinterpret_cast<uintptr_t*>(bar+listener)!=base+p.listener)return 0;
  const auto rows=*reinterpret_cast<uintptr_t*>(bar+grid+sizeof(uintptr_t));if(!rows)return 0;
  const auto row=rows+slot*(wide?0x20:0x10);
  if(*reinterpret_cast<unsigned*>(row)!=1)return 0;
  const auto columns=*reinterpret_cast<uintptr_t*>(row+sizeof(uintptr_t));if(!columns)return 0;
  const auto button=*reinterpret_cast<uintptr_t*>(columns);
  if(!button||*reinterpret_cast<uintptr_t*>(button)!=base+p.button||!*reinterpret_cast<unsigned char*>(button+visible)||!*reinterpret_cast<uintptr_t*>(button+unit))return 0;
  if(*reinterpret_cast<uintptr_t*>(button+buttonListener)!=bar+listener)return 0;
  return button;
 }__except(1){return 0;}
}
// Reuse the verified native button-click dispatcher, with the independently
// identified CHeroBarButton class. Native callback owns selection/double click.
inline bool ClickHero131(uintptr_t base,unsigned build,uintptr_t bar,unsigned slot){
 const auto* p=HeroProfileFor131(build);const auto* skill=SkillProfile(build);if(!p||!skill||!HeroCode131(base,*p))return false;
 const auto button=HeroBarButton131(base,*p,bar,slot);if(!button)return false;
 auto api=*skill;api.buttonTable=p->button;return SkillClick131(button,base,api);
}
}
