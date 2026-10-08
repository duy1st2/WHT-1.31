#pragma once
#include "skills129.hpp"
#include <cmath>
#include <cstring>
#include <sstream>
namespace wht {
inline bool ParseUnitScale129(const std::wstring& args,float& scale){
 std::wistringstream in(args);std::wstring extra;float value=100;
 if(args.find_first_not_of(L" \t")!=std::wstring::npos&&(!(in>>value)||in>>extra))return false;
 if(!std::isfinite(value))return false;
 scale=value>10?value/100.f:value;scale=(std::max)(.05f,(std::min)(20.f,scale));return true;
}
inline bool ScaleFailure129(const char** failure,const char* reason){if(failure)*failure=reason;return false;}
using ScaleReal129=void(__cdecl*)(uintptr_t,int,float);
// A validated MinHook trampoline is the original function after its entry is patched.
inline ScaleReal129 ResolveScaleSetter129(const unsigned char* entry,ScaleReal129 original){
 if(original)return original;
 const unsigned char sig[]={0x55,0x8b,0xec,0x8b,0x55,0x08,0x8b,0x4d,0x0c,0x56};
 if(!entry||memcmp(entry,sig,sizeof(sig)))return nullptr;
 return reinterpret_cast<ScaleReal129>(const_cast<unsigned char*>(entry));
}
inline bool SetUnitScale129(uintptr_t base,float scale,const char** failure=nullptr,ScaleReal129 original=nullptr){
 if(failure)*failure=nullptr;
 if(!base||!std::isfinite(scale)||scale<.05f||scale>20)return ScaleFailure129(failure,"Invalid scale or runtime");
 const auto selected=SelectedSkill129(base);if(!selected.unit||!selected.identity)return ScaleFailure129(failure,"Select a unit first");
 __try {
  const auto setter=ResolveScaleSetter129(reinterpret_cast<const unsigned char*>(base+0x56cd30),original);
  if(!setter)return ScaleFailure129(failure,"Scale function signature mismatch");
  if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return ScaleFailure129(failure,"Selected object is not a supported CUnit");
  const auto model=*reinterpret_cast<uintptr_t*>(selected.unit+0x28);if(!model)return ScaleFailure129(failure,"Selected unit has no model");
  if(*reinterpret_cast<unsigned*>(model+8)<=2)return ScaleFailure129(failure,"Model scale property is unavailable");
  const auto properties=*reinterpret_cast<uintptr_t*>(model+12);if(!properties)return ScaleFailure129(failure,"Model property table is unavailable");
  const auto property=*reinterpret_cast<uintptr_t*>(properties+8);
  if(!property)return ScaleFailure129(failure,"Model scale property is empty");
  if(*reinterpret_cast<unsigned char*>(property+12)!=6)return ScaleFailure129(failure,"Model scale property type mismatch");
  if(*reinterpret_cast<unsigned char*>(property+13)&2)return ScaleFailure129(failure,"Model scale property is read-only");
  setter(model,2,scale);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){if(failure)*failure="Native scale access failed";return false;}
}
}
