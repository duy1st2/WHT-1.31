#pragma once
#include "common.hpp"
#include "fast_menu_transition_profile131.hpp"
#include <MinHook.h>
#include <atomic>
#include <cmath>
namespace wht {
// Preserve the native sequence setup and completion callback; shorten only its wait.
using FastMenuTransitionFn131=float(__cdecl*)(unsigned,uintptr_t,unsigned,unsigned,uintptr_t,unsigned);
inline FastMenuTransitionFn131 originalFastMenuTransition131=nullptr;
inline std::atomic<bool> fastMenuDurationActive131=false;
inline std::atomic<unsigned> fastMenuTransitions131=0,fastMenuShortened131=0;
inline float FastMenuDuration131(float native,bool enabled){return enabled&&std::isfinite(native)&&native>.001f?.001f:native;}
template<class Original> float RunFastMenuTransition131(bool enabled,Original original,unsigned kind,uintptr_t sequence,unsigned a,unsigned b,uintptr_t callback,unsigned context){
 return FastMenuDuration131(original(kind,sequence,a,b,callback,context),enabled);
}
inline float __cdecl FastMenuTransitionCall131(unsigned kind,uintptr_t sequence,unsigned a,unsigned b,uintptr_t callback,unsigned context){
 const bool enabled=fastMenuDurationActive131.load();
 const float native=originalFastMenuTransition131(kind,sequence,a,b,callback,context);
 const float result=FastMenuDuration131(native,enabled);
 ++fastMenuTransitions131;if(result!=native&&std::isfinite(native))++fastMenuShortened131;return result;
}
struct FastMenuDurationService131 {
 bool attempted=false,ready=false;unsigned reported=0;
 void Update(uintptr_t base,unsigned build,bool enabled,const std::wstring& folder){
  fastMenuDurationActive131=enabled&&ready;
  const auto calls=fastMenuTransitions131.load();if(calls!=reported){reported=calls;Log(folder,"WHT: Fast Menu native transitions="+std::to_string(calls)+", shortened="+std::to_string(fastMenuShortened131.load()));}
  if(!enabled||attempted)return;attempted=true;
  const auto* p=FastMenuTransitionFor131(build);
  if(!p||!FastMenuTransitionGuard131(base,*p)){Log(folder,"WHT: Fast Menu transition signature refused; duration unchanged.");return;}
  auto status=MH_Initialize();
  if(status==MH_OK||status==MH_ERROR_ALREADY_INITIALIZED){
   auto address=reinterpret_cast<void*>(base+p->entry);
   status=MH_CreateHook(address,reinterpret_cast<void*>(FastMenuTransitionCall131),reinterpret_cast<void**>(&originalFastMenuTransition131));
   if(status==MH_OK){status=MH_EnableHook(address);if(status==MH_OK)ready=true;else MH_RemoveHook(address);}
  }
  fastMenuDurationActive131=enabled&&ready;
  Log(folder,ready?"WHT: Fast Menu native transition hook active; original callbacks retained, wait capped at 0.001 s.":"WHT: Fast Menu native transition hook refused, MinHook status="+std::to_string(int(status))+"; duration unchanged.");
 }
 void Restore(){fastMenuDurationActive131=false;}
};
}
