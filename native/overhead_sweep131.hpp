#pragma once
#include "overhead_sampling131.hpp"
#include <vector>
#include <algorithm>
namespace wht {
// Pending values are handles only. Callers validate current native bindings
// when visiting each item; no native unit/frame is owned by this queue.
struct OverheadSweep131 {
 std::vector<uintptr_t> pending;
 size_t cursor=0;
 unsigned found=0;
 uintptr_t owner=0;
 decltype(OverheadSampleSettings131(Config{})) settings{};
 bool known=false,allowed=false,immediate=false,active=false,running=false;
 uint64_t revision=0;
 int64_t frequency=0;
 decltype(&QueryPerformanceCounter) workClock=&QueryPerformanceCounter;
 void Cancel(){++revision;pending.clear();cursor=found=0;active=false;}
 void Reset(){Cancel();owner=0;settings={};known=allowed=immediate=false;}
 void Configure(uintptr_t root,bool enabled,const Config& config){
  const auto key=OverheadSampleSettings131(config);
  const bool changed=known&&settings!=key;
  if(!known||owner!=root||allowed!=enabled||changed){
   Cancel();owner=root;allowed=enabled;settings=key;known=true;
   immediate=changed||!enabled;
  }
 }
 template<class Frames> bool Begin(const Frames& frames){
  if(running||active)return false;
  pending.assign(frames.begin(),frames.end());cursor=found=0;active=true;++revision;return true;
 }
 template<class Visit> bool Pump(int divisor,Visit visit){
  if(!active||running)return false;
  running=true;struct Leave {bool& flag;~Leave(){flag=false;}} leave{running};
  const auto generation=revision;
  if(!frequency){LARGE_INTEGER f{};if(QueryPerformanceFrequency(&f))frequency=f.QuadPart;}
  LARGE_INTEGER start{};const bool timed=!immediate&&frequency>0&&workClock&&workClock(&start);
  const auto budget=(std::max)(int64_t(1),frequency/(divisor>0?divisor:1000));
  size_t processed=0;
  while(cursor<pending.size()){
   if(!immediate&&processed){
    if(processed>=128)return false;
    LARGE_INTEGER now{};if(timed&&workClock(&now)&&now.QuadPart-start.QuadPart>=budget)return false;
   }
   const auto frame=pending[cursor++];++processed;
   const bool valid=visit(frame);
   if(revision!=generation)return false;
   if(valid)++found;
  }
  pending.clear();active=false;immediate=false;return true;
 }
};
}
