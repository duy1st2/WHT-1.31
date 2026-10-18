#pragma once
#include "life_labels131.hpp"
#include "stats_text.hpp"
namespace wht {
struct StatsProfile131 {unsigned build;uintptr_t helper,innerReturn;std::array<uintptr_t,5> callers;};
#ifdef _WIN64
inline constexpr StatsProfile131 statsProfiles131[]={
 {12164,0xcdb8e0,0xcdb99f,{0xcd9cc8,0xcd8e52,0xcda9a1,0xcdaa27,0xcdaab5}},
 {12173,0xd1b490,0xd1b54f,{0xd19878,0xd18a02,0xd1a551,0xd1a5d7,0xd1a665}}
};
inline constexpr unsigned statsOuterOffset131=0x240;
#else
inline constexpr StatsProfile131 statsProfiles131[]={
 {12164,0xb812a0,0xb8134b,{0xb7ff05,0xb7f27f,0xb80785,0xb80814,0xb808aa}},
 {12173,0xb40ea0,0xb40f4b,{0xb3fb05,0xb3ee7f,0xb40385,0xb40414,0xb404aa}}
};
inline constexpr unsigned statsOuterOffset131=0x218;
#endif
inline const StatsProfile131* StatsProfileFor131(unsigned build){for(const auto& p:statsProfiles131)if(p.build==build)return &p;return nullptr;}
inline int StatsKind131(uintptr_t caller,const StatsProfile131& p){for(unsigned n=0;n<p.callers.size();++n)if(caller==p.callers[n])return int(n);return -1;}
inline bool StatsCode131(uintptr_t base,const StatsProfile131& p,const LifeProfile131& life){
#ifdef _WIN64
 const unsigned char prefix[]={0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x81,0xec,0x30,2,0,0};
#else
 const unsigned char prefix[]={0x55,0x8b,0xec,0x81,0xec,4,2,0,0};
#endif
 __try{
  if(!base||memcmp(reinterpret_cast<void*>(base+p.helper),prefix,sizeof(prefix)))return false;
#ifndef _WIN64
  if(*reinterpret_cast<unsigned char*>(base+p.helper+0x13)!=0x53||*reinterpret_cast<unsigned char*>(base+p.helper+0x17)!=0x57||*reinterpret_cast<unsigned char*>(base+p.helper+0x61)!=0x5e)return false;
#endif
  const auto inner=base+p.innerReturn-5;if(*reinterpret_cast<unsigned char*>(inner)!=0xe8||inner+5+*reinterpret_cast<int32_t*>(inner+1)!=base+life.setText)return false;
  for(auto site:p.callers){const auto call=base+site-5;if(*reinterpret_cast<unsigned char*>(call)!=0xe8||call+5+*reinterpret_cast<int32_t*>(call+1)!=base+p.helper)return false;}
  return true;
 }__except(1){return false;}
}
inline uintptr_t StatsOuter131(uintptr_t slot){__try{return slot?*reinterpret_cast<uintptr_t*>(slot+statsOuterOffset131):0;}__except(1){return 0;}}
inline bool StatsFont131(uintptr_t base,const LifeProfile131& p,uintptr_t font){__try{return font&&*reinterpret_cast<uintptr_t*>(font)==base+p.font;}__except(1){return false;}}
struct StatsLabels131 {
 struct Label {uintptr_t font=0;std::string numeric,written;bool enabled=false;int kind=-1;};
 LifeSource131 owner;std::array<Label,8> labels{};
 void Reset(){owner={};labels={};}
 void Sync(const LifeSource131& source){if(!(owner==source)){Reset();owner=source;}}
 void Invalidate(uintptr_t font){for(auto& label:labels)if(label.font==font)label={};}
 template<class Set> bool Handle(uintptr_t font,const char* text,int kind,bool validFont,bool enabled,Set set){
  if(kind<0||kind>4||!validFont||!owner.ui||!owner.unit){Invalidate(font);return false;}
  std::array<char,129> safe{};std::string grouped;
  if(!CopyLifeText131(text,safe)||!GroupStatsText(safe.data(),grouped)){Invalidate(font);return false;}
  Label* found=nullptr;for(auto& label:labels)if(label.font==font){found=&label;break;}
  if(!found)for(auto& label:labels)if(!label.font){found=&label;break;}
  if(!found)return false;
  *found={font,safe.data(),enabled?grouped:std::string(safe.data()),enabled,kind};set(font,found->written.c_str());return true;
 }
 // Reformat the already identified fonts after native UI Update without
 // rescanning CInfoBar or waiting for the discovery timer.
 template<class Read,class Set> void Present(bool enabled,Read read,Set set){
  for(auto& label:labels){if(!label.font)continue;std::string current;
   if(read(label.font,current)&&current!=label.written)Handle(label.font,current.c_str(),label.kind,true,enabled,set);
  }
 }
 template<class Read,class Set> void Apply(bool enabled,Read read,Set set){
  for(auto& label:labels){if(!label.font)continue;std::string current;
   if(!read(label.font,current)||current!=label.written){label={};continue;}
   if(label.enabled==enabled)continue;
   std::string value=label.numeric;if(enabled&&!GroupStatsText(label.numeric,value)){label={};continue;}
   if(current!=value)set(label.font,value.c_str());label.written=std::move(value);label.enabled=enabled;
  }
 }
};
}
