#pragma once
#include "drop_all.hpp"
#include <vector>
#include <algorithm>
namespace wht {
struct SellShop131 {uintptr_t unit=0;DropObjectId id;float x=0,y=0;};
struct SellView131 {DropSnapshot owner;std::array<bool,6> pawnable{};std::vector<SellShop131> shops;bool scanDone=false;int gold=0;};
enum class SellResult131 {Idle,Working,Submitted,Complete,NoShop,Remaining,Cancelled,Empty};
struct SellAll131 {
 bool active=false,scanning=false,waiting=false,confirming=false;DropSnapshot owner;std::array<DropItem,6> original{};std::array<bool,6> ordered{};
 std::vector<SellShop131> shops;unsigned scanCursor=0,slot=0,shop=0,passes=0,submitted=0,sold=0,remaining=0,skipped=0;int initialGold=0,goldDelta=0;uint64_t next=0,deadline=0;
 void Reset(){*this={};}
 bool Start(const SellView131& view,uint64_t now,int selectedSlot=-1){
  const auto& source=view.owner;if(selectedSlot < -1||selectedSlot>=6||active||!source.root||!source.session||!source.unit||!source.id.Valid()||!std::isfinite(source.x)||!std::isfinite(source.y))return false;
  Reset();owner=source;initialGold=view.gold;
  for(unsigned i=0;i<6;++i)if((selectedSlot<0||i==unsigned(selectedSlot))&&source.items[i].handle&&source.items[i].id.Valid()){if(view.pawnable[i]){original[i]=source.items[i];++remaining;}else ++skipped;}
  if(!remaining)return false;active=scanning=true;deadline=now+16000;next=now;return true;
 }
 template<class Send> SellResult131 Step(const SellView131* view,uint64_t now,Send send){
  if(!active)return SellResult131::Idle;
  if(!view||view->owner.root!=owner.root||view->owner.session!=owner.session||view->owner.unit!=owner.unit||!(view->owner.id==owner.id)){active=false;return SellResult131::Cancelled;}
  goldDelta=view->gold-initialGold;remaining=0;
  for(unsigned i=0;i<6;++i){if(!original[i].handle)continue;bool held=false;for(const auto& item:view->owner.items)if(item.handle&&item.id==original[i].id)held=true;
   if(!held){if(ordered[i])++sold;original[i]={};}else ++remaining;}
  // Match WHT SellAll: confirmed inventory removal overrides the walking/
  // refusal deadline, but allow25ms for the independently updated gold field.
  if(waiting&&slot<6&&!original[slot].handle){
   if(!confirming){confirming=true;next=now+25;}
   if(now<next)return SellResult131::Working;
   waiting=confirming=false;shop=0;
  }
  if(!remaining){active=false;return skipped?SellResult131::Remaining:SellResult131::Complete;}
  if(now>=deadline){active=false;return SellResult131::Remaining;}if(now<next)return SellResult131::Working;
  if(scanning){for(const auto& candidate:view->shops){if(!candidate.unit||!candidate.id.Valid()||!std::isfinite(candidate.x)||!std::isfinite(candidate.y))continue;
    bool duplicate=false;for(const auto& s:shops)if(s.id==candidate.id)duplicate=true;if(duplicate)continue;
    shops.push_back(candidate);std::sort(shops.begin(),shops.end(),[&](const auto& a,const auto& b){auto distance=[&](const auto& s){const float x=s.x-owner.x,y=s.y-owner.y;return x*x+y*y;};return distance(a)<distance(b);});if(shops.size()>32)shops.resize(32);
   }
   if(!view->scanDone)return SellResult131::Working;scanning=false;if(shops.empty()){active=false;return SellResult131::NoShop;}}
  if(waiting){waiting=false;++shop;}
  while(slot<6&&!original[slot].handle){++slot;shop=0;}
  if(slot==6){if(passes<2){++passes;slot=shop=0;next=now+100;return SellResult131::Working;}active=false;return SellResult131::Remaining;}
  // Re-check pawnability by native identity, rather than the item's old slot.
  bool pawnable=false;for(unsigned i=0;i<6;++i)if(view->owner.items[i].handle&&view->owner.items[i].id==original[slot].id)pawnable=view->pawnable[i];
  if(!pawnable){original[slot]={};++skipped;++slot;shop=0;next=now+25;return SellResult131::Working;}
  while(shop<shops.size()){
   const auto& target=shops[shop];const float x=target.x-view->owner.x,y=target.y-view->owner.y,distance=x*x+y*y;
   if(!std::isfinite(distance)||distance>1200.f*1200.f||!send(view->owner,original[slot],target)){++shop;continue;}
   ordered[slot]=true;++submitted;waiting=true;next=now+(distance>600.f*600.f?4000:350);return SellResult131::Submitted;
  }
  ++slot;shop=0;next=now+25;return SellResult131::Working;
 }
};
}
