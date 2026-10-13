#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
namespace wht {
inline std::string PanelKeyName131(unsigned key){
 if((key>='A'&&key<='Z')||(key>='0'&&key<='9'))return std::string(1,char(key));
 if(key>=VK_NUMPAD0&&key<=VK_NUMPAD9)return "N"+std::to_string(key-VK_NUMPAD0);
 if(key>=VK_F1&&key<=VK_F24)return "F"+std::to_string(key-VK_F1+1);
 switch(key){case VK_ESCAPE:return "Esc";case VK_SPACE:return "Spc";case VK_TAB:return "Tab";case VK_RETURN:return "Enter";case VK_BACK:return "Back";case VK_DELETE:return "Del";default:break;}
 return key?MenuKeyName(std::to_wstring(MenuEngineKey(key))):std::string{};
}
inline unsigned PanelButtonKey131(uintptr_t button,unsigned slot){
 __try{
  if(!button||slot>=18||!*reinterpret_cast<uint8_t*>(button+(sizeof(void*)==8?0xf1:0x9d)))return {};
  auto data=*reinterpret_cast<uintptr_t*>(button+(sizeof(void*)==8?0x270:0x190));
  if(!data||!*reinterpret_cast<unsigned*>(data+8))return {};
  // Verified native key-event comparison and setter in both 1.31 builds.
  int code=*reinterpret_cast<int*>(data+(sizeof(void*)==8?0x2c50:0x2c4c));
  static constexpr unsigned inventoryKeys[]={VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD4,VK_NUMPAD5,VK_NUMPAD1,VK_NUMPAD2};
  return slot>=12?inventoryKeys[slot-12]:EngineKeyToVirtualKey(code);
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
struct HotkeyLabels131 {
 struct Entry{uintptr_t button=0;std::array<void*,3> fonts{};std::string text;};
 std::array<Entry,18> entries{};bool reported=false;
 void Reset(){entries={};reported=false;}
 template<class GetButton> bool Update(bool enabled,ToolMenuOverlay& menu,GetButton getButton){
  bool observed=false;
  for(unsigned slot=0;slot<18;++slot){auto& e=entries[slot];auto button=getButton(slot);
   auto text=enabled?PanelKeyName131(PanelButtonKey131(button,slot)):std::string{};
   for(unsigned n=0;n<3;++n){bool created=false;
    if(!e.fonts[n]&&!text.empty()){e.fonts[n]=NativeCreateFontString(menu.api,menu.independentHost,.012f,"",4);created=e.fonts[n]!=nullptr;
     if(created){menu.api.setWidth(e.fonts[n],.032f);menu.api.setHeight(e.fonts[n],.014f);menu.api.setTextAlign(e.fonts[n],1);}}
    if(e.fonts[n]&&button&&(created||e.button!=button)){float offset=n==0?.0028f:n==1?.0012f:.002f;menu.api.setRelPoint(e.fonts[n],0,reinterpret_cast<void*>(button),0,offset,-offset,1);}
    if(e.fonts[n]&&(created||e.text!=text)){auto label=text.empty()?std::string{}:std::string(n==2?"|cffffcc00":"|cff000000")+text+"|r";menu.api.setText(e.fonts[n],label.c_str());}
    observed|=n==2&&e.fonts[n]&&!text.empty();
   }
   e.button=button;e.text=text;
  }
  return observed;
 }
};
}
