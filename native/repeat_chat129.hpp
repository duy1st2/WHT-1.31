#pragma once
#include <deque>
#include <string>
#include <cstdint>
namespace wht {
struct RepeatChat129 {
 int slot=-1;bool all=false;uint64_t due=0;std::deque<std::string> pending;
 void Reset(){slot=-1;all=false;due=0;pending.clear();}
 bool Set(unsigned target,unsigned local){if(target<1||target>24||local>=24||target-1==local)return false;Reset();slot=int(target)-1;return true;}
 void ToggleAll(){const bool next=!all;Reset();all=next;}
 void Observe(unsigned sender,unsigned local,const char* text){
  if(sender>=24||sender==local||!text||!*text||*text=='#'||(!all&&slot!=int(sender))||pending.size()>=64)return;
  unsigned n=0;while(n<129&&text[n])++n;if(n>128)return;
  pending.emplace_back(text,n);
 }
 template<class Send> void Tick(bool allowed,uint64_t now,Send send){
  if(!allowed||pending.empty()||now<due)return;
  if(send(pending.front())){pending.pop_front();due=now+250;}
 }
};
}
