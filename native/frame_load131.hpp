#pragma once
#include "overhead_cadence.hpp"
namespace wht {
// WHT's FrameLoadCadence, sampled from the stock renderer rather than UI ticks.
struct FrameLoadSampler131 {
 FrameLoadCadence policy;
 uintptr_t owner=0;
 uint64_t sampledAt=0;
 bool sampled=false;
 void Reset(){policy={};owner=sampledAt=0;sampled=false;}
 template<class Sample>
 void Poll(uintptr_t root,bool enabled,bool foreground,uint64_t now,Sample sample){
  if(!root||!enabled){Reset();return;}
  if(owner!=root||(sampled&&now<sampledAt)){Reset();owner=root;}
  if(!foreground){policy.Suspend();sampled=false;return;}
  if(sampled&&now-sampledAt<250)return;
  sampled=true;sampledAt=now;
  float fps=0;
  if(sample(fps))policy.Update(fps,now);else policy.Suspend();
 }
};
}
