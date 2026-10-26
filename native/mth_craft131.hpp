#pragma once
#include "mth_recipe.hpp"
#include "craft_selection131.hpp"
#include "courier_route131.hpp"
#include "inventory_info131.hpp"
#include "target_cast131.hpp"
#include "time_boss131.hpp"
#include "all_skills131.hpp"
#include <functional>
namespace wht {
inline constexpr const wchar_t* craftCommands131[]={L"#shield",L"#ice",L"#wild",L"#frozen",L"#phoenix"};
inline int CraftCommand131(const std::wstring& name){for(int i=0;i<5;++i)if(name==craftCommands131[i])return i;return -1;}
inline unsigned CraftSlot131(int memory){return memory>=0&&memory<12?unsigned((memory%3)*4+memory/3):12;}
inline const char* CraftNotice131(MthResult r){switch(r){
 case MthResult::Submitted:return "MTH: purchase/combine sequence submitted; game determines result";
 case MthResult::Disabled:return "MTH: crafting cancelled";
 case MthResult::Busy:return "MTH: another action is active";
 case MthResult::NoHero:return "MTH: select your living hero";
 case MthResult::Gold:return "MTH: insufficient gold for this recipe";
 case MthResult::NoShop:return "MTH: required shop not found within 777 of hero";
 case MthResult::Changed:return "MTH: selection, hero or match changed; stopped";
 case MthResult::Unavailable:return "MTH: shop command card unavailable; stopped";
 case MthResult::Failed:return "MTH: native action refused; no retry";
 default:return nullptr;}}
struct MthCraftRun131 {
 MthRecipe recipe;SelfCastSource131 hero,shop;unsigned stage=0;uint64_t entered=0;int kind=-1;bool active=false,queued=false;
 void Reset(){*this={};}
 template<class Api> MthResult Finish(MthResult result,Api& api){if(api.Valid(hero)&&api.Selected()==shop.unit)api.Restore(hero);Reset();return result;}
 template<class Api> MthResult Enter(uint64_t now,Api& api){
  SelfCastSource131 next;if(!api.FindShop(hero,recipe.stages[stage].shop,next))return Finish(MthResult::NoShop,api);
  if(!api.Select(next)){if(api.Selected()==next.unit)shop=next;return Finish(MthResult::Failed,api);}
  shop=next;entered=now;return MthResult::Running;
 }
 template<class Api> MthResult Start(int action,uint64_t now,bool allowed,Api& api){
  if(active)return MthResult::Busy;Reset();if(!allowed)return MthResult::Disabled;
  std::array<uint32_t,6> bag{};int gold=0;if(!api.Capture(hero,bag,gold))return MthResult::NoHero;
  recipe=MakeMthRecipe(action,bag);if(!recipe.count){Reset();return MthResult::Failed;}
  if(gold<recipe.gold){Reset();return MthResult::Gold;}kind=action;active=true;return Enter(now,api);
 }
 template<class Api> MthResult Queue(int action,bool allowed,Api& api){
  if(active)return MthResult::Busy;Reset();if(!allowed)return MthResult::Disabled;
  std::array<uint32_t,6> bag{};int gold=0;if(!api.Capture(hero,bag,gold))return MthResult::NoHero;
  const auto requested=MakeMthRecipe(action,bag);if(!requested.count){Reset();return MthResult::Failed;}if(gold<requested.gold){Reset();return MthResult::Gold;}
  kind=action;active=queued=true;return MthResult::Running;
 }
 template<class Api> MthResult Tick(uint64_t now,bool allowed,bool ready,Api& api){
  if(!active)return MthResult::Idle;
  if(!api.Valid(hero)){Reset();return MthResult::Changed;}
  if(!allowed)return Finish(MthResult::Disabled,api);
  if(queued){if(api.Selected()!=hero.unit){Reset();return MthResult::Changed;}if(!ready)return MthResult::Running;const int action=kind;Reset();return Start(action,now,true,api);}
  if(api.Selected()!=shop.unit){Reset();return MthResult::Changed;}
  // Paused UI contexts never click or alter the user's selection.
  if(!ready)return MthResult::Running;
  if(now-entered<100)return MthResult::Running;
  if(!api.ShopValid(hero,shop,recipe.stages[stage].shop))return Finish(MthResult::NoShop,api);
  const auto& step=recipe.stages[stage];
  if(!api.Ready(step)){if(now-entered<1500)return MthResult::Running;return Finish(MthResult::Unavailable,api);}
  for(unsigned i=0;i<step.count;++i)if(!api.Valid(hero)||api.Selected()!=shop.unit||!api.ShopValid(hero,shop,step.shop)||!api.Click(CraftSlot131(step.slots[i])))return Finish(MthResult::Failed,api);
  if(++stage<recipe.count)return Enter(now,api);return Finish(MthResult::Submitted,api);
 }
};
// All native calls reuse verified 1.31 profiles. No 1.29 native adapter is used.
struct MthCraftApi131 {
 uintptr_t base=0,ui=0,worldFrame=0;unsigned build=0;SelectionRoster131& saved;
 std::function<uintptr_t()> selected;std::function<bool(uintptr_t)> alive;
 std::function<SkillButton131(unsigned)> data;std::function<bool(unsigned)> click;
 bool Read(uintptr_t unit,SelfCastSource131& out)const{return ReadCastTarget131(base,build,ui,unit,[&](uintptr_t v){return alive(v);},out);}
 bool Owned(uintptr_t unit)const{auto p=InventoryProfile(build);auto n=DropNetworkProfileFor131(build);return p&&n&&DropOwned131(base,build,DropNetworkNative131(base,*n,*p),unit);}
 bool Hero(uintptr_t unit)const{auto p=SkillProfile(build);return p&&HeroPoints131(unit,base,*p)>=0;}
 uintptr_t Selected()const{return selected();}
 bool Valid(const SelfCastSource131& source)const{SelfCastSource131 current;return Read(source.unit,current)&&SameSelfCastSource131(source,current)&&Owned(source.unit)&&Hero(source.unit);}
 bool Capture(SelfCastSource131& source,std::array<uint32_t,6>& bag,int& gold){
  InventoryInfo131 inventory;auto r=ResourceProfileFor131(build);const auto unit=Selected();
  if(!r||!ResourceCode131(base,*r)||!Read(unit,source)||!Owned(unit)||!Hero(unit)||!ReadInventoryInfo131(base,build,[&](){return Selected();},inventory)||!ReadSelectionRoster131(base,build,saved))return false;
  __try{unsigned local=24;auto world=ResourceWorld131(base,*r,local);if(!world)return false;
   auto player=reinterpret_cast<decltype(ResourceNative131::player)>(base+r->playerFn)(world,local);if(!ResourceIdentity131(player,base,r->playerTable))return false;
   gold=reinterpret_cast<decltype(ResourceNative131::integer)>(base+r->integerFn)(player+r->stateBase+r->stateStride)/10;
   bag=inventory.codes;return Selected()==unit&&Valid(source);
  }__except(1){return false;}
 }
 bool Code(uintptr_t unit,uint32_t code)const{auto p=BossProfileFor131(build);if(!p||!BossNative131(base,*p)||!InventoryIdentity131(unit,base,p->unit))return false;
  __try{return *reinterpret_cast<uint32_t*>(unit+bossCodeOffset131)==code;}__except(1){return false;}}
 bool ShopValid(const SelfCastSource131& source,const SelfCastSource131& shop,uint32_t code)const{
  SelfCastSource131 h,s;if(!Valid(source)||!Read(source.unit,h)||!Read(shop.unit,s)||!SameSelfCastSource131(shop,s)||source.session!=s.session||!Code(s.unit,code))return false;
  const double dx=double(h.x)-s.x,dy=double(h.y)-s.y;return dx*dx+dy*dy<=777.*777.;
 }
 bool FindShop(const SelfCastSource131& source,uint32_t code,SelfCastSource131& out)const{
  auto p=SellProfileFor131(build);auto pp=PickProfileFor131(build);if(!p||!pp||!InventoryIdentity131(worldFrame,base,pp->worldTable))return false;
  auto api=SellNative131(base,*p);if(!api||!Valid(source))return false;
  __try{auto list=api.list(worldFrame,true);if(list!=worldFrame+p->listOffset)return false;
   const unsigned count=*reinterpret_cast<unsigned*>(list);auto units=*reinterpret_cast<uintptr_t*>(list+sizeof(uintptr_t));if(count>8192||(count&&!units))return false;
   for(unsigned i=0;i<count;++i){const auto unit=*reinterpret_cast<uintptr_t*>(units+i*sizeof(uintptr_t));if(!Code(unit,code))continue;SelfCastSource131 candidate;if(Read(unit,candidate)&&ShopValid(source,candidate,code)){out=candidate;return true;}}return false;
  }__except(1){return false;}
 }
 bool Select(const SelfCastSource131& target)const{SelfCastSource131 current;if(!Read(target.unit,current)||!SameSelfCastSource131(target,current))return false;SelectionRoster131 one;one.count=1;one.units[0]=target.unit;one.ids[0]=target.id;return CraftSelectRoster131(base,build,ui,one);}
 void Restore(const SelfCastSource131& source)const{
  auto p=InventoryProfile(build);if(!p||!Valid(source))return;SelectionRoster131 live;
  __try{for(unsigned i=0;i<saved.count;++i)if(InventoryIdentity131(saved.units[i],base,p->unitTable)&&DropIdentity131(saved.units[i])==saved.ids[i]){live.units[live.count]=saved.units[i];live.ids[live.count]=saved.ids[i];++live.count;}}__except(1){return;}
  CraftSelectRoster131(base,build,ui,live);
 }
 bool Ready(const MthStage& step)const{for(unsigned i=0;i<step.count;++i)if(!data(CraftSlot131(step.slots[i])).order)return false;return true;}
 bool Click(unsigned slot)const{return slot<12&&click(slot);}
};
}
