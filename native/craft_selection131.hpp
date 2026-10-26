#pragma once
#include "craft_selection_profile131.hpp"
namespace wht {
inline bool CraftSelectionGuard131(uintptr_t base,const CraftSelectionProfile131& p){
 ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);
}
using CraftSelect131=void(__thiscall*)(uintptr_t,const NativeUnitList131*,unsigned,unsigned,unsigned,unsigned,unsigned);
inline void InvokeCraftSelect131(CraftSelect131 fn,uintptr_t manager,const NativeUnitList131& list){fn(manager,&list,0,1,0,0,0);}
inline bool CraftSelectRoster131(uintptr_t base,unsigned build,uintptr_t ui,const SelectionRoster131& roster){
 if(!base||!ui||roster.count>12)return false;const CraftSelectionProfile131* p=nullptr;for(const auto& candidate:craftSelectionProfiles131)if(candidate.build==build)p=&candidate;
 auto inv=InventoryProfile(build);if(!p||!inv||!CraftSelectionGuard131(base,*p))return false;
 __try{
  const auto manager=*reinterpret_cast<uintptr_t*>(ui+p->member);if(!InventoryIdentity131(manager,base,p->table))return false;
  for(unsigned i=0;i<roster.count;++i)if(!InventoryIdentity131(roster.units[i],base,inv->unitTable)||!(DropIdentity131(roster.units[i])==roster.ids[i]))return false;
  const NativeUnitList131 list{roster.count,roster.units.data()};
  // Exact native hero-bar group-selection policy: replace, select, zero flags.
  InvokeCraftSelect131(reinterpret_cast<CraftSelect131>(base+p->select),manager,list);return true;
 }__except(1){return false;}
}
}
