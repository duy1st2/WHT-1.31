#pragma once
#include "services129.hpp"
namespace wht {
struct TooltipAnchor129 {
 uintptr_t relative=0;int point=0;float x=0,y=0;
 bool operator==(const TooltipAnchor129& other)const{return relative==other.relative&&point==other.point&&x==other.x&&y==other.y;}
};
inline bool ReadTooltipAnchor129(uintptr_t base,uintptr_t layout,uintptr_t root,uintptr_t console,TooltipAnchor129& out,int point=8){
 __try {
  if(!layout||point<0||point>8||*reinterpret_cast<unsigned*>(layout+4)!=9)return false;
  const auto anchor=*reinterpret_cast<uintptr_t*>(layout+8+point*4);
  if(!anchor||*reinterpret_cast<uintptr_t*>(anchor)!=base+0xac9a2c)return false;
  out=*reinterpret_cast<TooltipAnchor129*>(anchor+4);
  return (out.relative==root||(console&&out.relative==console))&&out.point>=0&&out.point<=8&&std::isfinite(out.x)&&std::isfinite(out.y);
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline uintptr_t Tooltip129(uintptr_t base,uintptr_t ui){
 __try {
  // Lazy native accessor 1EC959 stores CUberToolTipWar3 in UI+1DC.
  const auto frame=ui?*reinterpret_cast<uintptr_t*>(ui+0x1dc):0;
  return frame&&*reinterpret_cast<uintptr_t*>(frame)==base+0xa94fe4?frame:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool TooltipPosition129(POINT cursor,int width,int height,bool foreground,float scale,float& x,float& y){
 if(!foreground||width<=0||height<=0||cursor.x<0||cursor.y<0||cursor.x>=width||cursor.y>=height)return false;
 x=static_cast<float>(cursor.x)*.8f*scale/width;
 float down=static_cast<float>(cursor.y)*.6f/height-.0075f;
 if(cursor.x<=width*.5f)x+=.2175f*scale;
 if(cursor.y<=height*.5f)down+=.065f;
 y=-down;return true;
}
struct MouseTooltip129 {
 uintptr_t ui=0,frame=0;bool active=false;TooltipAnchor129 original{},written{};
 void Reset(){*this={};}
 template<class SetPoint> void Apply(uintptr_t base,uintptr_t owner,bool enabled,POINT cursor,int width,int height,bool foreground,float scale,SetPoint setPoint){
  const auto currentFrame=Tooltip129(base,owner);
  if(ui!=owner||frame!=currentFrame){Reset();ui=owner;frame=currentFrame;}
  if(!frame)return;
  const auto root=ui+0xbc,console=*reinterpret_cast<uintptr_t*>(ui+0x468);
  TooltipAnchor129 current{};
  if(!ReadTooltipAnchor129(base,frame,root,console,current))return;
  float x=0,y=0;
  if(!enabled||!TooltipPosition129(cursor,width,height,foreground,scale,x,y)){
   if(active&&current==written&&!(current==original))setPoint(frame,8,original);
   active=false;return;
  }
  if(!active||!(current==written))original=current;
  const TooltipAnchor129 desired{root,0,x,y};
  if(!(current==desired))setPoint(frame,8,desired);
  written=desired;active=true;
 }
};
}
