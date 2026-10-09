#pragma once
#include "services129.hpp"
#include <cmath>
namespace wht {
inline bool InventoryBoundsHit129(float left,float bottom,float right,float top,float x,float y){
 return std::isfinite(left)&&std::isfinite(bottom)&&std::isfinite(right)&&std::isfinite(top)&&std::isfinite(x)&&std::isfinite(y)&&left>=0&&right<=.8f&&bottom>=0&&top<=.6f&&right-left>=.015f&&right-left<=.08f&&top-bottom>=.015f&&top-bottom<=.08f&&x>=left&&x<right&&y>=bottom&&y<top;
}
inline float QuickDropMouseX129(int pixel,int width,int height){
 const int classicWidth=(std::min)(width,height*4/3);
 return classicWidth>0?.4f+(float(pixel)-float(width)*.5f)*.8f/classicWidth:0.f;
}
inline int QuickDropSlot129(uintptr_t base,POINT p,int width,int height){
 if(!base||width<=0||height<=0||p.x<0||p.y<0||p.x>=width||p.y>=height)return -1;
 const float x=QuickDropMouseX129(p.x,width,height),y=.6f-float(p.y)*.6f/height;
 __try{
  for(int slot=0;slot<6;++slot){const auto button=Button129(base,true,slot,false);if(!button)continue;
   // Verified native hit test RVA 473350: Ymin, Xmin, Ymax, Xmax.
   const auto bounds=reinterpret_cast<const float*>(button+0x44);
   if(InventoryBoundsHit129(bounds[1],bounds[0],bounds[3],bounds[2],x,y))return slot;
  }
 }__except(EXCEPTION_EXECUTE_HANDLER){return -1;}
 return -1;
}
}
