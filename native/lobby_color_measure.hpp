#pragma once
#include "lobby_color.hpp"
#include <MinHook.h>
#include <intrin.h>
namespace wht {
// The native ellipsis helper compares visible length with a byte offset in the
// original colored string. Its sanitizer must strip markup, even when input
// sanitization preserves it for lobby chat and player names.
struct LobbyColorMeasurement {
 using Legacy=int(__fastcall*)(const char*,unsigned,char*,unsigned);
 using Modern=int(__cdecl*)(const char*,unsigned,char*,unsigned);
 inline static LobbyColorMeasurement* current=nullptr;
 uintptr_t base=0,copier=0,measurementReturn=0;void* original=nullptr;bool modern=false,ready=false;
 int Measure(const char* text,unsigned length,char* output,unsigned capacity){
  if(!output||!capacity)return 0;if(!text){output[0]=0;return 0;}
  char scratch[1024]{};unsigned removed=0;int changed=0;
  do{
   removed=0;
   if(modern)reinterpret_cast<void(__cdecl*)(const char*,unsigned,unsigned,char*,unsigned,unsigned*)>(copier)(text,length,0x600,output,capacity,&removed);
   else reinterpret_cast<void(__fastcall*)(const char*,unsigned,unsigned,char*,unsigned,unsigned*)>(copier)(text,length,0x600,output,capacity,&removed);
   if(!removed)break;changed=1;
   // Match the stock sanitizer's bounded intermediate buffer and repeated
   // stripping (escaped/nested markup can expose another token).
   size_t n=0;while(n+1<sizeof(scratch)&&output[n]){scratch[n]=output[n];++n;}scratch[n]=0;
   text=scratch;length=unsigned(n);
  }while(removed);
  return changed;
 }
 static int __fastcall LegacyHook(const char* text,unsigned length,char* output,unsigned capacity){
  auto& self=*current;
  if(reinterpret_cast<uintptr_t>(_ReturnAddress())==self.measurementReturn)return self.Measure(text,length,output,capacity);
  return reinterpret_cast<Legacy>(self.original)(text,length,output,capacity);
 }
 static int __cdecl ModernHook(const char* text,unsigned length,char* output,unsigned capacity){
  auto& self=*current;
  if(reinterpret_cast<uintptr_t>(_ReturnAddress())==self.measurementReturn)return self.Measure(text,length,output,capacity);
  return reinterpret_cast<Modern>(self.original)(text,length,output,capacity);
 }
 bool Install(uintptr_t image,bool newer,bool v126=false){
  base=image;modern=newer;copier=base+(modern?0x3e3770:v126?0x7b8db0:0x7b9550);
  const auto sanitizer=base+(modern?0x13fad0:v126?0x5bc6a0:0x5bce40);
  measurementReturn=base+(modern?0x13fd7c:v126?0x5bcf3c:0x5bd6dc);
  auto call=reinterpret_cast<unsigned char*>(measurementReturn-5);
  if(!LobbyColorPatch::Readable(call,5)||call[0]!=0xe8||measurementReturn+int32_t(LobbyColorPatch::Value(call+1))!=sanitizer)return false;
  if(current&&current!=this)return false;current=this;
  auto address=reinterpret_cast<void*>(sanitizer);
  if(MH_CreateHook(address,modern?reinterpret_cast<void*>(ModernHook):reinterpret_cast<void*>(LegacyHook),&original)!=MH_OK)return false;
  if(MH_EnableHook(address)!=MH_OK){MH_RemoveHook(address);original=nullptr;return false;}
  ready=true;return true;
 }
};
}
