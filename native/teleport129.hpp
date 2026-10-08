#pragma once
#include "skills129.hpp"
#include <cmath>
#include <cstring>
namespace wht {
struct TeleportApi129 {
 uintptr_t(__cdecl* agent)(uint32_t,uint32_t)=nullptr;
 void(__thiscall* stop)(uintptr_t,int)=nullptr;
 void(__thiscall* move)(uintptr_t,const float*,const float*,int,int,int,int,int,int,int,int)=nullptr;
 uintptr_t(__thiscall* position)(uintptr_t,float*)=nullptr;
 explicit operator bool()const{return agent&&stop&&move&&position;}
};
struct TeleportPosition129 {float x=0,y=0;};
inline bool TeleportFailure129(const char** error,const char* text){if(error)*error=text;return false;}
inline TeleportApi129 ResolveTeleportApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char moveSig[]={0x55,0x8b,0xec,0x83,0xec,0x10,0x53,0x56,0x8b,0x75,0x08,0x57,0x8b,0xf9};
  const unsigned char stopPrefix[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
  const unsigned char stopBody[]={0x64,0xa1,0,0,0,0,0x50,0x51,0x53,0x56,0x57};
  const unsigned char agentSig[]={0x55,0x8b,0xec,0x8b,0x4d,0x08,0x8b,0x15};
  const unsigned char readSig[]={0x55,0x8b,0xec,0x56,0x8b,0x75,0x08};
  if(memcmp(reinterpret_cast<void*>(base+0x26bef0),moveSig,sizeof(moveSig))||
     memcmp(reinterpret_cast<void*>(base+0x290690),stopPrefix,sizeof(stopPrefix))||
     memcmp(reinterpret_cast<void*>(base+0x29069a),stopBody,sizeof(stopBody))||
     memcmp(reinterpret_cast<void*>(base+0x707f0),agentSig,sizeof(agentSig))||
     memcmp(reinterpret_cast<void*>(base+0x3fcc50),readSig,sizeof(readSig))||
     *reinterpret_cast<uintptr_t*>(base+0xa9c504+0x180)!=base+0x26bef0)return {};
  return {reinterpret_cast<decltype(TeleportApi129::agent)>(base+0x707f0),
          reinterpret_cast<decltype(TeleportApi129::stop)>(base+0x290690),
          reinterpret_cast<decltype(TeleportApi129::move)>(base+0x26bef0),
          reinterpret_cast<decltype(TeleportApi129::position)>(base+0x3fcc50)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool LiveTeleportUnit129(uintptr_t base,const SkillSelection129& selected,const TeleportApi129& api){
 if(!base||!api||!selected.unit||!selected.identity)return false;
 if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return false;
 const auto current=SelectedSkill129(base);
 if(current.unit!=selected.unit||current.identity!=selected.identity)return false;
 const auto agent=api.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
 return agent&&*reinterpret_cast<uint32_t*>(agent+0xc)==0x2b61676c&&!*reinterpret_cast<uint32_t*>(agent+0x20);
}
inline bool TeleportUnit129(uintptr_t base,const SkillSelection129& selected,const TeleportApi129& api,
 float x,float y,TeleportPosition129& result,const char** error=nullptr){
 result={};if(error)*error=nullptr;
 if(!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>100000.f||std::abs(y)>100000.f)return TeleportFailure129(error,"Invalid cursor coordinates");
 __try {
  if(!LiveTeleportUnit129(base,selected,api))return TeleportFailure129(error,"Select a live unit first");
  // Match SetUnitPosition ABA00: stop current order, then use its 10 arguments.
  api.stop(selected.unit,1);
  if(!LiveTeleportUnit129(base,selected,api))return TeleportFailure129(error,"Selected unit changed while stopping its order");
  api.move(selected.unit,&x,&y,1,1,-1,0,0,0,-1,0);
  if(!LiveTeleportUnit129(base,selected,api))return TeleportFailure129(error,"Selected unit changed during teleport");
  float xyz[3]{};api.position(selected.unit+0x26c,xyz);
  if(!std::isfinite(xyz[0])||!std::isfinite(xyz[1]))return TeleportFailure129(error,"Cannot read the resulting unit position");
  result={xyz[0],xyz[1]};return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return TeleportFailure129(error,"Native teleport access failed");}
}
inline bool TeleportSelected129(uintptr_t base,float x,float y,TeleportPosition129& result,const char** error=nullptr){
 const auto selected=SelectedSkill129(base);
 if(!selected.unit||!selected.identity)return TeleportFailure129(error,"Select a unit first");
 const auto api=ResolveTeleportApi129(base);
 if(!api)return TeleportFailure129(error,"Teleport function signature mismatch");
 return TeleportUnit129(base,selected,api,x,y,result,error);
}
}
