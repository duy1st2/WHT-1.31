#pragma once
#include "services129.hpp"
#include <cstdio>
#include <cstring>
namespace wht {
using ButtonCooldownFn129=float(__thiscall*)(uintptr_t);
using TimerRemainingFn129=float*(__thiscall*)(uintptr_t,float*);
inline float TimerCooldown129(uintptr_t base,uintptr_t timer,TimerRemainingFn129 supplied=nullptr){
 __try {
  const auto vt=*reinterpret_cast<uintptr_t*>(timer);
  uintptr_t expected=vt==base+0xa6e238?base+0x3fc070:vt==base+0xa7baf8?base+0x3fc0b0:0;
  if(!expected||!*reinterpret_cast<uintptr_t*>(timer+0xc))return 0;
  const auto fn=supplied?supplied:*reinterpret_cast<TimerRemainingFn129*>(vt+0x18);
  if(!supplied&&reinterpret_cast<uintptr_t>(fn)!=expected)return 0;
  float value=0;fn(timer,&value);
  return std::isfinite(value)&&value>0&&value<86400?value:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline float BoundCooldown129(uintptr_t base,uintptr_t button,TimerRemainingFn129 supplied=nullptr){
 if(!base||!button)return 0;
 __try {
  if(*reinterpret_cast<uintptr_t*>(button)!=base+0xa9528c)return 0;
  const auto data=*reinterpret_cast<uintptr_t*>(button+0x194);if(!data)return 0;
  const auto ability=*reinterpret_cast<uintptr_t*>(data+0x6d4);if(!ability)return 0;
  const auto vt=*reinterpret_cast<uintptr_t*>(ability);
  if(vt==base+0xb1b5e0||vt==base+0xb1bc88){
   // SellUnit/SellItem query 7343C0: order array D0, waiting flags 9C,
   // initial timer 31C and replenishment timer 1C8, stride 1C.
   if(*reinterpret_cast<unsigned*>(ability+0xcc)!=12||*reinterpret_cast<unsigned*>(ability+0x100)!=12||*reinterpret_cast<unsigned*>(ability+0x1c4)!=12||*reinterpret_cast<unsigned*>(ability+0x318)!=12)return 0;
   const auto order=*reinterpret_cast<unsigned*>(data+8);if(!order)return 0;
   for(int slot=11;slot>=0;--slot)if(*reinterpret_cast<unsigned*>(ability+0xd0+slot*4)==order){
    if((*reinterpret_cast<unsigned*>(ability+0x9c+slot*4)&3)!=1)return 0;
    const float initial=TimerCooldown129(base,ability+0x31c+slot*0x1c,supplied);
    if(initial>0)return initial;
    const float count=*reinterpret_cast<float*>(ability+0x108+slot*0x10);
    if(!std::isfinite(count)||count<0||count>=.001f)return 0;
    return TimerCooldown129(base,ability+0x1c8+slot*0x1c,supplied);
   }
   return 0;
  }
  // Only CAbilitySpell's cooldown progress implementation owns this layout.
  // The query at 74A270 uses the timer D4 after checking active flags 20.
  if(*reinterpret_cast<uintptr_t*>(vt+0x2e0)!=base+0x74a270||(*reinterpret_cast<unsigned*>(ability+0x20)&0x600)!=0x200)return 0;
  return TimerCooldown129(base,ability+0xd4,supplied);
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline float ButtonCooldown129(uintptr_t base,uintptr_t button,ButtonCooldownFn129 supplied=nullptr){
 if(!supplied)return BoundCooldown129(base,button);
 __try {
  if(!base||!button||*reinterpret_cast<uintptr_t*>(button)!=base+0xa9528c||!*reinterpret_cast<uintptr_t*>(button+0x194))return 0;
  const float value=supplied(button);
  return std::isfinite(value)&&value>0&&value<86400?value:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline std::string CooldownText129(float value){
 if(!std::isfinite(value)||value<=0||value>=86400)return {};
 char buffer[32]{};const int seconds=static_cast<int>(std::ceil(value));
 if(seconds>=60)std::snprintf(buffer,sizeof(buffer),"%d:%02d",seconds/60,seconds%60);
 else if(value<1)std::snprintf(buffer,sizeof(buffer),"%.1f",value);
 else std::snprintf(buffer,sizeof(buffer),"%d",seconds);
 return buffer;
}
}
