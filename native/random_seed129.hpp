#pragma once
#include "hero_skill_points129.hpp"
#include <sstream>
namespace wht {
inline bool ParseRandomSeed129(const std::wstring& args,uint32_t& seed){
 std::wistringstream in(args);std::wstring token,extra;seed=0;if(!(in>>token))return true;
 if(in>>extra||token.size()>10)return false;
 uint64_t value=0;for(auto c:token){if(c<L'0'||c>L'9')return false;value=value*10+unsigned(c-L'0');if(value>0xffffffffULL)return false;}
 seed=uint32_t(value);return true;
}
using RandomSeedFn129=void(__cdecl*)(uint32_t);
inline RandomSeedFn129 ResolveRandomSeed129(uintptr_t base){
 if(!base)return nullptr;
 __try {
  const unsigned char signature[]={0x55,0x8b,0xec,0x56,0x8b,0x35};
  if(memcmp((void*)(base+0xa99a0),signature,sizeof(signature))||*reinterpret_cast<uintptr_t*>(base+0xa99a6)!=base+0xd61d90)return nullptr;
  if(*reinterpret_cast<unsigned char*>(base+0xa99af)!=0xe8||base+0xa99b4+*reinterpret_cast<int32_t*>(base+0xa99b0)!=base+0x5892a0)return nullptr;
  return reinterpret_cast<RandomSeedFn129>(base+0xa99a0);
 }__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
inline bool SetRandomSeed129(uintptr_t base,uint32_t seed,RandomSeedFn129 native,const char** error=nullptr){
 if(error)*error=nullptr;
 __try {
  if(!base||!native)return HeroSkillPointsFailure129(error,"Random seed function signature mismatch");
  if(!*reinterpret_cast<uintptr_t*>(base+0xd3b6f4))return HeroSkillPointsFailure129(error,"Match context unavailable");
  const auto rng=*reinterpret_cast<uintptr_t*>(base+0xd61d90);if(!rng)return HeroSkillPointsFailure129(error,"Random generator unavailable");
  native(seed);
  if(*reinterpret_cast<uintptr_t*>(base+0xd61d90)!=rng||*reinterpret_cast<uint32_t*>(rng)!=seed)return HeroSkillPointsFailure129(error,"Game did not accept random seed");
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Native random seed update failed");}
}
}
