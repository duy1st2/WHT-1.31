#pragma once
#include "cooldown131.hpp"
#include "overhead_cadence.hpp"
namespace wht {
struct RegenProfile131{unsigned build;uintptr_t portrait,unit,floatType,resolve;};
inline constexpr RegenProfile131 regenProfiles131[]={
 {12164,0x182c178,0x182eeb0,0x173186c,0x2fb0d0},
 {12173,0x17e1058,0x17e3d90,0x16e68c4,0x2dfc60}
};
inline uintptr_t SelectedUnit131(uintptr_t ui,uintptr_t base,const RegenProfile131& p){
 const auto portrait=Component131(ui,base,p.portrait);if(!portrait)return 0;
 __try{const auto unit=*reinterpret_cast<uintptr_t*>(portrait+0x244);return Identity131(unit,base,p.unit)?unit:0;}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool ReadRegen131(uintptr_t unit,uintptr_t base,const RegenProfile131& p,bool mana,float& value){
 if(!Identity131(unit,base,p.unit))return false;
 __try{
  // Verified CUnit Float members: native life operations use +1A4,
  // native mana operations use +1CC. Resolve their tags like GetFloat.
  const auto property=unit+(mana?0x1cc:0x1a4);if(!Identity131(property,base,p.floatType))return false;
  const auto low=*reinterpret_cast<uint32_t*>(property+8),high=*reinterpret_cast<uint32_t*>(property+12);
  const auto state=reinterpret_cast<uintptr_t(__cdecl*)(uint32_t,uint32_t)>(base+p.resolve)(low,high);if(!state)return false;
  value=*reinterpret_cast<float*>(state+0x7c);
  return std::isfinite(value)&&value>-10000&&value<100000;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string RegenText131(bool mana,float value){
 if(!std::isfinite(value)||value<=-10000||value>=100000)return {};
 char text[96]{};std::snprintf(text,sizeof(text),"|cfff9d112%s/s:|r %s%s%.2f|r",mana?"MP":"HP",value<0?"|cffff0303":mana?"|cffc3dbff":"|cff00ff00",value<0?"":"+",value);return text;
}
struct RegenOverlay131{
 ButtonUpdateCadence cadence;
 std::array<void*,2> frames{},fonts{};std::array<std::string,2> text{};bool reported=false;
 void Reset(){cadence={};frames={};fonts={};text={};reported=false;}
 template<class Menu> bool Update(uintptr_t base,uintptr_t ui,const RegenProfile131& p,const Config& config,bool allowed,Menu& menu){
  const auto unit=SelectedUnit131(ui,base,p);bool observed=false;
  if(!cadence.Due(GetTickCount64(),ui,unit,nullptr,(allowed&&config.showRegenHP?1u:0u)|(allowed&&config.showRegenMana?2u:0u)))return false;
  for(unsigned n=0;n<2;++n){float value=0;const bool enabled=allowed&&(n?config.showRegenMana:config.showRegenHP)&&ReadRegen131(unit,base,p,n!=0,value)&&value!=0;
   // Align the rows' left edge with Strength/Agility/Intelligence.
   // Keep the lower position and smaller font below the Intelligence value.
   const ToolMenuRect rect{.510f,n?0.f:.009f,.08f,.009f};
   if(enabled&&!frames[n]){frames[n]=menu.Frame(base,menu.independentHost,rect);if(frames[n])fonts[n]=menu.TextIn(frames[n],rect,"",false,.009f);}
   if(!frames[n]||!fonts[n])continue;
   if(!enabled){NativeFrameHide(frames[n]);continue;}
   const auto label=RegenText131(n!=0,value);if(text[n]!=label){menu.api.setText(fonts[n],label.c_str());text[n]=label;}
   NativeFrameShow(frames[n]);observed=true;
  }
  return observed;
 }
};
}
