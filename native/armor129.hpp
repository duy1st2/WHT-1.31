#pragma once
#include "hero_skill_points129.hpp"
#include <cmath>
namespace wht {
struct ArmorApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 float*(__cdecl* subtract)(float*,const float*,const float*)=nullptr;
 void(__thiscall* add)(uintptr_t,const float*)=nullptr;
 explicit operator bool()const{return agent&&subtract&&add;}
};
inline bool ParseArmor129(const std::wstring& args,float& value){
 value=50;if(args.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream in(args);std::wstring extra;
 return bool(in>>value)&&!(in>>extra)&&std::isfinite(value)&&std::abs(value)<=1000000.f;
}
inline ArmorApi129 ResolveArmor129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char subtract[]={0x55,0x8b,0xec,0x8b,0x45,0x10,0x8b,0x4d,0x0c};
  const unsigned char add[]={0x55,0x8b,0xec,0x53,0x56,0x57,0x8b,0xd9,0x6a,0x01,0xff,0x75,0x08};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x5a00b0),subtract,sizeof(subtract))||memcmp((void*)(base+0x280070),add,sizeof(add)))return {};
  return {reinterpret_cast<decltype(ArmorApi129::agent)>(base+0x707f0),reinterpret_cast<decltype(ArmorApi129::subtract)>(base+0x5a00b0),reinterpret_cast<decltype(ArmorApi129::add)>(base+0x280070)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool SetArmor129(uintptr_t base,const SkillSelection129& selected,const ArmorApi129& api,float requested,float& actual,const char** error=nullptr){
 actual=0;if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity||!std::isfinite(requested)||std::abs(requested)>1000000.f)return HeroSkillPointsFailure129(error,"Select a unit and use #armor [-1000000..1000000]");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return HeroSkillPointsFailure129(error,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return HeroSkillPointsFailure129(error,"Selected unit is no longer live");
  const float before=*reinterpret_cast<float*>(selected.unit+0x1e8);float delta=0;
  if(!std::isfinite(before)||api.subtract(&delta,&requested,&before)!=&delta||!std::isfinite(delta))return HeroSkillPointsFailure129(error,"Invalid armor value");
  if(delta)api.add(selected.unit,&delta);
  const auto after=SelectedSkill129(base);
  if(after.unit!=selected.unit||after.identity!=selected.identity||api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity))!=agent||*reinterpret_cast<uint32_t*>(agent+0x20))return HeroSkillPointsFailure129(error,"Unit changed during armor update");
  actual=*reinterpret_cast<float*>(selected.unit+0x1e8);
  return (std::isfinite(actual)&&std::abs(actual-requested)<.01f)||HeroSkillPointsFailure129(error,"Game did not apply requested armor");
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native armor access failed");}
}
}
