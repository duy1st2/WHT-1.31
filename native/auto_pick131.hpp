#pragma once
#include "drop_all.hpp"
#include <vector>
namespace wht {
// Game-thread policy only. Native enumeration/visibility/order adapters must be
// independently verified before this worker is exposed in a 1.31 menu.
struct PickCandidate131 {DropObjectId id;float x=0,y=0;uintptr_t object=0;};
struct PickSnapshot131 {DropSnapshot owner;unsigned capacity=0,occupied=0;std::vector<PickCandidate131> ground;};
enum class PickResult131 {Idle,Waiting,Submitted,Cancelled};
struct AutoPick131 {
 DropSnapshot owner;DropObjectId target;uint64_t next=0,submittedAt=0;bool bound=false,waiting=false;
 struct Failed {DropObjectId id;uint64_t until=0;};std::array<Failed,16> failures{};unsigned failedCursor=0;
 void Reset(){*this={};}
 bool Excluded(const DropObjectId& id,uint64_t now)const{for(const auto& failed:failures)if(failed.until>now&&failed.id==id)return true;return false;}
 template<class Send> PickResult131 Tick(uint64_t now,const PickSnapshot131* snapshot,Send send){
  if(!snapshot){Reset();return PickResult131::Cancelled;}
  const auto& source=snapshot->owner;
  if(!source.root||!source.session||!source.unit||!source.id.Valid()||!std::isfinite(source.x)||!std::isfinite(source.y)||!snapshot->capacity||snapshot->capacity>6||snapshot->occupied>snapshot->capacity){Reset();return PickResult131::Cancelled;}
  if(bound&&(source.root!=owner.root||source.session!=owner.session||source.unit!=owner.unit||!(source.id==owner.id))){Reset();return PickResult131::Cancelled;}
  owner=source;bound=true;
  if(now<next)return PickResult131::Waiting;next=now+250;
  if(snapshot->occupied>=snapshot->capacity){waiting=false;return PickResult131::Waiting;}
  if(waiting){bool present=false;for(const auto& item:snapshot->ground)if(item.id==target)present=true;
   if(present&&now-submittedAt<5000)return PickResult131::Waiting;
   if(present)failures[failedCursor++%failures.size()]={target,now+10000};waiting=false;
  }
  const PickCandidate131* best=nullptr;float distance=777.f*777.f;
  for(const auto& item:snapshot->ground){if(!item.id.Valid()||!std::isfinite(item.x)||!std::isfinite(item.y)||Excluded(item.id,now))continue;
   const float dx=item.x-source.x,dy=item.y-source.y,d=dx*dx+dy*dy;if(d<distance){distance=d;best=&item;}}
  if(!best)return PickResult131::Idle;
  if(!send(source,*best)){Reset();return PickResult131::Cancelled;}
  target=best->id;submittedAt=now;waiting=true;return PickResult131::Submitted;
 }
};
}
