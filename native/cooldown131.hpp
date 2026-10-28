#pragma once
#include "runtime131.hpp"
#include "component_lookup131.hpp"
#include "overhead_cadence.hpp"
#include <cstdio>
namespace wht {
struct CooldownProfile131 {unsigned build;uintptr_t button,commandBar,infoBar,inventoryBar,abilityGetter,spellRemaining;};
inline constexpr CooldownProfile131 cooldownProfiles131[]={
 {12164,0x182a764,0x18273e8,0x1827518,0x18276a8,0xb6d2a0,0x3a1d80},
 {12173,0x17df644,0x17dc2c8,0x17dc3f8,0x17dc588,0xb2cfa0,0x380290}
};
inline bool Identity131(uintptr_t value,uintptr_t base,uintptr_t table){__try{return value&&NativeClass131(value,*reinterpret_cast<uintptr_t*>(value),base+table);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
inline uintptr_t Component131(uintptr_t ui,uintptr_t base,uintptr_t table){return FindComponent131(ui,base,table,0x4d4);}
inline uintptr_t CooldownButton131(uintptr_t ui,uintptr_t base,const CooldownProfile131& p,unsigned slot){
 if(slot>=18)return 0;
 __try{
  uintptr_t button=0;
  if(slot<12){const auto bar=Component131(ui,base,p.commandBar);if(!bar)return 0;
   if(*reinterpret_cast<unsigned*>(bar+0x148)!=3)return 0;
   const auto rows=*reinterpret_cast<uintptr_t*>(bar+0x14c);if(!rows)return 0;
   const auto row=*reinterpret_cast<uintptr_t*>(rows+(slot/4)*0x10+4);if(!row)return 0;
   button=*reinterpret_cast<uintptr_t*>(row+(slot%4)*4);
  }else{const auto info=Component131(ui,base,p.infoBar);if(!info)return 0;
   const auto bar=*reinterpret_cast<uintptr_t*>(info+0x144);if(!Identity131(bar,base,p.inventoryBar))return 0;
   if(*reinterpret_cast<unsigned*>(bar+0x124)!=6)return 0;
   const auto list=*reinterpret_cast<uintptr_t*>(bar+0x128);if(!list)return 0;
   button=*reinterpret_cast<uintptr_t*>(list+(slot-12)*8+4);
  }
  return Identity131(button,base,p.button)?button:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline float ButtonCooldown131(uintptr_t button,uintptr_t base,const CooldownProfile131& p){
 if(!Identity131(button,base,p.button))return 0;
 __try{
  const auto data=*reinterpret_cast<uintptr_t*>(button+0x190);if(!data)return 0;
  const auto ability=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(base+p.abilityGetter)(data);if(!ability)return 0;
  auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+dos->e_lfanew);const auto end=base+nt->OptionalHeader.SizeOfImage;
  const auto vt=*reinterpret_cast<uintptr_t*>(ability);if(vt<base+4||vt+0x2e4>=end)return 0;
  const auto col=*reinterpret_cast<uintptr_t*>(vt-4);if(col<base||col+20>=end||*reinterpret_cast<unsigned*>(col)!=0)return 0;
  const auto type=*reinterpret_cast<uintptr_t*>(col+12);if(type<base||type+20>=end||memcmp(reinterpret_cast<void*>(type+8),".?AVCAbility",12))return 0;
  const auto getter=*reinterpret_cast<uintptr_t*>(vt+0x2e0);if(getter<base||getter>=end)return 0;
  // The native button query uses CAbility's paired virtual slots +2E0
  // (seconds) / +2E4 (normalized progress), both (float*, order), ret 8.
  float seconds=0;const auto order=*reinterpret_cast<unsigned*>(data+8);
  reinterpret_cast<float*(__thiscall*)(uintptr_t,float*,unsigned)>(getter)(ability,&seconds,order);
  return std::isfinite(seconds)&&seconds>0&&seconds<86400?seconds:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline std::string CooldownText131(float value){
 if(!std::isfinite(value)||value<=0||value>=86400)return {};
 char text[32]{};const auto seconds=static_cast<int>(std::ceil(value));
 if(seconds>=60)std::snprintf(text,sizeof(text),"%d:%02d",seconds/60,seconds%60);
 else if(value<1)std::snprintf(text,sizeof(text),"%.1f",value);
 else std::snprintf(text,sizeof(text),"%d",seconds);
 return text;
}
struct CooldownOverlay131 {
 struct Entry{uintptr_t button=0;std::array<void*,2> fonts{};std::string text;};
 std::array<Entry,18> entries{};ButtonUpdateCadence cadence;bool reported=false;
 void Reset(){entries={};cadence={};reported=false;}
 template<class Menu> bool Update(uintptr_t base,uintptr_t ui,const CooldownProfile131& profile,bool enabled,Menu& menu,uintptr_t selected=0){
  if(!cadence.Due(GetTickCount64(),ui,selected,&profile,enabled?1u:0u))return false;
  bool observed=false;
  for(unsigned slot=0;slot<18;++slot){auto& e=entries[slot];const auto button=enabled?CooldownButton131(ui,base,profile,slot):uintptr_t(0);
   if(!button){if(!e.text.empty())for(auto font:e.fonts)if(font)menu.api.setText(font,"");e.button=0;e.text.clear();continue;}
   const auto value=enabled?ButtonCooldown131(button,base,profile):0;const auto text=CooldownText131(value);observed|=value>0;
   for(unsigned layer=0;layer<2;++layer){bool created=false;
    if(!e.fonts[layer]&&!text.empty()){e.fonts[layer]=NativeCreateFontString(menu.api,menu.independentHost,.016f,"",4);created=e.fonts[layer]!=nullptr;
     // Both the text box and its glyphs must be centred on the native icon.
     // Left alignment put the digit at the box's left edge, outside the button.
     if(e.fonts[layer]){menu.api.setWidth(e.fonts[layer],.030f);menu.api.setHeight(e.fonts[layer],.024f);menu.api.setTextAlign(e.fonts[layer],2);}}
    if(e.fonts[layer]&&(created||e.button!=button)){const float offset=layer?0:.001f;menu.api.setRelPoint(e.fonts[layer],4,reinterpret_cast<void*>(button),4,offset,-offset,1);}
    if(e.fonts[layer]&&(created||e.text!=text)){const auto colored=text.empty()?std::string{}:std::string(layer?"|cffffffff":"|cff000000")+text+"|r";menu.api.setText(e.fonts[layer],colored.c_str());}
   }
   e.button=button;e.text=text;
  }
  return observed;
 }
};
}
