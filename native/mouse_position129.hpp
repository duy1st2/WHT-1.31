#pragma once
#include <windows.h>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <string>
namespace wht {
struct MousePosition129 {float x=0,y=0,z=0;};
inline bool ReadMousePosition129(uintptr_t base,MousePosition129& out){
 if(!base)return false;
 __try {
  const unsigned char signature[]={0x55,0x8b,0xec,0x83,0xec,0x2c,0xff,0x75,0x14};
  if(memcmp(reinterpret_cast<void*>(base+0x20dd70),signature,sizeof(signature)))return false;
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return false;
  const auto worldFrame=*reinterpret_cast<uintptr_t*>(ui+0x3fc);
  if(!worldFrame||*reinterpret_cast<uintptr_t*>(worldFrame)!=base+0xa942b0)return false;
  const float sx=*reinterpret_cast<float*>(base+0xd61b54),sy=*reinterpret_cast<float*>(base+0xd61b58);
  if(!std::isfinite(sx)||!std::isfinite(sy)||std::abs(sx)>4||std::abs(sy)>4)return false;
  float xyz[3]{};using Project=int(__thiscall*)(uintptr_t,float,float,float*,unsigned);
  if(!reinterpret_cast<Project>(base+0x20dd70)(worldFrame,sx,sy,xyz,0))return false;
  for(float v:xyz)if(!std::isfinite(v)||std::abs(v)>100000)return false;
  out={xyz[0],xyz[1],xyz[2]};return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string MousePositionText129(const MousePosition129& p,int width,int height){
 char text[160]{};std::snprintf(text,sizeof(text),"X: %.5f, Y: %.5f, Z: %.5f | Res: %d, %d",p.x,p.y,p.z,width,height);return text;
}
}
