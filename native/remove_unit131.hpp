#pragma once
#include "remove_unit_profile131.hpp"
#include "reselect131.hpp"
namespace wht {
inline const RemoveUnitProfile131* RemoveUnitProfileFor131(unsigned build){for(const auto& p:removeUnitProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool RemoveUnitCode131(uintptr_t base,const RemoveUnitProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};const auto* inv=InventoryProfile(p.build);if(!inv||!ResourceTextGuard131(base,code))return false;__try{return *reinterpret_cast<uintptr_t*>(base+inv->unitTable+(sizeof(uintptr_t)==8?0xf8:0x7c))==base+p.prepare;}__except(1){return false;}}
using RemoveUnitAgent131=uintptr_t(__cdecl*)(uint32_t,uint32_t);
using PrepareRemoveUnit131=void(__thiscall*)(uintptr_t);
using RemoveUnitNative131=void(__thiscall*)(uintptr_t,unsigned,unsigned);
struct RemoveUnitSource131 {uintptr_t unit=0,world=0,agent=0;DropObjectId id;unsigned local=24;};
inline bool SameRemoveUnit131(const RemoveUnitSource131& a,const RemoveUnitSource131& b){return a.unit&&a.world&&a.agent&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.world==b.world&&a.agent==b.agent&&a.id==b.id&&a.local==b.local;}
inline uintptr_t ResolveRemoveUnitAgent131(uintptr_t base,const RemoveUnitProfile131& p,const DropObjectId& id){if(!id.Valid())return 0;__try{const auto agent=reinterpret_cast<RemoveUnitAgent131>(base+p.agent)(id.low,id.high);return agent&&*reinterpret_cast<uint32_t*>(agent+(sizeof(uintptr_t)==8?0x18:0xc))==0x2b61676c?agent:0;}__except(1){return 0;}}
inline bool ValidRemoveUnit131(uintptr_t base,unsigned build,const RemoveUnitSource131& value){
 const auto* p=RemoveUnitProfileFor131(build);const auto* rp=ResourceProfileFor131(build);const auto* inv=InventoryProfile(build);if(!p||!rp||!inv||!value.unit||!value.agent||!RemoveUnitCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{unsigned local=24;if(ResourceWorld131(base,*rp,local)!=value.world||local!=value.local||ResolveRemoveUnitAgent131(base,*p,value.id)!=value.agent)return false;
 // Registry identity must still resolve before touching the original CUnit.
 return InventoryIdentity131(value.unit,base,inv->unitTable)&&DropIdentity131(value.unit)==value.id;
 }__except(1){return false;}
}
template<class Selected> bool ReadRemoveUnit131(uintptr_t base,unsigned build,Selected selected,RemoveUnitSource131& out){
 const auto* p=RemoveUnitProfileFor131(build);const auto* rp=ResourceProfileFor131(build);const auto* inv=InventoryProfile(build);if(!p||!rp||!inv||!RemoveUnitCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{RemoveUnitSource131 value;value.unit=selected();if(!InventoryIdentity131(value.unit,base,inv->unitTable))return false;value.id=DropIdentity131(value.unit);value.world=ResourceWorld131(base,*rp,value.local);value.agent=ResolveRemoveUnitAgent131(base,*p,value.id);
 if(selected()!=value.unit||!ValidRemoveUnit131(base,build,value))return false;out=value;return true;
 }__except(1){return false;}
}
inline bool InvokePrepareRemoveUnit131(PrepareRemoveUnit131 prepare,uintptr_t unit){if(!prepare||!unit)return false;__try{prepare(unit);return true;}__except(1){return false;}}
inline bool InvokeRemoveUnit131(RemoveUnitNative131 remove,uintptr_t unit){if(!remove||!unit)return false;__try{remove(unit,1,1);return true;}__except(1){return false;}}
template<class Read,class Valid,class Prepare,class Remove> bool RemoveUnitCommand131(const std::wstring& args,Read read,Valid valid,Prepare prepare,Remove remove,std::string& report){
 if(!UnitInfoArgs131(args)){report="Usage: select a unit, then #udel";return false;}
 RemoveUnitSource131 before,current;if(!read(before)||!read(current)||!SameRemoveUnit131(before,current)||!valid(current)){report="RemoveUnit: select a stable registered unit";return false;}
 if(!prepare(current)){report="RemoveUnit: native preparation failed; unit may have changed";return false;}
 // Preparation may clear selection or invoke map callbacks; validate original registry identity.
 if(!valid(current)){report="RemoveUnit: unit or match changed during preparation";return false;}
 if(!remove(current)){report="RemoveUnit: native removal failed; update may have applied";return false;}
 // Never access the unit after native removal, which may release it immediately.
 report="RemoveUnit invoked for selected unit";return true;
}
inline bool PrepareSelectedRemoval131(uintptr_t base,unsigned build,const RemoveUnitSource131& value){const auto* p=RemoveUnitProfileFor131(build);return p&&ValidRemoveUnit131(base,build,value)&&InvokePrepareRemoveUnit131(reinterpret_cast<PrepareRemoveUnit131>(base+p->prepare),value.unit);}
inline bool RemoveSelectedUnitNative131(uintptr_t base,unsigned build,const RemoveUnitSource131& value){const auto* p=RemoveUnitProfileFor131(build);return p&&ValidRemoveUnit131(base,build,value)&&InvokeRemoveUnit131(reinterpret_cast<RemoveUnitNative131>(base+p->remove),value.unit);}
}
