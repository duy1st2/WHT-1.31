#pragma once
#include "auto_pick131.hpp"
#include "goku_pick131.hpp"
#include "quick_drop_network131.hpp"
#include "drop_icon131.hpp"
namespace wht {
struct PickProfile131 {unsigned build;uintptr_t worldTable,list,listOffset,positionGetter,command,commandTable,commandRef;};
#ifdef _WIN64
inline constexpr PickProfile131 pickProfiles131[]={
 {12164,0x1c03a08,0xc6ec30,0xb28,0xd14260,0xdd8190,0x1bfa168,0xdd8260},
 {12173,0x1c328c0,0xcb1ba0,0xb10,0xd52cd0,0xe14280,0x1c28ff8,0xe142f8}};
#else
inline constexpr PickProfile131 pickProfiles131[]={
 {12164,0x1825828,0xb20960,0x6c8,0xbb4710,0xc5fce0,0x18206bc,0xc5fd85},
 {12173,0x17da708,0xae0170,0x6c4,0xb73c70,0xc1ae40,0x17d559c,0xc1aeb8}};
#endif
inline const PickProfile131* PickProfileFor131(unsigned build){for(const auto& p:pickProfiles131)if(p.build==build)return &p;return nullptr;}
struct PickApi131 {
 uintptr_t(__thiscall* list)(uintptr_t,bool)=nullptr;
 void(__cdecl* order)(unsigned,uintptr_t,const float*,const float*,uintptr_t,unsigned)=nullptr;
 explicit operator bool()const{return list&&order;}
};
inline PickApi131 PickNative131(uintptr_t base,const PickProfile131& p,const InventoryProfile131& inventory){
 __try{
#ifdef _WIN64
  const unsigned char list[]={0x40,0x57,0x48,0x83,0xec,0x30,0x48,0xc7,0x44};
  const unsigned char command[]={0x48,0x8b,0xc4,0x41,0x57,0x48,0x81,0xec,0x80};
  const unsigned char position[]={0x48,0x8d,0x81,0x28,0x02,0,0,0xc3};
  if(*reinterpret_cast<unsigned char*>(base+p.commandRef)!=0x48||base+p.commandRef+7+*reinterpret_cast<int32_t*>(base+p.commandRef+3)!=base+p.commandTable)return {};
  constexpr unsigned positionSlot=0x160;
#else
  const unsigned char list[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
  const unsigned char command[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
  const unsigned char position[]={0x8d,0x81,0xc0,0x01,0,0,0xc3};
  if(*reinterpret_cast<uintptr_t*>(base+p.commandRef)!=base+p.commandTable)return {};
  constexpr unsigned positionSlot=0xb0;
#endif
  if(!base||memcmp(reinterpret_cast<void*>(base+p.list),list,sizeof(list))||memcmp(reinterpret_cast<void*>(base+p.command),command,sizeof(command))||memcmp(reinterpret_cast<void*>(base+p.positionGetter),position,sizeof(position)))return {};
  if(*reinterpret_cast<uintptr_t*>(base+inventory.itemTable+positionSlot)!=base+p.positionGetter)return {};
  return {reinterpret_cast<decltype(PickApi131::list)>(base+p.list),reinterpret_cast<decltype(PickApi131::order)>(base+p.command)};
 }__except(1){return {};}
}
inline bool GroundPickItem131(uintptr_t base,const InventoryProfile131& p,uintptr_t item){
 if(!InventoryIdentity131(item,base,p.itemTable))return false;
 __try{
  if(*reinterpret_cast<uint32_t*>(item+(sizeof(uintptr_t)==8?0x38:0x20))&1)return false;
  const auto owner=item+(sizeof(uintptr_t)==8?0x8ac:0x5d8);
  return *reinterpret_cast<uint32_t*>(owner)==0xffffffffu&&*reinterpret_cast<uint32_t*>(owner+4)==0xffffffffu&&DropIdentity131(item).Valid();
 }__except(1){return false;}
}
template<class Selected,class Alive> bool ReadPickSnapshot131(uintptr_t base,unsigned build,uintptr_t worldFrame,Selected selected,Alive alive,PickSnapshot131& out,uint32_t requiredType=0,bool* acquired=nullptr){
 if(acquired)*acquired=false;
 auto p=PickProfileFor131(build);auto inventory=InventoryProfile(build);auto np=DropNetworkProfileFor131(build);if(!p||!inventory||!np||!InventoryIdentity131(worldFrame,base,p->worldTable))return false;
 auto api=PickNative131(base,*p,*inventory);auto network=DropNetworkNative131(base,*np,*inventory);if(!api||!network)return false;
 if(!DropSnapshot131(base,build,selected,alive,out.owner))return false;
 __try{
  out.capacity=InventoryNative131(base,*inventory).size(out.owner.unit);out.occupied=0;for(const auto& item:out.owner.items)if(item.handle)++out.occupied;
  if(!out.capacity||out.capacity>6||out.occupied>out.capacity)return false;
  out.ground.clear();
  if(requiredType){
   if(!ItemCodeGuard131(base,*inventory))return false;
   const bool held=InventoryHasType131(base,*inventory,InventoryNative131(base,*inventory),out.owner.unit,out.capacity,requiredType);
   if(acquired)*acquired=held;
   if(held)return selected()==out.owner.unit&&DropSession131(network)==out.owner.session&&alive(out.owner.unit)&&DropOwned131(base,build,network,out.owner.unit);
  }
  if(out.occupied==out.capacity)return true;
  const auto list=api.list(worldFrame,true);if(list!=worldFrame+p->listOffset)return false;
  const unsigned count=*reinterpret_cast<unsigned*>(list);const auto data=*reinterpret_cast<uintptr_t*>(list+sizeof(uintptr_t));if(count>8192||(count&&!data))return false;
  out.ground.reserve((std::min)(count,8192u));
  for(unsigned i=0;i<count;++i){auto item=*reinterpret_cast<uintptr_t*>(data+i*sizeof(uintptr_t));if(!GroundPickItem131(base,*inventory,item)||(requiredType&&ItemCodeValue131(base,*inventory,item)!=requiredType))continue;
   float xyz[3]{};network.position(item+(sizeof(uintptr_t)==8?0x228:0x1c0),xyz);
   if(!std::isfinite(xyz[0])||!std::isfinite(xyz[1]))continue;
   out.ground.push_back({DropIdentity131(item),xyz[0],xyz[1],item});
  }
  return selected()==out.owner.unit&&DropSession131(network)==out.owner.session&&alive(out.owner.unit)&&DropOwned131(base,build,network,out.owner.unit);
 }__except(1){return false;}
}
template<class Selected,class Alive> bool SendPick131(uintptr_t base,unsigned build,const DropSnapshot& source,const PickCandidate131& target,Selected selected,Alive alive){
 auto p=PickProfileFor131(build);auto inventory=InventoryProfile(build);auto np=DropNetworkProfileFor131(build);if(!p||!inventory||!np)return false;
 const auto api=PickNative131(base,*p,*inventory);const auto network=DropNetworkNative131(base,*np,*inventory);
 if(!api||!network||selected()!=source.unit||!alive(source.unit)||DropSession131(network)!=source.session||!DropOwned131(base,build,network,source.unit)||!GroundPickItem131(base,*inventory,target.object))return false;
 __try{if(!(DropIdentity131(source.unit)==source.id)||!(DropIdentity131(target.object)==target.id))return false;
  api.order(0xd0003,0,&target.x,&target.y,target.object,0);return true;
 }__except(1){return false;}
}
struct AutoPickIcon131:DropIcon131 {
 AutoPickIcon131(){area={.558f,.096f,.012f,.016f};upAsset="BTNAutoPick.blp";downAsset="BTNAutoPickDow.blp";}
 void Reset(){*this=AutoPickIcon131{};}
};
}
