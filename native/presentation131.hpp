#pragma once
#include "presentation_profile131.hpp"
namespace wht {
inline bool PresentationControl131(const std::wstring& section,const std::wstring& key){return section==L"Graphics"&&(key==L"VSync"||key==L"Unlock Limit FPS");}
// The engine's maxfps dispatcher uses trunc(1000/fps) milliseconds.
// Its scheduler setter accepts zero: no minimum frame interval.
inline bool EffectiveUnlock131(bool allowed,bool unlock,bool vsync){return allowed&&unlock&&!vsync;}
struct PresentationState131 {
 bool syncOwned=false,limitOwned=false,syncValue=false;
 template<class Adapter> bool Update(bool allowed,bool vsync,bool unlock,Adapter& api){
  const bool wantUnlock=EffectiveUnlock131(allowed,unlock,vsync);
  // Restore pacing before switching VSync on; never run both overrides.
  if(limitOwned&&!wantUnlock){unsigned original=0;if(!api.Read(17,original)||!(original?api.Apply(17,original):api.Unlock()))return false;limitOwned=false;}
  if(!allowed){if(syncOwned){unsigned original=0;if(!api.Read(19,original)||!api.Apply(19,original?1:0))return false;syncOwned=false;}return true;}
  if(!syncOwned||syncValue!=vsync){if(!api.Apply(19,vsync?1:0))return false;syncOwned=true;syncValue=vsync;}
  if(wantUnlock&&!limitOwned){if(!api.Unlock())return false;limitOwned=true;}
  return true;
 }
};
struct PresentationNative131 {
 uintptr_t base=0;const PresentationProfile131* profile=nullptr;
 bool Ready()const{
  if(!base||!profile)return false;
  __try{return *reinterpret_cast<uintptr_t*>(base+profile->settings)&&*reinterpret_cast<uintptr_t*>(base+profile->renderer);}__except(1){return false;}
 }
 bool Read(unsigned key,unsigned& value)const{
  if((key!=17&&key!=19)||!Ready())return false;
  __try{using Fn=bool(__thiscall*)(uintptr_t,unsigned,unsigned*);return reinterpret_cast<Fn>(base+profile->read)(*reinterpret_cast<uintptr_t*>(base+profile->settings),key,&value);}__except(1){return false;}
 }
 bool Apply(unsigned key,unsigned value)const{
  if((key!=17&&key!=19)||!Ready()||(key==17&&!value))return false;
  __try{using Fn=bool(__thiscall*)(uintptr_t,unsigned,unsigned);return reinterpret_cast<Fn>(base+profile->apply)(*reinterpret_cast<uintptr_t*>(base+profile->settings),key,value);}__except(1){return false;}
 }
 bool Unlock()const{
  if(!Ready())return false;
  __try{using Fn=void(__cdecl*)(unsigned,unsigned);reinterpret_cast<Fn>(base+profile->unlock)(0,0);return true;}__except(1){return false;}
 }
};
struct PresentationService131 {
 PresentationState131 state;PresentationNative131 native;bool checked=false,valid=false,reported=false;ULONGLONG next=0;
 void Update(uintptr_t base,unsigned build,bool allowed,bool vsync,bool unlock,const std::wstring& folder){
  const auto now=GetTickCount64();if(now<next)return;next=now+500;
  if(!checked){native={base,PresentationProfileFor131(build)};valid=native.profile&&PresentationGuard131(base,*native.profile);checked=true;if(!valid)Log(folder,"WHT 1.31 presentation profile rejected; VSync/FPS overrides unavailable.");}
  if(!valid||!native.Ready())return;
  const auto oldSync=state.syncOwned,oldLimit=state.limitOwned,oldValue=state.syncValue;
  if(!state.Update(allowed,vsync,unlock,native)){if(!reported){Log(folder,"WHT 1.31 presentation update refused; will retry.");reported=true;}return;}
  if(oldSync!=state.syncOwned||oldLimit!=state.limitOwned||oldValue!=state.syncValue){Log(folder,!allowed?"WHT 1.31 presentation: native preferences restored.":vsync?"WHT 1.31 presentation: VSync ON, FPS unlock inactive.":unlock?"WHT 1.31 presentation: VSync OFF, requested native frame interval 0 ms.":"WHT 1.31 presentation: VSync OFF, native frame limit.");}reported=false;
 }
 void Restore(){if(valid&&native.Ready())state.Update(false,false,false,native);}
};
}
