#pragma once
#include "services129.hpp"
namespace wht {
// Exact 1.29b: target constructor store 1E8942; active-mode comparison 1EADC0.
inline uintptr_t TargetMode129(uintptr_t base){
 if(!base)return 0;
 __try {
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return 0;
  const auto current=*reinterpret_cast<uintptr_t*>(ui+0x1c4);
  if(!current||current!=*reinterpret_cast<uintptr_t*>(ui+0x250))return 0;
  return *reinterpret_cast<uintptr_t*>(current)==base+0xa927c0?current:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool QuickCastPoint129(int x,int y,int width,int height){
 return width>0&&height>0&&x>=0&&x<width&&y>=height*.06f&&y<height*.75f;
}
inline bool QuickCastTransition129(bool enabled,uintptr_t before,uintptr_t after){return enabled&&!before&&after;}
struct SmartCast129 {
 unsigned key=0;uintptr_t target=0,unit=0;uint64_t identity=0;
 void Reset(){*this={};}
 void Arm(unsigned pressed,uintptr_t mode,uintptr_t selected,uint64_t id){
  Reset();if(pressed&&mode&&selected&&id){key=pressed;target=mode;unit=selected;identity=id;}
 }
 bool Valid(bool allowed,uintptr_t mode,uintptr_t selected,uint64_t id)const{
  return allowed&&target&&target==mode&&unit==selected&&identity==id;
 }
 template<class Cast> bool Release(unsigned released,bool allowed,uintptr_t mode,uintptr_t selected,uint64_t id,Cast cast){
  if(released!=key)return false;
  const bool valid=Valid(allowed,mode,selected,id);const auto pending=target;
  Reset();return valid&&cast(pending);
 }
};
template<class Click> bool ActivateSlot129(uintptr_t base,bool inventory,int slot,bool quick,ClickFn129 activate,Click&& cast){
 const auto before=TargetMode129(base);
 if(!ClickButton129(base,inventory,slot,activate))return false;
 const auto after=TargetMode129(base);
 if(QuickCastTransition129(quick,before,after))cast(after);
 return true;
}
inline bool CastAtCursor129(uintptr_t base,HWND window,uintptr_t expected){
 if(!window||GetForegroundWindow()!=window||!expected||TargetMode129(base)!=expected||GetAsyncKeyState(VK_LBUTTON)<0)return false;
 POINT cursor{};RECT client{};
 if(!GetCursorPos(&cursor)||!ScreenToClient(window,&cursor)||!GetClientRect(window,&client)||!QuickCastPoint129(cursor.x,cursor.y,client.right,client.bottom))return false;
 // Same OS mouse path as the legacy ModeCast=2 implementation, no local JASS orders.
 INPUT events[2]{};events[0].type=events[1].type=INPUT_MOUSE;
 events[0].mi.dwFlags=MOUSEEVENTF_LEFTDOWN;events[1].mi.dwFlags=MOUSEEVENTF_LEFTUP;
 return SendInput(2,events,sizeof(INPUT))==2;
}
}
