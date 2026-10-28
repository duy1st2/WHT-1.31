#pragma once
#include "clock_fps131.hpp"
#include <limits>
inline unsigned fpsTestCalls=0;inline uintptr_t fpsTestMode=0;
#ifdef _WIN64
inline void FpsTestSetter(uintptr_t bar,bool value){++fpsTestCalls;*reinterpret_cast<unsigned char*>(bar+fpsTestMode)=value;}
#else
inline void __fastcall FpsTestSetter(uintptr_t bar,void*,bool value){++fpsTestCalls;*reinterpret_cast<unsigned char*>(bar+fpsTestMode)=value;}
#endif
inline void CheckClockFps131(){
 for(const auto& p:wht::clockFpsProfiles131){
  alignas(uintptr_t) unsigned char time[0x300]{};uintptr_t singleton=reinterpret_cast<uintptr_t>(time);
  const uintptr_t base=reinterpret_cast<uintptr_t>(&singleton)-p.clockGlobal;
  *reinterpret_cast<uint32_t*>(time)=0xaa2b9d08u; // Observed live mutable manager header.
  *reinterpret_cast<uint32_t*>(time+4)=0x12345678u; // Header hash is not a high pointer word.
  // Independent native observations: normal channel +14/+18, current +40/+70.
  const unsigned value=sizeof(uintptr_t)==8?0x88:0x54;
  const unsigned scheduler=sizeof(uintptr_t)==8?0x18:0x14;
  const uintptr_t nativeSchedulerVT=sizeof(uintptr_t)==8?(p.build==12164?0x1c60ca8:0x1c8fbb8):(p.build==12164?0x1857788:0x180c6a8);
  *reinterpret_cast<uintptr_t*>(time+scheduler)=base+nativeSchedulerVT;
  *reinterpret_cast<float*>(time+value)=65.75f;*reinterpret_cast<unsigned*>(time+value+4)=2;*reinterpret_cast<float*>(time+value+8)=4096;
  uint32_t seconds=0;check(wht::SimulationSeconds131(base,p,seconds)&&seconds==8257,"simulation clock includes scheduler rollover");
  check(wht::TimerText131(seconds)=="|cfffcd21102|cffffffff:|cfffcd21117|cffffffff:|cfffcd21137|r","timer hours minutes seconds");
  check(wht::SimulationSeconds131(base,p,seconds)&&seconds==8257,"paused clock stays unchanged without wall time");
  *reinterpret_cast<float*>(time+value)=66.25f;check(wht::SimulationSeconds131(base,p,seconds)&&seconds==8258,"simulation advancement follows native value");
  *reinterpret_cast<float*>(time+value)=std::numeric_limits<float>::quiet_NaN();check(!wht::SimulationSeconds131(base,p,seconds),"invalid clock hidden");
  *reinterpret_cast<float*>(time+value)=1;*reinterpret_cast<unsigned*>(time+value+4)=~0u;check(!wht::SimulationSeconds131(base,p,seconds),"clock overflow rejected");
  *reinterpret_cast<unsigned*>(time+value+4)=0;
  *reinterpret_cast<uint32_t*>(time)=0x86194d8e;check(wht::SimulationSeconds131(base,p,seconds)&&seconds==1,"changing native header does not hide timer");
  *reinterpret_cast<uintptr_t*>(time+scheduler)=0;check(!wht::SimulationSeconds131(base,p,seconds),"unrelated scheduler rejected");
 }
 alignas(uintptr_t) unsigned char bar[0x300]{};const auto object=reinterpret_cast<uintptr_t>(bar);fpsTestMode=sizeof(uintptr_t)==8?0x210:0x140;
 const auto setter=reinterpret_cast<wht::FpsSetter131>(FpsTestSetter);wht::FpsState131 fps;fpsTestCalls=0;
 *reinterpret_cast<unsigned char*>(bar+fpsTestMode)=1;
 check(fps.Apply(object,fpsTestMode,true,false,setter)&&fpsTestCalls==1&&!bar[fpsTestMode],"unchecked FPS hides native display");
 fps.Apply(object,fpsTestMode,true,false,setter);check(fpsTestCalls==1,"FPS does not call setter every tick");
 fps.Apply(object,fpsTestMode,true,true,setter);check(fpsTestCalls==2&&bar[fpsTestMode],"FPS checkbox enables native display");
 fps.Apply(object,fpsTestMode,true,false,setter);fps.Apply(object,fpsTestMode,false,false,setter);
 check(fpsTestCalls==4&&bar[fpsTestMode]&&!fps.active,"leaving allowed mode restores initial display");
 bar[fpsTestMode]=3;check(!fps.Apply(object,fpsTestMode,true,true,setter)&&fpsTestCalls==4,"invalid FPS mode never changed");
}
