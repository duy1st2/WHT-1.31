#pragma once
#include <windows.h>
#include <cstring>
namespace wht {
// Legacy AND EAX,-5 -> AND EAX,-1. Exact 1.29 scheduler flag reset.
struct UnlockFps129 {
 bool ready=false,active=false,conflict=false;
 static bool Owns(unsigned char value,bool enabled){return value==(enabled?0xff:0xfb);}
 void Probe(uintptr_t base){
  const unsigned char signature[]={0x8b,0x46,0x44,0xa8,0x04,0x74,0x13,0x6a,0x00,0x83,0xe0,0xfb,0x6a,0x11,0x56,0x89,0x46,0x44,0xe8};
  ready=base&&!memcmp(reinterpret_cast<void*>(base+0x445847),signature,sizeof(signature));
 }
 bool Apply(uintptr_t base,bool enabled){
  if(!ready||conflict||!base)return false;
  auto target=reinterpret_cast<unsigned char*>(base+0x445852);
  if(!Owns(*target,active)){conflict=true;return false;}
  if(active==enabled)return true;
  DWORD old=0;if(!VirtualProtect(target,1,PAGE_EXECUTE_READWRITE,&old))return false;
  *target=enabled?0xff:0xfb;
  FlushInstructionCache(GetCurrentProcess(),target,1);
  DWORD ignored=0;VirtualProtect(target,1,old,&ignored);
  active=enabled;return true;
 }
};
}
