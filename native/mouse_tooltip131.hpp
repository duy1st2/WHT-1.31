#pragma once
#include "component_lookup131.hpp"
#include "black_bar131.hpp"
#include "console_ui131.hpp"
#include <algorithm>
namespace wht {
struct MouseTooltipProfile131 {unsigned build;uintptr_t table;};
#ifdef _WIN64
inline constexpr MouseTooltipProfile131 mouseTooltipProfiles131[]={{12164,0x1c0dbd0},{12173,0x1c3ca90}};
#else
inline constexpr MouseTooltipProfile131 mouseTooltipProfiles131[]={{12164,0x182a5c4},{12173,0x17df4a4}};
#endif
inline const MouseTooltipProfile131* MouseTooltipProfileFor131(unsigned build){for(const auto& p:mouseTooltipProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool TooltipPosition131(POINT cursor,int width,int height,bool foreground,float scale,float& x,float& y){
 if(!foreground||width<=0||height<=0||!std::isfinite(scale)||scale<=0||cursor.x<0||cursor.y<0||cursor.x>=width||cursor.y>=height)return false;
 x=float(cursor.x)*.8f*scale/width;float down=float(cursor.y)*.6f/height-.0075f;
 if(cursor.x<=width*.5f)x+=.2175f*scale;
 if(cursor.y<=height*.5f)down+=.065f;
 y=-down;return true;
}
inline bool TooltipRoot131(uintptr_t base,const BlackBarProfile131& p,const MouseTooltipProfile131& tip,uintptr_t ui,uintptr_t frame,uintptr_t& root){
 __try{
  if(!base||!ui||!frame||!ComponentIdentity131(ui,base+p.uiTable)||*reinterpret_cast<uintptr_t*>(frame)!=base+tip.table)return false;
  root=ui+(sizeof(uintptr_t)==8?0x158:0xbc);
  return *reinterpret_cast<unsigned*>(root+sizeof(uintptr_t))==9&&*reinterpret_cast<unsigned*>(frame+sizeof(uintptr_t))==9;
 }__except(1){return false;}
}
inline uintptr_t TooltipConsole131(uintptr_t base,unsigned build,uintptr_t console){
 const auto* p=ConsoleProfileFor131(build);__try{return p&&console&&*reinterpret_cast<uintptr_t*>(console)==base+p->table?console:0;}__except(1){return 0;}
}
struct MouseTooltip131 {
 uintptr_t owner=0,frame=0;bool active=false;LayoutPoint131 original{},written{};
 void Reset(){*this={};}
 template<class Set> bool Apply(uintptr_t ui,uintptr_t currentFrame,uintptr_t root,const LayoutPoint131& current,bool enabled,POINT cursor,int width,int height,bool foreground,float scale,Set set){
  if(owner!=ui||frame!=currentFrame){Reset();owner=ui;frame=currentFrame;}
  if(!ui||!frame||!root){Reset();return false;}
  float x=0,y=0;
  if(!enabled||!TooltipPosition131(cursor,width,height,foreground,scale,x,y)){
   if(active&&current==written&&!(current==original)&&!set(8,original))return false;
   active=false;return true;
  }
  if(!active||!(current==written))original=current;
  const LayoutPoint131 desired{root,0,x,y};
  if(!(current==desired)&&!set(8,desired))return false;
  written=desired;active=true;return true;
 }
 bool Update(uintptr_t base,unsigned build,uintptr_t ui,uintptr_t currentFrame,uintptr_t console,bool enabled,POINT cursor,int width,int height,bool foreground){
  const auto* p=BlackBarProfileFor131(build);const auto* tip=MouseTooltipProfileFor131(build);uintptr_t root=0;
  if(!p||!tip||!BlackBarCode131(base,*p)||!TooltipRoot131(base,*p,*tip,ui,currentFrame,root)){Reset();return false;}
  if(owner!=ui||frame!=currentFrame)Reset();
  console=TooltipConsole131(base,build,console);LayoutPoint131 current;
  if(!ReadLayoutPoint131(base,*p,currentFrame,root,8,current,console)){Reset();return false;}
  const float scale=width>0&&height>0?(std::max)(1.f,float(width)/(float(height)*4.f/3.f)):1.f;
  return Apply(ui,currentFrame,root,current,enabled,cursor,width,height,foreground,scale,[&](unsigned point,const LayoutPoint131& value){return SetLayoutPoint131(base,*p,currentFrame,root,point,value,console);});
 }
};
inline void UpdateMouseTooltip131(MouseTooltip131& state,uintptr_t base,unsigned build,uintptr_t ui,uintptr_t frame,uintptr_t console,HWND window,bool enabled){
 RECT client{};POINT cursor{};
 const bool input=GetForegroundWindow()==window&&!IsIconic(window)&&GetClientRect(window,&client)&&GetCursorPos(&cursor)&&ScreenToClient(window,&cursor);
 state.Update(base,build,ui,frame,console,enabled,cursor,client.right,client.bottom,input);
}
}
