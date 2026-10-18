#pragma once
#include "component_lookup131.hpp"
#include <cstring>
namespace wht {
// CGameUI::Update has the same two floats and context pointer in all four
// supplied clients. The complete 64-entry table and its RTTI COL are retained.
struct UiRefreshProfile131 {unsigned build;uintptr_t table,update;};
#ifdef _WIN64
inline constexpr UiRefreshProfile131 uiRefreshProfiles131[]={{12164,0x1c00ec0,0xc4d9e0},{12173,0x1c2fd78,0xc90f00}};
#else
inline constexpr UiRefreshProfile131 uiRefreshProfiles131[]={{12164,0x18240a0,0xb019d0},{12173,0x17d8f80,0xac15e0}};
#endif
inline std::array<uintptr_t,65> uiRefreshSlots131{};
inline uintptr_t uiRefreshUpdate131=0;inline DWORD uiRefreshThread131=0;
inline void(*uiRefreshCallback131)()=nullptr;
inline bool uiRefreshBusy131=false;inline uint64_t uiRefreshCalls131=0;
inline bool UiRefreshCode131(uintptr_t base,const UiRefreshProfile131& p){
 __try{
  if(!base||*reinterpret_cast<uintptr_t*>(base+p.table+12*sizeof(uintptr_t))!=base+p.update)return false;
#ifdef _WIN64
  const unsigned char prefix[]={0x48,0x89,0x5c,0x24,0x10,0x56,0x48,0x83,0xec};
  if(memcmp(reinterpret_cast<void*>(base+p.update),prefix,sizeof(prefix)))return false;
#else
  const unsigned char prefix[]={0x55,0x8b,0xec,0x83,0xec,0x10,0x53,0x56,0x57,0x89,0x4d,0xfc};
  if(memcmp(reinterpret_cast<void*>(base+p.update),prefix,sizeof(prefix)))return false;
  const unsigned end=p.build==12164?0x210:0x209;
  if(memcmp(reinterpret_cast<void*>(base+p.update+end),"\xc2\x0c\0",3))return false;
#endif
  const auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
  const auto nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(base+dos->e_lfanew);
  for(unsigned n=0;n<64;++n){const auto method=*reinterpret_cast<uintptr_t*>(base+p.table+n*sizeof(uintptr_t));if(method<base||method>=base+nt->OptionalHeader.SizeOfImage)return false;}
  return true;
 }__except(1){return false;}
}
inline void DispatchUiRefresh131(uintptr_t frame){
 if(frame!=uiRefreshOwner131||GetCurrentThreadId()!=uiRefreshThread131||uiRefreshBusy131||!uiRefreshCallback131)return;
 uiRefreshBusy131=true;
 __try{++uiRefreshCalls131;uiRefreshCallback131();}__except(1){}
 uiRefreshBusy131=false;
}
#ifdef _WIN64
inline uintptr_t UiRefreshCall131(uintptr_t frame,float elapsed,float time,uintptr_t context){
#else
inline uintptr_t __fastcall UiRefreshCall131(uintptr_t frame,void*,float elapsed,float time,uintptr_t context){
#endif
 const auto result=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,float,float,uintptr_t)>(uiRefreshUpdate131)(frame,elapsed,time,context);
 DispatchUiRefresh131(frame);return result;
}
inline bool InstallUiRefresh131(uintptr_t base,unsigned build,uintptr_t ui,void(*callback)()){
 const UiRefreshProfile131* p=nullptr;for(const auto& item:uiRefreshProfiles131)if(item.build==build)p=&item;
 if(!p||!ComponentIdentity131(ui,base+p->table))return false;
 if(uiRefreshOwner131==ui&&ComponentLookup131::Slot(ui,0)==uiRefreshProxyTable131){uiRefreshCallback131=callback;return true;}
 if(!UiRefreshCode131(base,*p))return false;
 __try{
  memcpy(uiRefreshSlots131.data(),reinterpret_cast<void*>(base+p->table-sizeof(uintptr_t)),sizeof(uiRefreshSlots131));
  uiRefreshOriginalTable131=base+p->table;uiRefreshUpdate131=base+p->update;
  uiRefreshSlots131[13]=reinterpret_cast<uintptr_t>(UiRefreshCall131);
  uiRefreshProxyTable131=reinterpret_cast<uintptr_t>(uiRefreshSlots131.data()+1);
  uiRefreshOwner131=ui;uiRefreshThread131=GetCurrentThreadId();uiRefreshCallback131=callback;
  *reinterpret_cast<uintptr_t*>(ui)=uiRefreshProxyTable131;return true;
 }__except(1){return false;}
}
inline void ForgetUiRefresh131(){uiRefreshOwner131=0;uiRefreshCallback131=nullptr;}
}
