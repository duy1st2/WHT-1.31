#pragma once
#include "skills129.hpp"
#include <cmath>
#include <cstdio>
namespace wht {
struct UnitInfo129 {uint32_t code=0;float x=0,y=0;};
inline bool ReadUnitInfo129(uintptr_t base,bool position,UnitInfo129& out){
 const auto selected=SelectedSkill129(base);if(!selected.unit||!selected.identity)return false;
 __try {
  const auto unit=selected.unit;
  if(*reinterpret_cast<uintptr_t*>(unit)!=base+0xa9c504)return false;
  out.code=*reinterpret_cast<uint32_t*>(unit+0x34);if(!out.code)return false;
  if(position){
   const unsigned char sig[]={0x55,0x8b,0xec,0x56,0x8b,0x75,0x08};
   if(memcmp(reinterpret_cast<void*>(base+0x3fcc50),sig,sizeof(sig)))return false;
   float xyz[3]{};
   using Position=uintptr_t(__thiscall*)(uintptr_t,float*);
   reinterpret_cast<Position>(base+0x3fcc50)(unit+0x26c,xyz);
   if(!std::isfinite(xyz[0])||!std::isfinite(xyz[1]))return false;
   out.x=xyz[0];out.y=xyz[1];
  }
  const auto current=SelectedSkill129(base);
  return current.unit==unit&&current.identity==selected.identity;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ReadUnitFlags129(uintptr_t base,uint32_t& flags){
 const auto selected=SelectedSkill129(base);if(!base||!selected.unit||!selected.identity)return false;
 __try {
  if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return false;
  flags=*reinterpret_cast<uint32_t*>(selected.unit+0x164);
  const auto current=SelectedSkill129(base);return current.unit==selected.unit&&current.identity==selected.identity;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string CursorFlagsText129(const POINT& screen,const POINT& client,bool hasUnit,uint32_t flags){
 char text[192]{};std::snprintf(text,sizeof(text),"MouseXY: [%ld, %ld], ClientXY: [%ld, %ld]",screen.x,screen.y,client.x,client.y);
 std::string result=text;if(hasUnit){std::snprintf(text,sizeof(text)," | Unit Flags: 0x%08X",unsigned(flags));result+=text;}return result;
}
inline std::string UnitCodeText129(uint32_t code){
 char text[80]{};char raw[5]{};
 for(unsigned i=0;i<4;++i){auto ch=(code>>(24-i*8))&255;raw[i]=ch>=32&&ch<=126?char(ch):'?';}
 std::snprintf(text,sizeof(text),"Unit ID: [%s] (0x%08X)",raw,unsigned(code));return text;
}
inline std::string UnitPositionText129(float x,float y){
 char text[128]{};std::snprintf(text,sizeof(text),"X: %.5f, Y: %.5f",x,y);return text;
}
}
