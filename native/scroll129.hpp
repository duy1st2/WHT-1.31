#pragma once
#include "common.hpp"
#include <cmath>
namespace wht {
using ScrollSetter129=void(__thiscall*)(uintptr_t,float);
struct Scroll129 {
 uintptr_t owner=0;float saved=0,written=0;bool active=false;
 void Reset(){*this={};}
 bool Apply(uintptr_t camera,unsigned field,int percent,bool enabled,ScrollSetter129 set){
  if(!camera){Reset();return true;}if(!set||percent<0||percent>700)return false;
  __try{
   if(owner!=camera)Reset();
   const float current=*reinterpret_cast<float*>(camera+field);
   if(!std::isfinite(current)||current<0||current>10000)return false;
   if(enabled){
    if(!active||current!=written)saved=current;
    owner=camera;
    // Raw 1.29 setters receive speed directly; the legacy setter subtracts .5.
    // Both native profiles treat zero as the default speed (1).
    const float value=percent?percent/100.f:1.f;
    if(current!=value)set(camera,value);
    written=*reinterpret_cast<float*>(camera+field);active=true;
   }else if(active){
    if(current==written&&current!=saved)set(camera,saved);
    Reset();
   }
   return true;
  }__except(EXCEPTION_EXECUTE_HANDLER){Reset();return false;}
 }
};
}
