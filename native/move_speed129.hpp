#pragma once
#include "hero_skill_points129.hpp"
#include <cmath>
namespace wht {
struct MoveSpeedApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 float*(__thiscall* get)(uintptr_t,float*)=nullptr;
 void(__thiscall* set)(uintptr_t,const float*)=nullptr;
 explicit operator bool()const{return agent&&get&&set;}
};
inline bool ParseMoveSpeed129(const std::wstring& args,float& value){
 value=522;if(args.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream in(args);std::wstring extra;
 if(!(in>>value)||in>>extra||!std::isfinite(value))return false;
 value=(std::max)(0.f,(std::min)(2000.f,value));return true;
}
inline MoveSpeedApi129 ResolveMoveSpeed129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char common[]={0x55,0x8b,0xec,0x8b,0x89,0xf8,0x03,0,0,0x85,0xc9};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x291a50),common,sizeof(common))||
     memcmp((void*)(base+0x29cf90),common,sizeof(common)))return {};
  return {reinterpret_cast<decltype(MoveSpeedApi129::agent)>(base+0x707f0),
   reinterpret_cast<decltype(MoveSpeedApi129::get)>(base+0x291a50),reinterpret_cast<decltype(MoveSpeedApi129::set)>(base+0x29cf90)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ApplyMoveSpeed129(uintptr_t base,const SkillSelection129& selected,const MoveSpeedApi129& api,float value,float& actual,const char** error=nullptr){
 actual=0;if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity||!std::isfinite(value)||value<0||value>2000)return HeroSkillPointsFailure129(error,"Select a unit and use #ms [0..2000]");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return HeroSkillPointsFailure129(error,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return HeroSkillPointsFailure129(error,"Selected unit is no longer live");
  const auto movement=*reinterpret_cast<uintptr_t*>(selected.unit+0x3f8);
  if(!movement)return HeroSkillPointsFailure129(error,"Unit has no movement data");
  api.set(selected.unit,&value);
  const auto after=SelectedSkill129(base);
  if(after.unit!=selected.unit||after.identity!=selected.identity||api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity))!=agent||*reinterpret_cast<uint32_t*>(agent+0x20)||*reinterpret_cast<uintptr_t*>(selected.unit+0x3f8)!=movement)return HeroSkillPointsFailure129(error,"Unit changed during speed update");
  if(api.get(selected.unit,&actual)!=&actual||!std::isfinite(actual)||actual<0)return HeroSkillPointsFailure129(error,"Could not read movement speed");
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native movement speed access failed");}
}
}
