#pragma once
#include "map_size_profile131.hpp"
#include <MinHook.h>
#include <sstream>
namespace wht {
inline constexpr uint32_t mapSizeNative131=0x10000000,mapSizeRaised131=0x7ffe0000;
inline const MapSizeProfile131* MapSizeProfileFor131(unsigned build){for(const auto& p:mapSizeProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool MapSizeContext131(uintptr_t base,const MapSizeProfile131& p){ResourceTextProfile131 guard{};guard.code=p.code;guard.count=p.codeCount;return base&&ResourceTextGuard131(base,guard);}
inline bool MapSizeRead131(uintptr_t at,void* out,size_t count){if(!at||!out||count>32)return false;__try{memcpy(out,reinterpret_cast<const void*>(at),count);return true;}__except(1){return false;}}
inline unsigned MapSizeCompare131(unsigned char* code,uint32_t limit){
 unsigned n=0;
#ifdef _WIN64
 code[n++]=0x48;
#endif
 code[n++]=0x3d;memcpy(code+n,&limit,4);return n+4;
}
// A register/stack-neutral CMP followed by an indirect JMP preserves CMP flags.
// MinHook handles thread instruction-pointer relocation when enabling/disabling.
// Stubs and disabled hook records stay allocated for the process lifetime, so an
// in-flight comparison can finish safely during an OFF/authentication transition.
struct MapSizeCompareHook131 {
 uintptr_t target=0,stub=0;unsigned length=0;bool created=false,active=false;
 std::array<unsigned char,5> owned{};
 bool Current()const{
  unsigned char actual[6]{},expected[6]{};const auto n=MapSizeCompare131(expected,mapSizeNative131);
  if(!target||length!=n||!MapSizeRead131(target,actual,n))return false;
  if(!active)return !memcmp(actual,expected,n);
  return !memcmp(actual,owned.data(),5)&&(n==5||actual[5]==expected[5]);
 }
 bool Create(uintptr_t at){
  if(created)return target==at&&Current();
  unsigned char actual[6]{},expected[6]{};const auto n=MapSizeCompare131(expected,mapSizeNative131);
  if(!MapSizeRead131(at,actual,n)||memcmp(actual,expected,n))return false;
  const auto init=MH_Initialize();if(init!=MH_OK&&init!=MH_ERROR_ALREADY_INITIALIZED)return false;
  auto memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));if(!memory)return false;
  unsigned pos=MapSizeCompare131(memory,mapSizeRaised131);memory[pos++]=0xff;memory[pos++]=0x25;
#ifdef _WIN64
  const uint32_t displacement=0;memcpy(memory+pos,&displacement,4);pos+=4;
#else
  const auto pointer=reinterpret_cast<uintptr_t>(memory+pos+4);memcpy(memory+pos,&pointer,4);pos+=4;
#endif
  const auto resume=at+n;memcpy(memory+pos,&resume,sizeof(resume));pos+=sizeof(resume);
  DWORD old=0;if(!VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&old)||!FlushInstructionCache(GetCurrentProcess(),memory,pos)){VirtualFree(memory,0,MEM_RELEASE);return false;}
  void* original=nullptr;if(MH_CreateHook(reinterpret_cast<void*>(at),memory,&original)!=MH_OK){VirtualFree(memory,0,MEM_RELEASE);return false;}
  target=at;stub=reinterpret_cast<uintptr_t>(memory);length=n;created=true;return true;
 }
 bool Enable(){
  if(active)return Current();if(!created||!Current()||MH_EnableHook(reinterpret_cast<void*>(target))!=MH_OK)return false;
  active=true;return MapSizeRead131(target,owned.data(),owned.size())&&owned[0]==0xe9&&Current();
 }
 bool Disable(){
  if(!active)return true;
  // Refuse to restore over a foreign modification of the installed branch.
  if(!Current()||MH_DisableHook(reinterpret_cast<void*>(target))!=MH_OK)return false;
  active=false;return Current();
 }
};
template<class Validate,class Enable,class Disable>
bool MapSizeTransition131(unsigned count,bool enabled,Validate validate,Enable enable,Disable disable){
 if(!count||count>3)return false;
 if(enabled){for(unsigned i=0;i<count;++i)if(!validate(i))return false;
  for(unsigned i=0;i<count;++i)if(!enable(i)){for(unsigned k=0;k<count;++k)disable(k);return false;}
  return true;
 }
 bool ok=true;for(unsigned i=0;i<count;++i){const bool restored=disable(i);ok=restored&&ok;}return ok;
}
struct MapSizeService131 {
 std::array<MapSizeCompareHook131,3> hooks;uintptr_t image=0;const MapSizeProfile131* profile=nullptr;uint64_t next=0;bool wanted=false,warned=false;
 bool Ready(uintptr_t base,unsigned build){
  const auto* p=MapSizeProfileFor131(build);if(!p||!p->count||p->count>3||(image&&(base!=image||profile!=p))||!MapSizeContext131(base,*p))return false;
  for(unsigned i=0;i<p->count;++i){if(hooks[i].created){if(hooks[i].target!=base+p->sites[i]||!hooks[i].Current())return false;}
   else{unsigned char actual[6]{},expected[6]{};const auto n=MapSizeCompare131(expected,mapSizeNative131);if(p->compareLength!=n||!MapSizeRead131(base+p->sites[i],actual,n)||memcmp(actual,expected,n))return false;}}
  return true;
 }
 bool Reset(){bool ok=true;for(auto& h:hooks){const bool restored=h.Disable();ok=restored&&ok;}return ok;}
 bool Update(uintptr_t base,unsigned build,bool enabled,std::string& report){
  bool ok=false;if(!enabled)ok=Reset();
  else if(Ready(base,build)){image=base;profile=MapSizeProfileFor131(build);ok=MapSizeTransition131(profile->count,true,[&](unsigned){return true;},[&](unsigned i){return hooks[i].Create(base+profile->sites[i])&&hooks[i].Enable();},[&](unsigned i){return hooks[i].Disable();});}
  else{Reset();}
  if(!ok){if(!warned)report="Map Size: guarded size checks unavailable or owned restoration pending";warned=true;}else warned=false;return ok;
 }
 void Poll(uintptr_t base,unsigned build,bool enabled,std::string& report){const auto now=GetTickCount64();if(now<next&&wanted==enabled)return;next=now+1000;wanted=enabled;Update(base,build,enabled,report);}
};
inline bool ParseMapSize131(const std::wstring& args,bool current,bool& enabled){
 if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;
 std::wistringstream in(args);std::wstring text,extra;if(!(in>>text)){enabled=!current;return true;}
 if((text!=L"0"&&text!=L"1")||(in>>extra))return false;enabled=text==L"1";return true;
}
template<class Ready,class Apply,class Save,class Reload>
bool MapSizeCommand131(const std::wstring& args,Config& config,Ready ready,Apply apply,Save save,Reload reload,std::string& report){
 bool enabled=false;if(!ParseMapSize131(args,config.unlockMap,enabled)){report="Usage: #mapsize [0|1]";return false;}
 if(enabled&&!ready()){report="Map Size: native size-check profile unavailable; setting unchanged";return false;}
 const bool saved=save(enabled?L"1":L"0");reload();if(!saved||config.unlockMap!=enabled){report="Map Size: setting could not be saved; native change not attempted";return false;}
 if(!apply(enabled)){report="Map Size: setting saved, but native application/restoration is incomplete";return false;}
 report=enabled?"Map Size: ON (guarded file-size limit 0x7FFE0000; reopen the map)":"Map Size: OFF (native size checks restored)";return true;
}
}
