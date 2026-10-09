#pragma once
#include "resource129.hpp"
#include <climits>
#include <sstream>
namespace wht {
enum class ResourceCommand129 { Gold, Lumber, Food, Full };
inline uint32_t TransferTarget129(const ResourceApi129& api,unsigned slot){
 __try {if(!api.player||!api.slot||slot>23)return 0;const auto target=api.player(slot);return target&&api.slot(target)==1?target:0;}
 __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool ParseResourceTransfer129(const std::wstring& args,bool lumberFirst,unsigned& slot,int& gold,int& lumber){
 std::wistringstream in(args);long long s=0,a=0,b=0;std::wstring extra;
 if(!(in>>s>>a>>b)||in>>extra||s<0||s>23||a<0||b<0||a>INT_MAX||b>INT_MAX||(!a&&!b))return false;
 slot=unsigned(s);gold=int(lumberFirst?b:a);lumber=int(lumberFirst?a:b);return true;
}
inline bool ParseSingleResourceTransfer129(const std::wstring& args,bool wood,unsigned& slot,int& gold,int& lumber){
 std::wistringstream in(args);long long s=0,amount=0;std::wstring extra;
 if(!(in>>s>>amount)||in>>extra||s<1||s>24||amount<1||amount>INT_MAX)return false;
 slot=unsigned(s-1);gold=wood?0:int(amount);lumber=wood?int(amount):0;return true;
}
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
inline bool TransferResources129(uint32_t source,uint32_t target,const ResourceCommandApi129& api,int gold,int lumber,bool& partial){
 partial=false;
 __try {
  if(!api||!source||!target||source==target||gold<0||lumber<0||(!gold&&!lumber)||api.local()!=source)return false;
  const int amounts[]={gold,lumber};int from[2]{},to[2]{};
  for(unsigned i=0;i<2;++i){from[i]=api.get(source,i+1);to[i]=api.get(target,i+1);
   if(from[i]<amounts[i]||to[i]<0||to[i]>INT_MAX-amounts[i])return false;}
  for(unsigned i=0;i<2;++i){if(!amounts[i])continue;
   if(api.local()!=source||api.get(source,i+1)!=from[i]||api.get(target,i+1)!=to[i])return false;
   api.set(source,i+1,from[i]-amounts[i]);partial=true;
   if(api.get(source,i+1)!=from[i]-amounts[i]||api.local()!=source)return false;
   api.set(target,i+1,to[i]+amounts[i]);
   if(api.get(target,i+1)!=to[i]+amounts[i]||api.local()!=source)return false;
  }
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
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
