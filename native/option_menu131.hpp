#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
namespace wht {
inline bool OptionMenuHit131(POINT point,int width,int height){if(width<=0||height<=0)return false;const int classic=(std::min)(width,height*4/3);POINT centered{point.x-(width-classic)/2,point.y};return ToolMenuRect{.178f,.576f,.09f,.024f}.Hit(centered,classic,height);}
struct OptionMenu131 {
 bool f10=false;unsigned mouse=0;
 void Reset(){f10=false;mouse=0;}
 void ToolToggle(unsigned key){if(key==VK_F10)f10=true;}
 bool Held(UINT message,unsigned key){
  if(key==VK_F10&&f10){if(message==WM_KEYUP||message==WM_SYSKEYUP){f10=false;return true;}if(message==WM_KEYDOWN||message==WM_SYSKEYDOWN)return true;}
  const unsigned button=message==WM_LBUTTONUP?1:message==WM_RBUTTONUP?2:0;if(button&&(mouse&button)){mouse&=~button;return true;}return false;
 }
 bool Key(UINT message,unsigned key,bool enabled,bool typing,bool foreground,bool modifiers){
  if((message!=WM_KEYDOWN&&message!=WM_SYSKEYDOWN)||key!=VK_F10||!enabled||typing||!foreground||modifiers)return false;f10=true;return true;
 }
 bool Mouse(UINT message,POINT point,int width,int height,bool enabled,bool typing,bool foreground){
  const unsigned button=message==WM_LBUTTONDOWN||message==WM_LBUTTONDBLCLK?1:message==WM_RBUTTONDOWN||message==WM_RBUTTONDBLCLK?2:0;
  if(!button||!enabled||typing||!foreground||!OptionMenuHit131(point,width,height))return false;mouse|=button;return true;
 }
};
}
