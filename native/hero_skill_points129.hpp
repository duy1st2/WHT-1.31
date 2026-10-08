#pragma once
#include "skills129.hpp"
#include <cstring>
#include <limits>
#include <sstream>
namespace wht {
struct HeroSkillPointsApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 int(__thiscall* get)(uintptr_t)=nullptr;
 int(__thiscall* modify)(uintptr_t,int)=nullptr;
 explicit operator bool()const{return agent&&get&&modify;}
};
inline bool HeroSkillPointsFailure129(const char** failure,const char* reason){if(failure)*failure=reason;return false;}
inline HeroSkillPointsApi129 ResolveHeroSkillPointsApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agentSig[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char getSig[]={0x0f,0xb6,0x41,0x37,0x83,0xf8,0x41,0x72};
  const unsigned char modifySig[]={0x55,0x8b,0xec,0x0f,0xb6,0x41,0x37};
  const unsigned char wrapperSig[]={0x55,0x8b,0xec,0x56,0xff,0x75,0x08,0xe8};
  if(memcmp(reinterpret_cast<void*>(base+0x707f0),agentSig,sizeof(agentSig))||
     memcmp(reinterpret_cast<void*>(base+0x292a50),getSig,sizeof(getSig))||
     memcmp(reinterpret_cast<void*>(base+0x297730),modifySig,sizeof(modifySig))||
     memcmp(reinterpret_cast<void*>(base+0xaf4b0),wrapperSig,sizeof(wrapperSig)))return {};
  return {reinterpret_cast<decltype(HeroSkillPointsApi129::agent)>(base+0x707f0),
          reinterpret_cast<decltype(HeroSkillPointsApi129::get)>(base+0x292a50),
          reinterpret_cast<decltype(HeroSkillPointsApi129::modify)>(base+0x297730)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ParseHeroSkillPoints129(const std::wstring& argument,int& amount){
 amount=500;
 if(argument.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream input(argument);long long parsed=0;std::wstring extra;
 if(!(input>>parsed)||input>>extra||parsed<1||parsed>10000)return false;
 amount=int(parsed);return true;
}
inline bool AddHeroSkillPoints129(uintptr_t base,const SkillSelection129& selected,const HeroSkillPointsApi129& api,
 int amount,int& added,int& total,const char** failure=nullptr){
 if(failure)*failure=nullptr;added=total=0;
 __try {
  if(!base||!api||amount<1||amount>10000||!selected.unit||!selected.identity)
   return HeroSkillPointsFailure129(failure,"Select a hero and use #sp [1..10000]");
  if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)
   return HeroSkillPointsFailure129(failure,"Selected object is not a supported unit");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity)
   return HeroSkillPointsFailure129(failure,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))
   return HeroSkillPointsFailure129(failure,"Selected unit is no longer live");
  const auto unit=selected.unit;
  const auto kind=*reinterpret_cast<unsigned char*>(unit+0x37);
  if(kind<'A'||kind>'Z'||(*reinterpret_cast<uint32_t*>(unit+0x164)&0x40000000)||
     !*reinterpret_cast<uintptr_t*>(unit+0x3fc))
   return HeroSkillPointsFailure129(failure,"Select a hero first");
  const int before=api.get(unit);
  if(before<0||before>std::numeric_limits<int>::max()-amount)
   return HeroSkillPointsFailure129(failure,"Hero skill point value is invalid");
  api.modify(unit,amount);
  const auto after=SelectedSkill129(base);
  if(after.unit!=unit||after.identity!=selected.identity)
   return HeroSkillPointsFailure129(failure,"Selected hero changed during the command");
  total=api.get(unit);added=total-before;
  if(added<=0)return HeroSkillPointsFailure129(failure,"Game did not add skill points (possible hero skill cap)");
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(failure,"Native hero skill point access failed");}
}
inline bool AddSelectedHeroSkillPoints129(uintptr_t base,int amount,int& added,int& total,const char** failure=nullptr){
 const auto selected=SelectedSkill129(base);
 const auto api=ResolveHeroSkillPointsApi129(base);
 if(!api)return HeroSkillPointsFailure129(failure,"Hero skill point function signature mismatch");
 return AddHeroSkillPoints129(base,selected,api,amount,added,total,failure);
}
}
