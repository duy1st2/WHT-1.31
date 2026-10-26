#pragma once
#include "resource_text_profile131.hpp"
#include <cmath>
namespace wht {
inline bool ResourceTextPoint131(float x,float y,float sx,float sy,float drawnX,float drawnY,float vx,float vy,float nx,float ny,float (&point)[3]){
 const float values[]={x,y,sx,sy,drawnX,drawnY,vx,vy,nx,ny};for(float v:values)if(!std::isfinite(v))return false;
 if(vx<=0||vy<=0||nx<=0||ny<=0||vx>4||vy>4)return false;
 if(std::abs(drawnX-x-sx)<.000001f&&std::abs(drawnY-y-sy)<.000001f)return false;
 point[0]=((x+sx)*vx-sx)*nx;point[1]=((y+sy)*vy-sy)*ny;point[2]=0;return std::isfinite(point[0])&&std::isfinite(point[1]);
}
inline bool RepairResourceFont131(uintptr_t base,const ResourceTextProfile131& p,void* font){
 __try{
 const uintptr_t at=reinterpret_cast<uintptr_t>(font);if(!at||*reinterpret_cast<uintptr_t*>(at)!=base+p.fontTable)return false;
 constexpr bool wide=sizeof(uintptr_t)==8;
 const auto mesh=*reinterpret_cast<uintptr_t*>(at+(wide?0x110:0xac));if(!mesh)return false;
 const auto glyph=*reinterpret_cast<uintptr_t*>(mesh+(wide?0x10:8));if(!glyph)return false;
 auto f=[&](unsigned offset){return *reinterpret_cast<const float*>(glyph+offset);};auto u=[&](unsigned offset){return *reinterpret_cast<const unsigned*>(glyph+offset);};
 const float nx=*reinterpret_cast<const float*>(base+p.nx),ny=*reinterpret_cast<const float*>(base+p.ny);if(!std::isfinite(nx)||!std::isfinite(ny)||nx<=0||ny<=0)return false;
 const float scale=*reinterpret_cast<const float*>(at+(wide?0xa4:0x60));
 const float x=(*reinterpret_cast<const float*>(at+(wide?0x8c:0x48))+*reinterpret_cast<const float*>(at+(wide?0x12c:0xc4))*scale)/nx;
 const float y=(*reinterpret_cast<const float*>(at+(wide?0x88:0x44))+*reinterpret_cast<const float*>(at+(wide?0x130:0xc8))*scale)/ny;
 const unsigned horizontal=u(wide?0xa4:0x68),vertical=u(wide?0xa0:0x64),lines=u(wide?0x70:0x4c);if(horizontal>2||vertical>2||!lines||lines>1024)return false;
 const float sx=horizontal==2?f(wide?0x54:0x3c):horizontal==1?f(wide?0x54:0x3c)*.5f:0;
 const float height=float(lines)*f(wide?0x34:0x1c)+float(lines-1)*f(wide?0xa8:0x6c);
 const float sy=vertical==0?f(wide?0x58:0x40):vertical==1?(f(wide?0x58:0x40)-height)*.5f+height:height;
 if(std::abs(f(wide?0xb0:0x74)-x-sx)<.000001f&&std::abs(f(wide?0xb4:0x78)-y-sy)<.000001f)return false;
 float viewport[6]{},point[3]{};
 using View=void(__cdecl*)(float*,float*,float*,float*,float*,float*);reinterpret_cast<View>(base+p.viewport)(&viewport[0],&viewport[1],&viewport[2],&viewport[3],&viewport[4],&viewport[5]);
 if(!ResourceTextPoint131(x,y,sx,sy,f(wide?0xb0:0x74),f(wide?0xb4:0x78),viewport[1]-viewport[0],viewport[3]-viewport[2],nx,ny,point))return false;
 using Position=void(__cdecl*)(uintptr_t,const float*);reinterpret_cast<Position>(base+p.position)(mesh,point);return true;
 }__except(1){return false;}
}
}
