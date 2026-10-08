#pragma once
#include "native_ui129.hpp"
namespace wht {
using HeroLevelObject129=int(__thiscall*)(uintptr_t);
inline int OverheadHeroLevel129(uintptr_t base,uintptr_t bar,HeroLevelObject129 query=nullptr){
 __try {
  // World HP bars are CStatBar kind 5; portrait/XP/mana bars are excluded.
  if(!bar||*reinterpret_cast<uintptr_t*>(bar)!=base+0xa95104||*reinterpret_cast<unsigned*>(bar+0x13c)!=5)return 0;
  const auto unit=*reinterpret_cast<uintptr_t*>(bar+0x158);
  if(!unit)return 0;
  const auto code=*reinterpret_cast<unsigned char*>(unit+0x37);
  if(code<'A'||code>'Z'||(*reinterpret_cast<unsigned*>(unit+0x164)&0x40000000))return 0;
  // Same object getter called by native GetHeroLevel at 92967..9296B.
  const int level=(query?query:reinterpret_cast<HeroLevelObject129>(base+0x291920))(unit);
  return level>0?std::min(level,9999):0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
struct ShowLevel129 {
 struct Entry{std::array<void*,2> fonts{};void* background=nullptr;std::string text;};
 float(__thiscall* textWidth)(void*)=nullptr;
 void(__thiscall* backgroundColor)(void*,const uint32_t*)=nullptr;
 std::unordered_map<uintptr_t,Entry> entries;
 bool enabled=false;
 void Forget(uintptr_t bar){entries.erase(bar);} // Native parent owns/destroys fonts.
 template<class Api> void Write(uintptr_t bar,int level,Api& api){
  const auto text=enabled&&level>0?std::to_string(std::min(level,9999)):std::string{};
  auto found=entries.find(bar);
  if(found==entries.end()){
   if(text.empty()||entries.size()>=4096)return;
   found=entries.emplace(bar,Entry{}).first;
  }
  auto& e=found->second;
  if(!e.background&&!text.empty()&&api.textureCtor&&textWidth&&backgroundColor){
   auto memory=api.alloc(nativeFrameLayout.textureSize,"WHTLevelBackground",1,0);
   if(memory)e.background=api.textureCtor(memory,reinterpret_cast<void*>(bar),2,1);
  }
  for(unsigned n=0;n<2;++n){
   bool created=false;
   if(!e.fonts[n]&&!text.empty()){
    e.fonts[n]=NativeCreateFontString(api,reinterpret_cast<void*>(bar),.010546875f,"",2);
    if(e.fonts[n]){
     created=true;api.setFont(e.fonts[n],"FRIZQT__.ttf",.010546875f,1);
     api.setTextAlign(e.fonts[n],2);
     // Raw native anchor: overhead coordinates must not enter widescreen UI cache.
     api.setRelPoint(e.fonts[n],8,reinterpret_cast<void*>(bar),6,-.0025f+(n?0.f:.0006f),-.005f-(n?0.f:.0006f),1);
    }
   }
   if(e.fonts[n]&&(created||text!=e.text)){
    const auto value=text.empty()?std::string{}:std::string(n?"|cffffffff":"|cff000000")+text+"|r";
    api.setText(e.fonts[n],value.c_str());
   }
  }
  if(e.background&&backgroundColor){
   float width=e.fonts[1]&&!text.empty()&&textWidth?textWidth(e.fonts[1]):0.f;
   const bool visible=!text.empty()&&std::isfinite(width)&&width>0.f&&width<.8f;
   const uint32_t color=visible?0xc0000000:0;
   backgroundColor(e.background,&color);
   if(visible){
    api.setWidth(e.background,width+.002f);api.setHeight(e.background,.012546875f);
    api.setRelPoint(e.background,4,e.fonts[1],4,0.f,0.f,1);
   }
  }
  e.text=text;
 }
 template<class Api> void Apply(bool value,Api& api){
  enabled=value;
  if(!value)for(auto& pair:entries)Write(pair.first,0,api);
 }
};
}
