#pragma once
#include "hero_skill_points129.hpp"
#include <climits>
namespace wht {
struct BaseDamageApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 int(__thiscall* get)(uintptr_t,int)=nullptr;
 void(__thiscall* add)(uintptr_t,int,unsigned)=nullptr;
 explicit operator bool()const{return agent&&get&&add;}
};
inline BaseDamageApi129 ResolveBaseDamageApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char get[]={0x55,0x8b,0xec,0x8b,0x45,0x08,0x8b,0x84,0x81,0xa4,0,0,0,0x5d,0xc2,0x04};
  const unsigned char add[]={0x55,0x8b,0xec,0x8b,0x55,0x08,0xb8,0x01,0,0,0,0x56,0x8b,0x75,0x0c};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x720af0),get,sizeof(get))||
     memcmp((void*)(base+0x71e810),add,sizeof(add)))return {};
  return {reinterpret_cast<decltype(BaseDamageApi129::agent)>(base+0x707f0),
   reinterpret_cast<decltype(BaseDamageApi129::get)>(base+0x720af0),reinterpret_cast<decltype(BaseDamageApi129::add)>(base+0x71e810)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ParseBaseDamage129(const std::wstring& argument,int& value){
 value=50;if(argument.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream in(argument);long long parsed=0;std::wstring extra;
 if(!(in>>parsed)||in>>extra||parsed<0||parsed>1000000000)return false;value=int(parsed);return true;
}
inline bool SetBaseDamage129(uintptr_t base,const SkillSelection129& selected,const BaseDamageApi129& api,int value,int& actual,const char** error=nullptr){
 actual=0;if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity||value<0||value>1000000000)return HeroSkillPointsFailure129(error,"Select a unit and use #udmgb [0..1000000000]");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)
   return HeroSkillPointsFailure129(error,"Selected unit changed or is unsupported");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return HeroSkillPointsFailure129(error,"Selected unit is no longer live");
  const auto attack=*reinterpret_cast<uintptr_t*>(selected.unit+0x3f4);
  if(!attack)return HeroSkillPointsFailure129(error,"Selected unit has no attack data");
  const int before=api.get(attack,0);const long long delta=static_cast<long long>(value)-before;
  if(delta<INT_MIN||delta>INT_MAX)return HeroSkillPointsFailure129(error,"Base damage delta would overflow");
  if(delta)api.add(attack,int(delta),1); // Native weapon1 mask, matching BlzSetUnitBaseDamage.
  const auto after=SelectedSkill129(base);
  if(after.unit!=selected.unit||after.identity!=selected.identity)return HeroSkillPointsFailure129(error,"Selected unit changed during damage update");
  if(api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity))!=agent||*reinterpret_cast<uint32_t*>(agent+0x20)||
     *reinterpret_cast<uintptr_t*>(selected.unit+0x3f4)!=attack)return HeroSkillPointsFailure129(error,"Attack data changed during damage update");
  actual=api.get(attack,0);return actual==value||HeroSkillPointsFailure129(error,"Game did not apply requested base damage");
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native base damage access failed");}
}
inline bool SetSelectedBaseDamage129(uintptr_t base,int value,int& actual,const char** error=nullptr){
 const auto api=ResolveBaseDamageApi129(base);if(!api)return HeroSkillPointsFailure129(error,"Base damage function signature mismatch");
 return SetBaseDamage129(base,SelectedSkill129(base),api,value,actual,error);
}
}
