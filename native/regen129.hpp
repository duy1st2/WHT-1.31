#pragma once
#include "skills129.hpp"
#include <cmath>
#include <cstdio>
namespace wht {
using RegenResolve129=uintptr_t(__cdecl*)(uint32_t,uint32_t);
// Native 400D80 resolves stat tags through cdecl 707F0, then uses +7C
// as the regeneration rate. Life property: +1A0; mana property: +1C0.
inline bool ReadRegen129(uintptr_t base,uintptr_t unit,bool mana,RegenResolve129 resolve,float& value){
 __try{
  if(!unit||!resolve||*reinterpret_cast<uintptr_t*>(unit)!=base+0xa9c504)return false;
  const auto property=unit+(mana?0x1c0:0x1a0);
  const auto low=*reinterpret_cast<uint32_t*>(property+8),high=*reinterpret_cast<uint32_t*>(property+12);
  const auto state=resolve(low,high);if(!state)return false;
  value=*reinterpret_cast<float*>(state+0x7c);
  return std::isfinite(value)&&value>-10000.f&&value<100000.f;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string RegenText129(bool mana,float value){
 char text[128]{};std::snprintf(text,sizeof(text),"|CFFF9D112%s/s:|r %s%s%.2f|r",mana?"MP":"HP",value<0?"|CFFFF0303":mana?"|CFFC3DBFF":"|CFF00FF00",value<0?"":"+",value);return text;
}
struct Regen129 {
 std::array<void*,2> frames{},fonts{};
 void Reset(){frames={};fonts={};}
 template<class Menu> void Update(uintptr_t base,const Config& config,bool allowed,Menu& menu){
  if(!menu.initialized)return;
  const auto selection=SelectedSkill129(base);
  for(unsigned n=0;n<2;++n){
   float value=0;const bool enabled=allowed&&!config.hideConsoleUI&&(n?config.showRegenMana:config.showRegenHP)&&ReadRegen129(base,selection.unit,n!=0,reinterpret_cast<RegenResolve129>(base+0x707f0),value)&&value!=0;
   const ToolMenuRect rect{.519f,n?.001f:.010f,.08f,.012f};
   if(enabled&&!frames[n]){frames[n]=menu.Frame(base,menu.independentHost,rect);if(frames[n])fonts[n]=menu.TextIn(frames[n],rect,"",false,.010f);}
   if(!frames[n]||!fonts[n])continue;
   if(!enabled){NativeFrameHide(frames[n]);continue;}
   const auto text=RegenText129(n!=0,value);menu.api.setText(fonts[n],text.c_str());NativeFrameShow(frames[n]);
  }
 }
};
}

