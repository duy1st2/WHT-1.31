#pragma once
#include "common.hpp"
#include <cstdio>
namespace wht {
inline uint64_t UiPerfCounter131(){LARGE_INTEGER value{};return QueryPerformanceCounter(&value)&&value.QuadPart>=0?uint64_t(value.QuadPart):0;}
inline uint64_t UiPerfFrequency131(){static const uint64_t frequency=[](){LARGE_INTEGER value{};return QueryPerformanceFrequency(&value)&&value.QuadPart>0?uint64_t(value.QuadPart):uint64_t(0);}();return frequency;}
inline double UiPerfMilliseconds131(uint64_t ticks){const auto frequency=UiPerfFrequency131();return frequency?double(ticks)*1000.0/double(frequency):0.0;}
struct UiPerf131 {
 static constexpr unsigned reportLimit=120;
 struct Bucket {uint64_t calls=0,total=0,maximum=0;double Average()const{return calls?UiPerfMilliseconds131(total)/double(calls):0;}};
 std::array<Bucket,4> buckets{};uint64_t due=0;unsigned reports=0;bool stateKnown=false,lastAllowed=false;
 void Record(unsigned group,uint64_t elapsed){if(group>=buckets.size()||reports>=reportLimit)return;auto& b=buckets[group];++b.calls;b.total+=elapsed;b.maximum=(std::max)(b.maximum,elapsed);}
 void Report(const std::wstring& folder,bool allowed){
  const auto now=GetTickCount64();if(!stateKnown||lastAllowed!=allowed){stateKnown=true;lastAllowed=allowed;due=now+5000;reports=0;buckets={};return;}
  if(now<due||reports>=reportLimit)return;due=now+5000;++reports;
  char text[768]{};std::snprintf(text,sizeof(text),"WHT performance ms (QPC): allowed=%u update avg=%.3f max=%.3f calls=%llu; graphics avg=%.3f max=%.3f calls=%llu; MTH-toolbar avg=%.3f max=%.3f calls=%llu; bars avg=%.3f max=%.3f calls=%llu",unsigned(allowed),buckets[0].Average(),UiPerfMilliseconds131(buckets[0].maximum),buckets[0].calls,buckets[1].Average(),UiPerfMilliseconds131(buckets[1].maximum),buckets[1].calls,buckets[2].Average(),UiPerfMilliseconds131(buckets[2].maximum),buckets[2].calls,buckets[3].Average(),UiPerfMilliseconds131(buckets[3].maximum),buckets[3].calls);
  buckets={};Log(folder,text);
 }
};
struct UiPerfScope131 {
 UiPerf131& state;unsigned group;bool active;uint64_t started;
 UiPerfScope131(UiPerf131& s,unsigned g):state(s),group(g),active(g<s.buckets.size()&&s.reports<UiPerf131::reportLimit),started(active?UiPerfCounter131():0){if(!started)active=false;}
 ~UiPerfScope131(){if(!active)return;const auto ended=UiPerfCounter131();if(ended>=started)state.Record(group,ended-started);}
};
}
