#pragma once
#include <windows.h>
#include <cmath>
#include <cstring>
#include <sstream>
#include <string>
namespace wht {
inline bool ParsePan129(const std::wstring& args,float& x,float& y){
 std::wistringstream in(args);std::wstring extra;
 return bool(in>>x>>y)&&!(in>>extra)&&std::isfinite(x)&&std::isfinite(y)&&std::abs(x)<=65536.f&&std::abs(y)<=65536.f;
}
inline bool PanCamera129(uintptr_t base,uintptr_t camera,float x,float y){
 if(!base||!camera||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>65536.f||std::abs(y)>65536.f)return false;
 __try {
  const unsigned char sig[]={0x53,0x8b,0xdc,0x83,0xec,0x08,0x83,0xe4,0xf8};
  if(memcmp(reinterpret_cast<void*>(base+0x1e1a00),sig,sizeof(sig)))return false;
  using Pan=void(__thiscall*)(uintptr_t,float,float,float,unsigned);
  reinterpret_cast<Pan>(base+0x1e1a00)(camera,x,y,0.f,0);
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
