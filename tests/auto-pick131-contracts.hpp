#pragma once
#include "auto_pick131.hpp"
#include "auto_pick_native131.hpp"
inline void CheckAutoPick131(){
 for(const auto& p:wht::inventoryProfiles131){
  alignas(uintptr_t) unsigned char item[0x1000]{};const uintptr_t base=0x10000000,object=reinterpret_cast<uintptr_t>(item);
  *reinterpret_cast<uintptr_t*>(item)=base+p.itemTable;
  const unsigned idOffset=sizeof(uintptr_t)==8?0x18:0xc,ownerOffset=sizeof(uintptr_t)==8?0x8ac:0x5d8,flagsOffset=sizeof(uintptr_t)==8?0x38:0x20;
  const wht::DropObjectId id{13,17};memcpy(item+idOffset,&id,8);memset(item+ownerOffset,0xff,8);
  check(wht::GroundPickItem131(base,p,object),"visible unowned native item is eligible");
  item[flagsOffset]=1;check(!wht::GroundPickItem131(base,p,object),"hidden native item cannot be picked");item[flagsOffset]=0;
  item[ownerOffset]=0;check(!wht::GroundPickItem131(base,p,object),"item already in an inventory cannot be picked");memset(item+ownerOffset,0xff,8);
  memset(item+idOffset,0xff,8);check(!wht::GroundPickItem131(base,p,object),"deleted item ID is rejected");
 }
 wht::AutoPickIcon131 icon;icon.Reset();check(icon.area.bottom==.096f&&!strcmp(icon.upAsset,"BTNAutoPick.blp")&&!strcmp(icon.downAsset,"BTNAutoPickDow.blp"),"pickup icon retains independent placement and textures after reset");

 wht::PickSnapshot131 snap;snap.owner.root=1;snap.owner.session=2;snap.owner.unit=3;snap.owner.id={4,5};snap.capacity=6;
 snap.ground={{{10,11},30,40},{{20,21},100,100},{{30,31},900,0},{{0,0},0,0}};
 wht::AutoPick131 worker;unsigned sends=0;wht::DropObjectId last;
 auto send=[&](const wht::DropSnapshot& owner,const wht::PickCandidate131& item){check(owner.unit==3,"pickup uses the validated unit");++sends;last=item.id;return true;};
 check(worker.Tick(1000,&snap,send)==wht::PickResult131::Submitted&&sends==1&&last==snap.ground[0].id,"pickup selects nearest valid target within radius");
 check(worker.Tick(1100,&snap,send)==wht::PickResult131::Waiting&&sends==1,"pickup does not flood during throttle interval");
 check(worker.Tick(2000,&snap,send)==wht::PickResult131::Waiting&&sends==1,"walking to target does not repeat orders");
 check(worker.Tick(6000,&snap,send)==wht::PickResult131::Submitted&&sends==2&&last==snap.ground[1].id,"unreachable item times out and a different item is tried");
 check(worker.Excluded(snap.ground[0].id,7000)&&!worker.Excluded(snap.ground[0].id,16000),"unreachable target excluded for ten seconds");
 snap.occupied=6;check(worker.Tick(6250,&snap,send)==wht::PickResult131::Waiting&&sends==2,"full inventory waits without dispatch");
 snap.occupied=0;snap.owner.unit=33;check(worker.Tick(6500,&snap,send)==wht::PickResult131::Cancelled&&sends==2,"selection change cancels old target");snap.owner.unit=3;
 worker.Reset();snap.ground.erase(snap.ground.begin(),snap.ground.begin()+2);check(worker.Tick(7000,&snap,send)==wht::PickResult131::Idle&&sends==2,"out of range and invalid objects are ignored");
 snap.ground={{{40,41},1,1}};check(worker.Tick(7250,&snap,[](const auto&,const auto&){return false;})==wht::PickResult131::Cancelled&&!worker.bound,"native submission refusal resets worker");
 check(worker.Tick(7500,nullptr,send)==wht::PickResult131::Cancelled,"missing snapshot clears target only");
 {wht::AutoPick131 persistent;auto state=snap;state.capacity=6;state.occupied=6;unsigned picked=0;
  auto pickup=[&](const auto&,const auto&){++picked;return true;};
  for(unsigned now=0;now<5000;now+=250)check(persistent.Tick(now,&state,pickup)==wht::PickResult131::Waiting&&picked==0,"full bag repeatedly waits without invalidating worker");
  state.occupied=5;check(persistent.Tick(5000,&state,pickup)==wht::PickResult131::Submitted&&picked==1,"one freed slot resumes pickup without re-enable");
  check(persistent.Tick(5250,nullptr,pickup)==wht::PickResult131::Cancelled&&persistent.next==5500,"invalid read backs off rather than busy polling");
  check(persistent.Tick(5500,&state,pickup)==wht::PickResult131::Submitted&&picked==2,"same enabled worker resumes after transient invalid selection");
 }

}
