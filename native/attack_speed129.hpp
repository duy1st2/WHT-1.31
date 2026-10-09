#pragma once
#include "hero_skill_points129.hpp"
#include <algorithm>
#include <cmath>
namespace wht {
struct AttackSpeedApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 void(__thiscall* set)(uintptr_t,const float*,int)=nullptr;
 void(__thiscall* refresh)(uintptr_t,uintptr_t)=nullptr;
 explicit operator bool()const{return agent&&set&&refresh;}
};
inline bool ParseAttackSpeed129(const std::wstring& args,float& factor){
 factor=2.f;if(args.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream in(args);std::wstring extra;
 if(!(in>>factor)||in>>extra||!std::isfinite(factor))return false;
 factor=(std::clamp)(factor,.1f,50.f);return true;
}
inline AttackSpeedApi129 ResolveAttackSpeed129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char set[]={0x55,0x8b,0xec,0x8b,0x55,0x08,0xf3,0x0f,0x10,0x41,0x08};
  const unsigned char refresh[]={0x55,0x8b,0xec,0x83,0xec,0x08,0x56,0x57,0x8b,0xf9};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x70130),set,sizeof(set))||memcmp((void*)(base+0x722c90),refresh,sizeof(refresh)))return {};
  return {reinterpret_cast<decltype(AttackSpeedApi129::agent)>(base+0x707f0),reinterpret_cast<decltype(AttackSpeedApi129::set)>(base+0x70130),reinterpret_cast<decltype(AttackSpeedApi129::refresh)>(base+0x722c90)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool SetAttackSpeed129(uintptr_t base,const SkillSelection129& selected,const AttackSpeedApi129& api,float requested,float& actual,float& effective,const char** error=nullptr){
 actual=effective=0;if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity||!std::isfinite(requested)||requested<.1f||requested>50.f)return HeroSkillPointsFailure129(error,"Select a unit and use #as [0.1..50]");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return HeroSkillPointsFailure129(error,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return HeroSkillPointsFailure129(error,"Selected unit is no longer live");
  const auto attack=*reinterpret_cast<uintptr_t*>(selected.unit+0x3f4);if(!attack)return HeroSkillPointsFailure129(error,"Selected unit has no attack data");
  // The native attack constructor installs the bounded float property at +1B0.
  if(*reinterpret_cast<uintptr_t*>(attack+0x1b0)!=base+0xa6e2f0)return HeroSkillPointsFailure129(error,"Attack speed property signature mismatch");
  const float minimum=*reinterpret_cast<float*>(base+0xdb4cac),maximum=*reinterpret_cast<float*>(base+0xdb4c6c);
  if(!std::isfinite(minimum)||!std::isfinite(maximum)||minimum<=0||maximum<minimum)return HeroSkillPointsFailure129(error,"Invalid map attack speed limits");
  api.set(attack+0x1b0,&requested,1);
  auto after=SelectedSkill129(base);
  if(after.unit!=selected.unit||after.identity!=selected.identity||api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity))!=agent||*reinterpret_cast<uint32_t*>(agent+0x20)||*reinterpret_cast<uintptr_t*>(selected.unit+0x3f4)!=attack)return HeroSkillPointsFailure129(error,"Unit changed during attack speed update");
  actual=*reinterpret_cast<float*>(attack+0x1b4);
  if(!std::isfinite(actual)||std::abs(actual-requested)>.0001f)return HeroSkillPointsFailure129(error,"Game did not apply requested attack speed");
  api.refresh(attack,selected.unit); // Native animation/stat refresh, RET4.
  after=SelectedSkill129(base);
  if(after.unit!=selected.unit||after.identity!=selected.identity)return HeroSkillPointsFailure129(error,"Unit changed during attack speed refresh");
  effective=(std::clamp)(actual,minimum,maximum);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native attack speed access failed");}
}
}
