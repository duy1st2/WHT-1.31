#pragma once
#include "component_lookup131.hpp"
#include "common.hpp"
#include <cmath>
#include <cstring>
namespace wht {
struct LayoutPoint131 {
 uintptr_t relative=0;int point=0;float x=0,y=0;
 bool operator==(const LayoutPoint131& o)const{return relative==o.relative&&point==o.point&&x==o.x&&y==o.y;}
};
struct BlackBarProfile131 {unsigned build;uintptr_t uiTable,worldTable,anchorTable,setPoint,layoutInstruction;};
#ifdef _WIN64
inline constexpr BlackBarProfile131 blackBarProfiles131[]={
 {12164,0x1c00ec0,0x1c03a08,0x1c5dc18,0xfb5a70,0xc3c213},
 {12173,0x1c2fd78,0x1c328c0,0x1c8cb28,0xff25e0,0xc82aa3}
};
#else
inline constexpr BlackBarProfile131 blackBarProfiles131[]={
 {12164,0x18240a0,0x1825828,0x1856104,0xdfb030,0xaf35f0},
 {12173,0x17d8f80,0x17da708,0x180b024,0xdb9e30,0xab2591}
};
#endif
inline const BlackBarProfile131* BlackBarProfileFor131(unsigned build){for(const auto& p:blackBarProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool BlackBarCode131(uintptr_t base,const BlackBarProfile131& p){
#ifdef _WIN64
 const unsigned char code[]={0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x41,0x56,0x48,0x83,0xec,0x20,0x45,0x8b,0xf1,0x49,0x8b,0xf0,0x48,0x8b,0xd9};
 const unsigned char layout[]={0x48,0x81,0xc1,0x58,1,0,0};
#else
 const unsigned char code[]={0x55,0x8b,0xec,0x56,0x57,0x8b,0x7d,0x0c,0x8b,0xf1,0x85,0xff,0x74,0x77,0x3b,0xfe,0x74,0x73};
 const unsigned char layout[]={0x81,0xc1,0xbc,0,0,0};
#endif
 __try{return base&&!memcmp(reinterpret_cast<void*>(base+p.setPoint),code,sizeof(code))&&!memcmp(reinterpret_cast<void*>(base+p.layoutInstruction),layout,sizeof(layout));}__except(1){return false;}
}
inline bool ReadLayoutPoint131(uintptr_t base,const BlackBarProfile131& p,uintptr_t layout,uintptr_t root,unsigned point,LayoutPoint131& out,uintptr_t console=0){
 if(!layout||!root||point>8)return false;
 __try{
  if(*reinterpret_cast<unsigned*>(layout+sizeof(uintptr_t))!=9)return false;
  const auto anchor=*reinterpret_cast<uintptr_t*>(layout+2*sizeof(uintptr_t)+point*sizeof(uintptr_t));if(!anchor||*reinterpret_cast<uintptr_t*>(anchor)!=base+p.anchorTable)return false;
  out.relative=*reinterpret_cast<uintptr_t*>(anchor+sizeof(uintptr_t));out.point=*reinterpret_cast<int*>(anchor+2*sizeof(uintptr_t));out.x=*reinterpret_cast<float*>(anchor+2*sizeof(uintptr_t)+4);out.y=*reinterpret_cast<float*>(anchor+2*sizeof(uintptr_t)+8);
  return (out.relative==root||(console&&out.relative==console))&&out.point>=0&&out.point<=8&&std::isfinite(out.x)&&std::isfinite(out.y);
 }__except(1){return false;}
}
inline bool BlackBarLayouts131(uintptr_t base,const BlackBarProfile131& p,uintptr_t ui,uintptr_t viewport,uintptr_t& root,uintptr_t& layout){
 __try{
  if(!base||!ui||!viewport||!ComponentIdentity131(ui,base+p.uiTable)||*reinterpret_cast<uintptr_t*>(viewport)!=base+p.worldTable)return false;
  constexpr unsigned offset=sizeof(uintptr_t)==8?0x158:0xbc;root=ui+offset;layout=viewport+offset;
  return *reinterpret_cast<unsigned*>(root+sizeof(uintptr_t))==9&&*reinterpret_cast<unsigned*>(layout+sizeof(uintptr_t))==9;
 }__except(1){return false;}
}
inline bool SetLayoutPoint131(uintptr_t base,const BlackBarProfile131& p,uintptr_t layout,uintptr_t root,unsigned point,const LayoutPoint131& value,uintptr_t console=0){
 if((value.relative!=root&&(!console||value.relative!=console))||value.point<0||value.point>8||!std::isfinite(value.x)||!std::isfinite(value.y))return false;
 __try{
  using Set=void(__thiscall*)(uintptr_t,int,uintptr_t,int,float,float,int);
  reinterpret_cast<Set>(base+p.setPoint)(layout,int(point),value.relative,value.point,value.x,value.y,1);
  LayoutPoint131 observed;return ReadLayoutPoint131(base,p,layout,root,point,observed,console)&&observed==value;
 }__except(1){return false;}
}
struct BlackBar131 {
 struct Saved {LayoutPoint131 original{},written{};bool active=false;};
 uintptr_t owner=0,layout=0;std::array<Saved,2> saved{};
 bool Active()const{return saved[0].active||saved[1].active;}
 void Reset(){*this={};}
 template<class Set> bool Apply(uintptr_t ui,uintptr_t currentLayout,uintptr_t root,const std::array<LayoutPoint131,2>& current,bool enabled,Set set){
  if(owner!=ui||layout!=currentLayout){Reset();owner=ui;layout=currentLayout;}
  if(!ui||!currentLayout||!root){Reset();return false;}
  const unsigned points[]={6,2};const std::array<LayoutPoint131,2> desired{{{root,6,0,0},{root,2,0,-.02f}}};
  for(unsigned i=0;i<2;++i){auto& s=saved[i];
   if(enabled){
    if(!s.active||!(current[i]==s.written))s.original=current[i];
    if(!(current[i]==desired[i])&&!set(points[i],desired[i]))return false;
    s.written=desired[i];s.active=true;
   }else if(s.active){
    if(current[i]==s.written&&!(current[i]==s.original)&&!set(points[i],s.original))return false;
    s.active=false;
   }
  }
  return true;
 }
 bool Update(uintptr_t base,unsigned build,uintptr_t ui,uintptr_t viewport,bool enabled){
  const auto* p=BlackBarProfileFor131(build);uintptr_t root=0,currentLayout=0;
  if(!p||!BlackBarCode131(base,*p)||!BlackBarLayouts131(base,*p,ui,viewport,root,currentLayout)){Reset();return false;}
  if(owner!=ui||layout!=currentLayout)Reset();
  std::array<LayoutPoint131,2> current{};
  if(!ReadLayoutPoint131(base,*p,currentLayout,root,6,current[0])||!ReadLayoutPoint131(base,*p,currentLayout,root,2,current[1])){Reset();return false;}
  return Apply(ui,currentLayout,root,current,enabled,[&](unsigned point,const LayoutPoint131& value){return SetLayoutPoint131(base,*p,currentLayout,root,point,value);});
 }
};
}
