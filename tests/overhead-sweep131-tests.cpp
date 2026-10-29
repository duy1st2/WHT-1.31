#include "../native/overhead_sweep131.hpp"
#ifndef _WIN64
#include "../native/state_bars131.hpp"
#else
#include "../native/gameplay64.hpp"
#endif
#include <stdexcept>
#include <cstdio>
#include <numeric>
void Check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int64_t ticks=0,step=100;
BOOL WINAPI Clock(LARGE_INTEGER* out){out->QuadPart=ticks;ticks+=step;return TRUE;}
BOOL WINAPI FailedClock(LARGE_INTEGER*){return FALSE;}
void NativeDiscovery(){
 constexpr unsigned total=300;
 alignas(uintptr_t) std::array<unsigned char,0x400> host{},manager{};
 alignas(uintptr_t) std::array<unsigned char,0x700> unit{};
 std::vector<std::array<uintptr_t,4>> nodes(total);
 std::vector<std::array<unsigned char,0x280>> frames(total);
 const auto u=reinterpret_cast<uintptr_t>(unit.data()),h=reinterpret_cast<uintptr_t>(host.data());*reinterpret_cast<uintptr_t*>(unit.data())=0x1000;
 const auto managerOffset=sizeof(uintptr_t)==8?0xb8:0x74,firstOffset=sizeof(uintptr_t)==8?0x2b0:0x18c;
 const auto typeOffset=sizeof(uintptr_t)==8?0x1e8:0x134,unitOffset=sizeof(uintptr_t)==8?0x210:0x154;
 *reinterpret_cast<uintptr_t*>(host.data()+managerOffset)=reinterpret_cast<uintptr_t>(manager.data());*reinterpret_cast<uintptr_t*>(manager.data()+firstOffset)=reinterpret_cast<uintptr_t>(nodes.data());
 for(unsigned i=0;i<total;++i){nodes[i][1]=i+1<total?reinterpret_cast<uintptr_t>(&nodes[i+1]):0;nodes[i][2]=reinterpret_cast<uintptr_t>(frames[i].data());*reinterpret_cast<uintptr_t*>(frames[i].data())=0x2000;*reinterpret_cast<unsigned*>(frames[i].data()+typeOffset)=5;*reinterpret_cast<uintptr_t*>(frames[i].data()+unitOffset)=u;}
 wht::Config config;config.showCooldown=config.showRegenHP=config.showRegenMana=false;bool allowed=true;wht::NativeFrameApi raw{};
#ifdef _WIN64
 wht::Profile64 profile{};profile.statVtable=0x2000;profile.unitVtable=0x1000;wht::ToolMenuOverlay menu;menu.independentHost=host.data();wht::Gameplay64 overlay;
 auto update=[&](){overlay.Update(0,1,profile,config,allowed,menu,raw);};
#else
 wht::Profile131 profile{};wht::RegenProfile131 regen{};regen.unit=0x1000;wht::StateBarProfile131 bar{};bar.bar=0x2000;wht::StateBars131 overlay;
 auto update=[&](){overlay.Update(0,host.data(),profile,regen,bar,config,allowed,raw);};
#endif
 overlay.sweep.workClock=nullptr;
 update();Check(overlay.sweep.active&&overlay.sweep.cursor==128&&overlay.sweep.found==128,"actual service discovers at most128 registered native frames per regular tick");
 // Rebind a not-yet-visited native frame after collection. Its current binding
 // must be read at processing time, without using a captured unit pointer.
 const auto changed=overlay.sweep.pending[overlay.sweep.cursor];*reinterpret_cast<uintptr_t*>(changed+unitOffset)=0;
 update();Check(overlay.sweep.active&&overlay.sweep.cursor==256&&overlay.sweep.found==255,"actual service skips a frame detached between chunks");update();Check(!overlay.sweep.active&&overlay.sweep.found==299&&overlay.sampling.interval==800,"actual service drains all handles and publishes final crowd count");
 allowed=false;update();Check(!overlay.sweep.active&&!overlay.sweep.immediate&&overlay.sweep.cursor==total,"disable visits every current frame in the immediate path");update();Check(!overlay.sweep.active&&overlay.sweep.cursor==total,"disabled service does not start repeated scans");
 allowed=true;update();Check(overlay.sweep.active&&overlay.sweep.cursor==128,"re-enable restarts incremental discovery");config.stateTagShowMax=!config.stateTagShowMax;update();Check(!overlay.sweep.active&&overlay.sweep.cursor==total,"changed display settings cancel old queue and apply immediately");overlay.Reset();Check(!overlay.sweep.active&&!overlay.sweep.known&&overlay.sweep.pending.empty(),"actual service teardown clears queued native handles");(void)h;
}
int main(){try{
 wht::Config config;wht::OverheadSweep131 sweep;sweep.workClock=nullptr;sweep.Configure(1,true,config);std::vector<uintptr_t> frames(1000);std::iota(frames.begin(),frames.end(),uintptr_t(1));Check(sweep.Begin(frames),"initial queue");std::vector<uintptr_t> visited;
 auto visit=[&](uintptr_t frame){visited.push_back(frame);return frame%2==0;};
 for(unsigned i=0;i<7;++i){const auto before=visited.size();Check(!sweep.Pump(1000,visit)&&visited.size()-before==128,"hard128 item limit and work resumes across ticks");}
 Check(sweep.Pump(1000,visit)&&visited==frames&&sweep.found==500&&!sweep.active,"every handle processed exactly once; valid count published at completion");Check(!sweep.Pump(1000,visit)&&visited.size()==1000,"completed queue cannot repeat");
 for(auto divisor:{1000,2000,3000}){sweep.Cancel();sweep.workClock=Clock;sweep.frequency=1000000;ticks=0;step=100;Check(sweep.Begin(frames),"timed queue");const auto before=visited.size();Check(!sweep.Pump(divisor,visit),"timed queue yields");const unsigned expected=divisor==1000?10:divisor==2000?5:4;Check(visited.size()-before==expected,"WHT1/0.5/0.33ms cooperative budget");}
 sweep.Cancel();ticks=0;step=2000;sweep.Begin(frames);const auto before=visited.size();Check(!sweep.Pump(1000,visit)&&visited.size()==before+1,"at least one item advances under a slow clock");sweep.Cancel();sweep.workClock=FailedClock;sweep.Begin(frames);const auto failedBefore=visited.size();Check(!sweep.Pump(1000,visit)&&visited.size()==failedBefore+128,"failed QPC retains hard item bound");
 sweep.Configure(1,false,config);Check(!sweep.active&&sweep.immediate,"disable cancels pending regular sweep");sweep.Begin(frames);Check(sweep.Pump(3000,visit)&&!sweep.immediate,"disable bypasses budget and drains immediately");sweep.Configure(1,true,config);sweep.Begin(frames);config.stateTagShowMax=!config.stateTagShowMax;sweep.Configure(1,true,config);Check(!sweep.active&&sweep.immediate,"settings change invalidates queued handles");sweep.Begin(frames);Check(sweep.Pump(1000,visit),"settings apply in one pass");
 sweep.Begin(frames);sweep.Configure(2,true,config);Check(!sweep.active&&sweep.owner==2&&sweep.pending.empty(),"new owner discards old native handles");sweep.workClock=nullptr;sweep.Begin(frames);unsigned canceled=0;Check(!sweep.Pump(1000,[&](uintptr_t){++canceled;sweep.Reset();return true;})&&canceled==1&&!sweep.running&&!sweep.active&&!sweep.known,"callback cancellation does not continue stale queue");
 sweep.Configure(3,true,config);sweep.Begin(frames);unsigned nested=0;Check(!sweep.Pump(1000,[&](uintptr_t){++nested;Check(!sweep.Pump(1000,visit)&&!sweep.Begin(frames),"reentrant drain/begin refused");return true;})&&nested==128,"reentry cannot exceed per-tick bound");sweep.Reset();
 NativeDiscovery();puts("Bounded overhead queue, timing, cancellation and actual native-service discovery contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
