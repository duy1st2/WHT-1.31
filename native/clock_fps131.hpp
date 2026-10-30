#pragma once
#include "tool_menu.hpp"
#include "hud_style.hpp"
#include <cmath>
#include <cstdio>

namespace wht {
struct ClockFpsProfile131 {
 unsigned build;
 uintptr_t clockGlobal,clockValue,schedulerVtable;
 uintptr_t resourceOffset,resourceTable,fontTable,fpsSetter,fpsMode,fpsFont;
};
#ifdef _WIN64
inline constexpr ClockFpsProfile131 clockFpsProfiles131[]={
 {12164,0x21916e0,0x88,0x1c60ca8,0x6c8,0x1c12a30,0x1c58298,0xce7f10,0x210,0x218},
 {12173,0x21be6e0,0x88,0x1c8fbb8,0x6d8,0x1c418f0,0x1c871a8,0xd27590,0x210,0x218}
};
#else
inline constexpr ClockFpsProfile131 clockFpsProfiles131[]={
 {12164,0x1b65a1c,0x54,0x1857788,0x414,0x182d360,0x1853b0c,0xb8ca70,0x140,0x144},
 {12173,0x1b1da14,0x54,0x180c6a8,0x438,0x17e2240,0x1808a2c,0xb4c500,0x140,0x144}
};
#endif
inline const ClockFpsProfile131* ClockFpsProfile(unsigned build){for(const auto& p:clockFpsProfiles131)if(p.build==build)return &p;return nullptr;}
// NiPse header is mutable during simulation. Validate the stable first
// scheduler vtable (CSiDispatch<CPrRelation,int>), not that header.
// Its first scheduler is the simulation channel. Its current float wraps
// at the scheduler period, incrementing the adjacent cycle counter.
inline bool SimulationSeconds131(uintptr_t base,const ClockFpsProfile131& p,uint32_t& seconds,double* precise=nullptr){
 seconds=0;if(!base)return false;
 __try {
  const auto clock=*reinterpret_cast<uintptr_t*>(base+p.clockGlobal);
  if(!clock||*reinterpret_cast<uintptr_t*>(clock+(sizeof(uintptr_t)==8?0x18:0x14))!=base+p.schedulerVtable)return false;
  const auto field=clock+p.clockValue;
  const float current=*reinterpret_cast<float*>(field),period=*reinterpret_cast<float*>(field+8);
  const unsigned cycles=*reinterpret_cast<unsigned*>(field+4);
  if(!std::isfinite(current)||!std::isfinite(period)||current<0||period<=0||period>864000||current>period)return false;
  const double total=double(current)+double(cycles)*period;
  if(total>=864000)return false;seconds=static_cast<uint32_t>(total);if(precise)*precise=total;return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string TimerText131(uint32_t seconds){char text[96]{};std::snprintf(text,sizeof(text),"|cfffcd211%02u|cffffffff:|cfffcd211%02u|cffffffff:|cfffcd211%02u|r",seconds/3600,(seconds/60)%60,seconds%60);return text;}
inline void ClockDiagnostic131(uintptr_t base,unsigned build,char* text,size_t size){
 const auto* p=ClockFpsProfile(build);if(!p)return;
 __try {
  const auto clock=*reinterpret_cast<uintptr_t*>(base+p->clockGlobal);
  if(!clock){std::snprintf(text,size,"clock singleton null");return;}
  const auto field=clock+p->clockValue;
  std::snprintf(text,size,"clock raw: tag=%08X current=%.6f cycles=%u period=%.6f",*reinterpret_cast<unsigned*>(clock),*reinterpret_cast<float*>(field),*reinterpret_cast<unsigned*>(field+4),*reinterpret_cast<float*>(field+8));
 }__except(EXCEPTION_EXECUTE_HANDLER){std::snprintf(text,size,"clock native read exception");}
}
struct ClockOverlay131 {
 void* frame=nullptr;void* font=nullptr;bool reported=false;uint32_t last=~0u;bool lastHide=false;uint64_t diagnosticDue=0;unsigned diagnosticCount=0;
 void Diagnose(uintptr_t base,unsigned build,bool enabled,const std::wstring& folder){
  const auto now=GetTickCount64();if(!enabled||reported||diagnosticCount>=4||now<diagnosticDue)return;
  diagnosticDue=now+5000;++diagnosticCount;char text[192]{};ClockDiagnostic131(base,build,text,sizeof(text));
  Log(folder,std::string("WHT timer: ")+text+" frame="+(frame?"yes":"no")+" font="+(font?"yes":"no"));
 }
 void Reset(){*this={};last=~0u;}
 bool Update(uintptr_t base,unsigned build,bool enabled,ToolMenuOverlay& menu,bool hideTime=false){
  const auto* p=ClockFpsProfile(build);uint32_t seconds=0;
  if(!enabled){if(frame)NativeFrameHide(frame);return false;}
  // Preserve the last simulation time on a transient read failure. No wall-time fallback.
  if(!p||!SimulationSeconds131(base,*p,seconds))return false;
  if(!menu.initialized)return false;
  if(!frame){const ToolMenuRect rect{HudStyle::Center(hideTime)+.06f,HudStyle::timerY-.0125f,.12f,.025f};frame=menu.Frame(base,menu.independentHost,rect);if(frame){font=menu.TextIn(frame,rect,"",true,HudStyle::timerSize);if(font)menu.api.setRelPoint(font,0,frame,0,0,0,1);lastHide=hideTime;}}
  if(!frame||!font)return false;
  if(lastHide!=hideTime){menu.api.setAbsPoint(frame,0,HudStyle::Center(hideTime)-.06f,HudStyle::timerY+.0125f,1);lastHide=hideTime;}
  if(last!=seconds){last=seconds;const auto text=TimerText131(seconds);menu.api.setText(font,text.c_str());}
  NativeFrameShow(frame);return true;
 }
};
using FpsSetter131=void(__thiscall*)(uintptr_t,bool);
inline uintptr_t FpsBar131(uintptr_t base,uintptr_t ui,const ClockFpsProfile131& p){
 if(!base||!ui)return 0;
 __try {
  const auto bar=*reinterpret_cast<uintptr_t*>(ui+p.resourceOffset);
  if(!bar||*reinterpret_cast<uintptr_t*>(bar)!=base+p.resourceTable||*reinterpret_cast<unsigned char*>(bar+p.fpsMode)>1)return 0;
  for(unsigned i=0;i<2;++i){const auto font=*reinterpret_cast<uintptr_t*>(bar+p.fpsFont+i*sizeof(uintptr_t));if(!font||*reinterpret_cast<uintptr_t*>(font)!=base+p.fontTable)return 0;}
  return bar;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
struct FpsState131 {
 uintptr_t owner=0,styledFont=0;LONG styledWidth=0,styledHeight=0;bool active=false,saved=false,reported=false;
 void Reset(){*this={};}
 bool Apply(uintptr_t bar,uintptr_t modeOffset,bool allowed,bool enabled,FpsSetter131 set){
  if(owner!=bar){owner=bar;active=false;}
  if(!bar||!set)return false;
  __try {
   const auto mode=*reinterpret_cast<unsigned char*>(bar+modeOffset);if(mode>1)return false;
   if(allowed){if(!active){saved=mode!=0;active=true;}if((mode!=0)!=enabled)set(bar,enabled);return true;}
   if(active){if((mode!=0)!=saved)set(bar,saved);active=false;}
  }__except(EXCEPTION_EXECUTE_HANDLER){}return false;
 }
 bool Update(uintptr_t base,uintptr_t ui,unsigned build,bool allowed,bool enabled,NativeFrameApi* api=nullptr){
  const auto* p=ClockFpsProfile(build);if(!p)return false;
  __try {
#ifdef _WIN64
   const unsigned char prefix[]={0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x0f,0xb6,0xda,0x48,0x8b,0xf9};
#else
   const unsigned char prefix[]={0x55,0x8b,0xec,0x53,0x8a,0x5d,0x08,0x56,0x8b,0xf1};
#endif
   if(memcmp(reinterpret_cast<void*>(base+p->fpsSetter),prefix,sizeof(prefix)))return false;
   const auto bar=FpsBar131(base,ui,*p);
   const bool result=Apply(bar,p->fpsMode,allowed,enabled,reinterpret_cast<FpsSetter131>(base+p->fpsSetter));
   if(result&&bar&&allowed&&enabled&&api){const auto font=*reinterpret_cast<uintptr_t*>(bar+p->fpsFont);
    if((styledFont!=font||styledWidth!=ToolMenuOverlay::viewportWidth||styledHeight!=ToolMenuOverlay::viewportHeight)&&StyleNativeFps(reinterpret_cast<void*>(font),*api)){
     styledFont=font;styledWidth=ToolMenuOverlay::viewportWidth;styledHeight=ToolMenuOverlay::viewportHeight;
    }
   }
   return result;
  }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
};
}
