#pragma once
#include "ability_button_profile131.hpp"
#include "remove_ability131.hpp"
namespace wht {
inline const AbilityButtonProfile131* AbilityButtonProfileFor131(unsigned build){for(const auto& p:abilityButtonProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool AbilityButtonCode131(uintptr_t base,const AbilityButtonProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
struct AbilityButtonSource131 {RemoveAbilitySource131 source;int hidden=0;};
inline bool SameAbilityButton131(const AbilityButtonSource131& a,const AbilityButtonSource131& b){return SameRemovalUnit131(a.source,b.source)&&SameRemovalAbility131(a.source,b.source)&&a.hidden==b.hidden;}
template<class Selected> bool ReadAbilityButton131(uintptr_t base,unsigned build,uint32_t code,Selected selected,AbilityButtonSource131& out){
 const auto* p=AbilityButtonProfileFor131(build);if(!p||!AbilityButtonCode131(base,*p))return false;
 AbilityButtonSource131 value;if(!ReadRemoveAbility131(base,build,code,selected,value.source,1)||!value.source.ability)return false;
 __try{value.hidden=*reinterpret_cast<int*>(value.source.ability+p->counter);if(value.hidden<0||value.hidden>1000000)return false;out=value;return true;}__except(1){return false;}
}
using AbilityButtonSetter131=void(__thiscall*)(uintptr_t,uint32_t,unsigned);
using AbilityButtonNotify131=void(__thiscall*)(uintptr_t);
inline bool ApplyAbilityButton131(uintptr_t base,unsigned build,uint32_t code,bool hide,const AbilityButtonSource131& value){
 const auto* p=AbilityButtonProfileFor131(build);if(!p||!code||!value.source.unit||!value.source.ability||value.hidden<0||value.hidden>1000000||(!hide&&value.hidden==0)||!AbilityButtonCode131(base,*p))return false;
 __try{
  if(!(DropIdentity131(value.source.unit)==value.source.id)||*reinterpret_cast<uintptr_t*>(value.source.ability)!=value.source.abilityTable||!(DropIdentity131(value.source.ability)==value.source.abilityId)||*reinterpret_cast<int*>(value.source.ability+p->counter)!=value.hidden)return false;
#if defined(_WIN64)
  // Verified engine inline hide/unhide operations, followed by its own owner notifier.
  *reinterpret_cast<int*>(value.source.ability+p->counter)=value.hidden+(hide?1:-1);
  reinterpret_cast<AbilityButtonNotify131>(base+p->notify)(value.source.ability);
#else
  reinterpret_cast<AbilityButtonSetter131>(base+p->setter)(value.source.unit,code,hide?1:0);
#endif
  return true;
 }__except(1){return false;}
}
template<class Read,class Apply,class Refresh> bool AbilityButtonCommand131(const std::wstring& args,bool hide,Read read,Apply apply,Refresh refresh,std::string& report){
 uint32_t code=0;if(!ParseRemoveAbility131(args,code)){report=hide?"Usage: #hideb AHhb":"Usage: #showb AHhb";return false;}
 AbilityButtonSource131 before,current,after,final;
 if(!read(code,before)||!read(code,current)||!SameAbilityButton131(before,current)||!before.source.ability||before.hidden<0||before.hidden>1000000){report="Ability Button: select a stable unit and ability";return false;}
 if(!hide&&before.hidden==0){report="Ability already shown: "+RemoveAbilityName131(code);return true;}
 if(hide&&before.hidden==1000000){report="Ability Button: hide counter limit reached";return false;}
 if(!apply(code,hide,before)){report="Ability Button: update failed; counter may have changed";return false;}
 if(!read(code,after)||!SameRemovalUnit131(before.source,after.source)||!SameRemovalAbility131(before.source,after.source)||after.hidden!=before.hidden+(hide?1:-1)){report="Ability Button: game did not confirm counter update";return false;}
 if(!refresh(before.source.unit)){report="Ability Button: counter updated, panel refresh failed";return false;}
 if(!read(code,final)||!SameAbilityButton131(after,final)){report="Ability Button: selection or ability changed after refresh";return false;}
 report=std::string(hide?"Hide Ability: ":"Show Ability: ")+RemoveAbilityName131(code)+" (hide count: "+std::to_string(final.hidden)+")";return true;
}
}
