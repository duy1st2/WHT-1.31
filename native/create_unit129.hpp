#pragma once
#include "remove_ability129.hpp"
#include <cmath>
namespace wht {
inline bool ParseCreateUnit129(const std::wstring& args,uint32_t& code,unsigned& count){
 std::wistringstream in(args);std::wstring raw,quantity,extra;count=1;
 if(!(in>>raw)||!ParseRemoveAbility129(raw,code))return false;
 if(in>>quantity){
  if(quantity.empty()||quantity.size()>2)return false;
  unsigned value=0;for(auto c:quantity){if(c<L'0'||c>L'9')return false;value=value*10+unsigned(c-L'0');}
  if(value<1||value>50)return false;count=value;
 }
 return !(in>>extra);
}
struct CreateUnitApi129 {
 uint32_t(__cdecl* local)()=nullptr;
 uint32_t(__cdecl* create)(uint32_t,uint32_t,const float*,const float*,const float*)=nullptr;
 uint32_t(__cdecl* type)(uint32_t)=nullptr;
 explicit operator bool()const{return local&&create&&type;}
};
inline CreateUnitApi129 ResolveCreateUnit129(uintptr_t base){
 if(!base)return {};
 __try {
  const unsigned char local[]={0x56,0x8b,0x35};
  const unsigned char create[]={0x55,0x8b,0xec,0x51,0x83,0x3d};
  const unsigned char type[]={0x55,0x8b,0xec,0xff,0x75,0x08};
  if(memcmp((void*)(base+0x93790),local,sizeof(local))||memcmp((void*)(base+0x8ee00),create,sizeof(create))||memcmp((void*)(base+0x97d30),type,sizeof(type)))return {};
  return {reinterpret_cast<decltype(CreateUnitApi129::local)>(base+0x93790),reinterpret_cast<decltype(CreateUnitApi129::create)>(base+0x8ee00),reinterpret_cast<decltype(CreateUnitApi129::type)>(base+0x97d30)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline unsigned CreateUnits129(uintptr_t base,const CreateUnitApi129& api,uint32_t code,unsigned count,float x,float y,float spacing=40.f){
 if(!base||!api||!code||count<1||count>50||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>100000||std::abs(y)>100000||!std::isfinite(spacing)||spacing<0||spacing>1000)return 0;
 unsigned spawned=0;
 __try {
  const auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(!world)return 0;
  const auto owner=api.local();if(!owner)return 0;
  const float facing=270.f;
  for(unsigned i=0;i<count;++i){
   if(*reinterpret_cast<uintptr_t*>(base+0xd3b6f4)!=world)break;
   const float spawnX=x+float(i)*spacing;if(std::abs(spawnX)>100000)break;
   const auto unit=api.create(owner,code,&spawnX,&y,&facing);
   if(!unit||api.type(unit)!=code)break;
   ++spawned;
  }
 }__except(EXCEPTION_EXECUTE_HANDLER){}
 return spawned;
}
}
