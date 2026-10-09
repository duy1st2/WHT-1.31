#pragma once
#include <string>
#include <sstream>
#include <cstring>
namespace wht {
inline bool ParseGameSpeed129(const std::wstring& args,unsigned& speed){
 std::wistringstream in(args);std::wstring token,extra;speed=1;
 if(!(in>>token))return true;if(in>>extra)return false;
 if(token==L"0"||token==L"slow")speed=0;
 else if(token==L"1"||token==L"normal")speed=1;
 else if(token==L"2"||token==L"fast")speed=2;
 else return false;return true;
}
using GameSpeedSet129=void(__thiscall*)(uintptr_t,unsigned);
inline GameSpeedSet129 ResolveGameSpeed129(uintptr_t base){
 if(!base)return nullptr;
 __try {
  const unsigned char sig[]={0x55,0x8b,0xec,0x8a,0x45,0x08,0x3c,0x02,0x0f,0xb6,0xd0};
  if(memcmp((void*)(base+0xc2450),sig,sizeof(sig)))return nullptr;
  return reinterpret_cast<GameSpeedSet129>(base+0xc2450);
 }__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
inline uintptr_t GameSpeedContext129(uintptr_t base){
 __try {
  if(!base)return 0;const auto state=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);
  return state?*reinterpret_cast<uintptr_t*>(state+0x30):0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool SetGameSpeed129(uintptr_t base,unsigned requested,GameSpeedSet129 set,unsigned& actual){
 actual=0;
 __try {
  if(requested>2||!set)return false;
  const auto context=GameSpeedContext129(base);if(!context)return false;
  // Respect the map's game-speed lock, as SetGameSpeed does.
  if(*reinterpret_cast<unsigned*>(context+0x38)&0x4000)return false;
  set(context,requested);
  if(GameSpeedContext129(base)!=context)return false;
  actual=*reinterpret_cast<unsigned char*>(context+0x47);return actual==requested;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline const char* GameSpeedName129(unsigned value){return value==0?"Slow":value==1?"Normal":value==2?"Fast":"Unknown";}
}
