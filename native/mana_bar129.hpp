#pragma once
#include "state_values129.hpp"
#include "resource129.hpp"
namespace wht {
using ManaOwner129=uintptr_t(__thiscall*)(uintptr_t);
inline bool ManaEnemy129(uintptr_t unit,ManaOwner129 owner,const ResourceApi129& api){
 if(!unit||!owner||!api.player||!api.local||!api.ally||!api.resolve)return false;
 __try {
  const auto object=owner(unit);if(!object)return false;
  const unsigned slot=*reinterpret_cast<const unsigned char*>(object+0x30);
  if(slot>=24)return false;
  const auto player=api.player(slot),local=api.local();
  if(!player||!local||api.resolve(player)!=object)return false;
  if(api.observer&&api.observer(local))return false;
  return player!=local&&!api.ally(local,player);
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline uint32_t ManaColor129(const Config& config,bool enemy){return enemy?config.manaBarColorEnemy:config.manaBarColor;}
using ManaCtor129=void*(__thiscall*)(void*,void*,uintptr_t,int);
using ManaBind129=void(__thiscall*)(uintptr_t,uintptr_t);
inline bool ManaEligible129(const Config& config,bool allowed,bool visible,bool hero,float maximum){
 return allowed&&visible&&config.manaBarEnable&&(hero?config.manaBarHero:config.manaBarNonHero)&&std::isfinite(maximum)&&maximum>0;
}
struct ManaBar129 {
 ResourceApi129 playerApi{};
 struct Entry{void* frame=nullptr;};
 std::unordered_map<uintptr_t,Entry> entries;
 bool active=false;
 void Forget(uintptr_t bar){entries.erase(bar);} // Child frames belong to native HP parent.
 template<class Api> void Update(uintptr_t base,uintptr_t hp,const Config& config,Api& api){
  uintptr_t unit=0;bool visible=false,hero=false;
  if(!SampleStateBar129(base,hp,unit,visible,hero))return;
  float current=0,maximum=0;
  const bool enabled=active&&visible&&config.manaBarEnable&&(hero?config.manaBarHero:config.manaBarNonHero)&&ReadStateValues129(base,unit,true,reinterpret_cast<RegenResolve129>(base+0x707f0),reinterpret_cast<StatRead129>(base+0x400d80),reinterpret_cast<StatRead129>(base+0x400e20),current,maximum)&&ManaEligible129(config,active,visible,hero,maximum);
  auto found=entries.find(hp);
  if(!enabled){if(found!=entries.end()&&found->second.frame){reinterpret_cast<ManaBind129>(base+0x25b7e0)(reinterpret_cast<uintptr_t>(found->second.frame),0);NativeFrameHide(found->second.frame);}return;}
  if(found==entries.end()){if(entries.size()>=4096)return;found=entries.emplace(hp,Entry{}).first;}
  auto& entry=found->second;
  if(!entry.frame){
   // 2218D0: CSimpleStatusBar parent, CUnit safe reference +158, kind +13C.
   // Kind 0 selects native mana fraction 235A00 in 25B7E0's jump table.
   auto memory=api.alloc(0x160,"WHTMana129",1,0);
   if(memory)entry.frame=reinterpret_cast<ManaCtor129>(base+0x2218d0)(memory,reinterpret_cast<void*>(hp),unit,0);
  }
  if(!entry.frame)return;
  const float width=*reinterpret_cast<float*>(hp+0x58),height=*reinterpret_cast<float*>(hp+0x5c);
  if(!std::isfinite(width)||!std::isfinite(height)||width<=0||height<=0||width>100||height>100){NativeFrameHide(entry.frame);return;}
  const float mpWidth=std::isfinite(config.barWidthMp)&&config.barWidthMp>0&&config.barWidthMp<=100?config.barWidthMp:width;
  const float mpHeight=std::isfinite(config.barHeightMp)&&config.barHeightMp>0&&config.barHeightMp<=100?config.barHeightMp:height;
  api.setWidth(entry.frame,mpWidth);api.setHeight(entry.frame,mpHeight);
  api.setRelPoint(entry.frame,1,reinterpret_cast<void*>(hp),7,0.f,-.001f,1);
  reinterpret_cast<ManaBind129>(base+0x25b7e0)(reinterpret_cast<uintptr_t>(entry.frame),unit);
  // CStatBar's foreground is CSimpleTexture at +138; native ctor uses 486360.
  auto texture=*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(entry.frame)+0x138);
  if(texture){const uint32_t color=ManaColor129(config,ManaEnemy129(unit,reinterpret_cast<ManaOwner129>(base+0x292340),playerApi));reinterpret_cast<void(__thiscall*)(void*,const uint32_t*)>(base+0x486360)(texture,&color);}
  NativeFrameShow(entry.frame);
 }
 template<class Api> void Apply(uintptr_t base,const Config& config,bool allowed,Api& api){active=allowed;for(auto& pair:entries)Update(base,pair.first,config,api);}
};
}
