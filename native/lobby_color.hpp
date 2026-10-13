#pragma once
#include <windows.h>
#include <cstdint>
#include <cstring>
namespace wht {
// Preserve color only in chat/name input sanitization. The shared font copier
// must still remove color tokens when measuring and truncating displayed text.
struct LobbyColorPatch {
 unsigned char* targets[2]{};bool active=false,conflict=false;
 static bool Readable(const void* p,size_t n){MEMORY_BASIC_INFORMATION m{};return p&&VirtualQuery(p,&m,sizeof(m))&&m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&uintptr_t(p)+n<=uintptr_t(m.BaseAddress)+m.RegionSize;}
 static uint32_t Value(const void* p){uint32_t v;memcpy(&v,p,4);return v;}
 bool Probe(uintptr_t base,uintptr_t rva,bool modern=false){
  if(targets[0]||!base)return false;auto start=reinterpret_cast<unsigned char*>(base+rva);
  const unsigned char legacy[]={0x81,0xec,0x08,0x04,0,0,0xa1},newer[]={0x55,0x8b,0xec,0x81,0xec,0x08,0x04,0,0};
  if(!Readable(start,0xb0)||memcmp(start,modern?newer:legacy,modern?sizeof(newer):sizeof(legacy)))return false;
  const unsigned flags[]={modern?0x4au:0x57u,modern?0x94u:0x87u},calls[]={modern?0x59u:0x60u,modern?0xaau:0x9cu};
  uintptr_t copier=0;
  for(unsigned i=0;i<2;++i){if(start[flags[i]]!=0x68||Value(start+flags[i]+1)!=0x600||start[calls[i]]!=0xe8)return false;
   auto fn=reinterpret_cast<uintptr_t>(start+calls[i]+5)+int32_t(Value(start+calls[i]+1));if(i&&fn!=copier)return false;copier=fn;
  }
  const unsigned char oldCopy[]={0x83,0xec,0x08,0x53,0x8b,0x5c,0x24,0x1c},newCopy[]={0x55,0x8b,0xec,0x8b,0x55,0x1c,0x85,0xd2};
  if(!Readable(reinterpret_cast<void*>(copier),8)||memcmp(reinterpret_cast<void*>(copier),modern?newCopy:oldCopy,8))return false;
  targets[0]=start+flags[0]+1;targets[1]=start+flags[1]+1;return true;
 }
 bool Apply(bool enabled){
  if(!targets[0]||conflict)return false;
  // The native image may be gone by the time a late cleanup runs.
  for(auto p:targets)if(!Readable(p,4)){targets[0]=targets[1]=nullptr;active=false;return false;}
  for(auto p:targets)if(Value(p)!=(active?0x400u:0x600u)){conflict=true;return false;}
  if(active==enabled)return true;DWORD protection[2]{};
  for(unsigned i=0;i<2;++i)if(!VirtualProtect(targets[i],4,PAGE_EXECUTE_READWRITE,&protection[i])){for(unsigned j=0;j<i;++j){DWORD ignored=0;VirtualProtect(targets[j],4,protection[j],&ignored);}return false;}
  const uint32_t value=enabled?0x400:0x600;for(auto p:targets){memcpy(p,&value,4);FlushInstructionCache(GetCurrentProcess(),p,4);}active=enabled;
  bool restored=true;for(unsigned i=2;i>0;--i){DWORD ignored=0;if(!VirtualProtect(targets[i-1],4,protection[i-1],&ignored))restored=false;}return restored;
 }
};
}
