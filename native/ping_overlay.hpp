#pragma once
#include "ping_reading.hpp"
#include "tool_menu.hpp"
namespace wht {
struct PingOverlay {
 void* frame=nullptr;void* font=nullptr;void* owner=nullptr;uint64_t due=0;std::string applied;
 void Reset(){*this={};}
 void Update(uintptr_t base,const NativePingProfile& profile,bool enabled,ToolMenuOverlay& menu){
  if(owner!=menu.independentHost){Reset();owner=menu.independentHost;}
  if(!menu.initialized)return;
  if(!enabled){if(frame)NativeFrameHide(frame);return;}
  const ToolMenuRect rect{.555f,.548f,.12f,.016f};
  if(!frame){frame=menu.Frame(base,menu.independentHost,rect);if(frame)font=menu.TextIn(frame,rect,"",false,.012f);}
  if(!frame||!font)return;
  const auto now=GetTickCount64();if(now>=due){due=now+500;float ms=0;char value[48]{};
   if(ReadNativePing(base,profile,ms))std::snprintf(value,sizeof(value),"%.2f",double(ms));else std::snprintf(value,sizeof(value),"--");
   const auto text=std::string("|cffffff00Ping: ")+value+" ms|r";
   if(text!=applied){menu.api.setText(font,text.c_str());applied=text;}
  }
  NativeFrameShow(frame);
 }
};
}
