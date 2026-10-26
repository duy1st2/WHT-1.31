#pragma once
#include "mth_craft131.hpp"
#include "courier_base131.hpp"
namespace wht {
struct DeliveryBag131 {SelfCastSource131 source;unsigned capacity=0;std::array<DropObjectId,6> items{};};
inline bool DeliveryHas131(const DeliveryBag131& bag,const DropObjectId& id){if(!id.Valid())return false;for(unsigned i=0;i<bag.capacity&&i<6;++i)if(bag.items[i]==id)return true;return false;}
inline unsigned DeliveryCount131(const DeliveryBag131& bag){unsigned n=0;for(unsigned i=0;i<bag.capacity&&i<6;++i)n+=bag.items[i].Valid();return n;}
inline bool DeliveryCommand131(const std::wstring& name){return name==L"#ga"||name==L"#soi"||name==L"#courier"||name==L"#chuyendo";}
enum class DeliveryResult131 {Idle,Working,Sent,Complete,Full,Cancelled,Unavailable,LostItem,Refused,Timeout};
inline const char* DeliveryNotice131(DeliveryResult131 r){switch(r){
 case DeliveryResult131::Complete:return "Courier Delivery: completed; returning to base";
 case DeliveryResult131::Full:return "Courier Delivery: hero inventory full; remaining items kept; returning";
 case DeliveryResult131::Cancelled:return "Courier Delivery: cancelled";
 case DeliveryResult131::Unavailable:return "Courier Delivery: unit or inventory unavailable; stopped";
 case DeliveryResult131::LostItem:return "Courier Delivery: item receipt could not be confirmed; stopped";
 case DeliveryResult131::Refused:return "Courier Delivery: native action refused; no retry";
 case DeliveryResult131::Timeout:return "Courier Delivery: timed out; stopped";
 default:return nullptr;}}
struct CourierDelivery131 {
 bool active=false;SelfCastSource131 courier,hero;DropObjectId pending;unsigned delivered=0;uint64_t due=0,deadline=0;
 void Reset(){*this={};}
 bool Start(const DeliveryBag131& c,const DeliveryBag131& h,uint64_t now){
  if(active||!c.capacity||c.capacity>6||!h.capacity||h.capacity>6||!DeliveryCount131(c)||!c.source.unit||!h.source.unit||c.source.unit==h.source.unit||!c.source.ui||c.source.ui!=h.source.ui||!c.source.session||c.source.session!=h.source.session||!c.source.id.Valid()||!h.source.id.Valid())return false;
  Reset();courier=c.source;hero=h.source;active=true;deadline=now+60000;return true;
 }
 template<class Read,class Move,class Transfer,class Return> DeliveryResult131 Tick(uint64_t now,bool allowed,bool ready,Read read,Move move,Transfer transfer,Return back){
  if(!active)return DeliveryResult131::Idle;
  if(!allowed){active=false;return DeliveryResult131::Cancelled;}
  if(!ready)return DeliveryResult131::Working;
  if(now>=deadline){active=false;return DeliveryResult131::Timeout;}if(now<due)return DeliveryResult131::Working;due=now+100;
  DeliveryBag131 c,h;if(!read(courier,false,c)||!read(hero,true,h)||!SameSelfCastSource131(courier,c.source)||!SameSelfCastSource131(hero,h.source)||!c.capacity||c.capacity>6||!h.capacity||h.capacity>6){active=false;return DeliveryResult131::Unavailable;}
  if(pending.Valid()){
   const bool held=DeliveryHas131(c,pending),received=DeliveryHas131(h,pending);
   if(held)return DeliveryResult131::Working;
   if(!received){active=false;return DeliveryResult131::LostItem;}
   ++delivered;pending={};
  }
  const auto count=DeliveryCount131(c);if(!count||DeliveryCount131(h)>=h.capacity){active=false;back(c.source);return count?DeliveryResult131::Full:DeliveryResult131::Complete;}
  const double dx=double(c.source.x)-h.source.x,dy=double(c.source.y)-h.source.y;
  if(!std::isfinite(dx)||!std::isfinite(dy)){active=false;return DeliveryResult131::Unavailable;}
  if(dx*dx+dy*dy>300.*300.){due=now+1000;if(!move(c.source,h.source)){active=false;return DeliveryResult131::Refused;}return DeliveryResult131::Working;}
  for(unsigned i=0;i<c.capacity;++i)if(c.items[i].Valid()){
   // Record before submit: an uncertain native result must never be replayed.
   pending=c.items[i];if(!transfer(c.source,h.source,pending)){active=false;return DeliveryResult131::Refused;}return DeliveryResult131::Sent;
  }
  active=false;return DeliveryResult131::Unavailable;
 }
};
inline bool DeliveryRead131(MthCraftApi131& api,const SelfCastSource131& expected,bool hero,DeliveryBag131& out){
 SelfCastSource131 current;if(!api.Read(expected.unit,current)||!SameSelfCastSource131(expected,current)||!api.Owned(expected.unit)||(hero&&!api.Hero(expected.unit)))return false;
 auto p=InventoryProfile(api.build);if(!p)return false;auto inv=InventoryNative131(api.base,*p);if(!inv)return false;
 __try{if(!InventoryIdentity131(*reinterpret_cast<uintptr_t*>(expected.unit+p->inventoryOffset),api.base,p->inventoryTable))return false;
  out={};out.source=current;out.capacity=inv.size(expected.unit);if(!out.capacity||out.capacity>6)return false;
  for(unsigned i=0;i<out.capacity;++i){auto item=inv.item(expected.unit,i);if(!item)continue;if(!InventoryIdentity131(item,api.base,p->itemTable))return false;out.items[i]=DropIdentity131(item);if(!out.items[i].Valid())return false;}
  SelfCastSource131 after;return api.Read(expected.unit,after)&&SameSelfCastSource131(current,after)&&api.Owned(expected.unit);
 }__except(1){return false;}
}
inline bool DeliveryFindHero131(MthCraftApi131& api,const SelfCastSource131& courier,DeliveryBag131& out){
 SelfCastSource131 source;auto selected=api.Selected();if(selected!=courier.unit&&api.Read(selected,source)&&DeliveryRead131(api,source,true,out)&&out.source.session==courier.session)return true;
 auto p=SellProfileFor131(api.build);auto pp=PickProfileFor131(api.build);if(!p||!pp||!InventoryIdentity131(api.worldFrame,api.base,pp->worldTable))return false;auto native=SellNative131(api.base,*p);if(!native)return false;
 __try{auto list=native.list(api.worldFrame,true);if(list!=api.worldFrame+p->listOffset)return false;const auto count=*reinterpret_cast<unsigned*>(list);const auto units=*reinterpret_cast<uintptr_t*>(list+sizeof(uintptr_t));if(count>8192||(count&&!units))return false;
  double best=INFINITY;uint64_t bestId=UINT64_MAX;bool found=false;
  for(unsigned i=0;i<count;++i){const auto unit=*reinterpret_cast<uintptr_t*>(units+i*sizeof(uintptr_t));if(unit==courier.unit||!api.Hero(unit))continue;DeliveryBag131 candidate;if(!api.Read(unit,source)||source.session!=courier.session||!DeliveryRead131(api,source,true,candidate))continue;
   const double dx=double(candidate.source.x)-courier.x,dy=double(candidate.source.y)-courier.y,distance=dx*dx+dy*dy;const uint64_t id=(uint64_t(source.id.low)<<32)|source.id.high;
   if(std::isfinite(distance)&&(distance<best||(distance==best&&id<bestId))){best=distance;bestId=id;out=candidate;found=true;}}
  return found;
 }__except(1){return false;}
}
// Same verified action 0x13 / D0021 and native sender used by Sell131,
// with an owned Hero as the recipient. Selection is restored even on SEH.
inline bool DeliverySend131(MthCraftApi131& api,const SelfCastSource131& courier,const SelfCastSource131& hero,const DropObjectId& item){
 DeliveryBag131 c,h;if(!DeliveryRead131(api,courier,false,c)||!DeliveryRead131(api,hero,true,h)||!DeliveryHas131(c,item)||DeliveryCount131(h)>=h.capacity||c.source.session!=h.source.session)return false;
 const double dx=double(c.source.x)-h.source.x,dy=double(c.source.y)-h.source.y;if(dx*dx+dy*dy>300.*300.)return false;
 auto p=InventoryProfile(api.build);auto np=DropNetworkProfileFor131(api.build);auto select=CourierSelectNative131(api.base,api.build);SelectionRoster131 roster;if(!p||!np||!select||!ReadSelectionRoster131(api.base,api.build,roster))return false;
 auto net=DropNetworkNative131(api.base,*np,*p);if(!net||DropSession131(net)!=courier.session)return false;
 DropSnapshot target;target.id=hero.id;target.x=h.source.x;target.y=h.source.y;auto action=DropAction(target,{1,item});action[1]=0;
 DropBuffer131 buffer{net.table,action.data(),0,0,unsigned(action.size()),unsigned(action.size())};const NativeUnitList131 saved{roster.count,roster.units.data()},one{1,&courier.unit};bool changed=false;
 __try{__try{if(roster.count!=1||roster.units[0]!=courier.unit){changed=true;if(saved.count)select(&saved,2);select(&one,1);}net.send(&buffer,0);}
  __finally{if(changed){select(&one,2);if(saved.count)select(&saved,1);}}
  return true;
 }__except(1){return false;}
}
}
