#pragma once
#include "auto_heal.hpp"
#include "auto_skills_native131.hpp"
#include "potion_capability.hpp"
namespace wht {
inline unsigned PotionButtonCapability131(uintptr_t base,unsigned build,uintptr_t button,bool& unavailable){
 unavailable=false;
 if(!base||!button||(build!=12164&&build!=12173))return 0;
 __try {
#ifdef _WIN64
  const auto getter=base+(build==12164?0xcc3ee0:0xd03dc0);
  const unsigned char bytes[]={0x48,0x8b,0x81,0x78,0x2d,0,0,0xc3};
  if(memcmp(reinterpret_cast<void*>(getter),bytes,sizeof(bytes)))return 0;
  const auto data=*reinterpret_cast<uintptr_t*>(button+0x270);
  const auto ability=data?*reinterpret_cast<uintptr_t*>(data+0x2d78):0;
#else
  const auto getter=base+(build==12164?0xb6d2a0:0xb2cfa0);
  // Same native bound-ability getter used by the cooldown overlay.
  const auto data=*reinterpret_cast<uintptr_t*>(button+0x190);
  const auto ability=data?reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(getter)(data):0;
#endif
  const auto capability=NativePotionCapability(base,ability);
  if(capability){
   const auto vt=*reinterpret_cast<uintptr_t*>(ability);
   const auto pe=reinterpret_cast<const IMAGE_NT_HEADERS*>(base+reinterpret_cast<const IMAGE_DOS_HEADER*>(base)->e_lfanew);
   const auto end=base+pe->OptionalHeader.SizeOfImage;
   constexpr unsigned cooldownSlot=sizeof(uintptr_t)==8?0x5c0:0x2e0;
   if(vt>end-cooldownSlot-sizeof(uintptr_t)){unavailable=true;return capability;}
   const auto query=*reinterpret_cast<uintptr_t*>(vt+cooldownSlot);
   if(query<base||query>=end){unavailable=true;return capability;}
   float seconds=0;const auto order=*reinterpret_cast<unsigned*>(data+(sizeof(uintptr_t)==8?12:8));
   reinterpret_cast<float*(__thiscall*)(uintptr_t,float*,unsigned)>(query)(ability,&seconds,order);
   unavailable=!std::isfinite(seconds)||seconds>0;
  }
  return capability;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
struct AutoHeal131 {
 AutoHealState state;
 void Reset(){state={};}
 template<class Sample> bool Tick(uint64_t now,const Config& config,bool allowed,AutoSkillsApi131& api,Sample sample,bool mana=false){
  const bool enabled=mana?config.autoManaEnabled:config.autoHealEnabled;const auto& items=mana?config.autoManaItems:config.autoHealItems;const float percent=mana?config.autoManaPercent:config.autoHealPercent;
  if(!enabled||!allowed)return false;
  SelfCastSource131 expected;InventoryInfo131 bag;
  return state.Tick(enabled,allowed,percent,items,now,[&](AutoHealSample& out){
   if(!api.ReadSource(expected)||!sample(expected.unit,out.value,out.maximum)||!ReadInventoryInfo131(api.core.base,api.core.build,[&](){return api.core.Selected();},bag))return false;
   out.identity=1;out.items=bag.codes;
   const auto* p=InventoryProfile(api.core.build);const auto native=p?InventoryNative131(api.core.base,*p):InventoryApi131{};
   const auto usable=p&&native?InventoryMask131(api.core.base,*p,native,expected.unit,true):0;
   out.unavailableMask=(~usable)&63;
   for(unsigned i=0;i<6;++i)if(out.items[i]&&(usable&(1u<<i))){
    const auto button=api.button(i+12);bool unavailable=false;
    const auto capability=PotionButtonCapability131(api.core.base,api.core.build,button,unavailable);
    if(capability&(mana?2u:1u))out.detectedMask|=1u<<i;else if(capability)out.unavailableMask|=1u<<i;
    if(unavailable||!SkillButtonData131(button).order)out.unavailableMask|=1u<<i;
   }
   return true;
  },[&](uint64_t,unsigned slot,uint32_t code){
   const auto* p=InventoryProfile(api.core.build);const auto native=p?InventoryNative131(api.core.base,*p):InventoryApi131{};
   if(!p||!native||!(InventoryMask131(api.core.base,*p,native,expected.unit,true)&(1u<<slot)))return false;
   const auto cast=SelfCastNative131(api.core.base,api.core.build);if(!cast)return false;
   auto read=[&](SelfCastSource131& source){InventoryInfo131 current;float hp=0,max=0;return api.ReadSource(source)&&SameSelfCastSource131(source,expected)&&sample(source.unit,hp,max)&&std::isfinite(hp)&&std::isfinite(max)&&max>0&&(mana?hp>=0:hp>.405f)&&double(hp)*100<double(max)*percent&&ReadInventoryInfo131(api.core.base,api.core.build,[&](){return api.core.Selected();},current)&&current.codes[slot]==code;};
   return ActivateAutoSkill131(read,read,[&](){return api.Target();},[&](){return api.core.click(slot+12);},SelfCastOrder131,[&](const SelfCastSource131& source,uintptr_t mode,unsigned order){return FinishSelfCast131(cast,source,mode,order);},[&](uintptr_t mode){api.CancelOwned(mode,cast);});
  },mana);
 }
};
}
