#pragma once
#include "common.hpp"
#include <cmath>
#include <cstring>
namespace wht {
using FogEffectFn129=void(__thiscall*)(uintptr_t,uint32_t,float,float,float,const uint32_t*);
using FogParameterRead129=void(__cdecl*)(uintptr_t,unsigned,unsigned,void*);
struct FogEffect129 {
 uintptr_t scene=0,parameters=0,table=0;std::array<uintptr_t,5> fields{};
 unsigned style=0;float start=0,end=0,density=0;uint32_t color=0;bool observed=false,active=false;
 void Reset(){*this={};}
 bool Valid()const{
  __try{
   if(!observed||!scene||*reinterpret_cast<uintptr_t*>(scene+0x3e4)!=parameters||!parameters||*reinterpret_cast<unsigned*>(parameters+8)<5||*reinterpret_cast<uintptr_t*>(parameters+0xc)!=table||!table)return false;
   for(unsigned n=0;n<5;++n)if(!fields[n]||*reinterpret_cast<uintptr_t*>(table+n*4)!=fields[n])return false;return true;
  }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 bool Capture(uintptr_t object,unsigned mode,float nearFog,float farFog,float amount,const uint32_t* tint){
  Reset();if(!object||!tint||!std::isfinite(nearFog)||!std::isfinite(farFog)||!std::isfinite(amount))return false;
  __try{
   scene=object;parameters=*reinterpret_cast<uintptr_t*>(scene+0x3e4);if(!parameters)return false;
   table=*reinterpret_cast<uintptr_t*>(parameters+0xc);if(!table)return false;
   for(unsigned n=0;n<5;++n)fields[n]=*reinterpret_cast<uintptr_t*>(table+n*4);
   style=mode;start=nearFog;end=farFog;density=amount;color=*tint;observed=true;
  }__except(EXCEPTION_EXECUTE_HANDLER){Reset();return false;}
  if(!Valid()){Reset();return false;}return true;
 }
 bool ReadCurrent(uintptr_t object,FogParameterRead129 read){
  uint32_t initialColor=0;if(!read||!Capture(object,0,0,0,0,&initialColor))return false;
  __try{
   const unsigned types[]={1,5,6,6,6};
   for(unsigned n=0;n<5;++n)if(*reinterpret_cast<unsigned char*>(fields[n]+0xc)!=types[n]||(*reinterpret_cast<unsigned char*>(fields[n]+0xd)&2)){Reset();return false;}
   // Generic getter 56C440 evaluates a parameter and copies three words.
   std::array<std::array<uint32_t,3>,5> values{};
   for(unsigned n=0;n<5;++n)read(parameters,n,types[n],values[n].data());
   color=values[0][0];style=values[1][0];
   std::memcpy(&density,&values[2][0],4);std::memcpy(&start,&values[3][0],4);std::memcpy(&end,&values[4][0],4);
   if(style>3||!std::isfinite(density)||!std::isfinite(start)||!std::isfinite(end)){Reset();return false;}
  }__except(EXCEPTION_EXECUTE_HANDLER){Reset();return false;}
  return Valid();
 }
 template<class Set> bool Apply(uintptr_t currentScene,bool remove,Set set){
  if(!currentScene)return true;if(scene!=currentScene||!Valid()){Reset();return !remove;}
  if(active==remove)return true;
  set(scene,remove?1:style,remove?0.f:start,remove?0.f:end,remove?0.f:density,&color);active=remove;return true;
 }
};
}
