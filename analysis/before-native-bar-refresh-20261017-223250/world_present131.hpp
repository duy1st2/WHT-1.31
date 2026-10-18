#pragma once
#include "component_lookup131.hpp"
#include "overlay_frames.hpp"
namespace wht {
// Native overhead frames are pooled and detached between draw passes.
// Attached visibility is not feature policy: the native parent clips children.
inline bool OverheadEligible131(bool allowed,bool hideInvulnerable,unsigned unitFlags){return allowed&&!(hideInvulnerable&&(unitFlags&8));}
inline unsigned OverheadUnitFlags131(uintptr_t unit){__try{return *reinterpret_cast<unsigned*>(unit+(sizeof(void*)==8?0x38:0x20));}__except(1){return 0;}}
struct WorldPresentation131 {uintptr_t unit=0;bool mana=false,host=false;};
inline void PresentWorldChildren131(uintptr_t base,uintptr_t bar,uintptr_t barTable,uintptr_t unitTable,uintptr_t hostTable,uintptr_t textureTable,uintptr_t tint,const WorldPresentation131& policy,void* mana,void* host,uint32_t color){
 __try{
  if(!policy.unit||!ComponentIdentity131(bar,base+barTable)||!ComponentIdentity131(policy.unit,base+unitTable)||*reinterpret_cast<uintptr_t*>(bar+(sizeof(void*)==8?0x210:0x154))!=policy.unit)return;
  if(!*reinterpret_cast<unsigned char*>(bar+(sizeof(void*)==8?0xf1:0x9d)))return;
  const auto show=[&](void* child,uintptr_t table){
   const auto frame=reinterpret_cast<uintptr_t>(child);
   if(!ComponentIdentity131(frame,base+table)||ComponentLookup131::Slot(frame,sizeof(void*)==8?0xc0:0x78)!=bar)return false;
   // Native selection may detach children while preserving their requested
   // visibility. Reattach only children still bound to the same visible unit.
   const auto flags=reinterpret_cast<const unsigned char*>(frame+(sizeof(void*)==8?0xf0:0x9c));
   if(!flags[0]||!flags[1])NativeFrameShow(child);return true;
  };
  if(policy.host&&host)show(host,hostTable);
  if(policy.mana&&mana&&show(mana,barTable)){
   const auto texture=ComponentLookup131::Slot(reinterpret_cast<uintptr_t>(mana),sizeof(void*)==8?0x1e0:0x130);
   if(!ComponentIdentity131(texture,base+textureTable))return;
   const auto bytes=reinterpret_cast<const unsigned char*>(texture+(sizeof(void*)==8?0xb8:0x74));
   const uint32_t actual=(uint32_t(bytes[0])<<24)|(uint32_t(bytes[3])<<16)|(uint32_t(bytes[2])<<8)|bytes[1];
   if(actual!=color)reinterpret_cast<void(__thiscall*)(uintptr_t,const uint32_t*)>(base+tint)(texture,&color);
  }
 }__except(1){}
}
}
