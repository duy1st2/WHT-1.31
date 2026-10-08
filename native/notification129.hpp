#pragma once
#include "notification_text.hpp"
#include "common.hpp"
namespace wht {
using LocalMessage129=void(__thiscall*)(uintptr_t,float,float,const char*,float,unsigned);
inline bool DisplayNotification129(uintptr_t base,const char* colored,LocalMessage129 display){
 __try {
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return false;
  if(!*reinterpret_cast<uintptr_t*>(ui+0x424)||!*reinterpret_cast<uintptr_t*>(ui+0x428))return false;
  display(ui,0.f,0.f,colored,4.f,0xffffffffu);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ShowNotification129(uintptr_t base,const std::string& text,LocalMessage129 display){
 if(!base||!display||text.empty()||text.size()>1024||text.find('\0')!=std::string::npos)return false;
 const auto colored=NotificationText(text);
 return DisplayNotification129(base,colored.c_str(),display);
}
}
