#pragma once
#include "hero_skill_points129.hpp"
namespace wht {
struct ReselectApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 uintptr_t(__thiscall* player)(uintptr_t,unsigned)=nullptr;
 void(__cdecl* clear)()=nullptr;
 void(__thiscall* select)(uintptr_t,uintptr_t,unsigned,int,int,int)=nullptr;
 void(__thiscall* update)(uintptr_t,int)=nullptr;
 explicit operator bool()const{return agent&&player&&clear&&select&&update;}
};
inline ReselectApi129 ResolveReselect129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char select[]={0x55,0x8b,0xec,0x83,0xec,0x08,0x53,0x57};
  const unsigned char player[]={0x55,0x8b,0xec,0x8b,0x45,0x08,0x83,0xf8,0x1b};
  const unsigned char update[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
  const unsigned char clear[]={0x8b,0x0d};
  // Confirm raw selection entry and the exact SelectUnit call target.
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0xefc40),select,sizeof(select))||memcmp((void*)(base+0x777d0),player,sizeof(player))||memcmp((void*)(base+0xf3070),update,sizeof(update))||memcmp((void*)(base+0x8a810),clear,sizeof(clear)))return {};
  const auto target=*reinterpret_cast<int32_t*>(base+0xa6842);
  if(base+0xa6846+target!=base+0xefc40)return {};
  return {reinterpret_cast<decltype(ReselectApi129::agent)>(base+0x707f0),reinterpret_cast<decltype(ReselectApi129::player)>(base+0x777d0),reinterpret_cast<decltype(ReselectApi129::clear)>(base+0x8a810),reinterpret_cast<decltype(ReselectApi129::select)>(base+0xefc40),reinterpret_cast<decltype(ReselectApi129::update)>(base+0xf3070)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ReselectUnit129(uintptr_t base,const SkillSelection129& selected,const ReselectApi129& api,const char** error=nullptr){
 if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity)return HeroSkillPointsFailure129(error,"Select a unit first");
  const auto before=SelectedSkill129(base);
  if(before.unit!=selected.unit||before.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return HeroSkillPointsFailure129(error,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<unsigned*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<unsigned*>(agent+0x20))return HeroSkillPointsFailure129(error,"Selected unit is no longer live");
  if(!(*reinterpret_cast<unsigned*>(selected.unit+0x20)&2))return HeroSkillPointsFailure129(error,"Unit cannot be selected");
  const auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(!world)return HeroSkillPointsFailure129(error,"Match context unavailable");
  const auto slot=*reinterpret_cast<uint16_t*>(world+0x28);if(slot>=24)return HeroSkillPointsFailure129(error,"Local player unavailable");
  const auto player=api.player(world,slot);if(!player)return HeroSkillPointsFailure129(error,"Player context unavailable");
  const auto manager=*reinterpret_cast<uintptr_t*>(player+0x34);if(!manager)return HeroSkillPointsFailure129(error,"Selection manager unavailable");
  api.clear();
  if(*reinterpret_cast<uintptr_t*>(base+0xd3b6f4)!=world||api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity))!=agent||*reinterpret_cast<unsigned*>(agent+0x20))return HeroSkillPointsFailure129(error,"Unit or match changed during reselection");
  api.select(manager,selected.unit,slot,0,1,1);api.update(manager,0);
  const auto after=SelectedSkill129(base);
  return (after.unit==selected.unit&&after.identity==selected.identity)||HeroSkillPointsFailure129(error,"Game did not reselect the unit");
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native reselection failed");}
}
}
