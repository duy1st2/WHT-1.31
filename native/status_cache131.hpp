#pragma once
#include "common.hpp"
#include <cmath>
#include <limits>
namespace wht {
// WHT MemoryOverlay: sample RAM every 500 ms and write only changed text.
struct StatusCache131 {
 uintptr_t owner=0,font=0;uint64_t sampledAt=0;int megabytes=0;
 bool sampled=false,valid=false,polling=false,textKnown=false;std::string applied;
 void Reset(){owner=font=sampledAt=0;megabytes=0;sampled=valid=polling=textKnown=false;applied.clear();}
 template<class Sample,class Write>
 bool Update(uint64_t now,uintptr_t root,uintptr_t target,const std::string& prefix,bool authorized,bool showMemory,bool externalMemory,Sample sample,Write write){
  if(!root||!target){Reset();return false;}
  if(owner!=root||font!=target){Reset();owner=root;font=target;}
  const bool want=authorized&&showMemory&&!externalMemory;
  if(want!=polling){polling=want;sampled=false;}
  if(want&&(!sampled||now-sampledAt>=500)){sampled=true;sampledAt=now;float mb=0,percent=0;if(sample(mb,percent)&&std::isfinite(mb)&&mb>=0&&double(mb)<=double((std::numeric_limits<int>::max)())){megabytes=int(mb);valid=true;}}
  auto text=prefix;if(!authorized)text+=" |cffff4444Login LoadFileMapWar3|r";else if(want&&valid)text+=" | "+std::to_string(megabytes)+" MB";
  if(!textKnown||text!=applied){if(!write(text))return false;applied=text;textKnown=true;return true;}return false;
 }
};
}
