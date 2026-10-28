#pragma once
#include "common.hpp"
#include "overhead_cadence.hpp"
#include <tuple>
namespace wht {
inline auto OverheadSampleSettings131(const Config& c){
 return std::make_tuple(c.barWidthHp,c.barHeightHp,c.colorPlayer,c.hpBarColor,c.hpBarColorEnemy,
  c.hideInvulnerableBars,c.stateTagEnable,c.stateTagHero,c.stateTagNonHero,c.stateTagFontSize,
  c.stateTagHpValue,c.stateTagManaValue,c.stateTagShowMax,c.stateTagShowLevel,c.lifeManaCommas);
}
// WHT's sampling policies, using the port's validated roots and native unit IDs.
// Only expensive data sampling is paced; native clipping/movement is untouched.
struct OverheadSampling131 {
 OverheadCadence cadence;CrowdBarCadence crowd;
 decltype(&GetTickCount64) sampleClock=&GetTickCount64;
 uint64_t Now()const{return sampleClock();}
 decltype(OverheadSampleSettings131(Config{})) settings{};
 uintptr_t owner=0,priority=0;bool known=false,permission=false;uint64_t interval=200;
 void Reset(){cadence={};crowd={};settings={};owner=priority=0;known=permission=false;interval=200;}
 void Configure(uintptr_t root,uintptr_t selected,const Config& config,bool allowed){
  const auto key=OverheadSampleSettings131(config);const bool context=!known||owner!=root||permission!=allowed;
  if(context||settings!=key){cadence={};settings=key;}
  if(context){crowd={};interval=200;}
  owner=root;priority=selected;permission=allowed;known=true;
 }
 bool Full(uint64_t now){return owner&&cadence.Full(owner,permission,now);}
 void Load(unsigned count){interval=crowd.Update(count);}
 void Invalidate(uintptr_t unit){cadence.units.erase(unit);}
 bool Unit(uintptr_t unit,uint64_t identity,uintptr_t frame,bool hero,uint64_t now,const Config& config,bool allowed){
  Configure(owner,priority,config,allowed);
  if(!owner||!allowed)return true;
  return cadence.Unit(owner,unit,identity,now,frame,(hero||unit==priority)?200:interval);
 }
};
}
