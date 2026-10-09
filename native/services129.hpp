#pragma once
#include "common.hpp"
#include "periodic_action.hpp"
#include <cmath>
#include <algorithm>

namespace wht {
// Observe a simulation clock at the first active UI update, consuming the
// transition even if sound is disabled. Enabling mid-match stays silent.
struct MatchStart129 {
 uintptr_t owner=0;bool consumed=false;
 bool Observe(uintptr_t ui,bool clockReady,float seconds,bool enabled){
  if(owner!=ui){owner=ui;consumed=false;}
  if(!ui||!clockReady||consumed)return false;
  consumed=true;
  return enabled&&std::isfinite(seconds)&&seconds>=0&&seconds<=5;
 }
};
inline bool ToggleMultiboard129(uintptr_t base,void(__thiscall* minimize)(uintptr_t),void(__thiscall* maximize)(uintptr_t)){
 if(!base||!minimize||!maximize)return false;
 __try {
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return false;
  const auto board=*reinterpret_cast<uintptr_t*>(ui+0x3ec);
  if(!board||*reinterpret_cast<uintptr_t*>(board)!=base+0xa9af4c)return false;
  if(!*reinterpret_cast<uintptr_t*>(board+0x174)||!*reinterpret_cast<uintptr_t*>(board+0x1b0))return false;
  const auto state=*reinterpret_cast<unsigned*>(board+0x1c4);if(state>1)return false;
  if(state)maximize(board);else minimize(board);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// CHeroBar is a 7x1 native grid; each row contains one CHeroBarButton.
inline uintptr_t HeroButton129(uintptr_t base,int slot){
 if(!base||slot<0||slot>=7)return 0;
 __try {
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return 0;
  const auto bar=*reinterpret_cast<uintptr_t*>(ui+0x41c);
  if(!bar||*reinterpret_cast<uintptr_t*>(bar)!=base+0xa96aa0)return 0;
  if(*reinterpret_cast<unsigned*>(bar+0x128)!=7||*reinterpret_cast<unsigned*>(bar+0x12c)!=1)return 0;
  if(!*reinterpret_cast<unsigned*>(bar+0x180))return 0;
  const auto rows=*reinterpret_cast<uintptr_t*>(bar+0x154);if(!rows)return 0;
  const auto column=*reinterpret_cast<uintptr_t*>(rows+slot*0x10+4);if(!column)return 0;
  const auto button=*reinterpret_cast<uintptr_t*>(column);
  if(!button||*reinterpret_cast<uintptr_t*>(button)!=base+0xa96a08)return 0;
  if(!*reinterpret_cast<unsigned*>(button+0x94)||!*reinterpret_cast<uintptr_t*>(button+0x1c8))return 0;
  if(*reinterpret_cast<uintptr_t*>(button+0x128)!=bar+0x160||*reinterpret_cast<uintptr_t*>(bar+0x160)!=base+0xa96b18)return 0;
  return button;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool SelectHero129(uintptr_t base,int slot,void(__thiscall* click)(uintptr_t,int)){
 const auto button=HeroButton129(base,slot);if(!button||!click)return false;
 // The game's hero callback handles selection, dead heroes and double-click
 // camera centering. No synthetic unit handles or legacy offsets are used.
 __try {click(button,1);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Verified against the exact 1.29.2.9231 executable. Simulation scheduler,
// not wall time: 3F4910 resolves manager+44; 3FAC80 advances scheduler+40.
inline bool ReadClock129(uintptr_t base,float& seconds){
 seconds=0;if(!base)return false;
 __try {
  const auto manager=*reinterpret_cast<uintptr_t*>(base+0xd5e9ec);
  if(!manager)return false;
  const auto scheduler=*reinterpret_cast<uintptr_t*>(manager+0x44);
  if(!scheduler)return false;
  const float value=*reinterpret_cast<float*>(scheduler+0x40);
  if(!std::isfinite(value)||value<0||value>=864000)return false;
  seconds=value;return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
using ClearFrame129=void(__thiscall*)(uintptr_t);
using ClickFn129=void(__thiscall*)(uintptr_t,int);
struct QuickChatApi129 {
 void(__thiscall* open)(uintptr_t,int)=nullptr;
 void(__thiscall* text)(uintptr_t,const char*,int)=nullptr;
 int(__thiscall* submit)(uintptr_t,unsigned)=nullptr;
};
inline uintptr_t ChatBar129(uintptr_t base){
 if(!base)return 0;
 __try {
  auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return 0;
  auto bar=*reinterpret_cast<uintptr_t*>(ui+0x43c);
  if(!bar||*reinterpret_cast<uintptr_t*>(bar)!=base+0xa9587c)return 0;
  return bar;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool ChatOpen129(uintptr_t base,bool& open){
 auto bar=ChatBar129(base);if(!bar)return false;
 __try {const auto mode=*reinterpret_cast<unsigned*>(bar+0x270);if(mode>1)return false;open=mode!=0;return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ValidQuickChat129(const std::string& text){return QuickChatText(Wide(text))==text;}
inline bool SendQuickChat129(uintptr_t base,const std::string& text,const QuickChatApi129& api){
 if(text.empty()||text.size()>128||!api.open||!api.text||!api.submit)return false;
 if(!ValidQuickChat129(text))return false;
 bool typing=true;if(!ChatOpen129(base,typing)||typing)return false;
 __try {
  const auto bar=ChatBar129(base),ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);
  const auto edit=*reinterpret_cast<uintptr_t*>(bar+0x274),label=*reinterpret_cast<uintptr_t*>(bar+0x278);
  if(!edit||!label||*reinterpret_cast<uintptr_t*>(edit)!=base+0xa95730)return false;
  api.open(bar,1);api.text(edit,text.c_str(),1);
  // CGameUI's Enter callback 1EE2F0 ignores its one event argument.
  return api.submit(ui,0)!=0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
using FpsMode129=void(__thiscall*)(uintptr_t,int);
struct FpsState129 {
 uintptr_t owner=0;int saved=0;bool active=false;
 void Apply(uintptr_t base,bool enabled,FpsMode129 set){
  if(!base||!set)return;
  __try {
   auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);
   auto bar=ui?*reinterpret_cast<uintptr_t*>(ui+0x40c):0;
   if(owner!=bar){owner=bar;active=false;}
   if(!bar||*reinterpret_cast<uintptr_t*>(bar)!=base+0xa99b54)return;
   auto mode=*reinterpret_cast<int*>(bar+0x148);if(mode<0||mode>1)return;
   for(auto offset:{0x14c,0x150}){auto font=*reinterpret_cast<uintptr_t*>(bar+offset);if(!font||*reinterpret_cast<uintptr_t*>(font)!=base+0xac8fa4)return;}
   if(enabled){if(!active){saved=mode;active=true;}if(mode!=1)set(bar,1);}
   else if(active){if(mode!=saved)set(bar,saved);active=false;}
  }__except(EXCEPTION_EXECUTE_HANDLER){}
 }
};
struct TimeOfDay129 {
 uintptr_t owner=0;bool active=false,wasShown=false;
 void Apply(uintptr_t base,bool hide,ClearFrame129 show,ClearFrame129 hideFn){
  if(!base||!show||!hideFn)return;
  __try {
   auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);
   auto frame=ui?*reinterpret_cast<uintptr_t*>(ui+0x438):0;
   if(owner!=frame){owner=frame;active=false;}
   if(!frame||*reinterpret_cast<uintptr_t*>(frame)!=base+0xa9a32c)return;
   // FDF bits 0/1 are local/inherited HIDDEN flags, not visibility flags.
   const bool visible=(*reinterpret_cast<unsigned*>(frame+0xb8)&3)==0;
   if(hide){if(!active){wasShown=visible;active=true;}if(visible)hideFn(frame);}
   else if(active){if(wasShown&&!visible)show(frame);active=false;}
  }__except(EXCEPTION_EXECUTE_HANDLER){}
 }
};
// Native initialization at 1E7CBD stores the named SimpleConsole at UI +468.
inline bool IsConsole129(uintptr_t base,uintptr_t candidate){
 __try{return candidate&&*reinterpret_cast<uintptr_t*>(candidate)==base+0xa98b40;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct ConsoleState129 {
 uintptr_t owner=0;bool active=false,wasShown=false;
 void Apply(uintptr_t base,bool hide,ClearFrame129 show,ClearFrame129 hideFn){
  if(!base||!show||!hideFn)return;
  __try {
   const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);
   const auto frame=ui?*reinterpret_cast<uintptr_t*>(ui+0x468):0;
   if(!IsConsole129(base,frame))return;
   if(owner!=frame){owner=frame;active=false;}
   if(!frame)return;
   const bool visible=*reinterpret_cast<unsigned*>(frame+0x94)!=0;
   // +94 is requested visibility; +98 is actual render attachment.
   // The virtual Hide method only detaches (+98), so clear +94 first
   // or a subsequent native Show/update will reattach the console.
   if(hide){
    if(!active){wasShown=visible;active=true;}
    *reinterpret_cast<unsigned*>(frame+0x94)=0;
    if(*reinterpret_cast<unsigned*>(frame+0x98)!=0)hideFn(frame);
   }else if(active){
    if(wasShown){*reinterpret_cast<unsigned*>(frame+0x94)=1;show(frame);}
    active=false;
   }
  }__except(EXCEPTION_EXECUTE_HANDLER){}
 }
};
// Buttons are pointers in 1.29, rather than the contiguous legacy array.
inline uintptr_t Button129(uintptr_t base,bool inventory,int slot,bool requireAction=true){
 if(!base||slot<0||slot>=(inventory?6:12))return 0;
 __try {
  auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return 0;
  auto bar=*reinterpret_cast<uintptr_t*>(ui+(inventory?0x404:0x408));if(!bar)return 0;
  uintptr_t button=0;
  if(inventory){
   bar=*reinterpret_cast<uintptr_t*>(bar+0x14c);if(!bar)return 0;
   if(*reinterpret_cast<uintptr_t*>(bar)!=base+0xa97474)return 0;
   const auto count=*reinterpret_cast<unsigned*>(bar+0x12c);if(count!=6)return 0;
   auto list=*reinterpret_cast<uintptr_t*>(bar+0x130);if(!list)return 0;
   button=*reinterpret_cast<uintptr_t*>(list+slot*8+4);
  }else{
   if(*reinterpret_cast<uintptr_t*>(bar)!=base+0xa95c60)return 0;
   auto rows=*reinterpret_cast<uintptr_t*>(bar+0x154);if(!rows)return 0;
   auto row=*reinterpret_cast<uintptr_t*>(rows+(slot/4)*0x10+4);if(!row)return 0;
   button=*reinterpret_cast<uintptr_t*>(row+(slot%4)*4);
  }
  if(!button||*reinterpret_cast<uintptr_t*>(button)!=base+0xa9528c)return 0;
    if(requireAction&&(!*reinterpret_cast<uintptr_t*>(button+0x128)||!*reinterpret_cast<uintptr_t*>(button+0x194)))return 0;
  return button;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool IsShop129(uintptr_t base){
 __try {
  bool any=false;
  for(int i=0;i<12;++i){auto button=Button129(base,false,i);if(!button)continue;any=true;
   auto data=*reinterpret_cast<uintptr_t*>(button+0x194);
   switch(*reinterpret_cast<unsigned*>(data+8)){case 0xd000f:case 0xd0016:case 0xd0012:case 0xd0004:case 0xd0019:return false;}
  }
  return any;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ClickButton129(uintptr_t base,bool inventory,int slot,ClickFn129 click){
 auto button=Button129(base,inventory,slot);if(!button||!click)return false;
 unsigned saved=0;bool changed=false;
 __try {
  auto flags=reinterpret_cast<unsigned*>(button+0x13c);saved=*flags;*flags=saved|2;changed=true;
  // Native callback accepts one integer; use its real signature, not Show().
  click(button,1);
  if(Button129(base,inventory,slot)==button&&*flags==(saved|2))*flags=saved;
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){
  if(changed&&Button129(base,inventory,slot)==button){
   __try {auto flags=reinterpret_cast<unsigned*>(button+0x13c);if(*flags==(saved|2))*flags=saved;}
   __except(EXCEPTION_EXECUTE_HANDLER){}
  }
  return false;
 }
}
inline uintptr_t MessageFrame129(uintptr_t base,bool chat){
 if(!base)return 0;
 __try {
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);
  if(!ui)return 0;
  const auto frame=*reinterpret_cast<uintptr_t*>(ui+(chat?0x42c:0x428));
  if(!frame)return 0;
  if(*reinterpret_cast<uintptr_t*>(frame)!=base+0xac92d8)return 0;
  // CSimpleMessageFrame owns a bounded array of message slots (48B7B0).
  const auto count=*reinterpret_cast<unsigned*>(frame+0x128);
  if(count>4096||(count&&!*reinterpret_cast<uintptr_t*>(frame+0x18c)))return 0;
  return frame;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool ClearMessages129(uintptr_t base,bool chat,bool allowed,ClearFrame129 clear){
 if(!allowed||!clear)return false;
 const auto frame=MessageFrame129(base,chat);if(!frame)return false;
 __try {clear(frame);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct MessageTimers129 {
 PeriodicAction text,chat;
 void Tick(uintptr_t base,const Config& c,bool allowed,uint64_t now,ClearFrame129 clear){
  if(text.Poll(MessageFrame129(base,false),allowed&&c.clearTextEnabled,c.clearTextSeconds,now))ClearMessages129(base,false,allowed,clear);
  if(chat.Poll(MessageFrame129(base,true),allowed&&c.clearChatEnabled,c.clearChatSeconds,now))ClearMessages129(base,true,allowed,clear);
 }
};
struct AutoClick129 {
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
struct WindowServices129 {
 HWND owner=nullptr;bool borderless=false,clipped=false;LONG_PTR style=0;
 WINDOWPLACEMENT placement{sizeof(WINDOWPLACEMENT)};RECT previousClip{};
 void Apply(HWND h,bool full,bool lock){
  if(!h||!IsWindow(h))return;
  if(owner!=h){owner=h;borderless=false;style=0;}
  if(full!=borderless){
   if(full){
    MONITORINFO info{sizeof(info)};
    if(GetMonitorInfoW(MonitorFromWindow(h,MONITOR_DEFAULTTONEAREST),&info)){
     style=GetWindowLongPtrW(h,GWL_STYLE);GetWindowPlacement(h,&placement);
     SetWindowLongPtrW(h,GWL_STYLE,style&~WS_OVERLAPPEDWINDOW);
     SetWindowPos(h,nullptr,info.rcMonitor.left,info.rcMonitor.top,info.rcMonitor.right-info.rcMonitor.left,info.rcMonitor.bottom-info.rcMonitor.top,SWP_NOZORDER|SWP_FRAMECHANGED);
     borderless=true;
    }
   }else{
    SetWindowLongPtrW(h,GWL_STYLE,style);SetWindowPlacement(h,&placement);
    SetWindowPos(h,nullptr,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);borderless=false;
   }
  }
  RECT r{};POINT a{},b{};
  const bool active=lock&&GetForegroundWindow()==h&&!IsIconic(h)&&GetClientRect(h,&r);
  if(active){
   a={r.left,r.top};b={r.right,r.bottom};ClientToScreen(h,&a);ClientToScreen(h,&b);
   if(!clipped)GetClipCursor(&previousClip);
   r={a.x,a.y,b.x,b.y};if(ClipCursor(&r))clipped=true;
  }else if(clipped||GetForegroundWindow()==h){ClipCursor(nullptr);clipped=false;}
 }
};
}
