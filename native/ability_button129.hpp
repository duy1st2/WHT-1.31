#pragma once
#include "skills129.hpp"
#include <cstring>
namespace wht {
inline bool AbilityCode129(const std::wstring& text,uint32_t& code){
 if(text.size()!=4)return false;uint32_t value=0;
 for(auto ch:text){if(ch<33||ch>126)return false;value=(value<<8)|uint32_t(ch);}
 code=value;return true;
}
inline bool SetAbilityButton129(uintptr_t base,uint32_t code,bool hide){
 if(!base||!code)return false;const auto selected=SelectedSkill129(base);
 if(!selected.unit||!selected.identity)return false;
 __try {
  const unsigned char lookupSig[]={0x55,0x8b,0xec,0x56,0x8b,0x75,0x08};
  const unsigned char hideSig[]={0x55,0x8b,0xec,0x6a,0x01,0x6a,0x01,0x6a,0x01,0x6a,0x01};
  if(memcmp(reinterpret_cast<void*>(base+0x2d6570),lookupSig,sizeof(lookupSig))||memcmp(reinterpret_cast<void*>(base+0x2d7a10),hideSig,sizeof(hideSig)))return false;
  if(*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)return false;
  using Find=uintptr_t(__thiscall*)(uintptr_t,uint32_t,int,int,int,int);
  const auto ability=reinterpret_cast<Find>(base+0x2d6570)(selected.unit,code,1,1,1,1);if(!ability)return false;
  const int hidden=*reinterpret_cast<int*>(ability+0x40);if(hidden<0||hidden>1000000)return false;
  if(!hide&&!hidden)return true;
  const auto current=SelectedSkill129(base);if(current.unit!=selected.unit||current.identity!=selected.identity)return false;
  using Set=void(__thiscall*)(uintptr_t,uint32_t,int);
  reinterpret_cast<Set>(base+0x2d7a10)(selected.unit,code,hide?1:0);
  return *reinterpret_cast<int*>(ability+0x40)==hidden+(hide?1:-1);
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
