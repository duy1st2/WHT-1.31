#pragma once
#include "common.hpp"
#include <array>
#include <cmath>
#include <cstdio>
namespace wht {
struct CameraPanProfile131 {unsigned build;uintptr_t pan;std::array<unsigned char,24> code;};
#ifdef _WIN64
inline constexpr CameraPanProfile131 cameraPanProfiles131[]={
{12164,0xc34670,{{0x48,0x8b,0xc4,0x55,0x56,0x57,0x48,0x8d,0x68,0xa9,0x48,0x81,0xec,0xe0,0x00,0x00,0x00,0x48,0xc7,0x45,0x9f,0xfe,0xff,0xff}}},
{12173,0xc7af40,{{0x48,0x8b,0xc4,0x55,0x56,0x57,0x48,0x8d,0x68,0xa9,0x48,0x81,0xec,0xe0,0x00,0x00,0x00,0x48,0xc7,0x45,0x9f,0xfe,0xff,0xff}}},
};
#else
inline constexpr CameraPanProfile131 cameraPanProfiles131[]={
{12164,0xaebac0,{{0x53,0x8b,0xdc,0x83,0xec,0x08,0x83,0xe4,0xf8,0x83,0xc4,0x04,0x55,0x8b,0x6b,0x04,0x89,0x6c,0x24,0x04,0x8b,0xec,0x6a,0xff}}},
{12173,0xaaa9c0,{{0x53,0x8b,0xdc,0x83,0xec,0x08,0x83,0xe4,0xf8,0x83,0xc4,0x04,0x55,0x8b,0x6b,0x04,0x89,0x6c,0x24,0x04,0x8b,0xec,0x6a,0xff}}},
};
#endif
inline const CameraPanProfile131* CameraPanProfileFor131(unsigned build){for(const auto& p:cameraPanProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool CameraPanCode131(uintptr_t base,const CameraPanProfile131& p){__try{return base&&!memcmp(reinterpret_cast<void*>(base+p.pan),p.code.data(),p.code.size());}__except(1){return false;}}
inline bool FocusPoint131(float x,float y){return std::isfinite(x)&&std::isfinite(y)&&std::abs(x)<=65536.f&&std::abs(y)<=65536.f;}
using CameraPanFn131=void(__thiscall*)(uintptr_t,float,float,float,unsigned);
inline bool InvokeCameraPan131(CameraPanFn131 pan,uintptr_t camera,float x,float y){
 if(!pan||!camera||!FocusPoint131(x,y))return false;
 __try{pan(camera,x,y,0.f,0);return true;}__except(1){return false;}
}
inline bool PanCameraNative131(uintptr_t base,unsigned build,uintptr_t camera,uintptr_t cameraTable,float x,float y){
 const auto* p=CameraPanProfileFor131(build);if(!p||!CameraPanCode131(base,*p))return false;
 __try{if(!camera||*reinterpret_cast<uintptr_t*>(camera)!=base+cameraTable)return false;}__except(1){return false;}
 return InvokeCameraPan131(reinterpret_cast<CameraPanFn131>(base+p->pan),camera,x,y);
}
template<class Read,class Camera,class Pan> bool FocusSelected131(const std::wstring& args,Read read,Camera camera,Pan pan,std::string& report){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_not_of(L" \t")!=std::wstring::npos){report="Usage: select a unit, then #focus";return false;}
 const auto before=camera();float x=0,y=0;
 if(!before||!read(x,y)||!FocusPoint131(x,y)||camera()!=before){report="Camera Focus: no valid selected unit or camera";return false;}
 if(!pan(before,x,y)){report="Camera Focus: native camera call unavailable";return false;}
 char text[128]{};std::snprintf(text,sizeof(text),"Camera Focus: X: %.5f, Y: %.5f",x,y);report=text;return true;
}
}
