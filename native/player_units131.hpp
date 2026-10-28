#pragma once
#include "player_units_profile131.hpp"
#include "add_ability131.hpp"
#include "resource_commands131.hpp"
#include <atomic>
#include <vector>
namespace wht {
inline const PlayerUnitsProfile131* PlayerUnitsProfileFor131(unsigned build){for(const auto& p:playerUnitsProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool PlayerUnitsCode131(uintptr_t base,const PlayerUnitsProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
using PlayerUnitVisit131=bool(__cdecl*)(uintptr_t,void*);
using PlayerUnitEnumerate131=void(__cdecl*)(uint32_t,PlayerUnitVisit131,void*,bool);
inline bool InvokePlayerUnits131(PlayerUnitEnumerate131 enumerate,PlayerUnitVisit131 visit,void* context){if(!enumerate||!visit||!context)return false;__try{enumerate(0x2b773375,visit,context,false);return true;}__except(1){return false;}}
inline bool PlayerUnitOwner131(uintptr_t base,unsigned build,uintptr_t unit,unsigned& owner){const auto* inv=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(!inv||!np)return false;const auto api=DropNetworkNative131(base,*np,*inv);if(!api)return false;__try{owner=api.owner(unit);return owner<28;}__except(1){return false;}}
inline bool PlayerUnitContext131(uintptr_t base,unsigned build,const ResourceCommandSource131& source){ResourceCommandSource131 current;return ReadResourceCommand131(base,build,current)&&SameResourcePlayer131(source,current);}
inline bool PlayerUnitValid131(uintptr_t base,unsigned build,const ResourceCommandSource131& context,const RemoveUnitSource131& unit){unsigned owner=28;return unit.world==context.world&&unit.local==context.local&&PlayerUnitContext131(base,build,context)&&ValidRemoveUnit131(base,build,unit)&&PlayerUnitOwner131(base,build,unit.unit,owner)&&owner==context.physical;}
struct PlayerUnitCollect131 {uintptr_t base=0;unsigned build=0,visited=0;ResourceCommandSource131 context;std::vector<RemoveUnitSource131>* units=nullptr;bool failed=false;};
inline bool __cdecl CollectPlayerUnit131(uintptr_t unit,void* pointer){
 auto& state=*static_cast<PlayerUnitCollect131*>(pointer);
 // Collection performs no ability changes and cannot invalidate registry links.
 try{
  if(++state.visited>65536||!PlayerUnitContext131(state.base,state.build,state.context)){state.failed=true;return false;}
  unsigned owner=28;if(!PlayerUnitOwner131(state.base,state.build,unit,owner)){state.failed=true;return false;}if(owner!=state.context.physical)return true;
  RemoveUnitSource131 source;if(!ReadRemoveUnit131(state.base,state.build,[&](){return unit;},source)||!PlayerUnitValid131(state.base,state.build,state.context,source)||state.units->size()>=8192){state.failed=true;return false;}
  for(const auto& previous:*state.units)if(previous.id==source.id){state.failed=true;return false;}
  state.units->push_back(source);return true;
 }catch(...){state.failed=true;return false;}
}
inline bool ReadPlayerUnits131(uintptr_t base,unsigned build,const ResourceCommandSource131& context,std::vector<RemoveUnitSource131>& units){
 const auto* p=PlayerUnitsProfileFor131(build);if(!p||!PlayerUnitsCode131(base,*p)||!PlayerUnitContext131(base,build,context))return false;
 units.clear();units.reserve(8192);PlayerUnitCollect131 state{base,build,0,context,&units,false};
 if(!InvokePlayerUnits131(reinterpret_cast<PlayerUnitEnumerate131>(base+p->enumerate),CollectPlayerUnit131,&state)||state.failed||!state.visited||!PlayerUnitContext131(base,build,context)){units.clear();return false;}
 for(const auto& unit:units)if(!PlayerUnitValid131(base,build,context,unit)){units.clear();return false;}return true;
}
template<class Valid,class Apply> bool PlayerAbilitiesBatch131(const std::vector<RemoveUnitSource131>& units,Valid valid,Apply apply,std::string& report){
 if(units.size()>8192){report="Player Abilities: unit limit exceeded; no action taken";return false;}
 // Validate the complete snapshot before the first mutation.
 for(size_t i=0;i<units.size();++i){if(!valid(units[i])){report="Player Abilities: roster changed; no action taken";return false;}for(size_t j=0;j<i;++j)if(units[i].id==units[j].id){report="Player Abilities: duplicate unit; no action taken";return false;}}
 size_t confirmed=0;for(const auto& unit:units){std::string detail;if(!valid(unit)||!apply(unit,detail)||!valid(unit)){report="Player Abilities: confirmed "+std::to_string(confirmed)+"/"+std::to_string(units.size())+"; stopped (latest update may have applied)";if(!detail.empty())report+="; "+detail;return false;}++confirmed;}
 report="Player Abilities: confirmed "+std::to_string(confirmed)+"/"+std::to_string(units.size());return true;
}
inline std::atomic_flag playerAbilitiesBusy131=ATOMIC_FLAG_INIT;
inline bool PlayerAbilitiesCommand131(uintptr_t base,unsigned build,bool add,const std::wstring& args,std::string& report){
 uint32_t code=0;if(!ParseRemoveAbility131(args,code)){report=add?"Usage: #addall AHhb":"Usage: #raddall AHhb";return false;}
 if(playerAbilitiesBusy131.test_and_set()){report="Player Abilities: batch already running";return false;}
 struct Release {~Release(){playerAbilitiesBusy131.clear();}} release;
 ResourceCommandSource131 context;std::vector<RemoveUnitSource131> units;
 const auto* ap=AddAbilityProfileFor131(build);const auto* rp=RemoveAbilityProfileFor131(build);
 if(!ap||!rp||!AddAbilityCode131(base,*ap)||!RemoveAbilityCode131(base,*rp)||!ReadResourceCommand131(base,build,context)||!ReadPlayerUnits131(base,build,context,units)){report="Player Abilities: native roster unavailable; no action taken";return false;}
 auto valid=[&](const RemoveUnitSource131& unit){return PlayerUnitValid131(base,build,context,unit);};
 const bool ok=PlayerAbilitiesBatch131(units,valid,[&](const RemoveUnitSource131& unit,std::string& detail){
  auto selected=[&]()->uintptr_t{return valid(unit)?unit.unit:0;};
  if(add)return AddAbilityCommand131(base,build,args,selected,detail);
  return RemoveAbilityCommand131(args,[&](uint32_t raw,RemoveAbilitySource131& out){return ReadRemoveAbility131(base,build,raw,selected,out);},[&](uintptr_t u,uintptr_t ability){return valid(unit)&&RemoveSelectedAbilityNative131(base,build,u,ability);},[&](uintptr_t u){return valid(unit)&&RefreshRemovedAbilityNative131(base,build,u);},detail);
 },report);report=std::string(add?"Add All [":"Remove All [")+RemoveAbilityName131(code)+"] | "+report;return ok;
}
}
