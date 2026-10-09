#pragma once
#include "inventory_actions129.hpp"
namespace wht {
inline bool TargetCast129(uintptr_t base,bool inventory,int slot,const SkillSelection129* target=nullptr){
 if(!base||slot<0||slot>=(inventory?6:12))return false;
 __try{
  const auto api=ResolveInventoryActions129(base);const auto drop=ResolveDropItems129(base);const auto selected=SelectedSkill129(base);
  if(!api.handle||!api.order||!LiveDropUnit129(base,selected,drop))return false;
  auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(!world)return false;auto context=*reinterpret_cast<uintptr_t*>(world+0x1c);if(!context)return false;
  auto unit=api.handle(context,selected.unit,0);if(!unit||api.owner(unit)!=api.local())return false;
  if(target){
   if(!target->unit||!target->identity||*reinterpret_cast<uintptr_t*>(target->unit)!=base+0xa9c504)return false;
   auto identity=(uint64_t(*reinterpret_cast<uint32_t*>(target->unit+0xc))<<32)|*reinterpret_cast<uint32_t*>(target->unit+0x10);
   if(identity!=target->identity)return false;
   auto agent=drop.agent(uint32_t(identity>>32),uint32_t(identity));
   if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return false;
  }
  auto destination=target?api.handle(context,target->unit,0):unit;if(!destination)return false;
  const auto button=Button129(base,inventory,slot);if(!button)return false;
  if(inventory){
   const unsigned char sig[]={0x55,0x8b,0xec,0x56,0xff,0x75,0x08};if(memcmp((void*)(base+0xafa20),sig,sizeof(sig)))return false;
   auto raw=drop.item(selected.unit,unsigned(slot));if(!raw)return false;auto item=api.handle(context,raw,0);if(!item)return false;
   return reinterpret_cast<int(__cdecl*)(uint32_t,uint32_t,uint32_t)>(base+0xafa20)(unit,item,destination)!=0;
  }
  auto data=*reinterpret_cast<uintptr_t*>(button+0x194);if(!data)return false;auto order=*reinterpret_cast<uint32_t*>(data+8);if(!order)return false;
  return api.order(unit,order,destination)!=0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool SelfCast129(uintptr_t base,bool inventory,int slot){return TargetCast129(base,inventory,slot);}
}
