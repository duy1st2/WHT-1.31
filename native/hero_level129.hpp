#pragma once
#include "hero_skill_points129.hpp"
#include <climits>
namespace wht {
struct HeroLevelApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 int(__thiscall* level)(uintptr_t)=nullptr;
 int(__thiscall* required)(uintptr_t,int)=nullptr;
 int(__thiscall* experience)(uintptr_t)=nullptr;
 uint32_t*(__cdecl* real)(uint32_t*,int)=nullptr;
 void(__thiscall* add)(uintptr_t,const uint32_t*,int,int,int,int,int)=nullptr;
 int(__thiscall* strip)(uintptr_t,int)=nullptr;
 explicit operator bool()const{return agent&&level&&required&&experience&&real&&add&&strip;}
};
inline HeroLevelApi129 ResolveHeroLevelApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char level[]={0x8b,0x81,0xfc,0x03,0,0,0x85,0xc0,0x74,0x0a};
  const unsigned char required[]={0x55,0x8b,0xec,0x8b,0x89,0xfc,0x03,0,0,0x85,0xc9};
  const unsigned char experience[]={0x8b,0x81,0xfc,0x03,0,0,0x85,0xc0,0x75,0x01,0xc3};
  const unsigned char real[]={0x55,0x8b,0xec,0x8b,0x45,0x0c,0x85,0xc0};
  const unsigned char strip[]={0x55,0x8b,0xec,0x0f,0xb6,0x41,0x37,0x83,0xf8,0x41};
  const unsigned char add[]={0x55,0x8b,0xec,0x8b,0x89,0xfc,0x03,0,0,0x85,0xc9,0x74,0x06};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x291920),level,sizeof(level))||
     memcmp((void*)(base+0x291ae0),required,sizeof(required))||memcmp((void*)(base+0x293780),experience,sizeof(experience))||
     memcmp((void*)(base+0x5a1620),real,sizeof(real))||memcmp((void*)(base+0x29ea60),strip,sizeof(strip))||
     memcmp((void*)(base+0x290910),add,sizeof(add)))return {};
  return {reinterpret_cast<decltype(HeroLevelApi129::agent)>(base+0x707f0),reinterpret_cast<decltype(HeroLevelApi129::level)>(base+0x291920),
   reinterpret_cast<decltype(HeroLevelApi129::required)>(base+0x291ae0),reinterpret_cast<decltype(HeroLevelApi129::experience)>(base+0x293780),
   reinterpret_cast<decltype(HeroLevelApi129::real)>(base+0x5a1620),reinterpret_cast<decltype(HeroLevelApi129::add)>(base+0x290910),
   reinterpret_cast<decltype(HeroLevelApi129::strip)>(base+0x29ea60)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ParseHeroLevel129(const std::wstring& argument,int& value){
 value=10;if(argument.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream in(argument);long long number=0;std::wstring extra;
 if(!(in>>number)||in>>extra||number<1||number>10000)return false;value=int(number);return true;
}
inline bool SetHeroLevel129(uintptr_t base,const SkillSelection129& selected,const HeroLevelApi129& api,int requested,int& actual,const char** error=nullptr){
 actual=0;if(error)*error=nullptr;
 __try {
  auto valid=[&](){
   if(!base||!api||!selected.unit||!selected.identity)return false;
   const auto current=SelectedSkill129(base);if(current.unit!=selected.unit||current.identity!=selected.identity)return false;
   const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
   if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return false;
   const auto unit=selected.unit;const auto kind=*reinterpret_cast<unsigned char*>(unit+0x37);
   return *reinterpret_cast<uintptr_t*>(unit)==base+0xa9c504&&kind>='A'&&kind<='Z'&&
    !(*reinterpret_cast<uint32_t*>(unit+0x164)&0x40000000)&&*reinterpret_cast<uintptr_t*>(unit+0x3fc);
  };
  if(requested<1||requested>10000||!valid())return HeroSkillPointsFailure129(error,"Select a live hero and use #lvl [1..10000]");
  const int before=api.level(selected.unit);if(before<1||before>10000)return HeroSkillPointsFailure129(error,"Invalid hero level");
  if(requested<before)api.strip(selected.unit,before-requested);
  else if(requested>before){
   const int target=api.required(selected.unit,requested-1),xp=api.experience(selected.unit);
   const long long delta=static_cast<long long>(target)-xp;
   if(!valid()||xp<0||delta<=0||delta>INT_MAX)return HeroSkillPointsFailure129(error,"Invalid experience required for level");
   uint32_t value=0;const auto real=api.real(&value,int(delta));
   if(real!=&value||!valid())return HeroSkillPointsFailure129(error,"Hero changed before level update");
   api.add(selected.unit,real,1,0,0,0,0); // SetHeroLevel: show eye candy, other native flags zero.
  }
  if(!valid())return HeroSkillPointsFailure129(error,"Hero changed during level update");
  actual=api.level(selected.unit);
  return actual==requested||HeroSkillPointsFailure129(error,"Game did not apply requested hero level");
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native hero level access failed");}
}
inline bool SetSelectedHeroLevel129(uintptr_t base,int requested,int& actual,const char** error=nullptr){
 const auto api=ResolveHeroLevelApi129(base);if(!api)return HeroSkillPointsFailure129(error,"Hero level function signature mismatch");
 return SetHeroLevel129(base,SelectedSkill129(base),api,requested,actual,error);
}
}
