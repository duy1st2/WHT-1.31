#pragma once
#include "adaptive_effects.hpp"
#include "adaptive_effects_profile131.hpp"
#include <algorithm>
#include "view_commands131.hpp"
namespace wht {
inline bool EffectsReadScale131(uintptr_t base,unsigned build,float& value){const auto* p=EffectsProfileFor131(build);if(!p||!EffectsCode131(base,*p))return false;
 __try{value=*reinterpret_cast<float*>(base+p->scale);return std::isfinite(value)&&value>=0&&value<=1;}__except(1){return false;}}
inline bool EffectsRead131(uintptr_t base,unsigned build,float& fps,float& scale){const auto* p=EffectsProfileFor131(build);if(!p||!EffectsReadScale131(base,build,scale))return false;
 __try{const auto renderer=*reinterpret_cast<uintptr_t*>(base+p->renderer);if(!renderer)return false;fps=*reinterpret_cast<uint32_t*>(renderer+(sizeof(uintptr_t)==8?0x84:0x50))/100.f;return std::isfinite(fps)&&fps>=0&&fps<=10000;}__except(1){return false;}}
// The 1.31 setter returns bool in AL and is cdecl on x86 (the old WHT stdcall ABI is not reused).
using EffectsSetter131=bool(__cdecl*)(float);
inline bool InvokeEffectsSetter131(EffectsSetter131 set,float value){if(!set||!std::isfinite(value)||value<0||value>1)return false;__try{return set(value);}__except(1){return false;}}
inline bool EffectsSet131(uintptr_t base,unsigned build,float value){const auto* p=EffectsProfileFor131(build);float before=0,after=0;if(!p||!EffectsReadScale131(base,build,before)||!InvokeEffectsSetter131(reinterpret_cast<EffectsSetter131>(base+p->setter),value))return false;return EffectsReadScale131(base,build,after)&&after==value;}
struct EffectsController131 {
 AdaptiveEffectsState state;bool warned=false;
 template<class Read,class Set> bool Restore(Read read,Set set){if(!state.owned){state={};return true;}float fps=0,current=0;if(!read(fps,current))return false;return state.Restore(current,set);}
 template<class Read,class Set> bool Tick(uint64_t now,uintptr_t world,bool enabled,bool foreground,Read read,Set set,std::string& report){
  if(!enabled||!world){const bool ok=Restore(read,set);if(!ok&&!warned)report="Auto Effects: could not restore native emission setting";warned=!ok;return ok;}
  if(!foreground){state.Suspend();return true;}if(!state.Due(now))return true;
  float fps=0,current=0;bool ok=read(fps,current);if(ok)ok=state.Update(world,true,fps,current,now,set);else state.Suspend();
  if(!ok&&!warned)report="Auto Effects: native particle/FPS profile or write unavailable";warned=!ok;return ok;
 }
 bool Update(uintptr_t base,unsigned build,uint64_t now,uintptr_t world,bool enabled,bool foreground,std::string& report){return Tick(now,world,enabled,foreground,[&](float& fps,float& current){if(!enabled||!world){fps=0;return EffectsReadScale131(base,build,current);}return EffectsRead131(base,build,fps,current);},[&](float value){return EffectsSet131(base,build,value);},report);}
 bool Reset(uintptr_t base,unsigned build){return Restore([&](float& fps,float& current){fps=0;return EffectsReadScale131(base,build,current);},[&](float value){return EffectsSet131(base,build,value);});}
};
template<class Ready,class Save,class Reload> bool EffectsCommand131(const std::wstring& args,Config& config,Ready ready,Save save,Reload reload,std::string& report){
 bool value=false;if(!ViewToggle131(args,config.autoEffects,value)){report="Usage: #autoeffects [0|1]";return false;}
 if(value&&!ready()){report="Auto Effects: verified particle/FPS profile required";return false;}
 if(!save(value)){report="Auto Effects: could not save settings";return false;}reload();if(config.autoEffects!=value){report="Auto Effects: setting reload failed";return false;}report=value?"Auto Effects: ON (adaptive particle emission)":"Auto Effects: OFF";return true;
}
}
