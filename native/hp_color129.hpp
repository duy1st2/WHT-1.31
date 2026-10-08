#pragma once
#include "state_values129.hpp"
#include "player_colors129.hpp"
#include "mana_bar129.hpp"
namespace wht {
inline bool HpConfiguredColor129(bool playerMode,uint32_t normal,uint32_t enemyColor,bool enemy,bool knownPlayer,uint32_t playerColor,uint32_t& output){
 if(playerMode&&!knownPlayer)return false;
 output=playerMode?playerColor:enemy?enemyColor:normal;return true;
}
using UnitOwnerObject129=uintptr_t(__thiscall*)(uintptr_t);
using TextureColor129=void(__thiscall*)(void*,const uint32_t*);
inline bool PlayerHpColor129(unsigned index,uint32_t& color){
 return PlayerColor129(index,color);
}
inline bool UnitHpColor129(uintptr_t unit,UnitOwnerObject129 owner,uint32_t& color){
 __try{if(!unit||!owner)return false;const auto player=owner(unit);return player&&PlayerHpColor129(*reinterpret_cast<unsigned*>(player+0x2c4),color);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool HpTexture129(uintptr_t base,uintptr_t hp,uintptr_t& texture,uint32_t& color){
 __try{
  if(!hp||*reinterpret_cast<uintptr_t*>(hp)!=base+0xa95104||*reinterpret_cast<unsigned*>(hp+0x13c)!=5)return false;
  texture=*reinterpret_cast<uintptr_t*>(hp+0x138);if(!texture)return false;
  const auto bytes=reinterpret_cast<unsigned char*>(texture+0x6c);
  color=(uint32_t(bytes[0])<<24)|(uint32_t(bytes[3])<<16)|(uint32_t(bytes[2])<<8)|bytes[1];return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct HpColor129 {
 ResourceApi129 playerApi{};
 bool playerMode=false;uint32_t normalColor=0xff32cd32,enemyColor=0xffff4242;
 struct Entry{uintptr_t texture=0;uint32_t original=0,written=0;bool tinted=false;};
 std::unordered_map<uintptr_t,Entry> entries;
 bool active=false;
 void Forget(uintptr_t hp){entries.erase(hp);}
 template<class Setter> static void Tint(Entry& entry,uintptr_t texture,uint32_t nativeColor,bool enabled,uint32_t desired,Setter set){
  if(entry.texture!=texture)entry={texture,nativeColor,0,false};
  if(!entry.tinted||nativeColor!=entry.written)entry.original=nativeColor;
  if(enabled){entry.written=desired;entry.tinted=true;if(nativeColor!=desired)set(texture,desired);}
  else if(entry.tinted){if(nativeColor==entry.written)set(texture,entry.original);entry.tinted=false;}
 }
 void Update(uintptr_t base,uintptr_t hp){
  uintptr_t texture=0;uint32_t nativeColor=0,desired=0;if(!HpTexture129(base,hp,texture,nativeColor))return;
  auto found=entries.find(hp);if(found==entries.end()){if(!active||entries.size()>=4096)return;found=entries.emplace(hp,Entry{}).first;}
  uintptr_t unit=0;bool visible=false,hero=false;bool enabled=active&&SampleStateBar129(base,hp,unit,visible,hero);
  if(enabled){
   const auto owner=reinterpret_cast<UnitOwnerObject129>(base+0x292340);
   uint32_t playerColor=0;const bool known=playerMode&&UnitHpColor129(unit,owner,playerColor);
   const bool enemy=!playerMode&&ManaEnemy129(unit,owner,playerApi);
   enabled=HpConfiguredColor129(playerMode,normalColor,enemyColor,enemy,known,playerColor,desired);
  }
  Tint(found->second,texture,nativeColor,enabled,desired,[base](uintptr_t t,uint32_t c){reinterpret_cast<TextureColor129>(base+0x486360)(reinterpret_cast<void*>(t),&c);});
 }
 void Apply(uintptr_t base,bool enabled,const Config& config){active=enabled;playerMode=config.colorPlayer;normalColor=config.hpBarColor;enemyColor=config.hpBarColorEnemy;for(auto& pair:entries)Update(base,pair.first);}
};
}
