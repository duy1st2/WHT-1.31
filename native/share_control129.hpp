#pragma once
#include "hero_skill_points129.hpp"
namespace wht {
struct ShareControlApi129 {
 uint32_t(__cdecl* local)()=nullptr;
 uint32_t(__cdecl* player)(int)=nullptr;
 int(__cdecl* slot)(uint32_t)=nullptr;
 void(__cdecl* set)(uint32_t,uint32_t,int,int)=nullptr;
 int(__cdecl* get)(uint32_t,uint32_t,int)=nullptr;
 explicit operator bool()const{return local&&player&&slot&&set&&get;}
};
inline ShareControlApi129 ResolveShareControl129(uintptr_t b){
 if(!b)return {};
 __try {
  const unsigned char player[]={0x55,0x8b,0xec,0xa1};
  const unsigned char set[]={0x55,0x8b,0xec,0x51,0xa1};
  const unsigned char get[]={0x55,0x8b,0xec,0x56,0xff,0x75,0x08};
  const unsigned char slot[]={0x55,0x8b,0xec,0xff,0x75,0x08};
  const unsigned char local[]={0x56,0x8b,0x35};
  if(memcmp((void*)(b+0xa4240),player,sizeof(player))||memcmp((void*)(b+0xa94a0),set,sizeof(set))||memcmp((void*)(b+0x944c0),get,sizeof(get))||memcmp((void*)(b+0x94980),slot,sizeof(slot))||memcmp((void*)(b+0x93790),local,sizeof(local)))return {};
  return {reinterpret_cast<decltype(ShareControlApi129::local)>(b+0x93790),reinterpret_cast<decltype(ShareControlApi129::player)>(b+0xa4240),reinterpret_cast<decltype(ShareControlApi129::slot)>(b+0x94980),reinterpret_cast<decltype(ShareControlApi129::set)>(b+0xa94a0),reinterpret_cast<decltype(ShareControlApi129::get)>(b+0x944c0)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ShareControl129(const ShareControlApi129& api,bool enabled,unsigned& changed,const char** error=nullptr){
 changed=0;if(error)*error=nullptr;
 __try {
  if(!api)return HeroSkillPointsFailure129(error,"Share control function signature mismatch");
  const auto local=api.local();if(!local)return HeroSkillPointsFailure129(error,"Local player unavailable");
  for(int slot=0;slot<24;++slot){
   const auto player=api.player(slot);if(!player||player==local||api.slot(player)!=1)continue;
   if(api.local()!=local)return HeroSkillPointsFailure129(error,"Local player changed");
   const auto previous=api.get(local,player,6)!=0;
   if(previous==enabled)continue;
   api.set(local,player,6,enabled?1:0);
   if((api.get(local,player,6)!=0)!=enabled)return HeroSkillPointsFailure129(error,"Game refused shared control update");
   ++changed;
  }
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native shared control update failed");}
}
}
