#pragma once
#include "reselect129.hpp"
namespace wht {
struct DropItemsApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 uintptr_t(__thiscall* item)(uintptr_t,unsigned)=nullptr;
 unsigned(__thiscall* size)(uintptr_t)=nullptr;
 int(__thiscall* drop)(uintptr_t,uintptr_t)=nullptr;
 explicit operator bool()const{return agent&&item&&size&&drop;}
};
inline DropItemsApi129 ResolveDropItems129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char agent[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char item[]={0x55,0x8b,0xec,0x8b,0x89,0x04,0x04,0,0};
  const unsigned char size[]={0x8b,0x89,0x04,0x04,0,0};
  const unsigned char drop[]={0x55,0x8b,0xec,0x83,0xec,0x18,0x56,0x8b,0xb1,0x04,0x04,0,0};
  if(memcmp((void*)(base+0x707f0),agent,sizeof(agent))||memcmp((void*)(base+0x291820),item,sizeof(item))||memcmp((void*)(base+0x2960f0),size,sizeof(size))||memcmp((void*)(base+0x29b610),drop,sizeof(drop)))return {};
  // UnitRemoveItem must call this exact raw-unit drop entry.
  if(*reinterpret_cast<unsigned char*>(base+0xaf682)!=0xe8||base+0xaf687+*reinterpret_cast<int32_t*>(base+0xaf683)!=base+0x29b610)return {};
  return {reinterpret_cast<decltype(DropItemsApi129::agent)>(base+0x707f0),reinterpret_cast<decltype(DropItemsApi129::item)>(base+0x291820),reinterpret_cast<decltype(DropItemsApi129::size)>(base+0x2960f0),reinterpret_cast<decltype(DropItemsApi129::drop)>(base+0x29b610)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool LiveDropUnit129(uintptr_t base,const SkillSelection129& selected,const DropItemsApi129& api){
 if(!base||!api||!selected.unit||!selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return false;
 const auto current=SelectedSkill129(base);if(current.unit!=selected.unit||current.identity!=selected.identity)return false;
 const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
 return agent&&*reinterpret_cast<uint32_t*>(agent+0xc)==0x2b61676c&&!*reinterpret_cast<uint32_t*>(agent+0x20);
}
inline bool DropItemSlot129(uintptr_t base,const SkillSelection129& selected,const DropItemsApi129& api,unsigned slot,const char** error=nullptr){
 if(error)*error=nullptr;
 __try{
  if(!LiveDropUnit129(base,selected,api))return HeroSkillPointsFailure129(error,"Select a live unit first");
  const auto count=api.size(selected.unit);if(count>6||slot>=count)return HeroSkillPointsFailure129(error,"Inventory slot unavailable");
  const auto item=api.item(selected.unit,slot);if(!item)return HeroSkillPointsFailure129(error,"Inventory slot is empty");
  if(!api.drop(selected.unit,item))return HeroSkillPointsFailure129(error,"Game refused to drop the item");
  if(!LiveDropUnit129(base,selected,api))return HeroSkillPointsFailure129(error,"Selected unit changed during drop");
  for(unsigned i=0;i<count;++i)if(api.item(selected.unit,i)==item)return HeroSkillPointsFailure129(error,"Item remains in inventory");
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native item drop failed");}
}
inline bool DropItems129(uintptr_t base,const SkillSelection129& selected,const DropItemsApi129& api,unsigned& dropped,const char** error=nullptr){
 dropped=0;if(error)*error=nullptr;
 __try {
  if(!LiveDropUnit129(base,selected,api))return HeroSkillPointsFailure129(error,"Select a live unit first");
  const auto count=api.size(selected.unit);if(count>6)return HeroSkillPointsFailure129(error,"Invalid inventory size");
  uintptr_t items[6]{};for(unsigned slot=0;slot<count;++slot)items[slot]=api.item(selected.unit,slot);
  for(unsigned slot=0;slot<count;++slot){
   if(!items[slot])continue;
   if(!LiveDropUnit129(base,selected,api))return HeroSkillPointsFailure129(error,"Selected unit changed");
   if(api.item(selected.unit,slot)!=items[slot])return HeroSkillPointsFailure129(error,"Inventory changed");
   if(!api.drop(selected.unit,items[slot]))return HeroSkillPointsFailure129(error,"Game refused to drop an item");
   if(!LiveDropUnit129(base,selected,api))return HeroSkillPointsFailure129(error,"Selected unit changed during drop");
   for(unsigned remaining=0;remaining<count;++remaining)if(api.item(selected.unit,remaining)==items[slot])return HeroSkillPointsFailure129(error,"Item remains in inventory");
   ++dropped;
  }
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native item drop failed");}
}
}
