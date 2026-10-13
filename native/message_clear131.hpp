#pragma once
#include "common.hpp"
#include "periodic_action.hpp"
namespace wht {
struct MessageProfile131{unsigned build;uintptr_t table,clear,textOffset,chatOffset;};
#ifdef _WIN64
inline constexpr MessageProfile131 messageProfiles131[]={
 {12164,0x1c5a610,0xfc9690,0x6f8,0x708},
 {12173,0x1c89520,0x10060b0,0x708,0x718}};
#else
inline constexpr MessageProfile131 messageProfiles131[]={
 {12164,0x1854b88,0xe0cfe0,0x42c,0x434},
 {12173,0x1809aa8,0xdcbe10,0x450,0x458}};
#endif
inline const MessageProfile131* MessageProfileFor131(unsigned build){for(const auto& p:messageProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool MessageCode131(uintptr_t base,const MessageProfile131& p){
 __try{
#ifdef _WIN64
  static constexpr unsigned char prefix[]={0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x57,0x48,0x83,0xec,0x20};
#else
  static constexpr unsigned char prefix[]={0x55,0x8b,0xec,0x51,0x53,0x56,0x8b,0xf1,0x33,0xdb};
#endif
  return !memcmp(reinterpret_cast<void*>(base+p.clear),prefix,sizeof(prefix));
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline uintptr_t MessageFrame131(uintptr_t base,uintptr_t ui,const MessageProfile131& p,bool chat){
 __try{if(!base||!ui)return 0;auto frame=*reinterpret_cast<uintptr_t*>(ui+(chat?p.chatOffset:p.textOffset));
  if(!frame||*reinterpret_cast<uintptr_t*>(frame)!=base+p.table)return 0;
  auto count=*reinterpret_cast<unsigned*>(frame+(sizeof(void*)==8?0x1d0:0x120));
  auto active=*reinterpret_cast<unsigned*>(frame+(sizeof(void*)==8?0x1d4:0x124));
  if(count>4096||active>count||(count&&!*reinterpret_cast<uintptr_t*>(frame+(sizeof(void*)==8?0x250:0x184))))return 0;
  return frame;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool ClearMessageFrame131(uintptr_t base,uintptr_t frame,const MessageProfile131& p){
 if(!frame||!MessageCode131(base,p))return false;
 __try{reinterpret_cast<void(__thiscall*)(uintptr_t)>(base+p.clear)(frame);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct MessageTimers131{
 std::array<PeriodicAction,2> timers{};unsigned reported=0;
 void Reset(){timers={};reported=0;}
 template<class Clear> unsigned Tick(uintptr_t text,uintptr_t chat,const Config& c,bool allowed,uint64_t now,Clear clear){
  unsigned result=0;
  if(timers[0].Poll(text,allowed&&c.clearTextEnabled,c.clearTextSeconds,now)&&clear(text))result|=1;
  if(timers[1].Poll(chat,allowed&&c.clearChatEnabled,c.clearChatSeconds,now)&&clear(chat))result|=2;
  return result;
 }
 unsigned Update(uintptr_t base,uintptr_t ui,unsigned build,const Config& c,bool allowed,uint64_t now){
  auto p=MessageProfileFor131(build);if(!p||!MessageCode131(base,*p)){timers={};return 0;}
  return Tick(MessageFrame131(base,ui,*p,false),MessageFrame131(base,ui,*p,true),c,allowed,now,[&](uintptr_t frame){return ClearMessageFrame131(base,frame,*p);});
 }
};
}
