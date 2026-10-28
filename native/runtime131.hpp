#include "menu_capabilities131.hpp"
#include "menu_fog_vertex_profile131.hpp"
#include "presentation131.hpp"
#include "slot_remap131.hpp"
#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
#include <sstream>
#include <cmath>

namespace wht {
struct Profile131 {
 unsigned build;
 const char* hash;
 uintptr_t uiVtable,simpleVtable;
 uintptr_t uiGet,cameraSet,alloc,frameCtor,fontCtor,fontSet,fontText,fontAlign;
 uintptr_t textureCtor,textureSet,backdropCtor,backdropSet,stringAssign;
 uintptr_t width,height,absPoint,relPoint;
 uintptr_t cameraVtable,readFloat;
 uintptr_t archiveOpen,fileInfo,mpqVtable;
};
inline constexpr Profile131 profiles131[]={
 {12164,"9FEAB92F0D8898904122E25AC0ECE42C2070EBD443E4716A62B4767B0F6D0D72",
  0x18240a0,0x1853bc4,
  0xafa860,0xaedaa0,0x28c130,0xdfee80,0xdfcee0,0xdfe260,0xdfe830,0xdfe340,
  0xdfcfb0,0xdfe9f0,0xe1f240,0xdffc80,0x2c08a0,0xdfb110,0xdfaea0,0xdfaf70,0xdfb030,0x1823978,0xd68490,0x2d7ee0,0x2d7880,0x172f874},
 {12173,"93E3D7B38952D6649383C636AF4F766AA599CA25E10A150774F221F1CB4C3EA1",
  0x17d8f80,0x1808ae4,
  0xab9910,0xaac990,0x275cd0,0xdbdc80,0xdbbce0,0xdbd060,0xdbd630,0xdbd140,
  0xdbbdb0,0xdbd7f0,0xdde070,0xdbea80,0x2aa6d0,0xdb9f10,0xdb9ca0,0xdb9d70,0xdb9e30,0x17d8858,0xd23020,0x2c1d20,0x2c16c0,0x16e48c4}
};
inline const Profile131* FindProfile131(const std::string& hash){
 for(const auto& p:profiles131)if(hash==p.hash)return &p;return nullptr;
}
inline bool CameraCommand131(const std::wstring& text,int& field,float& value){
 std::wistringstream in(text);std::wstring cmd,extra;float candidate=0;
 if(!(in>>cmd>>candidate)||(in>>extra)||!std::isfinite(candidate))return false;
 FoldCommandName(cmd);
 if(cmd==L"#cam"||cmd==L"#zoom"){if(candidate<500||candidate>10000)return false;field=0;}
 else if(cmd==L"#fogz"){if(candidate<1||candidate>50000)return false;field=1;}
 else return false;value=candidate;return true;
}
using CameraSetter131=void(__thiscall*)(uintptr_t,int,float,float,int);
struct CameraOverride131 {
 uintptr_t owner=0;unsigned seen=0;float requested[2]{};bool active=false;
 void Reset(){owner=0;seen=0;active=false;requested[0]=requested[1]=0;}
 float Observe(uintptr_t object,int field,float value,bool enabled,const Config& config){
  if(owner!=object){Reset();owner=object;}
  if(field>=0&&field<2&&std::isfinite(value)){requested[field]=value;seen|=1u<<field;}
  active=enabled;
  return enabled&&field==0?config.camera:enabled&&field==1?config.fogZ:value;
 }
 template<class Setter> void Restore(Setter set){
  if(active&&owner)for(int field=0;field<2;++field)if(seen&(1u<<field))set(owner,field,requested[field],0.f,1);
  active=false;
 }
};
}
