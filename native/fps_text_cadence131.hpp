#pragma once
#include "fps_text_cadence.hpp"
#include "hud_style.hpp"
#include <MinHook.h>
namespace wht {
using FpsFontText131=void(__thiscall*)(uintptr_t,const char*);
inline FpsFontText131 originalFpsFontText131=nullptr;
#ifdef _WIN64
inline void FpsFontTextHook131(uintptr_t font,const char* text){
#else
inline void __fastcall FpsFontTextHook131(uintptr_t font,void*,const char* text){
#endif
 if(!AllowFpsText(font,text,GetTickCount64()))return;
 char fpsInteger[64]{},styled[96]{};originalFpsFontText131(font,StyledFpsText(IntegerFpsText(text,fpsInteger,sizeof(fpsInteger)),styled,sizeof(styled)));
}
inline bool InstallFpsTextCadence131(uintptr_t address){
 const auto status=MH_Initialize();if(status!=MH_OK&&status!=MH_ERROR_ALREADY_INITIALIZED)return false;
 auto target=reinterpret_cast<void*>(address);
 if(MH_CreateHook(target,reinterpret_cast<void*>(FpsFontTextHook131),reinterpret_cast<void**>(&originalFpsFontText131))!=MH_OK)return false;
 if(MH_EnableHook(target)==MH_OK)return true;
 MH_RemoveHook(target);originalFpsFontText131=nullptr;return false;
}
}
