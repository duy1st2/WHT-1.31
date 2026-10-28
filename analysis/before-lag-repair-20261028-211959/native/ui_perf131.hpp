#pragma once
#include "common.hpp"
#include <cstdio>
namespace wht {
struct UiPerf131 {
 struct Bucket {uint64_t calls=0,total=0,maximum=0;double Average()const{return calls?double(total)/calls:0;}};
 std::array<Bucket,3> buckets{};uint64_t due=0;unsigned reports=0;bool stateKnown=false,lastAllowed=false;
 void Record(unsigned group,uint64_t elapsed){if(group>=buckets.size()||reports>=12)return;auto& b=buckets[group];++b.calls;b.total+=elapsed;b.maximum=(std::max)(b.maximum,elapsed);}
 void Report(const std::wstring& folder,bool allowed){
  const auto now=GetTickCount64();if(!stateKnown||lastAllowed!=allowed){stateKnown=true;lastAllowed=allowed;due=now+5000;reports=0;buckets={};return;}
  if(now<due||reports>=12)return;due=now+5000;++reports;
  char text[512]{};std::snprintf(text,sizeof(text),"WHT performance ms (unscaled wall clock): allowed=%u update avg=%.2f max=%llu calls=%llu; graphics avg=%.2f max=%llu calls=%llu; MTH-toolbar avg=%.2f max=%llu calls=%llu",unsigned(allowed),buckets[0].Average(),buckets[0].maximum,buckets[0].calls,buckets[1].Average(),buckets[1].maximum,buckets[1].calls,buckets[2].Average(),buckets[2].maximum,buckets[2].calls);
  buckets={};Log(folder,text);
 }
};
struct UiPerfScope131 {
 UiPerf131& state;unsigned group;uint64_t started;
 UiPerfScope131(UiPerf131& s,unsigned g):state(s),group(g),started(GetTickCount64()){}
 ~UiPerfScope131(){const auto ended=GetTickCount64();if(ended>=started)state.Record(group,ended-started);}
};
}
