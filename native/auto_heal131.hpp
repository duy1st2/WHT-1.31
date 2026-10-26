#pragma once
#include "auto_heal.hpp"
#include "auto_skills_native131.hpp"
namespace wht {
struct AutoHeal131 {
 AutoHealState state;
 void Reset(){state={};}
 template<class Sample> bool Tick(uint64_t now,const Config& config,bool allowed,AutoSkillsApi131& api,Sample sample,bool mana=false){
  const bool enabled=mana?config.autoManaEnabled:config.autoHealEnabled;const auto& items=mana?config.autoManaItems:config.autoHealItems;const float percent=mana?config.autoManaPercent:config.autoHealPercent;
  if(!enabled||!allowed||items.empty())return false;
  SelfCastSource131 expected;InventoryInfo131 bag;
  return state.Tick(enabled,allowed,percent,items,now,[&](AutoHealSample& out){
   if(!api.ReadSource(expected)||!sample(expected.unit,out.value,out.maximum)||!ReadInventoryInfo131(api.core.base,api.core.build,[&](){return api.core.Selected();},bag))return false;
   out.identity=1;out.items=bag.codes;return true;
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
