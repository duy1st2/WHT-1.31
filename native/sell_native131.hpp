#pragma once
#include "sell_all131.hpp"
#include "auto_pick_native131.hpp"
namespace wht {
struct SellProfile131 {unsigned build;uintptr_t unitList,listOffset,ability,purchaseTable,pawnCode;};
#ifdef _WIN64
inline constexpr SellProfile131 sellProfiles131[]={
 {12164,0xc6ecd0,0xb08,0xd0d6d0,0x19e17c0,0x9fdbd3},
 {12173,0xcb1c40,0xaf0,0xd4c170,0x1a107d8,0xa48e33}};
#else
inline constexpr SellProfile131 sellProfiles131[]={
 {12164,0xb20a10,0x6b8,0xbaf150,0x17337ec,0x8e7916},
 {12173,0xae0220,0x6b4,0xb6e5f0,0x16e8844,0x8b5e96}};
#endif
inline const SellProfile131* SellProfileFor131(unsigned build){for(const auto& p:sellProfiles131)if(p.build==build)return &p;return nullptr;}
struct SellApi131 {uintptr_t(__thiscall* list)(uintptr_t,bool)=nullptr;uintptr_t(__thiscall* ability)(uintptr_t,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned)=nullptr;explicit operator bool()const{return list&&ability;}};
inline SellApi131 SellNative131(uintptr_t base,const SellProfile131& p){
 __try{
#ifdef _WIN64
  const unsigned char list[]={0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20};
  const unsigned char ability[]={0x40,0x53,0x41,0x54,0x41,0x55,0x48,0x83,0xec,0x30};
  const unsigned char pawn[]={0x8b,0x40,0x38,0xc1,0xe8,0x0d,0x24,0x01};
#else
  const unsigned char list[]={0x55,0x8b,0xec,0x80,0x7d,0x08,0,0x56,0x57,0x8b,0xf9};
  const unsigned char ability[]={0x55,0x8b,0xec,0x56,0x8b,0x75,0x08,0x81,0xfe,0x75,0x62,0x4e};
  const unsigned char pawn[]={0x8b,0x40,0x20,0xc1,0xe8,0x0d,0x24,0x01};
#endif
  if(!base||memcmp(reinterpret_cast<void*>(base+p.unitList),list,sizeof(list))||memcmp(reinterpret_cast<void*>(base+p.ability),ability,sizeof(ability))||memcmp(reinterpret_cast<void*>(base+p.pawnCode),pawn,sizeof(pawn)))return {};
  return {reinterpret_cast<decltype(SellApi131::list)>(base+p.unitList),reinterpret_cast<decltype(SellApi131::ability)>(base+p.ability)};
 }__except(1){return {};}
}
inline bool Pawnable131(uintptr_t base,const InventoryProfile131& p,uintptr_t item){__try{return InventoryIdentity131(item,base,p.itemTable)&&DropIdentity131(item).Valid()&&(*reinterpret_cast<uint32_t*>(item+(sizeof(uintptr_t)==8?0x38:0x20))&0x2000)!=0;}__except(1){return false;}}
inline bool Merchant131(uintptr_t base,const SellProfile131& p,const SellApi131& api,uintptr_t unit){
 __try{auto ability=api.ability(unit,0x41706974,0,0,1,1,0);return InventoryIdentity131(ability,base,p.purchaseTable);}__except(1){return false;}
}
inline bool ShopAccess131(uintptr_t base,const ResourceProfile131& p,const DropNetworkApi131& api,uintptr_t world,unsigned local,uintptr_t shop){
 __try{const auto slot=api.owner(shop);if(slot>=28)return false;if(slot>=24||slot==local)return true;
  auto own=reinterpret_cast<decltype(ResourceNative131::player)>(base+p.playerFn)(world,local);if(!ResourceIdentity131(own,base,p.playerTable))return false;
  auto alliance=*reinterpret_cast<uintptr_t*>(own+p.allianceOffset);if(!ResourceIdentity131(alliance,base,p.allianceTable))return false;
  return (uint32_t(reinterpret_cast<decltype(ResourceNative131::integer)>(base+p.integerFn)(alliance+p.allianceMask))&(1u<<slot))!=0;
 }__except(1){return false;}
}
template<class Selected,class Alive> bool ReadSellView131(uintptr_t base,unsigned build,Selected selected,Alive alive,SellView131& out){
 const auto* p=SellProfileFor131(build);const auto* inventory=InventoryProfile(build);const auto* resource=ResourceProfileFor131(build);if(!p||!inventory||!resource||!SellNative131(base,*p)||!ResourceCode131(base,*resource))return false;
 if(!DropSnapshot131(base,build,selected,alive,out.owner))return false;
 __try{auto api=InventoryNative131(base,*inventory);out.pawnable={};for(unsigned i=0;i<6;++i)if(out.owner.items[i].handle)out.pawnable[i]=Pawnable131(base,*inventory,api.item(out.owner.unit,i));
  unsigned local=24;auto world=ResourceWorld131(base,*resource,local);if(world!=out.owner.root)return false;
  auto player=reinterpret_cast<decltype(ResourceNative131::player)>(base+resource->playerFn)(world,local);if(!ResourceIdentity131(player,base,resource->playerTable))return false;
  out.gold=reinterpret_cast<decltype(ResourceNative131::integer)>(base+resource->integerFn)(player+resource->stateBase+resource->stateStride)/10;return selected()==out.owner.unit;
 }__except(1){return false;}
}
template<class Alive> bool ScanSellShops131(uintptr_t base,unsigned build,uintptr_t worldFrame,unsigned& cursor,SellView131& view,Alive alive){
 auto p=SellProfileFor131(build);auto pick=PickProfileFor131(build);auto inventory=InventoryProfile(build);auto np=DropNetworkProfileFor131(build);auto resource=ResourceProfileFor131(build);if(!p||!pick||!inventory||!np||!resource||!InventoryIdentity131(worldFrame,base,pick->worldTable))return false;
 const auto api=SellNative131(base,*p);const auto network=DropNetworkNative131(base,*np,*inventory);if(!api||!network)return false;
 __try{unsigned local=24;auto world=ResourceWorld131(base,*resource,local);if(world!=view.owner.root)return false;
  const auto list=api.list(worldFrame,cursor==0);if(list!=worldFrame+p->listOffset)return false;
  const unsigned count=*reinterpret_cast<unsigned*>(list);const auto data=*reinterpret_cast<uintptr_t*>(list+sizeof(uintptr_t));if(count>8192||(count&&!data))return false;
  const unsigned end=(std::min)(count,cursor+128);view.shops.clear();
  for(;cursor<end;++cursor){auto unit=*reinterpret_cast<uintptr_t*>(data+cursor*sizeof(uintptr_t));if(!InventoryIdentity131(unit,base,inventory->unitTable)||!alive(unit)||!ShopAccess131(base,*resource,network,world,local,unit))continue;
   float xyz[3]{};network.position(unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);const float dx=xyz[0]-view.owner.x,dy=xyz[1]-view.owner.y;
   if(!std::isfinite(dx)||!std::isfinite(dy)||dx*dx+dy*dy>1200.f*1200.f||!Merchant131(base,*p,api,unit))continue;
   auto id=DropIdentity131(unit);if(id.Valid())view.shops.push_back({unit,id,xyz[0],xyz[1]});
  }
  view.scanDone=cursor>=count;return true;
 }__except(1){return false;}
}
template<class Selected,class Alive> bool SendSell131(uintptr_t base,unsigned build,const DropSnapshot& source,const DropItem& item,const SellShop131& shop,Selected selected,Alive alive){
 auto p=SellProfileFor131(build);auto inventory=InventoryProfile(build);auto np=DropNetworkProfileFor131(build);auto resource=ResourceProfileFor131(build);if(!p||!inventory||!np||!resource)return false;
 auto network=DropNetworkNative131(base,*np,*inventory);auto api=SellNative131(base,*p);auto inv=InventoryNative131(base,*inventory);if(!network||!api||!inv||selected()!=source.unit||!alive(source.unit)||DropSession131(network)!=source.session)return false;
 __try{unsigned local=24;auto world=ResourceWorld131(base,*resource,local);if(world!=source.root||!DropOwned131(base,build,network,source.unit)||!(DropIdentity131(source.unit)==source.id)||!InventoryIdentity131(shop.unit,base,inventory->unitTable)||!alive(shop.unit)||!(DropIdentity131(shop.unit)==shop.id)||!ShopAccess131(base,*resource,network,world,local,shop.unit)||!Merchant131(base,*p,api,shop.unit))return false;
  bool held=false;const auto count=inv.size(source.unit);if(count>6)return false;for(unsigned i=0;i<count;++i){auto current=inv.item(source.unit,i);if(Pawnable131(base,*inventory,current)&&DropIdentity131(current)==item.id)held=true;}if(!held)return false;
  float xyz[3]{};network.position(shop.unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);const float dx=xyz[0]-source.x,dy=xyz[1]-source.y;if(!std::isfinite(dx)||!std::isfinite(dy)||dx*dx+dy*dy>1200.f*1200.f)return false;
  auto target=source;target.id=shop.id;auto action=DropAction(target,item);action[1]=0;
  DropBuffer131 buffer{network.table,action.data(),0,0,39,39};network.send(&buffer,0);return true;
 }__except(1){return false;}
}
struct SellIcon131:DropIcon131 {SellIcon131(){area={.558f,.040f,.012f,.016f};upAsset="BTNSellItem.blp";downAsset="BTNSellItemDow.blp";}void Reset(){*this=SellIcon131{};}};
}
