#pragma once
#include "hero_skill_points129.hpp"
#include <cmath>
namespace wht {
struct AttackCooldownApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 float*(__thiscall* get)(uintptr_t,float*,unsigned)=nullptr;
 void(__thiscall* set)(uintptr_t,float,unsigned)=nullptr;
 explicit operator bool()const{return agent&&get&&set;}
};
inline bool ParseAttackCooldown129(const std::wstring& args,float& seconds,unsigned& weapon){
 std::wistringstream in(args);int number=1;std::wstring extra;
 if(!(in>>seconds)||!std::isfinite(seconds)||seconds<.01f||seconds>3600.f)return false;
 in>>std::ws;if(!in.eof()){if(!(in>>number)||number<1||number>2||in>>extra)return false;}
 weapon=unsigned(number-1);return true;
}
inline AttackCooldownApi129 ResolveAttackCooldown129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char get[]={0x55,0x8b,0xec,0x8b,0x45,0x0c,0x8b,0x8c,0xc1,0x5c,0x01,0,0};
  const unsigned char set[]={0x55,0x8b,0xec,0x8b,0x45,0x0c,0x8d,0x55,0x08,0x6a,0x01,0x52,0x8d,0x0c,0xc1};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x720990),get,sizeof(get))||memcmp((void*)(base+0x722d80),set,sizeof(set)))return {};
  return {reinterpret_cast<decltype(AttackCooldownApi129::agent)>(base+0x707f0),reinterpret_cast<decltype(AttackCooldownApi129::get)>(base+0x720990),reinterpret_cast<decltype(AttackCooldownApi129::set)>(base+0x722d80)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool SetAttackCooldown129(uintptr_t base,const SkillSelection129& selected,const AttackCooldownApi129& api,float seconds,unsigned weapon,float& actual,const char** error=nullptr){
 actual=0;if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity||weapon>1||!std::isfinite(seconds)||seconds<.01f||seconds>3600.f)return HeroSkillPointsFailure129(error,"Select a unit and use #attackcd seconds [1|2]");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return HeroSkillPointsFailure129(error,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return HeroSkillPointsFailure129(error,"Selected unit is no longer live");
  const auto attack=*reinterpret_cast<uintptr_t*>(selected.unit+0x3f4);
  if(!attack)return HeroSkillPointsFailure129(error,"Selected unit has no attack data");
  api.set(attack,seconds,weapon);
  const auto after=SelectedSkill129(base);
  if(after.unit!=selected.unit||after.identity!=selected.identity||api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity))!=agent||*reinterpret_cast<uint32_t*>(agent+0x20)||*reinterpret_cast<uintptr_t*>(selected.unit+0x3f4)!=attack)return HeroSkillPointsFailure129(error,"Unit changed during cooldown update");
  if(api.get(attack,&actual,weapon)!=&actual||!std::isfinite(actual)||std::abs(actual-seconds)>.0001f)return HeroSkillPointsFailure129(error,"Game did not apply requested attack cooldown");
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native attack cooldown access failed");}
}
}
