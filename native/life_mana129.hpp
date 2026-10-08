#pragma once
#include "skills129.hpp"
#include "life_mana.hpp"
namespace wht {
using FontText129=void(__thiscall*)(uintptr_t,const char*);
struct LifeLabels129 {SkillSelection129 selection;std::array<uintptr_t,2> fonts{};};
inline LifeLabels129 ReadLifeLabels129(uintptr_t base){
 LifeLabels129 result;result.selection=SelectedSkill129(base);if(!result.selection.unit)return {};
 __try {
  const auto portrait=*reinterpret_cast<uintptr_t*>(result.selection.ui+0x434);
  for(unsigned n=0;n<2;++n){result.fonts[n]=*reinterpret_cast<uintptr_t*>(portrait+0x250+n*4);if(!result.fonts[n]||*reinterpret_cast<uintptr_t*>(result.fonts[n])!=base+0xac8fa4)return {};}
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
 return result;
}
struct LifeMana129 {
 struct Label {std::string numeric,written;bool valid=false,widthCleared=false;};
 LifeLabels129 owner;std::array<Label,2> labels{};
 void Reset(){*this={};}
 bool Sync(uintptr_t base){
  const auto current=ReadLifeLabels129(base);
  if(current.selection.unit!=owner.selection.unit||current.selection.identity!=owner.selection.identity||current.selection.ui!=owner.selection.ui||current.fonts!=owner.fonts){Reset();owner=current;}
  return current.selection.unit!=0;
 }
 static bool NumericCaller(unsigned index,uintptr_t caller){return caller==(index?0x25a299:0x25a0a3);}
 static bool WidthCaller(unsigned index,uintptr_t caller){return caller==(index?0x25a2cb:0x25a0d5);}
 void Write(unsigned index,unsigned flags,FontText129 set){
  auto& label=labels[index];if(!label.valid||!set)return;
  std::string value=label.widthCleared&&!(flags&2)?"":label.numeric;
  if(flags&1){std::string grouped;if(GroupLifeMana(value,grouped))value=std::move(grouped);}
  if(value!=label.written){set(owner.fonts[index],value.c_str());label.written=std::move(value);}
 }
 bool Handle(uintptr_t base,uintptr_t font,const char* text,uintptr_t caller,unsigned flags,FontText129 set){
  if(!set||!Sync(base))return false;
  for(unsigned n=0;n<2;++n)if(owner.fonts[n]==font){
   auto& label=labels[n];std::string grouped;
   if(text&&NumericCaller(n,caller)&&GroupLifeMana(text,grouped)){
    label.numeric=text;label.valid=true;label.widthCleared=false;
    // Force the native write even if the value repeats after a game clear.
    auto value=flags&1?grouped:label.numeric;set(font,value.c_str());label.written=value;return true;
   }
   if(text&&!*text&&WidthCaller(n,caller)&&label.valid){
    label.widthCleared=true;
    if(flags&2)return true;
    set(font,text);label.written.clear();return true;
   }
   label={};return false;
  }
  return false;
 }
 void Apply(uintptr_t base,unsigned flags,FontText129 set){if(!Sync(base))return;for(unsigned n=0;n<2;++n)Write(n,flags,set);}
};
}
