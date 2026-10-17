#pragma once
#include "common.hpp"
#include <cmath>
namespace wht {
struct InventoryProfile131 {
 unsigned build;uintptr_t unitTable,inventoryTable,itemTable,inventoryOffset,itemFn,sizeFn,dropFn;
};
#ifdef _WIN64
inline constexpr InventoryProfile131 inventoryProfiles131[]={
 {12164,0x1c16140,0x1a4fb18,0x1c18060,0x550,0xd12090,0xd120d0,0xd122a0},
 {12173,0x1c45000,0x1a7eb28,0x1c46f20,0x550,0xd50b50,0xd50b90,0xd50d60}
};
#else
inline constexpr InventoryProfile131 inventoryProfiles131[]={
 {12164,0x182eeb0,0x1765fb4,0x182f8f4,0x44c,0xbb27f0,0xbb2830,0xbb29a0},
 {12173,0x17e3d90,0x171afe4,0x17e47d4,0x444,0xb71db0,0xb71df0,0xb71f60}
};
#endif
inline const InventoryProfile131* InventoryProfile(unsigned build){for(const auto& p:inventoryProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool InventoryIdentity131(uintptr_t object,uintptr_t base,uintptr_t table){__try{return object&&*reinterpret_cast<uintptr_t*>(object)==base+table;}__except(1){return false;}}
struct InventoryApi131 {
 uintptr_t(__thiscall* item)(uintptr_t,unsigned)=nullptr;
 unsigned(__thiscall* size)(uintptr_t)=nullptr;
 bool(__thiscall* drop)(uintptr_t,uintptr_t)=nullptr;
 explicit operator bool()const{return item&&size&&drop;}
};
inline InventoryApi131 InventoryNative131(uintptr_t base,const InventoryProfile131& p){
 if(!base)return {};
 __try {
#ifdef _WIN64
  const unsigned char getter[]={0x48,0x8b,0x89},drop[]={0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x30,0x48,0x8b,0xb9};
  if(memcmp(reinterpret_cast<void*>(base+p.itemFn),getter,sizeof(getter))||memcmp(reinterpret_cast<void*>(base+p.sizeFn),getter,sizeof(getter))||memcmp(reinterpret_cast<void*>(base+p.dropFn),drop,sizeof(drop)))return {};
  if(*reinterpret_cast<unsigned*>(base+p.itemFn+3)!=p.inventoryOffset||*reinterpret_cast<unsigned*>(base+p.sizeFn+3)!=p.inventoryOffset||*reinterpret_cast<unsigned*>(base+p.dropFn+13)!=p.inventoryOffset)return {};
#else
  const unsigned char getter[]={0x55,0x8b,0xec,0x8b,0x89},size[]={0x8b,0x89},drop[]={0x55,0x8b,0xec,0x83,0xec,0x0c,0x56,0x8b,0xb1};
  if(memcmp(reinterpret_cast<void*>(base+p.itemFn),getter,sizeof(getter))||memcmp(reinterpret_cast<void*>(base+p.sizeFn),size,sizeof(size))||memcmp(reinterpret_cast<void*>(base+p.dropFn),drop,sizeof(drop)))return {};
  if(*reinterpret_cast<unsigned*>(base+p.itemFn+5)!=p.inventoryOffset||*reinterpret_cast<unsigned*>(base+p.sizeFn+2)!=p.inventoryOffset||*reinterpret_cast<unsigned*>(base+p.dropFn+9)!=p.inventoryOffset)return {};
#endif
  return {reinterpret_cast<decltype(InventoryApi131::item)>(base+p.itemFn),reinterpret_cast<decltype(InventoryApi131::size)>(base+p.sizeFn),reinterpret_cast<decltype(InventoryApi131::drop)>(base+p.dropFn)};
 }__except(1){return {};}
}
// Native inventory tooltip only offers Use/shortcut when CItem flags bit9
// is set. This capability is independent of cooldown and current mana.
inline bool InventoryUseFlagCode131(uintptr_t base,unsigned build){
 if(!base||(build!=12164&&build!=12173))return false;
 __try{
#ifdef _WIN64
  const uintptr_t rva=build==12164?0xcb51ad:0xcf52dd;
  const unsigned char code[]={0x41,0x8b,0x4d,0x38,0xc1,0xe9,9,0xf6,0xc1,1};
#else
  const uintptr_t rva=build==12164?0xb6055e:0xb2029e;
  const unsigned char code[]={0x8b,0x46,0x20,0xc1,0xe8,9,0xa8,1};
#endif
  return !memcmp(reinterpret_cast<void*>(base+rva),code,sizeof(code));
 }__except(1){return false;}
}
inline bool InventoryItemUsable131(uintptr_t base,const InventoryProfile131& p,uintptr_t item){
 __try{return InventoryIdentity131(item,base,p.itemTable)&&(*reinterpret_cast<unsigned*>(item+(sizeof(uintptr_t)==8?0x38:0x20))&0x200)!=0;}__except(1){return false;}
}
// Inventory button data remains populated even when the slot is empty.
// Read the selected unit's actual inventory through the verified native API.
inline unsigned InventoryMask131(uintptr_t base,const InventoryProfile131& p,const InventoryApi131& api,uintptr_t unit,bool usableOnly=false){
 if(!api||!InventoryIdentity131(unit,base,p.unitTable)||(usableOnly&&!InventoryUseFlagCode131(base,p.build)))return 0;
 __try{
  if(!InventoryIdentity131(*reinterpret_cast<uintptr_t*>(unit+p.inventoryOffset),base,p.inventoryTable))return 0;
  const auto count=api.size(unit);if(count>6)return 0;
  unsigned mask=0;for(unsigned i=0;i<count;++i){const auto item=api.item(unit,i);if(InventoryIdentity131(item,base,p.itemTable)&&(!usableOnly||InventoryItemUsable131(base,p,item)))mask|=1u<<i;}
  return mask;
 }__except(1){return 0;}
}
// Raw-unit native drop preserves the 1.29 local-mode behavior: remove the
// item through CAbilityInventory, then position it at the selected unit.
template<class Selected,class Alive> bool DropSlot131(uintptr_t base,const InventoryProfile131& p,const InventoryApi131& api,unsigned slot,Selected selected,Alive alive){
 if(!api||slot>=6)return false;
 __try {
  const auto unit=selected();if(!InventoryIdentity131(unit,base,p.unitTable)||!alive(unit))return false;
  if(!InventoryIdentity131(*reinterpret_cast<uintptr_t*>(unit+p.inventoryOffset),base,p.inventoryTable))return false;
  const auto count=api.size(unit);if(count>6||slot>=count)return false;
  const auto item=api.item(unit,slot);if(!InventoryIdentity131(item,base,p.itemTable)||selected()!=unit)return false;
  if(!api.drop(unit,item)||selected()!=unit||!InventoryIdentity131(unit,base,p.unitTable))return false;
  for(unsigned i=0;i<count;++i)if(api.item(unit,i)==item)return false;
  return true;
 }__except(1){return false;}
}
struct DropAllResult131 {unsigned dropped=0;bool complete=false;};
template<class Selected,class Alive> DropAllResult131 DropAll131(uintptr_t base,const InventoryProfile131& p,const InventoryApi131& api,Selected selected,Alive alive){
 DropAllResult131 result;if(!api)return result;
 __try {
  const auto unit=selected();if(!InventoryIdentity131(unit,base,p.unitTable)||!alive(unit))return result;
  if(!InventoryIdentity131(*reinterpret_cast<uintptr_t*>(unit+p.inventoryOffset),base,p.inventoryTable))return result;
  const auto count=api.size(unit);if(count>6)return result;
  uintptr_t items[6]{};for(unsigned i=0;i<count;++i)items[i]=api.item(unit,i);
  for(unsigned i=0;i<count;++i){if(!items[i])continue;
   if(selected()!=unit||api.item(unit,i)!=items[i])return result;
   if(!DropSlot131(base,p,api,i,[&](){return selected()==unit?unit:uintptr_t(0);},alive))return result;
   ++result.dropped;
  }
  result.complete=true;return result;
 }__except(1){return result;}
}
inline bool InventoryHit131(const float* bounds,float x,float y){
 const float bottom=bounds[0],left=bounds[1],top=bounds[2],right=bounds[3];
 return std::isfinite(bottom)&&std::isfinite(left)&&std::isfinite(top)&&std::isfinite(right)&&std::isfinite(x)&&std::isfinite(y)&&
  right-left>=.015f&&right-left<=.08f&&top-bottom>=.015f&&top-bottom<=.08f&&x>=left&&x<right&&y>=bottom&&y<top;
}
template<class Button> int InventorySlotHit131(POINT point,int width,int height,Button get){
 if(width<=0||height<=0||point.x<0||point.y<0||point.x>=width||point.y>=height)return -1;
 const int classic=(std::min)(width,height*4/3);if(!classic)return -1;
 const float x=.4f+(float(point.x)-float(width)*.5f)*.8f/classic,y=.6f-float(point.y)*.6f/height;
 __try {for(unsigned slot=0;slot<6;++slot){const auto button=get(slot);if(!button)continue;
  if(!*reinterpret_cast<unsigned char*>(button+(sizeof(uintptr_t)==8?0xf1:0x9d)))continue;
  if(InventoryHit131(reinterpret_cast<const float*>(button+(sizeof(uintptr_t)==8?0x88:0x44)),x,y))return int(slot);
 }}__except(1){}return -1;
}
struct QuickDropInput131 {
 bool consumed=false;
 template<class Hit,class Drop> bool Mouse(unsigned message,bool enabled,Hit hit,Drop drop){
  if(message==WM_RBUTTONUP){const bool result=consumed;consumed=false;return result;}
  if(message!=WM_RBUTTONDOWN&&message!=WM_RBUTTONDBLCLK)return false;
  consumed=false;if(!enabled)return false;const int slot=hit();if(slot<0||slot>=6)return false;
  consumed=true;drop(unsigned(slot));return true;
 }
};
}
