#pragma once
#include "remove_ability_profile131.hpp"
#include "reselect131.hpp"
#include <sstream>
namespace wht {
inline const RemoveAbilityProfile131* RemoveAbilityProfileFor131(unsigned build){for(const auto& p:removeAbilityProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool RemoveAbilityCode131(uintptr_t base,const RemoveAbilityProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};if(!ResourceTextGuard131(base,code))return false;__try{const auto at=base+p.caller;return *reinterpret_cast<unsigned char*>(at)==0xe8&&at+5+*reinterpret_cast<int32_t*>(at+1)==base+p.remove;}__except(1){return false;}}
inline bool ParseRemoveAbility131(const std::wstring& args,uint32_t& code){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring token,extra;if(!(in>>token)||(in>>extra))return false;uint32_t value=0;
 if(token.size()>=2&&token[0]==L'0'&&(token[1]==L'x'||token[1]==L'X')){if(token.size()!=10)return false;for(size_t i=2;i<10;++i){const auto c=token[i];unsigned digit=0;if(c>=L'0'&&c<=L'9')digit=unsigned(c-L'0');else if(c>=L'a'&&c<=L'f')digit=unsigned(c-L'a')+10;else if(c>=L'A'&&c<=L'F')digit=unsigned(c-L'A')+10;else return false;value=(value<<4)|digit;}}
 else{if(token.size()!=4)return false;for(auto c:token){if(c<33||c>126)return false;value=(value<<8)|uint32_t(c);}}
 if(!value)return false;code=value;return true;
}
inline std::string RemoveAbilityName131(uint32_t code){std::string value;for(unsigned i=0;i<4;++i){const unsigned c=(code>>(24-8*i))&255;value+=c>=32&&c<=126?char(c):'?';}return value;}
using RemoveAbilityNative131=void(__thiscall*)(uintptr_t,uintptr_t);
using RefreshAbilityNative131=void(__cdecl*)(uintptr_t);
inline bool InvokeRemoveAbility131(RemoveAbilityNative131 remove,uintptr_t unit,uintptr_t ability){if(!remove||!unit||!ability)return false;__try{remove(unit,ability);return true;}__except(1){return false;}}
inline bool InvokeRefreshAbility131(RefreshAbilityNative131 refresh,uintptr_t unit){if(!refresh||!unit)return false;__try{refresh(unit);return true;}__except(1){return false;}}
struct RemoveAbilitySource131 {uintptr_t unit=0,world=0,ability=0,abilityTable=0;DropObjectId id,abilityId;unsigned local=24;};
inline bool SameRemovalUnit131(const RemoveAbilitySource131& a,const RemoveAbilitySource131& b){return a.unit&&a.world&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.world==b.world&&a.local==b.local&&a.id==b.id;}
inline bool SameRemovalAbility131(const RemoveAbilitySource131& a,const RemoveAbilitySource131& b){return a.ability==b.ability&&a.abilityTable==b.abilityTable&&a.abilityId==b.abilityId;}
inline bool ReadRemovalAbilityIdentity131(uintptr_t base,uintptr_t ability,uintptr_t& table,DropObjectId& id){
 __try{if(!base||!ability)return false;const auto pe=*reinterpret_cast<uint32_t*>(base+60);if(pe>0x1000)return false;const auto size=*reinterpret_cast<uint32_t*>(base+pe+24+56);if(size<0x1000||size>0x10000000)return false;
  table=*reinterpret_cast<uintptr_t*>(ability);if(table<base+0x1000||table>base+size-sizeof(uintptr_t))return false;id=DropIdentity131(ability);return true;
 }__except(1){return false;}
}
template<class Selected> bool ReadRemoveAbility131(uintptr_t base,unsigned build,uint32_t code,Selected selected,RemoveAbilitySource131& out,unsigned includeHidden=0){
 const auto* p=RemoveAbilityProfileFor131(build);const auto* inv=InventoryProfile(build);const auto* rp=ResourceProfileFor131(build);const auto* sp=SellProfileFor131(build);
 if(!code||!p||!inv||!rp||!sp||!RemoveAbilityCode131(base,*p)||!ResourceCode131(base,*rp))return false;const auto api=SellNative131(base,*sp);if(!api)return false;
 __try{RemoveAbilitySource131 value;value.unit=selected();if(!InventoryIdentity131(value.unit,base,inv->unitTable))return false;value.id=DropIdentity131(value.unit);if(!value.id.Valid())return false;
  value.world=ResourceWorld131(base,*rp,value.local);if(!value.world)return false;value.ability=api.ability(value.unit,code,includeHidden,1,1,1,0);
  if(value.ability&&!ReadRemovalAbilityIdentity131(base,value.ability,value.abilityTable,value.abilityId))return false;
  unsigned local=24;if(selected()!=value.unit||!(DropIdentity131(value.unit)==value.id)||!InventoryIdentity131(value.unit,base,inv->unitTable)||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local)return false;
  out=value;return true;
 }__except(1){return false;}
}
template<class Read,class Remove,class Refresh> bool RemoveAbilityCommand131(const std::wstring& args,Read read,Remove remove,Refresh refresh,std::string& report){
 uint32_t code=0;if(!ParseRemoveAbility131(args,code)){report="Usage: select a unit, then #rabi AHhb (or #rabi 0x41486862)";return false;}
 RemoveAbilitySource131 before,current,after,final;
 if(!read(code,before)||!read(code,current)||!SameRemovalUnit131(before,current)||!SameRemovalAbility131(before,current)){report="Remove Ability: select a stable unit and ability";return false;}
 if(!before.ability){report="Ability already absent: "+RemoveAbilityName131(code);return true;}
 if(!remove(before.unit,before.ability)){report="Remove Ability: native removal failed";return false;}
 // Query again rather than touching the removed ability, which may already be freed.
 if(!read(code,after)||!SameRemovalUnit131(before,after)||after.ability){report="Remove Ability: native result or selection changed; removal may have applied";return false;}
 if(!refresh(before.unit)){report="Remove Ability: removed, but panel refresh failed";return false;}
 if(!read(code,final)||!SameRemovalUnit131(before,final)||final.ability){report="Remove Ability: game did not confirm absence after refresh";return false;}
 report="Removed Ability: "+RemoveAbilityName131(code);return true;
}
inline bool RemoveSelectedAbilityNative131(uintptr_t base,unsigned build,uintptr_t unit,uintptr_t ability){const auto* p=RemoveAbilityProfileFor131(build);return p&&RemoveAbilityCode131(base,*p)&&InvokeRemoveAbility131(reinterpret_cast<RemoveAbilityNative131>(base+p->remove),unit,ability);}
inline bool RefreshRemovedAbilityNative131(uintptr_t base,unsigned build,uintptr_t unit){const auto* p=RemoveAbilityProfileFor131(build);return p&&RemoveAbilityCode131(base,*p)&&InvokeRefreshAbility131(reinterpret_cast<RefreshAbilityNative131>(base+p->refresh),unit);}
}
