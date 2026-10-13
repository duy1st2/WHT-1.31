#pragma once
#include <windows.h>
#include <cstdint>
#include <cstring>
namespace wht {
struct ParityPatch129 {
 uintptr_t address=0;uint32_t original=0,written=0;bool active=false,conflict=false;
 bool Probe(uintptr_t at,uint32_t expected){if(address||!at)return false;__try{if(*reinterpret_cast<uint32_t*>(at)!=expected)return false;address=at;original=written=expected;return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
 bool Apply(uint32_t value,bool enabled){
  if(!address||conflict)return false;
  __try{if(*reinterpret_cast<uint32_t*>(address)!=written){conflict=true;return false;}}
  __except(EXCEPTION_EXECUTE_HANDLER){conflict=true;return false;}
  const auto target=enabled?value:original;if(target==written){active=enabled;return true;}
  DWORD protection=0;if(!VirtualProtect(reinterpret_cast<void*>(address),4,PAGE_EXECUTE_READWRITE,&protection))return false;
  *reinterpret_cast<uint32_t*>(address)=target;FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(address),4);
  DWORD ignored=0;bool ok=VirtualProtect(reinterpret_cast<void*>(address),4,protection,&ignored)!=FALSE;written=target;active=enabled;return ok;
 }
};
inline bool InspectableUnit129(uintptr_t base,uintptr_t unit){
 if(!base||!unit)return false;
 __try{return *reinterpret_cast<uintptr_t*>(unit)==base+0xa9c504&&*reinterpret_cast<uint32_t*>(unit+0xc)!=0xffffffff&&*reinterpret_cast<uint32_t*>(unit+0x10)!=0xffffffff&&
   *reinterpret_cast<uintptr_t*>(unit+0x28)&&(*reinterpret_cast<uint32_t*>(unit+0x20)&3)==2&&!(*reinterpret_cast<uint32_t*>(unit+0x5c)&0x100);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
