#pragma once
#include "random_seed_profile131.hpp"
#include "resource131.hpp"
#include <sstream>
namespace wht {
inline const RandomSeedProfile131* RandomSeedProfileFor131(unsigned build){for(const auto& p:randomSeedProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool RandomSeedCode131(uintptr_t base,const RandomSeedProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
inline bool ParseRandomSeed131(const std::wstring& args,uint32_t& seed){
 seed=0;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring token,extra;if(!(in>>token))return true;if(token.size()>10||in>>extra)return false;
 uint64_t value=0;for(auto ch:token){if(ch<L'0'||ch>L'9')return false;value=value*10+unsigned(ch-L'0');if(value>0xffffffffULL)return false;}seed=uint32_t(value);return true;
}
struct RandomSeedState131 {uint32_t seed=0,state=0;};
inline bool SameRandomState131(const RandomSeedState131& a,const RandomSeedState131& b){return a.seed==b.seed&&a.state==b.state;}
inline uint32_t RandomRotate131(uint32_t value,unsigned count){return (value<<count)|(value>>(32-count));}
inline RandomSeedState131 ExpectedRandomSeed131(uint32_t seed,const unsigned char* table){
 uint32_t state=((seed%59)<<10)|((seed%61)<<2)|((seed%53)<<18)|((seed+(seed/47)*17)<<26);
 const unsigned a=state>>24,b=(state>>16)&255,c=(state>>8)&255,d=state&255;
 const unsigned i=a>=4?a-4:a+184,j=b>=12?b-12:b+200,k=c>=24?c-24:c+212,l=d>=28?d-28:d+216;
 uint32_t x=0,y=0,z=0,w=0;memcpy(&x,table+i,4);memcpy(&y,table+j,4);memcpy(&z,table+k,4);memcpy(&w,table+l,4);
 return {seed+(RandomRotate131(x,1)^RandomRotate131(y,2)^RandomRotate131(z,3)^w),uint32_t((i<<24)|(j<<16)|(k<<8)|l)};
}
struct RandomSeedSource131 {uintptr_t world=0,rng=0;unsigned local=24;RandomSeedState131 value;};
inline bool SameRandomContext131(const RandomSeedSource131& a,const RandomSeedSource131& b){return a.world&&a.rng&&a.local<24&&a.world==b.world&&a.rng==b.rng&&a.local==b.local;}
inline bool ReadRandomSeed131(uintptr_t base,unsigned build,RandomSeedSource131& out){
 const auto* p=RandomSeedProfileFor131(build);const auto* rp=ResourceProfileFor131(build);if(!p||!rp||!RandomSeedCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{RandomSeedSource131 value;value.world=ResourceWorld131(base,*rp,value.local);if(!value.world)return false;value.rng=*reinterpret_cast<uintptr_t*>(base+p->rng);if(!value.rng)return false;value.value=*reinterpret_cast<const RandomSeedState131*>(value.rng);
 unsigned local=24;if(*reinterpret_cast<uintptr_t*>(base+p->rng)!=value.rng||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local)return false;out=value;return true;
 }__except(1){return false;}
}
using RandomSeedNative131=void(__cdecl*)(uint32_t);
inline bool InvokeRandomSeed131(RandomSeedNative131 native,uint32_t seed){if(!native)return false;__try{native(seed);return true;}__except(1){return false;}}
inline bool ApplyRandomSeed131(uintptr_t base,unsigned build,uint32_t seed,const RandomSeedSource131& expected){
 RandomSeedSource131 now;if(!ReadRandomSeed131(base,build,now)||!SameRandomContext131(expected,now)||!SameRandomState131(expected.value,now.value))return false;
 const auto* p=RandomSeedProfileFor131(build);return p&&InvokeRandomSeed131(reinterpret_cast<RandomSeedNative131>(base+p->set),seed);
}
template<class Read,class Apply> bool RandomSeedCommand131(const std::wstring& args,const RandomSeedProfile131& profile,Read read,Apply apply,std::string& report){
 uint32_t seed=0;if(!ParseRandomSeed131(args,seed)){report="Usage: #srand [0..4294967295] (default 0)";return false;}
 RandomSeedSource131 before,current,after;if(!read(before)||!read(current)||!SameRandomContext131(before,current)||!SameRandomState131(before.value,current.value)){report="Random Seed: stable match and game RNG required";return false;}
 const auto expected=ExpectedRandomSeed131(seed,profile.code[3].bytes);
 if(!apply(seed,before)){report="Random Seed: native update failed or match changed";return false;}
 if(!read(after)||!SameRandomContext131(before,after)||!SameRandomState131(expected,after.value)){report="Random Seed: native result not confirmed; update may have applied";return false;}
 report="Random Seed: "+std::to_string(seed);return true;
}
}
