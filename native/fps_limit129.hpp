#pragma once
#include <cstdint>
namespace wht {
struct FpsApi129 {
 uintptr_t(__cdecl* context)(unsigned);
 unsigned(__cdecl* get)(uintptr_t);
 void(__cdecl* set)(unsigned,uintptr_t);
};
// Native setter 440230 takes whole milliseconds; round up to respect the cap.
struct FpsLimit129 {
 uintptr_t owner=0;unsigned saved=0,written=0;bool active=false;
 template<class Api> bool Apply(unsigned fps,bool enabled,Api& api,bool unlimited=false){
  if(fps&&(fps<60||fps>1000))return false;
  const bool on=enabled&&(fps||unlimited);
  if(!on&&!active)return true;
  const auto currentOwner=api.context(0);
  if(currentOwner!=owner){owner=currentOwner;active=false;}
  if(!owner)return false;
  const auto current=api.get(owner);
  if(current>1000)return false;
  if(!on){if(active&&current==written){api.set(saved,owner);if(api.get(owner)!=saved)return false;}active=false;return true;}
  if(!active||current!=written)saved=current;
  const auto interval=fps?(1000u+fps-1u)/fps:0u;
  if(current!=interval){api.set(interval,owner);if(api.get(owner)!=interval){active=false;return false;}}
  written=interval;active=true;return true;
 }
};
}
