#pragma once
#include "ability_button129.hpp"
#include <sstream>
namespace wht {
inline bool ParseRemoveAbility129(const std::wstring& args,uint32_t& code){
 std::wistringstream in(args);std::wstring token,extra;
 if(!(in>>token)||in>>extra)return false;
 if(token.size()>=2&&token[0]==L'0'&&(token[1]==L'x'||token[1]==L'X')){
  if(token.size()!=10)return false;uint32_t value=0;
  for(size_t i=2;i<token.size();++i){const auto c=token[i];unsigned digit=0;
   if(c>=L'0'&&c<=L'9')digit=unsigned(c-L'0');
   else if(c>=L'a'&&c<=L'f')digit=unsigned(c-L'a')+10;
   else if(c>=L'A'&&c<=L'F')digit=unsigned(c-L'A')+10;else return false;
   value=(value<<4)|digit;
  }
  if(!value)return false;code=value;return true;
 }
 return AbilityCode129(token,code);
}
struct RemoveAbilityApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 uintptr_t(__thiscall* find)(uintptr_t,uint32_t,int,int,int,int)=nullptr;
 void(__thiscall* remove)(uintptr_t,uintptr_t)=nullptr;
 void(__cdecl* refresh)(uintptr_t)=nullptr;
 explicit operator bool()const{return agent&&find&&remove&&refresh;}
};
inline bool RemoveAbilityFailure129(const char** error,const char* text){if(error)*error=text;return false;}
inline RemoveAbilityApi129 ResolveRemoveAbilityApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agentSig[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char findSig[]={0x55,0x8b,0xec,0x56,0x8b,0x75,0x08};
  const unsigned char removeSig[]={0x55,0x8b,0xec,0x51,0x53,0x57,0x8b,0x7d,0x08,0x8b,0xd9,0x89,0x5d,0xfc};
  const unsigned char refreshSig[]={0x55,0x8b,0xec,0x6a,0,0x6a,0,0xe8,0xd4,0x58,0xff,0xff,0x83,0xc4,0x08};
  if(memcmp(reinterpret_cast<void*>(base+0x707f0),agentSig,sizeof(agentSig))||
     memcmp(reinterpret_cast<void*>(base+0x2d6570),findSig,sizeof(findSig))||
     memcmp(reinterpret_cast<void*>(base+0x2d8530),removeSig,sizeof(removeSig))||
     memcmp(reinterpret_cast<void*>(base+0x1f6d00),refreshSig,sizeof(refreshSig)))return {};
  return {reinterpret_cast<decltype(RemoveAbilityApi129::agent)>(base+0x707f0),
          reinterpret_cast<decltype(RemoveAbilityApi129::find)>(base+0x2d6570),
          reinterpret_cast<decltype(RemoveAbilityApi129::remove)>(base+0x2d8530),
          reinterpret_cast<decltype(RemoveAbilityApi129::refresh)>(base+0x1f6d00)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool LiveRemoveAbilityUnit129(uintptr_t base,const SkillSelection129& selected,const RemoveAbilityApi129& api){
 if(!base||!api||!selected.unit||!selected.identity)return false;
 if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return false;
 const auto current=SelectedSkill129(base);
 if(current.unit!=selected.unit||current.identity!=selected.identity)return false;
 const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
 return agent&&*reinterpret_cast<uint32_t*>(agent+0xc)==0x2b61676c&&!*reinterpret_cast<uint32_t*>(agent+0x20);
}
inline bool RemoveAbilityFromUnit129(uintptr_t base,const SkillSelection129& selected,const RemoveAbilityApi129& api,
 uint32_t code,bool& removed,const char** error=nullptr){
 removed=false;if(error)*error=nullptr;
 if(!code)return RemoveAbilityFailure129(error,"Invalid ability rawcode");
 __try {
  if(!LiveRemoveAbilityUnit129(base,selected,api))return RemoveAbilityFailure129(error,"Select a live unit first");
  // UnitRemoveAbility AF590 uses these lookup flags, then native removal and UI refresh.
  const auto ability=api.find(selected.unit,code,0,1,1,1);
  if(!ability)return true;
  api.remove(selected.unit,ability);
  if(!LiveRemoveAbilityUnit129(base,selected,api))return RemoveAbilityFailure129(error,"Selected unit changed during ability removal");
  api.refresh(selected.unit);
  if(!LiveRemoveAbilityUnit129(base,selected,api))return RemoveAbilityFailure129(error,"Selected unit changed during panel refresh");
  if(api.find(selected.unit,code,0,1,1,1))return RemoveAbilityFailure129(error,"Ability is still present after native removal");
  removed=true;return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return RemoveAbilityFailure129(error,"Native ability removal failed");}
}
inline bool RemoveSelectedAbility129(uintptr_t base,uint32_t code,bool& removed,const char** error=nullptr){
 removed=false;if(error)*error=nullptr;const auto selected=SelectedSkill129(base);
 if(!selected.unit||!selected.identity)return RemoveAbilityFailure129(error,"Select a unit first");
 const auto api=ResolveRemoveAbilityApi129(base);
 if(!api)return RemoveAbilityFailure129(error,"Ability removal function signature mismatch");
 return RemoveAbilityFromUnit129(base,selected,api,code,removed,error);
}
}
