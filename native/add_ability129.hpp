#pragma once
#include "remove_ability129.hpp"
#include <algorithm>
namespace wht {
inline bool ParseAddAbility129(const std::wstring& args,uint32_t& code,int& level){
 std::wistringstream in(args);std::wstring raw,extra;level=1;
 if(!(in>>raw)||!ParseRemoveAbility129(raw,code))return false;
 in>>std::ws;if(in.eof())return true;
 return bool(in>>level)&&!(in>>extra)&&level>=1&&level<=10000;
}
inline bool StepAbilityLevel129(uintptr_t base,uintptr_t ability,bool up){
 __try {
  const auto table=*reinterpret_cast<uintptr_t*>(ability);
  const auto function=*reinterpret_cast<uintptr_t*>(table+(up?0x2e4:0x2e8));
  MEMORY_BASIC_INFORMATION info{};
  if(!VirtualQuery(reinterpret_cast<void*>(function),&info,sizeof(info))||
     reinterpret_cast<uintptr_t>(info.AllocationBase)!=base||!(info.Protect&0xf0)||(info.Protect&PAGE_GUARD))return false;
  using Step=void(__thiscall*)(uintptr_t);reinterpret_cast<Step>(function)(ability);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct AddAbilityApi129 {
 RemoveAbilityApi129 common;
 void(__cdecl* before)(uintptr_t)=nullptr;
 uintptr_t(__cdecl* add)(uintptr_t,uint32_t,int,int,int)=nullptr;
 void(__cdecl* after)(uintptr_t)=nullptr;
 int(__thiscall* maximum)(uintptr_t)=nullptr;
 bool(*step)(uintptr_t,uintptr_t,bool)=nullptr;
 explicit operator bool()const{return common&&before&&add&&after&&maximum&&step;}
};
struct AddAbilityResult129 {bool added=false;int level=0;};
inline AddAbilityApi129 ResolveAddAbilityApi129(uintptr_t base){
 const auto common=ResolveRemoveAbilityApi129(base);if(!common)return {};
 __try {
  const unsigned char addSig[]={0x55,0x8b,0xec,0x57,0x8b,0x7d,0x08,0x85,0xff,0x75,0x05,0x33,0xc0,0x5f,0x5d,0xc3};
  const unsigned char callbackSig[]={0x55,0x8b,0xec,0x6a,0x01,0xff,0x75,0x08,0x68};
  const unsigned char maxSig[]={0x56,0x8b,0xf1,0x8b,0x46,0x58,0x85,0xc0,0x74,0x0b,0x83,0x78,0x2c,0};
  if(memcmp(reinterpret_cast<void*>(base+0x6f3ba0),addSig,sizeof(addSig))||
     memcmp(reinterpret_cast<void*>(base+0x2e0d20),callbackSig,sizeof(callbackSig))||
     *reinterpret_cast<uintptr_t*>(base+0x2e0d29)!=base+0x2e0d00||
     memcmp(reinterpret_cast<void*>(base+0x2dd220),callbackSig,sizeof(callbackSig))||
     *reinterpret_cast<uintptr_t*>(base+0x2dd229)!=base+0x2dd200||
     memcmp(reinterpret_cast<void*>(base+0x6f5c0),maxSig,sizeof(maxSig)))return {};
  return {common,reinterpret_cast<decltype(AddAbilityApi129::before)>(base+0x2e0d20),
          reinterpret_cast<decltype(AddAbilityApi129::add)>(base+0x6f3ba0),
          reinterpret_cast<decltype(AddAbilityApi129::after)>(base+0x2dd220),
          reinterpret_cast<decltype(AddAbilityApi129::maximum)>(base+0x6f5c0),StepAbilityLevel129};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline int CurrentAbilityLevel129(uintptr_t ability){
 const int stored=*reinterpret_cast<int*>(ability+0x50);return stored>=0&&stored<10000?stored+1:0;
}
inline bool AddAbilityToUnit129(uintptr_t base,const SkillSelection129& selected,const AddAbilityApi129& api,
 uint32_t code,int requested,AddAbilityResult129& result,const char** error=nullptr){
 result={};if(error)*error=nullptr;
 if(!code||requested<1||requested>10000||!api)return RemoveAbilityFailure129(error,"Invalid ability, level or runtime");
 __try {
  if(!LiveRemoveAbilityUnit129(base,selected,api.common))return RemoveAbilityFailure129(error,"Select a live unit first");
  auto ability=api.common.find(selected.unit,code,0,1,1,1);
  if(!ability){
   api.before(selected.unit);
   if(!LiveRemoveAbilityUnit129(base,selected,api.common))return RemoveAbilityFailure129(error,"Unit changed before ability creation");
   api.add(selected.unit,code,0,0,0);
   if(!LiveRemoveAbilityUnit129(base,selected,api.common))return RemoveAbilityFailure129(error,"Unit changed during ability creation");
   api.after(selected.unit);
   if(!LiveRemoveAbilityUnit129(base,selected,api.common))return RemoveAbilityFailure129(error,"Unit changed during post-add notification");
   api.common.refresh(selected.unit);
   if(!LiveRemoveAbilityUnit129(base,selected,api.common))return RemoveAbilityFailure129(error,"Unit changed after ability creation");
   ability=api.common.find(selected.unit,code,0,1,1,1);
   if(!ability)return RemoveAbilityFailure129(error,"Ability was not added (unknown rawcode or map restriction)");
   result.added=true;
  }
  result.level=CurrentAbilityLevel129(ability);
  if(!result.level)return RemoveAbilityFailure129(error,"Ability level is unavailable");
  // Legacy #sabi only calls SetUnitAbilityLevel for a requested level above one.
  if(requested>1){
   const int maximum=api.maximum(ability);
   if(maximum<1||maximum>10000)return RemoveAbilityFailure129(error,"Ability level limit is unavailable");
   const int target=(std::min)(requested,maximum);
   for(int steps=0;result.level!=target&&steps<10000;++steps){
    const bool up=result.level<target;const int previous=result.level;
    if(!api.step(base,ability,up))return RemoveAbilityFailure129(error,"Native ability level change failed");
    if(!LiveRemoveAbilityUnit129(base,selected,api.common)||api.common.find(selected.unit,code,0,1,1,1)!=ability)
     return RemoveAbilityFailure129(error,"Unit or ability changed during level update");
    result.level=CurrentAbilityLevel129(ability);
    if(result.level!=previous+(up?1:-1))return RemoveAbilityFailure129(error,"Ability did not advance to the requested level");
   }
   if(result.level!=target)return RemoveAbilityFailure129(error,"Ability level update did not finish");
   api.common.refresh(selected.unit);
   if(!LiveRemoveAbilityUnit129(base,selected,api.common)||api.common.find(selected.unit,code,0,1,1,1)!=ability)
    return RemoveAbilityFailure129(error,"Unit or ability changed during panel refresh");
   result.level=CurrentAbilityLevel129(ability);
   if(result.level!=target)return RemoveAbilityFailure129(error,"Map changed the resulting ability level");
  }
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return RemoveAbilityFailure129(error,"Native ability creation or level update failed");}
}
inline bool AddSelectedAbility129(uintptr_t base,uint32_t code,int level,AddAbilityResult129& result,const char** error=nullptr){
 result={};if(error)*error=nullptr;const auto selected=SelectedSkill129(base);
 if(!selected.unit||!selected.identity)return RemoveAbilityFailure129(error,"Select a unit first");
 const auto api=ResolveAddAbilityApi129(base);
 if(!api)return RemoveAbilityFailure129(error,"Ability creation function signature mismatch");
 return AddAbilityToUnit129(base,selected,api,code,level,result,error);
}
}
