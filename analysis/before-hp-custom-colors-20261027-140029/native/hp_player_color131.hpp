#include "stat_paint131.hpp"
#pragma once
#include "resource131.hpp"
#include "player_colors129.hpp"
#include <unordered_map>
#include <unordered_set>
namespace wht {
struct HpColorProfile131 {unsigned build;uintptr_t bar,unit,texture,tint,owner;};
#ifdef _WIN64
inline constexpr HpColorProfile131 hpColorProfiles131[]={
 {12164,0x1c14f08,0x1c16140,0x1c58218,0xfb7b00,0xd006a0},
 {12173,0x1c43dc8,0x1c45000,0x1c87128,0xff4670,0xd3f2d0}
};
#else
inline constexpr HpColorProfile131 hpColorProfiles131[]={
 {12164,0x182e5fc,0x182eeb0,0x1853acc,0xdfce40,0xba33a0},
 {12173,0x17e34dc,0x17e3d90,0x18089ec,0xdbbc40,0xb62a50}
};
#endif
inline const HpColorProfile131* HpColorProfileFor131(unsigned build){for(const auto& p:hpColorProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool HpColorCode131(uintptr_t base,const HpColorProfile131& p){
 const auto* resource=ResourceProfileFor131(p.build);if(!resource||!ResourceCode131(base,*resource))return false;
#ifdef _WIN64
 const unsigned char color[]={0x0f,0xb6,0x42,3,0x88,0x81,0xb8,0,0,0,0x0f,0xb6,0x42,2,0x88,0x81,0xbb,0,0,0,0x0f,0xb6,0x42,1,0x88,0x81,0xba,0,0,0,0x0f,0xb6,2,0x88,0x81,0xb9,0,0,0};
 const unsigned char owner[]={0x8b,0x81,0x98,1,0,0,0xc3};constexpr unsigned ownerSlot=0x1d8;
#else
 const unsigned char color[]={0x55,0x8b,0xec,0x8b,0x55,8,0x0f,0xb6,0x42,3,0x88,0x41,0x74,0x0f,0xb6,0x42,2,0x88,0x41,0x77,0x0f,0xb6,0x42,1,0x88,0x41,0x76,0x0f,0xb6,2,0x88,0x41,0x75};
 const unsigned char owner[]={0x8b,0x81,0x64,1,0,0,0xc3};constexpr unsigned ownerSlot=0xec;
#endif
 __try{return base&&!memcmp(reinterpret_cast<void*>(base+p.tint),color,sizeof(color))&&!memcmp(reinterpret_cast<void*>(base+p.owner),owner,sizeof(owner))&&*reinterpret_cast<uintptr_t*>(base+p.unit+ownerSlot)==base+p.owner;}__except(1){return false;}
}
inline bool ReadHpTexture131(uintptr_t base,const HpColorProfile131& p,uintptr_t bar,uintptr_t unit,uintptr_t& texture,uint32_t& color){
 __try{
  if(!base||!bar||!unit||!NativeClass131(bar,*reinterpret_cast<uintptr_t*>(bar),base+p.bar)||*reinterpret_cast<uintptr_t*>(unit)!=base+p.unit||*reinterpret_cast<unsigned*>(bar+(sizeof(uintptr_t)==8?0x1e8:0x134))!=5||*reinterpret_cast<uintptr_t*>(bar+(sizeof(uintptr_t)==8?0x210:0x154))!=unit)return false;
  texture=*reinterpret_cast<uintptr_t*>(bar+(sizeof(uintptr_t)==8?0x1e0:0x130));if(!texture||*reinterpret_cast<uintptr_t*>(texture)!=base+p.texture)return false;
  const auto bytes=reinterpret_cast<const unsigned char*>(texture+(sizeof(uintptr_t)==8?0xb8:0x74));color=(uint32_t(bytes[0])<<24)|(uint32_t(bytes[3])<<16)|(uint32_t(bytes[2])<<8)|bytes[1];return true;
 }__except(1){return false;}
}
inline bool UnitPlayerColor131(uintptr_t base,const HpColorProfile131& p,uintptr_t unit,uint32_t& color){
 const auto* r=ResourceProfileFor131(p.build);if(!r)return false;
 __try{
  unsigned local=24;const auto world=ResourceWorld131(base,*r,local);if(!world||!unit||*reinterpret_cast<uintptr_t*>(unit)!=base+p.unit)return false;
  const unsigned owner=reinterpret_cast<unsigned(__thiscall*)(uintptr_t)>(base+p.owner)(unit);if(owner>=28)return false;
  const auto player=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,unsigned)>(base+r->playerFn)(world,owner);
  return ResourceIdentity131(player,base,r->playerTable)&&PlayerColor129(*reinterpret_cast<unsigned*>(player+r->colorOffset),color);
 }__except(1){return false;}
}
inline bool SetHpTexture131(uintptr_t base,const HpColorProfile131& p,uintptr_t bar,uintptr_t unit,uintptr_t texture,uint32_t color){
 uintptr_t current=0;uint32_t before=0;if(!ReadHpTexture131(base,p,bar,unit,current,before)||current!=texture)return false;
 __try{
  reinterpret_cast<void(__thiscall*)(uintptr_t,const uint32_t*)>(base+p.tint)(texture,&color);
  return ReadHpTexture131(base,p,bar,unit,current,before)&&current==texture&&before==color;
 }__except(1){return false;}
}
struct HpPlayerColor131 {
 struct TintState {uintptr_t texture=0;uint32_t original=0,written=0;bool active=false;};
 struct Entry {TintState tint;uintptr_t unit=0;uint64_t due=0;bool known=false;uint32_t desired=0;};
 std::unordered_map<uintptr_t,Entry> entries;uintptr_t checkedBase=0;unsigned checkedBuild=0;bool ready=false;
 void Reset(){entries.clear();checkedBase=0;checkedBuild=0;ready=false;}
 void Prune(const std::unordered_set<uintptr_t>& frames){for(auto i=entries.begin();i!=entries.end();)if(!frames.count(i->first))i=entries.erase(i);else ++i;}
 template<class Set> static bool Tint(TintState& state,uintptr_t texture,uint32_t native,bool enabled,uint32_t desired,Set set){
  if(state.texture!=texture)state={texture,native,0,false};
  if(!state.active||native!=state.written)state.original=native;
  if(enabled){if(native!=desired&&!set(texture,desired))return false;state.written=desired;state.active=true;}
  else if(state.active){if(native==state.written&&native!=state.original&&!set(texture,state.original))return false;state.active=false;}
  return true;
 }
 void Update(uintptr_t base,unsigned build,uintptr_t bar,uintptr_t unit,bool enabled,uint64_t now){
  auto found=entries.find(bar);if(!enabled&&found==entries.end())return;
  const auto* p=HpColorProfileFor131(build);if(!p)return;
  if(checkedBase!=base||checkedBuild!=build){checkedBase=base;checkedBuild=build;ready=HpColorCode131(base,*p);}if(!ready)return;
  uintptr_t texture=0;uint32_t native=0;if(!ReadHpTexture131(base,*p,bar,unit,texture,native))return;
  if(found==entries.end()){if(entries.size()>=4096)return;found=entries.emplace(bar,Entry{}).first;}
  auto& e=found->second;
  if(enabled&&(e.unit!=unit||e.tint.texture!=texture||now>=e.due)){e.unit=unit;e.due=now+200;e.known=UnitPlayerColor131(base,*p,unit,e.desired);}
  StatPaintRecord131 record;record.unit=unit;record.unitTable=base+p->unit;record.texture=texture;record.textureTable=base+p->texture;record.tint=base+p->tint;record.desired=e.desired;SetStatPaint131(base,p->bar,bar,true,record,enabled&&e.known);
  Tint(e.tint,texture,native,enabled&&e.known,e.desired,[&](uintptr_t t,uint32_t color){return SetHpTexture131(base,*p,bar,unit,t,color);});
  if(!enabled&&!e.tint.active)entries.erase(found);
 }
};
}
