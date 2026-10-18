#pragma once
#include "common.hpp"
#include <algorithm>
namespace wht {
// Same toggle/cadence policy as AutoClick129; no inherited native game RVA.
struct AutoClick131 {
 bool left=false,right=false;uint64_t due=0;
 void Reset(){*this={};}
 bool Key(unsigned key,const Config& c){
  if(key&&key==c.autoClickLeftKey){left=!left;due=0;return true;}
  if(key&&key==c.autoClickRightKey){right=!right;due=0;return true;}
  return false;
 }
 template<class Send> void Tick(const Config& c,bool allowed,uint64_t now,Send send){
  if(!allowed||(!left&&!right)){due=0;return;}
  if(now<due)return;due=now+(std::max)(5u,(std::min)(1000u,c.autoClickInterval));
  if(left){send(WM_LBUTTONDOWN,MK_LBUTTON);send(WM_LBUTTONUP,0);}
  if(right){send(WM_RBUTTONDOWN,MK_RBUTTON);send(WM_RBUTTONUP,0);}
 }
};
// Called on the existing UI scheduler; never scan game objects or add a timer.
inline void UpdateAutoClick131(AutoClick131& state,const Config& c,HWND window,bool allowed){
 POINT cursor{};RECT rect{};
 const bool inside=allowed&&GetForegroundWindow()==window&&!IsIconic(window)&&GetClientRect(window,&rect)&&GetCursorPos(&cursor)&&ScreenToClient(window,&cursor)&&cursor.x>=0&&cursor.y>=0&&cursor.x<rect.right&&cursor.y<rect.bottom;
 const bool physical=(GetAsyncKeyState(VK_LBUTTON)&0x8000)||(GetAsyncKeyState(VK_RBUTTON)&0x8000);
 const bool modified=(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000)||(GetKeyState(VK_MENU)&0x8000);
 state.Tick(c,inside&&!physical&&!modified,GetTickCount64(),[&](UINT message,WPARAM key){PostMessageW(window,message,key,MAKELPARAM(cursor.x,cursor.y));});
}
}
