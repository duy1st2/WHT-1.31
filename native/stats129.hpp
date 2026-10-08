#pragma once
#include "life_mana129.hpp"
#include "stats_text.hpp"
namespace wht {
inline int StatsKind129(uintptr_t caller){
 constexpr uintptr_t sites[]={0x25544a,0x254d69,0x255c06,0x255d25,0x255e48};
 for(int n=0;n<5;++n)if(caller==sites[n])return n;return -1;
}
inline uintptr_t StatsOuter129(uintptr_t returnSlot){
 // 2591A0: PUSH EBP, SUB ESP,204h, PUSH ESI/EDI, PUSH text.
 // The outer return is EBP+4, 218h above the inner setter's return slot.
 __try{return *reinterpret_cast<uintptr_t*>(returnSlot+0x218);}
 __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool StatsFont129(uintptr_t base,uintptr_t font){
 __try{return font&&*reinterpret_cast<uintptr_t*>(font)==base+0xac8fa4;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ReadStats129(uintptr_t font,std::array<char,129>& text){
 __try{
  const auto value=*reinterpret_cast<const char**>(font+0xa0);if(!value)return false;
  for(unsigned n=0;n<text.size();++n){text[n]=value[n];if(!text[n])return true;}
 }__except(EXCEPTION_EXECUTE_HANDLER){}
 return false;
}
struct Stats129 {
 struct Label {uintptr_t font=0;std::string numeric,written;bool enabled=false;};
 SkillSelection129 owner;std::array<Label,8> labels{};
 void Reset(){*this={};}
 bool Sync(uintptr_t base){
  const auto current=SelectedSkill129(base);
  if(current.ui!=owner.ui||current.unit!=owner.unit||current.identity!=owner.identity){Reset();owner=current;}
  return current.unit&&current.identity;
 }
 void Invalidate(uintptr_t font){for(auto& label:labels)if(label.font==font)label={};}
 void Remember(Label& label,const std::string& expected){std::array<char,129> actual{};if(!ReadStats129(label.font,actual)||actual.data()!=expected)label={};else label.written=expected;}
 bool Handle(uintptr_t base,uintptr_t font,const char* text,int kind,bool enabled,FontText129 set){
  if(kind<0||kind>4||!text||!set||!Sync(base)||!StatsFont129(base,font)){Invalidate(font);return false;}
  std::string grouped;if(!GroupStatsText(text,grouped)){Invalidate(font);return false;}
  Label* found=nullptr;for(auto& label:labels)if(label.font==font){found=&label;break;}
  if(!found)for(auto& label:labels)if(!label.font){found=&label;break;}
  if(!found)return false;
  *found={font,text,{},enabled};std::string value=enabled?grouped:std::string(text);set(font,value.c_str());Remember(*found,value);return true;
 }
 void Apply(uintptr_t base,bool enabled,FontText129 set){
  if(!set||!Sync(base))return;
  for(auto& label:labels){
   if(!label.font)continue;std::array<char,129> actual{};
   if(!StatsFont129(base,label.font)||!ReadStats129(label.font,actual)||actual.data()!=label.written){label={};continue;}
   if(label.enabled==enabled)continue;
   std::string value=label.numeric;if(enabled&&!GroupStatsText(label.numeric,value)){label={};continue;}
   set(label.font,value.c_str());label.enabled=enabled;Remember(label,value);
  }
 }
};
}
