#pragma once
#include "mth_route131.hpp"
#include "sell_native131.hpp"
#include "detail_tooltip_profile131.hpp"
namespace wht {
struct CourierSelectProfile131 {unsigned build;uintptr_t table,send,removeCall,addCall,copy;};
#ifdef _WIN64
inline constexpr CourierSelectProfile131 courierSelectProfiles131[]={{12164,0x1bc68b8,0xdd7750,0xa44755,0xa44765,0xaf3b70},{12173,0x1bf5898,0xe13990,0xa8f86e,0xa8f87e,0xb3c4d0}};
#else
inline constexpr CourierSelectProfile131 courierSelectProfiles131[]={{12164,0x180750c,0xc5f230,0x92c287,0x92c29b,0x9d0cb0},{12173,0x17bc53c,0xc1a3d0,0x8f9548,0x8f955c,0x991a10}};
#endif
struct NativeUnitList131 {unsigned count;const uintptr_t* units;};
using NativeSelect131=void(__cdecl*)(const NativeUnitList131*,unsigned);
inline NativeSelect131 CourierSelectNative131(uintptr_t base,unsigned build){
 const CourierSelectProfile131* p=nullptr;for(const auto& v:courierSelectProfiles131)if(v.build==build)p=&v;if(!base||!p)return nullptr;
 __try{
#ifdef _WIN64
  const unsigned char first[]={0x4c,0x8b,0xdc,0x57,0x41,0x56,0x41,0x57,0x48,0x81,0xec,0x80,0,0,0};
  const unsigned char second[]={0x48,0x8b,0xc4,0x41,0x56,0x48,0x81,0xec,0x80,0,0,0};
  if(p->build==12164?memcmp((void*)(base+p->send),first,sizeof(first)):memcmp((void*)(base+p->send),second,sizeof(second)))return nullptr;
  const unsigned char count[]={0x8b,0x57,0x20},root[]={0x48,0x8b,0x4f,0x18};
  if(memcmp((void*)(base+p->copy+0x59),count,3)||memcmp((void*)(base+p->copy+0x6b),root,4))return nullptr;
#else
  const unsigned char prefix[]={0x55,0x8b,0xec,0x6a,0xff};if(memcmp((void*)(base+p->send),prefix,5))return nullptr;
  const unsigned char count[]={0x8b,0x46,0x10},root[]={0x8b,0x4e,0x0c};
  if(memcmp((void*)(base+p->copy+0x67),count,3)||memcmp((void*)(base+p->copy+0x78),root,3))return nullptr;
#endif
  for(auto rva:{p->removeCall,p->addCall}){const auto at=base+rva;if(*reinterpret_cast<unsigned char*>(at)!=0xe8||at+5+*reinterpret_cast<int32_t*>(at+1)!=base+p->send)return nullptr;}
#ifdef _WIN64
  bool remove=false,add=false;for(unsigned i=2;i<=10;++i){const auto r=base+p->removeCall-i,a=base+p->addCall-i;remove|=*reinterpret_cast<uint16_t*>(r)==0x02b2;add|=*reinterpret_cast<uint16_t*>(a)==0x01b2;}if(!remove||!add)return nullptr;
#else
  if(*reinterpret_cast<uint16_t*>(base+p->removeCall-3)!=0x026a||*reinterpret_cast<uint16_t*>(base+p->addCall-3)!=0x016a)return nullptr;
#endif
  return reinterpret_cast<NativeSelect131>(base+p->send);
 }__except(1){return nullptr;}
}
struct SelectionRoster131 {std::array<uintptr_t,12> units{};std::array<DropObjectId,12> ids{};unsigned count=0;};
inline bool ReadSelectionSet131(uintptr_t set,uintptr_t base,const InventoryProfile131& p,SelectionRoster131& out){
 out={};__try{const unsigned n=*reinterpret_cast<unsigned*>(set+4*sizeof(uintptr_t));if(n>12)return false;auto node=*reinterpret_cast<uintptr_t*>(set+3*sizeof(uintptr_t));
  for(unsigned i=0;i<n;++i){if(!node||(node&1))return false;const auto next=*reinterpret_cast<uintptr_t*>(node+sizeof(uintptr_t)),unit=*reinterpret_cast<uintptr_t*>(node+2*sizeof(uintptr_t));if(!InventoryIdentity131(unit,base,p.unitTable))return false;const auto id=DropIdentity131(unit);if(!id.Valid())return false;
   for(unsigned k=0;k<i;++k)if(out.units[k]==unit)return false;out.units[i]=unit;out.ids[i]=id;++out.count;node=next;}
  return !node||(node&1);
 }__except(1){return false;}
}
inline bool ReadSelectionRoster131(uintptr_t base,unsigned build,SelectionRoster131& out){
 const auto* inv=InventoryProfile(build);const auto* rp=ResourceProfileFor131(build);const CourierSelectProfile131* sp=nullptr;for(const auto& v:courierSelectProfiles131)if(v.build==build)sp=&v;
 if(!inv||!rp||!sp||!ResourceCode131(base,*rp)||!CourierSelectNative131(base,build))return false;
 __try{unsigned local=24;auto world=ResourceWorld131(base,*rp,local);if(!world)return false;auto api=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,unsigned)>(base+rp->playerFn);auto player=api(world,local);if(!InventoryIdentity131(player,base,rp->playerTable))return false;
  thread_local ComponentLookup131 selectionLookup;const auto selection=selectionLookup.Find(player,base,sp->table,static_cast<unsigned>(rp->stateBase),GetTickCount64());
  if(!selection)return false;SelectionRoster131 peer;const unsigned first=sizeof(uintptr_t)==8?0x388:0x1c4,second=sizeof(uintptr_t)==8?0x3d0:0x1e8;
  if(!ReadSelectionSet131(selection+first,base,*inv,out)||!ReadSelectionSet131(selection+second,base,*inv,peer)||out.count!=peer.count)return false;
  for(unsigned i=0;i<out.count;++i){bool found=false;for(unsigned j=0;j<peer.count;++j)if(out.units[i]==peer.units[j]&&out.ids[i]==peer.ids[j])found=true;if(!found)return false;}return true;
 }__except(1){return false;}
}
template<class Select,class Point> bool CourierPointFlow131(const SelectionRoster131& original,uintptr_t courier,Select select,Point point){
 if(!courier||original.count>12)return false;
 if(original.count==1&&original.units[0]==courier){point();return true;}
 const NativeUnitList131 saved{original.count,original.units.data()},one{1,&courier};
 if(saved.count)select(&saved,2);select(&one,1);point();select(&one,2);if(saved.count)select(&saved,1);return true;
}
inline bool SendCourierPointNative131(const SelectionRoster131& original,uintptr_t courier,NativeSelect131 select,PointOrder131 point,float x,float y){
 if(!courier||!select||!point||original.count>12||!std::isfinite(x)||!std::isfinite(y))return false;
 const NativeUnitList131 saved{original.count,original.units.data()},one{1,&courier};bool changed=false,ok=false;
 __try{
  __try{if(original.count==1&&original.units[0]==courier){point(0xd0012,0,&x,&y,0);ok=true;}
   else {changed=true;if(saved.count)select(&saved,2);select(&one,1);point(0xd0012,0,&x,&y,0);ok=true;}}
  __finally{if(changed){select(&one,2);if(saved.count)select(&saved,1);}}
 }__except(1){return false;}return ok;
}
inline bool RouteMovable131(uintptr_t base,unsigned build,uintptr_t unit){
 const DetailProfile131* p=nullptr;for(const auto& v:detailProfiles131)if(v.build==build)p=&v;const auto* inv=InventoryProfile(build);if(!p||!inv||!InventoryIdentity131(unit,base,inv->unitTable))return false;
 const auto move=ComponentLookup131::Slot(unit,sizeof(void*)==8?0x560:build==12164?0x454:0x44c);if(!ComponentIdentity131(move,base+p->move)||ComponentLookup131::Slot(move,sizeof(void*)==8?0x50:0x30)!=unit||!DetailSignatureMatch131(base+p->moveGet,p->moveSignature))return false;
 __try{float speed=NAN;const auto returned=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,float*)>(base+p->moveGet)(move,&speed);return returned==reinterpret_cast<uintptr_t>(&speed)&&std::isfinite(speed)&&speed>0&&speed<1e6f&&ComponentIdentity131(move,base+p->move)&&ComponentLookup131::Slot(move,sizeof(void*)==8?0x50:0x30)==unit;}__except(1){return false;}
}
}
