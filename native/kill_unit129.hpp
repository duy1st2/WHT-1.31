#pragma once
#include "skills129.hpp"
#include <cstring>
namespace wht {
struct KillUnitApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 void(__thiscall* kill)(uintptr_t,const float*)=nullptr;
 explicit operator bool()const{return agent&&kill;}
};
inline bool KillUnitFailure129(const char** failure,const char* reason){if(failure)*failure=reason;return false;}
inline KillUnitApi129 ResolveKillUnitApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agentSig[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char killSig[]={0x55,0x8b,0xec,0x8b,0x55,0x08,0xb8};
  const unsigned char nativeSig[]={0x55,0x8b,0xec,0xff,0x75,0x08,0xe8};
  if(memcmp(reinterpret_cast<void*>(base+0x707f0),agentSig,sizeof(agentSig))||
     memcmp(reinterpret_cast<void*>(base+0x268cb0),killSig,sizeof(killSig))||
     *reinterpret_cast<uintptr_t*>(base+0x268cb7)!=base+0xd2d88c||
     memcmp(reinterpret_cast<void*>(base+0xa1ca0),nativeSig,sizeof(nativeSig))||
     *reinterpret_cast<uintptr_t*>(base+0xa9c504+0x124)!=base+0x268cb0)return {};
  return {reinterpret_cast<decltype(KillUnitApi129::agent)>(base+0x707f0),
          reinterpret_cast<decltype(KillUnitApi129::kill)>(base+0x268cb0)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool KillUnit129(uintptr_t base,const SkillSelection129& selected,const KillUnitApi129& api,const char** failure=nullptr){
 if(failure)*failure=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity)return KillUnitFailure129(failure,"Select a unit first");
  if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return KillUnitFailure129(failure,"Selected object is not a supported unit");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity)return KillUnitFailure129(failure,"Selected unit changed");
  const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))
   return KillUnitFailure129(failure,"Selected unit is no longer live");
  const auto zero=reinterpret_cast<const float*>(base+0xd2d88c);
  if(*zero!=0.f)return KillUnitFailure129(failure,"Native zero-life value changed");
  // KillUnit A1CA0 calls this exact CUnit virtual method with the engine's zero constant.
  api.kill(selected.unit,zero);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return KillUnitFailure129(failure,"Native KillUnit call failed");}
}
inline bool KillSelectedUnit129(uintptr_t base,const char** failure=nullptr){
 if(failure)*failure=nullptr;
 const auto selected=SelectedSkill129(base);
 if(!selected.unit||!selected.identity)return KillUnitFailure129(failure,"Select a unit first");
 const auto api=ResolveKillUnitApi129(base);
 if(!api)return KillUnitFailure129(failure,"KillUnit function signature mismatch");
 return KillUnit129(base,selected,api,failure);
}
}
