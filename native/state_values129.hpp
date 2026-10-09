#pragma once
#include "regen129.hpp"
namespace wht {
using StatRead129=void(__thiscall*)(uintptr_t,float*);
inline bool ReadStateValues129(uintptr_t base,uintptr_t unit,bool mana,RegenResolve129 resolve,StatRead129 current,StatRead129 maximum,float& value,float& limit){
 float rate=0;if(!current||!maximum||!ReadRegen129(base,unit,mana,resolve,rate))return false;
 __try{
  const auto property=unit+(mana?0x1c0:0x1a0);current(property,&value);maximum(property,&limit);
  return std::isfinite(value)&&std::isfinite(limit)&&limit>0&&limit<1.e12f;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string GroupStateDigits129(const std::string& plain){
 std::string result;
 for(size_t i=0;i<plain.size();){
  if(plain[i]<'0'||plain[i]>'9'){result+=plain[i++];continue;}
  size_t end=i;while(end<plain.size()&&plain[end]>='0'&&plain[end]<='9')++end;
  for(size_t start=i;i<end;++i){if(i>start&&(end-i)%3==0)result+=',';result+=plain[i];}
 }
 return result;
}
inline std::string StateValueText129(float value,float maximum,bool showMax,bool mana,bool commas=false){
 if(!std::isfinite(value)||!std::isfinite(maximum)||maximum<=0)return {};
 char text[96]{};value=std::floor(std::clamp(value,0.f,maximum));
 if(showMax)std::snprintf(text,sizeof(text),"%.0f/%.0f",value,std::floor(maximum));else std::snprintf(text,sizeof(text),"%.0f",value);
 return std::string(mana?"|cffc3dbff":"|cff00ff00")+(commas?GroupStateDigits129(text):std::string(text))+"|r";
}
inline bool SampleStateBar129(uintptr_t base,uintptr_t bar,uintptr_t& unit,bool& visible,bool& hero){
  __try{
   if(!bar||*reinterpret_cast<uintptr_t*>(bar)!=base+0xa95104||*reinterpret_cast<unsigned*>(bar+0x13c)!=5)return false;
   unit=*reinterpret_cast<uintptr_t*>(bar+0x158);
   if(unit&&*reinterpret_cast<uintptr_t*>(unit)==base+0xa9c504){hero=*reinterpret_cast<unsigned char*>(unit+0x37)>='A'&&*reinterpret_cast<unsigned char*>(unit+0x37)<='Z';visible=*reinterpret_cast<unsigned*>(bar+0x94)!=0;}
  }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 return true;
}
inline float StateFontSize129(float configured){
 return (std::isfinite(configured)?std::clamp(configured,1.f,20.f):4.f)*.0015f;
}
inline float StateFontOffset129(float size,bool mana){return mana?-(size+.004f):size+.002f;}
template<class Api> void ReflowStateFont129(void* font,uintptr_t bar,bool mana,float size,float& applied,Api& api){
 if(!font||applied==size)return;
 api.setFont(font,"FRIZQT__.ttf",size,1);
 api.setRelPoint(font,4,reinterpret_cast<void*>(bar),4,0,StateFontOffset129(size,mana),1);
 applied=size;
}
struct StateValues129 {
 struct Entry {std::array<void*,2> fonts{};std::array<std::string,2> text{};std::array<float,2> sizes{};};
 std::unordered_map<uintptr_t,Entry> entries;
 bool active=false;
 void Forget(uintptr_t bar){entries.erase(bar);}
 template<class Api> void Write(uintptr_t base,uintptr_t bar,const Config& config,bool allowed,Api& api){
  uintptr_t unit=0;bool visible=false,hero=false;
  if(!SampleStateBar129(base,bar,unit,visible,hero))return;
  const bool enabled=allowed&&visible&&config.stateTagEnable&&(hero?config.stateTagHero:config.stateTagNonHero);
  auto found=entries.find(bar);if(found==entries.end()){if(!enabled||entries.size()>=4096)return;found=entries.emplace(bar,Entry{}).first;}
  auto& entry=found->second;const float fontSize=StateFontSize129(config.stateTagFontSize);
  for(unsigned n=0;n<2;++n){
   float value=0,maximum=0;std::string text;
   if(enabled&&(!n||config.stateTagManaValue)&&ReadStateValues129(base,unit,n!=0,reinterpret_cast<RegenResolve129>(base+0x707f0),reinterpret_cast<StatRead129>(base+0x400d80),reinterpret_cast<StatRead129>(base+0x400e20),value,maximum))text=StateValueText129(value,maximum,config.stateTagShowMax,n!=0,config.lifeManaCommas);
   if(!entry.fonts[n]&&!text.empty()){
    entry.fonts[n]=NativeCreateFontString(api,reinterpret_cast<void*>(bar),fontSize,"",2);
    if(entry.fonts[n])api.setTextAlign(entry.fonts[n],2);
   }
   ReflowStateFont129(entry.fonts[n],bar,n!=0,fontSize,entry.sizes[n],api);
   if(entry.fonts[n]&&entry.text[n]!=text){api.setText(entry.fonts[n],text.c_str());entry.text[n]=text;}
  }
 }
 template<class Api> void Apply(uintptr_t base,const Config& config,bool allowed,Api& api){active=allowed;for(auto& pair:entries)Write(base,pair.first,config,allowed,api);}
};
}
