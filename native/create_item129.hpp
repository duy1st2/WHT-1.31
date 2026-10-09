#pragma once
#include "create_unit129.hpp"
namespace wht {
struct CreateItemApi129 {
 uint32_t(__cdecl* create)(uint32_t,const float*,const float*)=nullptr;
 uint32_t(__cdecl* type)(uint32_t)=nullptr;
};
inline CreateItemApi129 ResolveCreateItem129(uintptr_t base){
 if(!base)return {};
 __try{const unsigned char create[]={0x55,0x8b,0xec,0x56,0xff,0x75,0x10};const unsigned char type[]={0x55,0x8b,0xec,0xff,0x75,0x08};
 if(memcmp((void*)(base+0x8e430),create,sizeof(create))||memcmp((void*)(base+0x92fc0),type,sizeof(type)))return {};
 return {reinterpret_cast<decltype(CreateItemApi129::create)>(base+0x8e430),reinterpret_cast<decltype(CreateItemApi129::type)>(base+0x92fc0)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline unsigned CreateItems129(uintptr_t base,const CreateItemApi129& api,uint32_t code,unsigned count,float x,float y){
 if(!base||!api.create||!api.type||!code||count<1||count>50||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>65536||std::abs(y)>65536)return 0;
 unsigned made=0;
 __try{auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(!world)return 0;
 for(unsigned i=0;i<count;++i){if(*reinterpret_cast<uintptr_t*>(base+0xd3b6f4)!=world)break;float sx=x+40.f*i;if(std::abs(sx)>65536)break;
 auto item=api.create(code,&sx,&y);if(!item||api.type(item)!=code)break;++made;}
 }__except(EXCEPTION_EXECUTE_HANDLER){}
 return made;
}
}
