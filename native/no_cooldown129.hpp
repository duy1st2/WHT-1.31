#pragma once
#include "hero_skill_points129.hpp"
namespace wht {
struct CooldownResetApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 void(__thiscall* reset)(uintptr_t)=nullptr;
 explicit operator bool()const{return agent&&reset;}
};
inline CooldownResetApi129 ResolveCooldownReset129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char reset[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
  // UnitResetCooldown AF760 tail-calls CUnit 2D8A10, with no stack arguments.
  const unsigned char wrapper[]={0x55,0x8b,0xec,0xff,0x75,0x08,0xe8};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x2d8a10),reset,sizeof(reset))||memcmp((void*)(base+0xaf760),wrapper,sizeof(wrapper)))return {};
  return {reinterpret_cast<decltype(CooldownResetApi129::agent)>(base+0x707f0),reinterpret_cast<decltype(CooldownResetApi129::reset)>(base+0x2d8a10)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ResetCooldown129(uintptr_t base,const SkillSelection129& selected,const CooldownResetApi129& api,const char** error=nullptr){
 if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity)return HeroSkillPointsFailure129(error,"Select a unit first");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return HeroSkillPointsFailure129(error,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return HeroSkillPointsFailure129(error,"Selected unit is no longer live");
  api.reset(selected.unit);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native cooldown reset failed");}
}
struct NoCooldown129 {
 bool enabled=false;uint64_t due=0;
 void Reset(){enabled=false;due=0;}
 template<class Apply> bool Tick(bool allowed,uint64_t now,Apply apply){
  if(!enabled||!allowed||now<due)return false;
  due=now+100;return apply();
 }
};
}
