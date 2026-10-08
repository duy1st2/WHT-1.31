#pragma once
#include "services129.hpp"
#include "cooldown129.hpp"
namespace wht {
inline unsigned LabelKey129(const Config& c,bool inventory,bool shop,int slot,unsigned nativeKey=0){
 if(slot<0||slot>=(inventory?6:12))return 0;
 const auto& binding=inventory?c.inventory[slot]:shop?c.shop[slot]:c.command[slot];
 const bool enabled=inventory?c.inventoryEnabled:shop?c.shopEnabled:c.commandEnabled;
 if(enabled&&binding.enabled&&binding.vk)return binding.vk;
 static constexpr unsigned defaults[]={VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD4,VK_NUMPAD5,VK_NUMPAD1,VK_NUMPAD2};
 return inventory?defaults[slot]:nativeKey;
}
inline std::string LabelName129(unsigned key){
 if(key>= 'A'&&key<= 'Z'||key>= '0'&&key<= '9')return std::string(1,static_cast<char>(key));
 if(key>=VK_F1&&key<=VK_F24)return "F"+std::to_string(key-VK_F1+1);
 if(key>=VK_NUMPAD0&&key<=VK_NUMPAD9)return "N"+std::to_string(key-VK_NUMPAD0);
 switch(key){case VK_SPACE:return "Spc";case VK_TAB:return "Tab";case VK_SHIFT:return "Shift";case VK_CONTROL:return "Ctrl";case VK_MENU:return "Alt";case VK_RETURN:return "Enter";case VK_BACK:return "Back";case VK_ESCAPE:return "Esc";case VK_DELETE:return "Del";case VK_INSERT:return "Ins";case VK_HOME:return "Home";case VK_END:return "End";case VK_PRIOR:return "PgUp";case VK_NEXT:return "PgDn";case VK_LEFT:return "Left";case VK_RIGHT:return "Right";case VK_UP:return "Up";case VK_DOWN:return "Down";}
 char name[64]{};const auto scan=MapVirtualKeyA(key,MAPVK_VK_TO_VSC);
 if(!scan||!GetKeyNameTextA(static_cast<LONG>(scan<<16),name,64))return {};
 return std::string(name).substr(0,4);
}
struct HotkeyLabels129 {
 struct Entry{uintptr_t button=0;std::array<void*,3> fonts{};std::string text;std::array<void*,2> cooldownFonts{};std::string cooldown;};
 std::array<Entry,18> entries{};
 void Reset(){*this={};}
 template<class Menu> void Update(uintptr_t base,const Config& c,bool allowed,Menu& menu,ButtonCooldownFn129 query=nullptr){
  const bool shop=IsShop129(base);
  for(int index=0;index<18;++index){
   const bool inventory=index>=12;const int slot=inventory?index-12:index;
   const auto button=Button129(base,inventory,slot,false);auto& e=entries[index];
   // Parent destruction also destroys its font children; never write old pointers.
   if(!button)continue;
   if(e.button!=button){e={};e.button=button;}
   if(!button)continue;
   const auto data=*reinterpret_cast<uintptr_t*>(button+0x194);
   const auto text=allowed&&c.showHotkeyInPanel&&data&&*reinterpret_cast<unsigned*>(data+8)?LabelName129(LabelKey129(c,inventory,shop,slot,EngineKeyToVirtualKey(*reinterpret_cast<int*>(data+0x5ac)))):std::string{};

   const float offsets[]={.0028f,.0012f,.002f};
   for(int i=0;i<3;++i){
    bool created=false;
    if(!e.fonts[i]&&!text.empty()){
     // Stock command buttons enable layer 2; tool-owned layer 4 is disabled.
     e.fonts[i]=NativeCreateFontString(menu.api,reinterpret_cast<void*>(button),.014f,"",2);
     if(e.fonts[i]){created=true;menu.api.setRelPoint(e.fonts[i],0,reinterpret_cast<void*>(button),0,offsets[i],-offsets[i],1);menu.api.setTextAlign(e.fonts[i],0);}
    }
    if(e.fonts[i]&&(created||text!=e.text)){const auto colored=text.empty()?std::string{}:std::string(i==2?"|cffffcc00":"|cff000000")+text+"|r";menu.api.setText(e.fonts[i],colored.c_str());}
   }
   e.text=text;
   const auto cooldown=allowed&&c.showCooldown&&data?CooldownText129(ButtonCooldown129(base,button,query)):std::string{};
   for(int i=0;i<2;++i){
    bool created=false;
    if(!e.cooldownFonts[i]&&!cooldown.empty()){
     e.cooldownFonts[i]=NativeCreateFontString(menu.api,reinterpret_cast<void*>(button),.020f,"",2);
     if(e.cooldownFonts[i]){created=true;const float offset=i==0?.001f:0.f;menu.api.setRelPoint(e.cooldownFonts[i],4,reinterpret_cast<void*>(button),4,offset,-offset,1);menu.api.setTextAlign(e.cooldownFonts[i],1);}
    }
    if(e.cooldownFonts[i]&&(created||e.cooldown!=cooldown)){
     const auto colored=cooldown.empty()?std::string{}:std::string(i==0?"|cff000000":"|cffffffff")+cooldown+"|r";
     menu.api.setText(e.cooldownFonts[i],colored.c_str());
    }
   }
   e.cooldown=cooldown;
  }
 }
};
}
