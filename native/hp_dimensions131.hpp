#pragma once
#include "component_lookup131.hpp"
#include "overlay_frames.hpp"
#include <unordered_map>
#include <sstream>
#include <cmath>
namespace wht {
struct HpDimensionProfile131 {unsigned build;uintptr_t bar,width,height;};
#ifdef _WIN64
inline constexpr HpDimensionProfile131 hpDimensionProfiles131[]={{12164,0x1c14f08,0xfb5c10,0xfb5830},{12173,0x1c43dc8,0xff2780,0xff23a0}};
inline constexpr unsigned hpDimensionOffset131=0x9c,hpDimensionKind131=0x1e8;
#else
inline constexpr HpDimensionProfile131 hpDimensionProfiles131[]={{12164,0x182e5fc,0xdfb110,0xdfaea0},{12173,0x17e34dc,0xdb9f10,0xdb9ca0}};
inline constexpr unsigned hpDimensionOffset131=0x58,hpDimensionKind131=0x134;
#endif
inline const HpDimensionProfile131* HpDimensionProfileFor131(unsigned build){for(const auto& p:hpDimensionProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool HpDimensionCode131(uintptr_t base,const HpDimensionProfile131& p){
 __try{
#ifdef _WIN64
  const unsigned char width[]={0xf3,0x0f,0x11,0x89,0x9c,0,0,0},height[]={0xf3,0x0f,0x11,0x89,0xa0,0,0,0};
#else
  const unsigned char width[]={0x55,0x8b,0xec,0xf3,0x0f,0x10,0x45,8,0x56,0x8b,0xf1,0x57,0xf3,0x0f,0x11,0x46,0x58},height[]={0x55,0x8b,0xec,0xf3,0x0f,0x10,0x45,8,0x56,0x8b,0xf1,0x57,0xf3,0x0f,0x11,0x46,0x5c};
#endif
  return base&&!memcmp(reinterpret_cast<void*>(base+p.width),width,sizeof(width))&&!memcmp(reinterpret_cast<void*>(base+p.height),height,sizeof(height));
 }__except(1){return false;}
}
struct HpDimensionLease131 {
 float original=0,written=0;bool active=false;
 template<class Set> bool Apply(float current,float desired,Set set){
  if(!std::isfinite(current)||current<0||current>100||!std::isfinite(desired)||desired<0||desired>100)return false;
  if(desired>0){const float saved=(!active||current!=written)?current:original;if(current!=desired&&!set(desired))return false;original=saved;written=desired;active=true;}
  else if(active){if(current==written&&current!=original&&!set(original))return false;active=false;}
  return true;
 }
};
struct HpDimensions131 {
 std::unordered_map<uintptr_t,std::array<HpDimensionLease131,2>> entries;
 uintptr_t checkedBase=0;unsigned checkedBuild=0;bool ready=false;
 void Reset(){entries.clear();checkedBase=0;checkedBuild=0;ready=false;}
 template<class Frames> void Prune(const Frames& frames){for(auto i=entries.begin();i!=entries.end();)if(!frames.count(i->first))i=entries.erase(i);else ++i;}
 void Update(uintptr_t base,unsigned build,uintptr_t bar,float width,float height,const NativeFrameApi& raw){
  auto found=entries.find(bar);if(width<=0&&height<=0&&found==entries.end())return;
  auto p=HpDimensionProfileFor131(build);if(!p)return;if(checkedBase!=base||checkedBuild!=build){checkedBase=base;checkedBuild=build;ready=HpDimensionCode131(base,*p);}if(!ready||!ComponentIdentity131(bar,base+p->bar))return;
  if(reinterpret_cast<uintptr_t>(raw.setWidth)!=base+p->width||reinterpret_cast<uintptr_t>(raw.setHeight)!=base+p->height)return;
  __try{
   if(*reinterpret_cast<unsigned*>(bar+hpDimensionKind131)!=5)return;
   if(found==entries.end()){if(entries.size()>=4096)return;found=entries.emplace(bar,std::array<HpDimensionLease131,2>{}).first;}
   const float wanted[]={width,height};for(unsigned i=0;i<2;++i){const auto current=*reinterpret_cast<float*>(bar+hpDimensionOffset131+4*i);const auto setter=i?raw.setHeight:raw.setWidth;found->second[i].Apply(current,wanted[i],[&](float value){setter(reinterpret_cast<void*>(bar),value);return true;});}
   if(!found->second[0].active&&!found->second[1].active)entries.erase(found);
  }__except(1){}
 }
};
inline bool HpDimensionValue131(const std::wstring& args,float& value){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream input(args);float candidate=0;std::wstring extra;if(!(input>>candidate)||(input>>extra)||!std::isfinite(candidate)||candidate<0||candidate>100)return false;value=candidate;return true;
}
template<class Save,class Reload> bool ApplyHpDimension131(const std::wstring& name,const std::wstring& args,Save save,Reload reload){
 if(name!=L"#whp"&&name!=L"#hhp")return false;float value=0;if(!HpDimensionValue131(args,value)||!save(L"Event",name==L"#whp"?L"whp":L"hhp",std::to_wstring(value)))return false;reload();return true;
}
}
