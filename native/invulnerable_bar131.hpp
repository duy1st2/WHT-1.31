#include "stat_paint131.hpp"
#pragma once
#include "console_ui131.hpp"
#include <unordered_set>
namespace wht {
struct InvulnerableProfile131 {unsigned build;uintptr_t bar,unit,ability,onAdd,add;};
#ifdef _WIN64
inline constexpr InvulnerableProfile131 invulnerableProfiles131[]={
 {12164,0x1c14f08,0x1c16140,0x1aa6e70,0x59a0c0,0xd0ba10},
 {12173,0x1c43dc8,0x1c45000,0x1ad5e70,0x5fd5c0,0xd4a4b0}
};
#else
inline constexpr InvulnerableProfile131 invulnerableProfiles131[]={
 {12164,0x182e5fc,0x182eeb0,0x17891d4,0x507d30,0xbadd00},
 {12173,0x17e34dc,0x17e3d90,0x173e204,0x4defd0,0xb6d1a0}
};
#endif
inline const InvulnerableProfile131* InvulnerableProfileFor131(unsigned build){for(const auto& p:invulnerableProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool InvulnerableCode131(uintptr_t base,const InvulnerableProfile131& p){
 const auto* frame=ConsoleProfileFor131(p.build);if(!frame||!ConsoleCode131(base,*frame))return false;
#ifdef _WIN64
 const unsigned char flag[]={0x83,0x49,0x38,8};constexpr unsigned op=0x18,branch=0x1f;constexpr unsigned char jump=0xe9;
#else
 const unsigned char flag[]={0x83,0x4e,0x20,8};constexpr unsigned op=0x15,branch=0x18;constexpr unsigned char jump=0xe8;
#endif
 __try{
  const auto call=base+p.onAdd+branch;
  return *reinterpret_cast<uintptr_t*>(base+p.ability+32*sizeof(uintptr_t))==base+p.onAdd&&*reinterpret_cast<unsigned char*>(call)==jump&&call+5+*reinterpret_cast<int32_t*>(call+1)==base+p.add&&!memcmp(reinterpret_cast<void*>(base+p.add+op),flag,sizeof(flag))&&*reinterpret_cast<uintptr_t*>(base+p.bar+26*sizeof(uintptr_t))==base+frame->hide&&*reinterpret_cast<uintptr_t*>(base+p.bar+27*sizeof(uintptr_t))==base+frame->show;
 }__except(1){return false;}
}
inline bool ReadInvulnerableBar131(uintptr_t base,const InvulnerableProfile131& p,uintptr_t bar,uintptr_t unit,bool& invulnerable,bool& attached){
 __try{
  if(!base||!bar||!unit||!NativeClass131(bar,*reinterpret_cast<uintptr_t*>(bar),base+p.bar)||*reinterpret_cast<uintptr_t*>(unit)!=base+p.unit||*reinterpret_cast<unsigned*>(bar+(sizeof(uintptr_t)==8?0x1e8:0x134))!=5||*reinterpret_cast<uintptr_t*>(bar+(sizeof(uintptr_t)==8?0x210:0x154))!=unit)return false;
  const auto visible=reinterpret_cast<unsigned char*>(bar+(sizeof(uintptr_t)==8?0xf0:0x9c));if(visible[0]>1||visible[1]>1)return false;
  invulnerable=(*reinterpret_cast<unsigned*>(unit+(sizeof(uintptr_t)==8?0x38:0x20))&8)!=0;attached=visible[1]!=0;return true;
 }__except(1){return false;}
}
struct InvulnerableBars131 {
 std::unordered_set<uintptr_t> suppressed;uintptr_t checkedBase=0;unsigned checkedBuild=0;bool ready=false;
 void Reset(){suppressed.clear();checkedBase=0;checkedBuild=0;ready=false;}
 void Prune(const std::unordered_set<uintptr_t>& current){for(auto i=suppressed.begin();i!=suppressed.end();)if(!current.count(*i))i=suppressed.erase(i);else ++i;}
 template<class Show,class Hide> bool Apply(uintptr_t bar,bool invulnerable,bool enabled,bool attached,Show show,Hide hide){
  if(enabled&&invulnerable){if(suppressed.size()>=4096&&!suppressed.count(bar))return false;suppressed.insert(bar);if(attached)hide(bar);return true;}
  if(suppressed.erase(bar))show(bar);return false;
 }
 void Update(uintptr_t base,unsigned build,uintptr_t bar,uintptr_t unit,bool enabled,bool& visible){
  if(!enabled&&suppressed.empty())return;
  const auto* p=InvulnerableProfileFor131(build);const auto* frame=ConsoleProfileFor131(build);if(!p||!frame)return;
  if(checkedBase!=base||checkedBuild!=build){checkedBase=base;checkedBuild=build;ready=InvulnerableCode131(base,*p);}if(!ready)return;
  bool invulnerable=false,attached=false;if(!ReadInvulnerableBar131(base,*p,bar,unit,invulnerable,attached))return;
  StatPaintRecord131 record;record.unit=unit;record.unitTable=base+p->unit;record.hideFn=base+frame->hide;SetStatPaint131(base,p->bar,bar,false,record,enabled&&invulnerable);
  Invoke(base,*frame,bar,invulnerable,enabled,attached,visible);
 }
 void Invoke(uintptr_t base,const ConsoleProfile131& p,uintptr_t bar,bool invulnerable,bool enabled,bool attached,bool& visible){
  using Callback=void(__thiscall*)(uintptr_t);
  __try{
   const bool hidden=Apply(bar,invulnerable,enabled,attached,reinterpret_cast<Callback>(base+p.show),reinterpret_cast<Callback>(base+p.hide));
   visible=!hidden&&*reinterpret_cast<unsigned char*>(bar+(sizeof(uintptr_t)==8?0xf1:0x9d))!=0;
  }__except(1){}
 }
};
}
