#pragma once
#include "kill_unit129.hpp"
namespace wht {
struct RemoveUnitApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 void(__thiscall* prepare)(uintptr_t)=nullptr;
 void(__thiscall* remove)(uintptr_t,int,int)=nullptr;
 explicit operator bool()const{return agent&&prepare&&remove;}
};
inline RemoveUnitApi129 ResolveRemoveUnitApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char prepare[]={0x56,0x8b,0xf1,0x8b,0x06,0xff,0x90,0x98,0,0,0};
  const unsigned char remove[]={0x55,0x8b,0xec,0x56,0x8b,0xf1,0x0f,0xb6,0x46,0x37};
  const unsigned char wrapper[]={0x55,0x8b,0xec,0x56,0xff,0x75,0x08,0xe8};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x2670d0),prepare,sizeof(prepare))||
     memcmp((void*)(base+0x29afe0),remove,sizeof(remove))||memcmp((void*)(base+0xa53e0),wrapper,sizeof(wrapper))||
     *reinterpret_cast<uintptr_t*>(base+0xa9c504+0x84)!=base+0x2670d0)return {};
  return {reinterpret_cast<decltype(RemoveUnitApi129::agent)>(base+0x707f0),
   reinterpret_cast<decltype(RemoveUnitApi129::prepare)>(base+0x2670d0),reinterpret_cast<decltype(RemoveUnitApi129::remove)>(base+0x29afe0)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool RemoveUnit129(uintptr_t base,const SkillSelection129& selected,const RemoveUnitApi129& api,const char** error=nullptr){
 if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity)return KillUnitFailure129(error,"Select a unit first");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)
   return KillUnitFailure129(error,"Selected unit changed or is unsupported");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))
   return KillUnitFailure129(error,"Selected unit is no longer live");
  api.prepare(selected.unit);
  // Preparation can run map callbacks. Resolve the identity again without reading the old unit.
  const auto live=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(live!=agent||!live||*reinterpret_cast<uint32_t*>(live+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(live+0x20))
   return KillUnitFailure129(error,"Unit was removed during preparation");
  // Native preparation can clear the portrait selection; retain the original identity.
  const uint64_t identity=(uint64_t(*reinterpret_cast<uint32_t*>(selected.unit+0xc))<<32)|
   *reinterpret_cast<uint32_t*>(selected.unit+0x10);
  if(identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)
   return KillUnitFailure129(error,"Unit identity changed during preparation");
  api.remove(selected.unit,1,1); // RemoveUnit A53E0; final access to the unit.
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return KillUnitFailure129(error,"Native RemoveUnit call failed");}
}
inline bool RemoveSelectedUnit129(uintptr_t base,const char** error=nullptr){
 const auto api=ResolveRemoveUnitApi129(base);
 if(!api)return KillUnitFailure129(error,"RemoveUnit function signature mismatch");
 return RemoveUnit129(base,SelectedSkill129(base),api,error);
}
}
