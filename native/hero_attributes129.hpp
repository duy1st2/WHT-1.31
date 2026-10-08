#pragma once
#include "hero_skill_points129.hpp"
namespace wht {
enum class HeroAttribute129 : unsigned { Strength, Agility, Intelligence, All };
struct HeroAttributesApi129 {
 using GetFn=int(__thiscall*)(uintptr_t,int);
 using SetFn=void(__thiscall*)(uintptr_t,int,int);
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 GetFn get[3]{};
 SetFn set[3]{};
 explicit operator bool()const{return agent&&get[0]&&get[1]&&get[2]&&set[0]&&set[1]&&set[2];}
};
inline HeroAttributesApi129 ResolveHeroAttributesApi129(uintptr_t base){
 if(!base)return {};
 __try {
  constexpr uintptr_t getters[]={0x292aa0,0x290c20,0x291760};
  constexpr uintptr_t setters[]={0x29db30,0x29c990,0x29ce80};
  constexpr uintptr_t getWrappers[]={0x92b70,0x92830,0x928a0};
  constexpr uintptr_t setWrappers[]={0xa8ac0,0xa87d0,0xa8840};
  const unsigned char agentSig[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char getSig[]={0x55,0x8b,0xec,0x81,0xec,0xc8,0,0,0};
  const unsigned char setSig[]={0x55,0x8b,0xec,0x81,0xec,0xcc,0,0,0};
  const unsigned char intSetSig[]={0x55,0x8b,0xec,0x56,0x8b,0xb1,0xfc,0x03};
  const unsigned char wrapperSig[]={0x55,0x8b,0xec,0x56,0xff,0x75,0x08,0xe8};
  if(memcmp(reinterpret_cast<void*>(base+0x707f0),agentSig,sizeof(agentSig)))return {};
  HeroAttributesApi129 api;api.agent=reinterpret_cast<decltype(api.agent)>(base+0x707f0);
  for(unsigned i=0;i<3;++i){
   const auto getterSignature=(i==2)?setSig:getSig;
   if(memcmp(reinterpret_cast<void*>(base+getters[i]),getterSignature,sizeof(getSig))||
      memcmp(reinterpret_cast<void*>(base+setters[i]),i==2?intSetSig:setSig,i==2?sizeof(intSetSig):sizeof(setSig))||
      memcmp(reinterpret_cast<void*>(base+getWrappers[i]),wrapperSig,sizeof(wrapperSig))||
      memcmp(reinterpret_cast<void*>(base+setWrappers[i]),wrapperSig,sizeof(wrapperSig)))return {};
   api.get[i]=reinterpret_cast<HeroAttributesApi129::GetFn>(base+getters[i]);
   api.set[i]=reinterpret_cast<HeroAttributesApi129::SetFn>(base+setters[i]);
  }
  return api;
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ParseHeroAttributeValue129(const std::wstring& argument,int& value){
 value=500;
 if(argument.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream input(argument);long long parsed=0;std::wstring extra;
 if(!(input>>parsed)||input>>extra||parsed<1||parsed>10000)return false;
 value=int(parsed);return true;
}
inline bool SetHeroAttributes129(uintptr_t base,const SkillSelection129& selected,const HeroAttributesApi129& api,
 HeroAttribute129 attribute,int value,int actual[3],const char** failure=nullptr){
 if(failure)*failure=nullptr;for(unsigned i=0;i<3;++i)actual[i]=0;
 __try {
  if(!base||!api||value<1||value>10000||unsigned(attribute)>3||!selected.unit||!selected.identity)
   return HeroSkillPointsFailure129(failure,"Select a hero and use #str/#agi/#int/#fullatt [1..10000]");
  if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)
   return HeroSkillPointsFailure129(failure,"Selected object is not a supported unit");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity)
   return HeroSkillPointsFailure129(failure,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))
   return HeroSkillPointsFailure129(failure,"Selected unit is no longer live");
  const auto unit=selected.unit;const auto kind=*reinterpret_cast<unsigned char*>(unit+0x37);
  if(kind<'A'||kind>'Z'||(*reinterpret_cast<uint32_t*>(unit+0x164)&0x40000000)||
     !*reinterpret_cast<uintptr_t*>(unit+0x3fc))
   return HeroSkillPointsFailure129(failure,"Select a hero first");
  const unsigned first=attribute==HeroAttribute129::All?0:unsigned(attribute);
  const unsigned last=attribute==HeroAttribute129::All?3:first+1;
  for(unsigned i=first;i<last;++i){
   api.set[i](unit,value,1); // permanent, matching SetHeroStr/Agi/Int native wrappers
   const auto after=SelectedSkill129(base);
   if(after.unit!=unit||after.identity!=selected.identity)
    return HeroSkillPointsFailure129(failure,"Selected hero changed during the command");
   actual[i]=api.get[i](unit,1); // include bonuses, matching the requested total
   if(actual[i]!=value)return HeroSkillPointsFailure129(failure,"Game did not apply the requested hero attribute");
  }
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(failure,"Native hero attribute access failed");}
}
inline bool SetSelectedHeroAttributes129(uintptr_t base,HeroAttribute129 attribute,int value,int actual[3],const char** failure=nullptr){
 const auto selected=SelectedSkill129(base);
 const auto api=ResolveHeroAttributesApi129(base);
 if(!api)return HeroSkillPointsFailure129(failure,"Hero attribute function signature mismatch");
 return SetHeroAttributes129(base,selected,api,attribute,value,actual,failure);
}
}
