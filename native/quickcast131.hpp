#pragma once
#include <windows.h>
#include <cstdint>
#include <cstring>
namespace wht {
struct QuickCastProfile131 {unsigned build;uintptr_t targetTable,compare;};
#ifdef _WIN64
inline constexpr QuickCastProfile131 quickCastProfiles131[]={{12164,0x1c012a0,0xc41e80},{12173,0x1c30158,0xc88210}};
#else
inline constexpr QuickCastProfile131 quickCastProfiles131[]={{12164,0x1824290,0xaf8544},{12173,0x17d9170,0xab76a4}};
#endif
inline const QuickCastProfile131* QuickCastProfile(unsigned build){for(const auto& p:quickCastProfiles131)if(p.build==build)return &p;return nullptr;}
inline uintptr_t QuickCastTarget131(uintptr_t base,uintptr_t ui,uintptr_t uiTable,const QuickCastProfile131& p){
 if(!base||!ui)return 0;
 __try{
#ifdef _WIN64
  const unsigned char comparison[]={0x48,0x8b,0x81,0xb8,3,0,0,0x48,0x39,0x81,0x18,3,0,0};
  constexpr uintptr_t currentOffset=0x318,targetOffset=0x3b8;
#else
  const unsigned char comparison[]={0x8b,0x81,0xc4,1,0,0,0x3b,0x81,0x48,2,0,0};
  constexpr uintptr_t currentOffset=0x1c4,targetOffset=0x248;
#endif
  if(memcmp(reinterpret_cast<void*>(base+p.compare),comparison,sizeof(comparison))||*reinterpret_cast<uintptr_t*>(ui)!=base+uiTable)return 0;
  const auto current=*reinterpret_cast<uintptr_t*>(ui+currentOffset);
  if(!current||current!=*reinterpret_cast<uintptr_t*>(ui+targetOffset)||*reinterpret_cast<uintptr_t*>(current)!=base+p.targetTable)return 0;
  return current;
 }__except(1){return 0;}
}
inline bool QuickCastPoint131(int x,int y,int width,int height){return width>0&&height>0&&x>=0&&x<width&&y>=height*.06f&&y<height*.75f;}
template<class Target,class Click,class Cast> bool ActivateQuickCast131(bool quick,Target target,Click click,Cast cast){
 const auto before=quick?target():uintptr_t(0);
 if(!click())return false;
 if(quick&&!before){const auto after=target();if(after)cast(after);}
 return true;
}
template<class Target> bool QuickCastCursor131(HWND window,uintptr_t expected,Target target){
 if(!window||GetForegroundWindow()!=window||!expected||target()!=expected||(GetAsyncKeyState(VK_LBUTTON)&0x8000))return false;
 POINT cursor{};RECT client{};
 if(!GetCursorPos(&cursor)||!ScreenToClient(window,&cursor)||!GetClientRect(window,&client)||!QuickCastPoint131(cursor.x,cursor.y,client.right,client.bottom))return false;
 // Same input path as the legacy feature: select the native target at the cursor.
 INPUT events[2]{};events[0].type=events[1].type=INPUT_MOUSE;events[0].mi.dwFlags=MOUSEEVENTF_LEFTDOWN;events[1].mi.dwFlags=MOUSEEVENTF_LEFTUP;
 if(GetForegroundWindow()!=window||target()!=expected)return false;
 return SendInput(2,events,sizeof(INPUT))==2;
}
}
