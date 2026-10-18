#pragma once
#include "common.hpp"
#include "clock_fps131.hpp"
#include <mmsystem.h>
namespace wht {
// Consume the first valid simulation observation even while disabled, as 1.29.
struct MatchStart131 {
 uintptr_t owner=0;bool consumed=false;
 void Reset(){owner=0;consumed=false;}
 bool Observe(uintptr_t ui,bool ready,double seconds,bool enabled){
  if(owner!=ui){owner=ui;consumed=false;}
  if(!ui||!ready||consumed)return false;
  consumed=true;return enabled&&std::isfinite(seconds)&&seconds>=0&&seconds<=5;
 }
};
inline void UpdateMatchStart131(MatchStart131& state,uintptr_t base,unsigned build,uintptr_t ui,bool enabled,HMODULE module,const std::wstring& folder){
 if(state.owner==ui&&state.consumed)return;
 const auto* p=ClockFpsProfile(build);uint32_t seconds=0;double precise=0;
 const bool ready=p&&SimulationSeconds131(base,*p,seconds,&precise);
 if(state.Observe(ui,ready,precise,enabled))
  if(!PlaySoundW(MAKEINTRESOURCEW(101),module,SND_RESOURCE|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP))Log(folder,"WHT: start sound unavailable; skipped.");
}
}
