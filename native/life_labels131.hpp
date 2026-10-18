#pragma once
#include "component_lookup131.hpp"
#include "common.hpp"
#include "life_mana.hpp"
#include <cstring>
namespace wht {
struct LifeProfile131 {unsigned build;uintptr_t ui,portrait,unit,font,setText,hpNumeric,mpNumeric,hpWidth,mpWidth,manaField;};
#ifdef _WIN64
inline constexpr LifeProfile131 lifeProfiles131[]={
 {12164,0x1c00ec0,0x1c10810,0x1c16140,0x1c58298,0xfb9660,0xce0bb4,0xce0dce,0xce0bd9,0xce0deb,0xce075d},
 {12173,0x1c2fd78,0x1c3f6d0,0x1c45000,0x1c871a8,0xff61d0,0xd206d0,0xd208ea,0xd206f5,0xd20907,0xd20279}
};
#else
inline constexpr LifeProfile131 lifeProfiles131[]={
 {12164,0x18240a0,0x182c178,0x182eeb0,0x1853b0c,0xdfe830,0xb85d1a,0xb85f77,0xb85d4c,0xb85fa1,0xb85986},
 {12173,0x17d8f80,0x17e1058,0x17e3d90,0x1808a2c,0xdbd630,0xb4588a,0xb45ae7,0xb458bc,0xb45b11,0xb454f6}
};
#endif
inline const LifeProfile131* LifeProfileFor131(unsigned build){for(const auto& p:lifeProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool LifeCode131(uintptr_t base,const LifeProfile131& p){
#ifdef _WIN64
 const unsigned char prefix[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0x81,8,1,0,0};
 const unsigned char hp[]={0x48,0x8b,0x8f,0xc8,3,0,0},mp[]={0x48,0x8d,0xb7,0xd0,3,0,0};
 constexpr unsigned hpBefore=0x10;
#else
 const unsigned char prefix[]={0x55,0x8b,0xec,0x56,0x8b,0xf1,0x8b,0x86,0xa8,0,0,0};
 const unsigned char hp[]={0x8b,0x8f,0x4c,2,0,0},mp[]={0x8d,0x9f,0x50,2,0,0};
 constexpr unsigned hpBefore=0x12;
#endif
 __try{
  if(!base||memcmp(reinterpret_cast<void*>(base+p.setText),prefix,sizeof(prefix))||memcmp(reinterpret_cast<void*>(base+p.hpNumeric-hpBefore),hp,sizeof(hp))||memcmp(reinterpret_cast<void*>(base+p.manaField),mp,sizeof(mp)))return false;
  for(auto site:{p.hpNumeric,p.mpNumeric,p.hpWidth,p.mpWidth}){const auto call=base+site-5;if(*reinterpret_cast<unsigned char*>(call)!=0xe8||call+5+*reinterpret_cast<int32_t*>(call+1)!=base+p.setText)return false;}
  for(auto site:{p.hpWidth,p.mpWidth}){
#ifdef _WIN64
   const auto load=base+site-12;if(memcmp(reinterpret_cast<void*>(load),"\x48\x8d\x15",3)||*reinterpret_cast<char*>(load+7+*reinterpret_cast<int32_t*>(load+3)))return false;
#else
   const auto load=base+site-10;if(*reinterpret_cast<unsigned char*>(load)!=0x68||**reinterpret_cast<const char**>(load+1))return false;
#endif
  }
  return true;
 }__except(1){return false;}
}
struct LifeSource131 {
 uintptr_t ui=0,unit=0;uint64_t identity=0;std::array<uintptr_t,2> fonts{};
 bool operator==(const LifeSource131& o)const{return ui==o.ui&&unit==o.unit&&identity==o.identity&&fonts==o.fonts;}
};
inline LifeSource131 ReadLifeSource131(uintptr_t base,const LifeProfile131& p,uintptr_t ui,uintptr_t portrait){
 LifeSource131 source;
 __try{
  if(!ui||!portrait||!ComponentIdentity131(ui,base+p.ui)||*reinterpret_cast<uintptr_t*>(portrait)!=base+p.portrait)return {};
  source.ui=ui;source.unit=*reinterpret_cast<uintptr_t*>(portrait+(sizeof(uintptr_t)==8?0x3b8:0x244));if(!source.unit||*reinterpret_cast<uintptr_t*>(source.unit)!=base+p.unit)return {};
  const auto id=reinterpret_cast<const uint32_t*>(source.unit+(sizeof(uintptr_t)==8?0x18:0xc));source.identity=(uint64_t(id[1])<<32)|id[0];
  for(unsigned n=0;n<2;++n){source.fonts[n]=*reinterpret_cast<uintptr_t*>(portrait+(sizeof(uintptr_t)==8?0x3c8:0x24c)+n*sizeof(uintptr_t));if(!source.fonts[n]||*reinterpret_cast<uintptr_t*>(source.fonts[n])!=base+p.font)return {};}
  return source;
 }__except(1){return {};}
}
inline bool CopyLifeText131(const char* text,std::array<char,129>& out){
 if(!text)return false;__try{for(unsigned n=0;n<out.size();++n){out[n]=text[n];if(!out[n])return true;}}__except(1){}return false;
}
inline bool ReadLifeText131(uintptr_t font,std::array<char,129>& out){__try{return CopyLifeText131(*reinterpret_cast<const char**>(font+(sizeof(uintptr_t)==8?0x108:0xa8)),out);}__except(1){return false;}}
struct LifeLabels131 {
 struct Label {std::string numeric,written;bool valid=false,widthCleared=false;};
 LifeSource131 owner;std::array<Label,2> labels{};
 void Reset(){owner={};labels={};}
 void Sync(const LifeSource131& source){if(!(owner==source)){Reset();owner=source;}}
 void Seed(unsigned index,const char* text){
  if(index>=labels.size()||labels[index].valid)return;
  std::array<char,129> safe{};std::string grouped;
  if(CopyLifeText131(text,safe)&&GroupLifeMana(safe.data(),grouped))labels[index]={safe.data(),safe.data(),true,false};
 }
 template<class Set> bool Handle(uintptr_t font,const char* text,uintptr_t caller,const LifeProfile131& p,unsigned flags,Set set){
  if(!owner.ui||!owner.unit)return false;
  for(unsigned n=0;n<2;++n)if(font&&font==owner.fonts[n]){
   auto& label=labels[n];std::array<char,129> safe{};if(!CopyLifeText131(text,safe)){label={};return false;}
   const std::string value=safe.data();std::string grouped;
   if(caller==(n?p.mpNumeric:p.hpNumeric)&&GroupLifeMana(value,grouped)){
    label.numeric=value;label.valid=true;label.widthCleared=false;label.written=flags&1?grouped:value;set(font,label.written.c_str());return true;
   }
   if(value.empty()&&caller==(n?p.mpWidth:p.hpWidth)&&label.valid){label.widthCleared=true;if(flags&2)return true;set(font,"");label.written.clear();return true;}
   label={};return false;
  }
  return false;
 }
 template<class Read,class Set> void Apply(unsigned flags,Read read,Set set){
  for(unsigned n=0;n<2;++n){auto& label=labels[n];if(!label.valid||!owner.fonts[n])continue;std::string current;
   if(!read(owner.fonts[n],current)||current!=label.written){label={};continue;}
   std::string value=label.widthCleared&&!(flags&2)?"":label.numeric;
   if(flags&1){std::string grouped;if(GroupLifeMana(value,grouped))value=std::move(grouped);}
   if(value!=label.written){set(owner.fonts[n],value.c_str());label.written=std::move(value);}
  }
 }
};
}
