#pragma once
#include "quick_drop131.hpp"
#include "drop_all.hpp"
#include "resource131.hpp"
namespace wht {
// CDataStore::GetData borrows this buffer until Send returns. No destructor runs.
struct DropBuffer131 {uintptr_t table;const void* data;uint32_t position,reserved,size,capacity;};
static_assert(sizeof(DropBuffer131)==(sizeof(uintptr_t)==8?32:24),"native CDataStore ABI");
static_assert(offsetof(DropBuffer131,size)==(sizeof(uintptr_t)==8?0x18:0x10),"native data length");
struct DropNetworkProfile131 {unsigned build;uintptr_t context,send,table,reader,position,owner;};
#ifdef _WIN64
inline constexpr DropNetworkProfile131 dropNetworkProfiles131[]={
 {12164,0x314c60,0xbd0100,0x19d85d0,0x318070,0xf14e90,0xd006a0},
 {12173,0x382750,0xc16b80,0x1a075e0,0x385af0,0xf50d00,0xd3f2d0}};
#else
inline constexpr DropNetworkProfile131 dropNetworkProfiles131[]={
 {12164,0x2bf7d0,0xa99280,0x172f55c,0x2c20e0,0xd6b990,0xba33a0},
 {12173,0x2a9600,0xa582c0,0x16e45ac,0x2abf10,0xd263d0,0xb62a50}};
#endif
inline const DropNetworkProfile131* DropNetworkProfileFor131(unsigned build){for(const auto& p:dropNetworkProfiles131)if(p.build==build)return &p;return nullptr;}
struct DropNetworkApi131 {
 uintptr_t(__cdecl* context)(unsigned)=nullptr;
 void(__cdecl* send)(DropBuffer131*,unsigned)=nullptr;
 uintptr_t(__thiscall* position)(uintptr_t,float*)=nullptr;
 unsigned(__thiscall* owner)(uintptr_t)=nullptr;
 uintptr_t table=0;
 explicit operator bool()const{return context&&send&&position&&owner&&table;}
};
inline DropNetworkApi131 DropNetworkNative131(uintptr_t base,const DropNetworkProfile131& p,const InventoryProfile131& inventory){
 __try{
#ifdef _WIN64
  const unsigned char entry[]={0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xf1,0x48,0x63,0xfa,0xb9,0x0d,0,0,0,0xe8};
  const unsigned char owner[]={0x8b,0x81,0x98,0x01,0,0,0xc3};
  const unsigned char pos[]={0x40,0x53,0x48,0x83,0xec,0x20,0x33,0xc0,0x48,0x8b,0xda};
  constexpr unsigned call=0x1a,readerSlot=0x50,ownerSlot=0x1d8;
#else
  const unsigned char entry[]={0x55,0x8b,0xec,0x83,0xec,0x14,0x53,0x56,0x57,0x6a,0x0d,0xe8};
  const unsigned char owner[]={0x8b,0x81,0x64,0x01,0,0,0xc3};
  const unsigned char pos[]={0x55,0x8b,0xec,0x56,0x8b,0x75,0x08,0x56};
  constexpr unsigned call=0xb,readerSlot=0x28,ownerSlot=0xec;
#endif
  if(!base||memcmp(reinterpret_cast<void*>(base+p.send),entry,sizeof(entry))||memcmp(reinterpret_cast<void*>(base+p.owner),owner,sizeof(owner))||memcmp(reinterpret_cast<void*>(base+p.position),pos,sizeof(pos)))return {};
  if(base+p.send+call+5+*reinterpret_cast<int32_t*>(base+p.send+call+1)!=base+p.context)return {};
  if(*reinterpret_cast<uintptr_t*>(base+p.table+readerSlot)!=base+p.reader||*reinterpret_cast<uintptr_t*>(base+inventory.unitTable+ownerSlot)!=base+p.owner)return {};
  return {reinterpret_cast<decltype(DropNetworkApi131::context)>(base+p.context),reinterpret_cast<decltype(DropNetworkApi131::send)>(base+p.send),reinterpret_cast<decltype(DropNetworkApi131::position)>(base+p.position),reinterpret_cast<decltype(DropNetworkApi131::owner)>(base+p.owner),base+p.table};
 }__except(1){return {};}
}
inline uintptr_t DropSession131(const DropNetworkApi131& api){
 if(!api)return 0;
 __try{auto tls=api.context(13);if(!tls)return 0;
  auto global=*reinterpret_cast<uintptr_t*>(tls+(sizeof(uintptr_t)==8?0x28:0x14));if(!global)return 0;
  auto session=*reinterpret_cast<uintptr_t*>(global+(sizeof(uintptr_t)==8?0xb8:0x5c));if(!session)return 0;
  if(*reinterpret_cast<uint8_t*>(session+(sizeof(uintptr_t)==8?0x6281:0x5dd1))||*reinterpret_cast<uint32_t*>(session+(sizeof(uintptr_t)==8?0xbc0:0x888))||*reinterpret_cast<int*>(session+(sizeof(uintptr_t)==8?0x4ec:0x3b8))<3)return 0;
  return session;
 }__except(1){return 0;}
}
inline DropObjectId DropIdentity131(uintptr_t object){DropObjectId id;memcpy(&id,reinterpret_cast<void*>(object+(sizeof(uintptr_t)==8?0x18:0xc)),sizeof(id));return id;}
// Submit exactly one synchronized action; inventory changes later in simulation.
// Own/alive are rechecked after native reads, alongside selection, slot and session.
template<class Selected,class Alive,class Own> bool QuickDropNetwork131(uintptr_t base,const InventoryProfile131& p,const InventoryApi131& inventory,const DropNetworkApi131& api,unsigned slot,Selected selected,Alive alive,Own own){
 if(!inventory||!api||slot>=6)return false;
 __try{
  const auto session=DropSession131(api),unit=selected();
  if(!session||!InventoryIdentity131(unit,base,p.unitTable)||!alive(unit)||!own(unit))return false;
  if(!InventoryIdentity131(*reinterpret_cast<uintptr_t*>(unit+p.inventoryOffset),base,p.inventoryTable))return false;
  const auto count=inventory.size(unit);if(count>6||slot>=count)return false;
  const auto item=inventory.item(unit,slot);if(!InventoryIdentity131(item,base,p.itemTable))return false;
  DropSnapshot source;source.unit=unit;source.session=session;source.id=DropIdentity131(unit);
  DropItem target;target.id=DropIdentity131(item);if(!source.id.Valid()||!target.id.Valid())return false;
  float xyz[3]{};api.position(unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);source.x=xyz[0];source.y=xyz[1];
  if(!std::isfinite(source.x)||!std::isfinite(source.y)||selected()!=unit||inventory.item(unit,slot)!=item||DropSession131(api)!=session||!alive(unit)||!own(unit)||!(DropIdentity131(unit)==source.id)||!(DropIdentity131(item)==target.id))return false;
  const auto action=DropAction(source,target);
  DropBuffer131 buffer{api.table,action.data(),0,0,unsigned(action.size()),unsigned(action.size())};api.send(&buffer,0);return true;
 }__except(1){return false;}
}
inline bool DropOwned131(uintptr_t base,unsigned build,const DropNetworkApi131& api,uintptr_t unit){
 auto p=ResourceProfileFor131(build);if(!p||!api)return false;unsigned local=24;
 __try{return ResourceWorld131(base,*p,local)&&api.owner(unit)==local;}__except(1){return false;}
}
// Batch uses the same verified sender. Synthetic nonzero tokens are internal
// liveness markers only; only native object IDs are serialized in the packet.
template<class Selected,class Alive> bool DropSnapshot131(uintptr_t base,unsigned build,Selected selected,Alive alive,DropSnapshot& out,const char** diagnostic=nullptr){
 if(diagnostic)*diagnostic="profiles";
 auto p=InventoryProfile(build);auto np=DropNetworkProfileFor131(build);auto rp=ResourceProfileFor131(build);if(!p||!np||!rp)return false;
 const auto inventory=InventoryNative131(base,*p);const auto api=DropNetworkNative131(base,*np,*p);if(diagnostic)*diagnostic=!inventory?"inventory API":"network API";if(!inventory||!api)return false;
 __try{
  const auto unit=selected(),session=DropSession131(api);unsigned local=24;const auto world=ResourceWorld131(base,*rp,local);
  if(diagnostic)*diagnostic=!session?"session":!world?"world":!InventoryIdentity131(unit,base,p->unitTable)?"selection":!alive(unit)?"life":api.owner(unit)!=local?"owner":"inventory identity";
  if(!session||!world||!InventoryIdentity131(unit,base,p->unitTable)||!alive(unit)||api.owner(unit)!=local||!InventoryIdentity131(*reinterpret_cast<uintptr_t*>(unit+p->inventoryOffset),base,p->inventoryTable))return false;
  const auto count=inventory.size(unit);if(diagnostic)*diagnostic="capacity";if(count>6)return false;
  out={};out.root=world;out.session=session;out.unit=unit;out.handle=1;out.id=DropIdentity131(unit);if(diagnostic)*diagnostic="unit ID";if(!out.id.Valid())return false;
  if(diagnostic)*diagnostic="position";float xyz[3]{};api.position(unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);out.x=xyz[0];out.y=xyz[1];if(!std::isfinite(out.x)||!std::isfinite(out.y))return false;
  for(unsigned slot=0;slot<count;++slot){auto item=inventory.item(unit,slot);if(!item)continue;if(diagnostic)*diagnostic="item identity";if(!InventoryIdentity131(item,base,p->itemTable))return false;
   const auto id=DropIdentity131(item);if(diagnostic)*diagnostic="item ID";if(!id.Valid())return false;out.items[slot]={1,id};}
  if(diagnostic)*diagnostic="revalidation or empty inventory";return selected()==unit&&DropSession131(api)==session&&alive(unit)&&api.owner(unit)==local;
 }__except(1){return false;}
}
inline bool SendDropSnapshot131(uintptr_t base,unsigned build,const DropSnapshot& source,const DropItem& target){
 auto p=InventoryProfile(build);auto np=DropNetworkProfileFor131(build);if(!p||!np)return false;auto api=DropNetworkNative131(base,*np,*p);
 if(!api||DropSession131(api)!=source.session||!DropOwned131(base,build,api,source.unit))return false;
 __try{const auto action=DropAction(source,target);DropBuffer131 buffer{api.table,action.data(),0,0,39,39};api.send(&buffer,0);return true;}__except(1){return false;}
}

}
