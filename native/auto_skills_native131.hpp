#pragma once
#include "auto_skills131.hpp"
#include "auto_visibility_profile131.hpp"
#include "mth_craft131.hpp"
namespace wht {
inline const AutoVisibilityProfile131* AutoVisibilityProfileFor131(unsigned build){for(const auto& p:autoVisibilityProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool AutoVisibilityCode131(uintptr_t base,const AutoVisibilityProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
using AutoFogQuery131=void(__thiscall*)(uintptr_t,const float*,unsigned*,unsigned*,const unsigned*);
inline void InvokeAutoFog131(AutoFogQuery131 query,uintptr_t context,const float* point,unsigned& visible,unsigned& explored,const unsigned* views){query(context,point,&visible,&explored,views);}
inline bool AutoVisibilityPolicy131(unsigned flags,unsigned unitMask,unsigned local,unsigned fog,unsigned viewMask){return local<24&&!(flags&0x30)&&(unitMask&(1u<<local))&&(fog&viewMask);}
struct AutoVision131 {
 uintptr_t base=0;unsigned build=0;bool ready=false,contextKnown=false,lastContext=false;
 bool Check(uintptr_t image,unsigned version){if(base!=image||build!=version){base=image;build=version;contextKnown=false;const auto* p=AutoVisibilityProfileFor131(version);ready=p&&AutoVisibilityCode131(image,*p);}return ready;}
};
struct AutoSkillsApi131 {
 MthCraftApi131 core;AutoVision131& vision;uintptr_t uiTable=0;
 std::function<uintptr_t(unsigned)> button;
 const std::wstring* folder=nullptr;
 bool ReadSource(SelfCastSource131& out){const auto selected=core.Selected();return core.Read(selected,out)&&core.Owned(selected)&&core.Selected()==selected;}
 SkillButton131 Data(unsigned slot){return core.data(slot);}
 uintptr_t Target(){const auto* p=QuickCastProfile(core.build);return p?QuickCastTarget131(core.base,core.ui,uiTable,*p):0;}
 bool ReadContext(uintptr_t& context,unsigned& local,unsigned& mask,const unsigned*& views){
  if(!vision.Check(core.base,core.build))return false;const auto* p=AutoVisibilityProfileFor131(core.build);const auto* rp=ResourceProfileFor131(core.build);const auto* pp=PickProfileFor131(core.build);if(!p||!rp||!pp||!ResourceCode131(core.base,*rp)||!InventoryIdentity131(core.worldFrame,core.base,pp->worldTable))return false;
  __try{unsigned slot=24;const auto world=ResourceWorld131(core.base,*rp,slot);context=*reinterpret_cast<uintptr_t*>(core.worldFrame+p->context);
   if(!world||context!=world||!ResourceIdentity131(context,core.base,rp->worldTable))return false;
   local=*reinterpret_cast<unsigned*>(core.worldFrame+p->local);mask=*reinterpret_cast<unsigned*>(core.worldFrame+p->mask);views=reinterpret_cast<const unsigned*>(core.worldFrame+p->vision);
   return local==slot&&local<24&&mask&&ReadableComponent131(reinterpret_cast<uintptr_t>(views),25*sizeof(unsigned))&&views[0]>=24&&views[0]<=32;
  }__except(1){return false;}
 }
 bool Context(uintptr_t& context,unsigned& local,unsigned& mask,const unsigned*& views){const bool ok=ReadContext(context,local,mask,views);if(folder&&(!vision.contextKnown||vision.lastContext!=ok)){vision.contextKnown=true;vision.lastContext=ok;Log(*folder,ok?"WHT: Auto Skills native vision profile/context accepted.":"WHT: Auto Skills native vision profile/context unavailable or refused; no target search allowed.");}return ok;}
 bool Visible(const SelfCastSource131& candidate){
  uintptr_t context=0;unsigned local=24,mask=0;const unsigned* views=nullptr;if(!Context(context,local,mask,views))return false;const auto* p=AutoVisibilityProfileFor131(core.build);
  __try{const auto flags=*reinterpret_cast<unsigned*>(candidate.unit+(sizeof(void*)==8?0x38:0x20)),unitMask=*reinterpret_cast<unsigned*>(candidate.unit+(sizeof(void*)==8?0x50:0x2c));if(flags&0x30||!(unitMask&(1u<<local)))return false;
   const float xyz[]={candidate.x,candidate.y,0};unsigned visible=0,explored=0;InvokeAutoFog131(reinterpret_cast<AutoFogQuery131>(core.base+p->query),context,xyz,visible,explored,views);
   return AutoVisibilityPolicy131(flags,unitMask,local,visible,mask);
  }__except(1){return false;}
 }
 bool Enemy(uintptr_t unit){
  auto np=DropNetworkProfileFor131(core.build);auto inv=InventoryProfile(core.build);auto rp=ResourceProfileFor131(core.build);if(!np||!inv||!rp||!ResourceCode131(core.base,*rp))return false;auto net=DropNetworkNative131(core.base,*np,*inv);if(!net)return false;
  __try{unsigned local=24;const auto world=ResourceWorld131(core.base,*rp,local);if(!world)return false;const unsigned owner=net.owner(unit);if(owner==local||owner>=28)return false;if(owner>=24)return owner==24;
   const auto own=reinterpret_cast<decltype(ResourceNative131::player)>(core.base+rp->playerFn)(world,local);if(!ResourceIdentity131(own,core.base,rp->playerTable))return false;
   const auto alliances=*reinterpret_cast<uintptr_t*>(own+rp->allianceOffset);if(!ResourceIdentity131(alliances,core.base,rp->allianceTable))return false;
   return !(unsigned(reinterpret_cast<decltype(ResourceNative131::integer)>(core.base+rp->integerFn)(alliances+rp->allianceMask))&(1u<<owner));
  }__except(1){return false;}
 }
 bool ReadEnemy(const SelfCastSource131& owner,uintptr_t unit,float range,SelfCastSource131& result){
  if(unit==owner.unit||!Enemy(unit)||!core.Read(unit,result)||result.ui!=owner.ui||result.session!=owner.session)return false;
  const double dx=double(result.x)-owner.x,dy=double(result.y)-owner.y;return std::isfinite(dx)&&std::isfinite(dy)&&dx*dx+dy*dy<=double(range)*range;
 }
 bool Candidate(const SelfCastSource131& owner,uintptr_t unit,float range,SelfCastSource131& result){return ReadEnemy(owner,unit,range,result)&&Visible(result);}
 AutoScanResult131 Scan(const SelfCastSource131& owner,float range,AutoSkillScan131& scan){
  auto sp=SellProfileFor131(core.build);auto pp=PickProfileFor131(core.build);auto inv=InventoryProfile(core.build);if(!sp||!pp||!inv||!InventoryIdentity131(core.worldFrame,core.base,pp->worldTable))return AutoScanResult131::Failed;
  const auto api=SellNative131(core.base,*sp);if(!api)return AutoScanResult131::Failed;
  __try{const auto list=api.list(core.worldFrame,scan.cursor==0);if(list!=core.worldFrame+sp->listOffset)return AutoScanResult131::Failed;
   const unsigned count=*reinterpret_cast<unsigned*>(list);const auto units=*reinterpret_cast<uintptr_t*>(list+sizeof(uintptr_t));if(count>8192||(count&&!units)||scan.cursor>count)return AutoScanResult131::Failed;
   const unsigned end=(std::min)(count,scan.cursor+128u);unsigned visibilityBudget=8;
   for(;scan.cursor<end;++scan.cursor){const auto unit=*reinterpret_cast<uintptr_t*>(units+scan.cursor*sizeof(uintptr_t));if(!InventoryIdentity131(unit,core.base,inv->unitTable))continue;SelfCastSource131 candidate;
    if(!ReadEnemy(scan.owner,unit,range,candidate))continue;if(!visibilityBudget)break;--visibilityBudget;if(!Visible(candidate))continue;const double dx=double(candidate.x)-scan.owner.x,dy=double(candidate.y)-scan.owner.y,distance=dx*dx+dy*dy;
    const uint64_t id=(uint64_t(candidate.id.low)<<32)|candidate.id.high,bestId=(uint64_t(scan.best.id.low)<<32)|scan.best.id.high;
    if(!scan.best.unit||distance<scan.distance||(distance==scan.distance&&id<bestId)){scan.best=candidate;scan.distance=distance;}
   }
   return scan.cursor>=count?AutoScanResult131::Done:AutoScanResult131::Working;
  }__except(1){return AutoScanResult131::Failed;}
 }
 void CancelOwned(uintptr_t mode,const SelfCastApi131& api){if(Target()!=mode)return;__try{api.cancel(core.ui);}__except(1){}}
 bool Activate(const SelfCastSource131& expected,const SelfCastSource131& destination,unsigned slot,const SkillButton131& requested,float range){
  if(!SameAutoSkill131(Data(slot),requested))return false;const auto api=SelfCastNative131(core.base,core.build);if(!api)return false;
  return ActivateAutoSkill131([&](SelfCastSource131& source){return ReadSource(source)&&SameSelfCastSource131(source,expected);},[&](SelfCastSource131& target){SelfCastSource131 source;return ReadSource(source)&&SameSelfCastSource131(source,expected)&&Candidate(source,destination.unit,range,target)&&SameSelfCastSource131(target,destination);},[&](){return Target();},[&](){return SameAutoSkill131(Data(slot),requested)&&core.click(slot);},SelfCastOrder131,[&](const SelfCastSource131& target,uintptr_t mode,unsigned order){return FinishSelfCast131(api,target,mode,order);},[&](uintptr_t mode){CancelOwned(mode,api);});
 }
 int Hover(POINT point,int width,int height){
  if(width<=0||height<=0||point.x<0||point.y<0||point.x>=width||point.y>=height)return -1;const int classic=(std::min)(width,height*4/3);if(!classic)return -1;
  const float x=.4f+(point.x-width*.5f)*.8f/classic,y=.6f-point.y*.6f/height;
  __try{for(unsigned slot=0;slot<12;++slot){const auto frame=button(slot);if(frame&&InventoryHit131(reinterpret_cast<const float*>(frame+(sizeof(void*)==8?0x88:0x44)),x,y)&&AutoSkillOrder131(Data(slot)))return int(slot);}}__except(1){}return -1;
 }
};
}
