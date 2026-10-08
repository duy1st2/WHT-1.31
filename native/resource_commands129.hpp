#pragma once
#include "resource129.hpp"
#include <climits>
#include <sstream>
namespace wht {
enum class ResourceCommand129 { Gold, Lumber, Food, Full };
struct ResourceCommandApi129 {
 uint32_t(__cdecl* local)()=nullptr;
 int(__cdecl* get)(uint32_t,int)=nullptr;
 void(__cdecl* set)(uint32_t,int,int)=nullptr;
 explicit operator bool()const{return local&&get&&set;}
};
inline ResourceCommandApi129 ResolveResourceCommandApi129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char local[]={0x56,0x8b,0x35};
  const unsigned char get[]={0x55,0x8b,0xec,0xff,0x75,0x08,0xe8};
  const unsigned char set[]={0x55,0x8b,0xec,0x56,0xff,0x75,0x08,0xe8};
  if(memcmp((void*)(base+0x93790),local,sizeof(local))||
     *reinterpret_cast<uintptr_t*>(base+0x93793)!=base+0xd3b6f4||
     memcmp((void*)(base+0x94a80),get,sizeof(get))||memcmp((void*)(base+0xa97a0),set,sizeof(set)))return {};
  return {reinterpret_cast<decltype(ResourceCommandApi129::local)>(base+0x93790),
   reinterpret_cast<decltype(ResourceCommandApi129::get)>(base+0x94a80),
   reinterpret_cast<decltype(ResourceCommandApi129::set)>(base+0xa97a0)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ParseResourceCommand129(ResourceCommand129 command,const std::wstring& argument,int& amount){
 amount=command==ResourceCommand129::Food?200:100000;
 if(argument.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 if(command==ResourceCommand129::Full)return false;
 std::wistringstream in(argument);long long number=0;std::wstring extra;
 if(!(in>>number)||in>>extra||number<1||number>INT_MAX)return false;
 amount=int(number);return true;
}
struct ResourceCommandResult129 {int gold=0,lumber=0,food=0;};
inline bool ApplyResourceCommand129(uint32_t player,const ResourceCommandApi129& api,ResourceCommand129 command,
 int amount,ResourceCommandResult129& result,const char** error=nullptr){
 if(error)*error=nullptr;result={};
 __try {
  if(!player||!api||amount<1||unsigned(command)>3||api.local()!=player){if(error)*error="Local player unavailable";return false;}
  const int states[]={1,2,4};int desired[3]{};
  const unsigned first=command==ResourceCommand129::Full?0:unsigned(command),last=command==ResourceCommand129::Full?3:first+1;
  for(unsigned i=first;i<last;++i){
   const int current=api.get(player,states[i]);
   if(current<0||(command!=ResourceCommand129::Full&&i<2&&current>INT_MAX-amount)){if(error)*error="Resource value would overflow";return false;}
   desired[i]=command==ResourceCommand129::Full?(i==2?300:1000000):i==2?amount:current+amount;
  }
  for(unsigned i=first;i<last;++i){
   if(api.local()!=player){if(error)*error="Local player changed";return false;}
   api.set(player,states[i],desired[i]);
   if(api.local()!=player||api.get(player,states[i])!=desired[i]){if(error)*error="Game did not apply requested resource value";return false;}
  }
  result.gold=api.get(player,1);result.lumber=api.get(player,2);result.food=api.get(player,4);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){if(error)*error="Native resource access failed";return false;}
}
}
