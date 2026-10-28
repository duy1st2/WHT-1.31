#pragma once
#include "quick_chat131.hpp"
#include "resource131.hpp"
#include <MinHook.h>
#include <mutex>
#include <deque>
#include <sstream>
namespace wht {
struct RepeatChatProfile131 {unsigned build;uintptr_t address,cookie;std::array<unsigned char,48> code;};
#ifdef _WIN64
inline constexpr RepeatChatProfile131 repeatChatProfiles131[]={
{12164,0xcc0c20,0x0,{{0x40,0x53,0x55,0x57,0x48,0x81,0xec,0x90,0x03,0x00,0x00,0x48,0x8b,0x05,0xe6,0x74,0x41,0x01,0x48,0x33,0xc4,0x48,0x89,0x84,0x24,0x80,0x03,0x00,0x00,0x41,0x8b,0xd9,0x49,0x8b,0xf8,0x48,0x8b,0xe9,0x83,0xfa,0xff,0x0f,0x84,0xac,0x01,0x00,0x00,0x48}}},
{12173,0xd00b40,0x0,{{0x40,0x53,0x55,0x57,0x48,0x81,0xec,0x90,0x03,0x00,0x00,0x48,0x8b,0x05,0xc6,0x45,0x40,0x01,0x48,0x33,0xc4,0x48,0x89,0x84,0x24,0x80,0x03,0x00,0x00,0x41,0x8b,0xd9,0x49,0x8b,0xf8,0x48,0x8b,0xe9,0x83,0xfa,0xff,0x0f,0x84,0x71,0x01,0x00,0x00,0x48}}},
};
#else
inline constexpr RepeatChatProfile131 repeatChatProfiles131[]={
{12164,0xb6a540,0x1ac8574,{{0x55,0x8b,0xec,0x81,0xec,0x60,0x03,0x00,0x00,0xa1,0x74,0x85,0xc9,0x01,0x33,0xc5,0x89,0x45,0xfc,0x8b,0x45,0x08,0x8b,0x55,0x0c,0x89,0x8d,0xa0,0xfc,0xff,0xff,0x89,0x85,0xa8,0xfc,0xff,0xff,0x89,0x95,0xa4,0xfc,0xff,0xff,0x53,0x56,0x57,0x83,0xf8}}},
{12173,0xb2a260,0x1a80574,{{0x55,0x8b,0xec,0x81,0xec,0x60,0x03,0x00,0x00,0xa1,0x74,0x05,0xcf,0x01,0x33,0xc5,0x89,0x45,0xfc,0x8b,0x45,0x08,0x8b,0x55,0x0c,0x89,0x8d,0xa0,0xfc,0xff,0xff,0x89,0x85,0xa8,0xfc,0xff,0xff,0x89,0x95,0xa4,0xfc,0xff,0xff,0x53,0x56,0x57,0x83,0xf8}}},
};
#endif
inline bool RepeatChatCode131(uintptr_t base,const RepeatChatProfile131& p){
 __try{if(!base)return false;const auto* code=reinterpret_cast<const unsigned char*>(base+p.address);
  for(unsigned i=0;i<p.code.size();++i){
#ifndef _WIN64
   if(i>=10&&i<14)continue;
#endif
   if(code[i]!=p.code[i])return false;
  }
#ifndef _WIN64
  if(*reinterpret_cast<const uintptr_t*>(code+10)!=base+p.cookie)return false;
#endif
  return true;
 }__except(1){return false;}
}
inline bool CopyRepeatChat131(const char* text,char (&out)[129]){
 __try{if(!text)return false;for(unsigned i=0;i<129;++i){out[i]=text[i];if(!out[i])return i>0;}return false;}__except(1){return false;}
}
struct RepeatChat131 {
 std::mutex mutex;int slot=-1;bool all=false,allowed=false;unsigned local=24;uint64_t due=0,generation=0;std::deque<std::string> pending;
 void Clear(){slot=-1;all=false;pending.clear();due=0;++generation;}
 void Reset(){std::lock_guard<std::mutex> lock(mutex);Clear();allowed=false;local=24;}
 void Publish(bool active,unsigned player){std::lock_guard<std::mutex> lock(mutex);if(!active||player>=24){Clear();allowed=false;local=24;}else{if(local!=player){Clear();}local=player;allowed=true;}}
 bool Command(const std::wstring& args,bool ready,std::string& report){
  std::wistringstream in(args);std::wstring value,extra;std::lock_guard<std::mutex> lock(mutex);
  if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos||(in>>value&&in>>extra)){report="Usage: #nhailai <1..24|all>; #nhailai to stop";return false;}
  if(value.empty()){Clear();report="Repeat Chat: OFF";return true;}
  if(!ready||!allowed||local>=24){report="Repeat Chat: native chat observer unavailable";return false;}
  if(value==L"all"){bool next=!all;Clear();all=next;report=all?"Repeat Chat: ALL":"Repeat Chat: OFF";return true;}
  unsigned target=0;if(value.size()>2||value.find_first_not_of(L"0123456789")!=std::wstring::npos){report="Repeat Chat: slot must be 1..24";return false;}
  for(auto c:value)target=target*10+unsigned(c-L'0');
  if(!target||target>24||target-1==local){report="Repeat Chat: choose another player (1..24)";return false;}
  Clear();slot=int(target)-1;report="Repeat Chat: "+std::to_string(target);return true;
 }
 void Observe(unsigned sender,const char* text,int channel){
  char copy[129]{};if(channel<0||channel>2||sender>=24||!CopyRepeatChat131(text,copy))return;
  std::string value(copy);if(!ValidQuickChat131(value)||value[0]=='#')return;
  for(unsigned char c:value)if(c<32||c==127)return;
  std::lock_guard<std::mutex> lock(mutex);if(!allowed||sender==local||(!all&&slot!=int(sender))||pending.size()>=64)return;pending.push_back(std::move(value));
 }
 template<class Send> bool Tick(bool ready,uint64_t now,Send send){
  std::string value;uint64_t token=0;{std::lock_guard<std::mutex> lock(mutex);if(!allowed||!ready||pending.empty()||now<due)return false;value=pending.front();token=generation;due=now+250;}
  bool sent=send(value);if(sent){std::lock_guard<std::mutex> lock(mutex);if(token==generation&&!pending.empty())pending.pop_front();}return sent;
 }
};
inline RepeatChat131 repeatChat131;
using ChatHistoryNative131=void(__thiscall*)(uintptr_t,unsigned,const char*,int);
inline ChatHistoryNative131 originalChatHistory131=nullptr;
inline bool repeatChatHookReady131=false;
#ifdef _WIN64
inline void ChatHistoryHook131(uintptr_t object,unsigned sender,const char* text,int channel){
#else
inline void __fastcall ChatHistoryHook131(uintptr_t object,void*,unsigned sender,const char* text,int channel){
#endif
 try{repeatChat131.Observe(sender,text,channel);}catch(...){}
 originalChatHistory131(object,sender,text,channel);
}
inline bool InstallRepeatChat131(uintptr_t base,unsigned build){
 if(repeatChatHookReady131)return true;const RepeatChatProfile131* profile=nullptr;for(const auto& p:repeatChatProfiles131)if(p.build==build)profile=&p;
 if(!profile||!RepeatChatCode131(base,*profile))return false;
 const auto status=MH_Initialize();if(status!=MH_OK&&status!=MH_ERROR_ALREADY_INITIALIZED)return false;
 auto target=reinterpret_cast<void*>(base+profile->address);
 if(MH_CreateHook(target,reinterpret_cast<void*>(ChatHistoryHook131),reinterpret_cast<void**>(&originalChatHistory131))!=MH_OK)return false;
 if(MH_EnableHook(target)==MH_OK){repeatChatHookReady131=true;return true;}MH_RemoveHook(target);originalChatHistory131=nullptr;return false;
}
inline void PublishRepeatChat131(uintptr_t base,unsigned build,bool allowed){
 unsigned local=24;const auto* p=ResourceProfileFor131(build);if(allowed&&p&&ResourceCode131(base,*p)&&ResourceWorld131(base,*p,local))repeatChat131.Publish(true,local);else repeatChat131.Reset();
}
}
