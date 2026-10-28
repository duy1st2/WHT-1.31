#pragma once
#include "no_cooldown_profile131.hpp"
#include "remove_unit131.hpp"
namespace wht {
inline const CooldownResetProfile131* CooldownResetProfileFor131(unsigned build){for(const auto& p:cooldownResetProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool CooldownResetCode131(uintptr_t base,const CooldownResetProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
using CooldownResetNative131=void(__thiscall*)(uintptr_t);
inline bool InvokeCooldownReset131(CooldownResetNative131 reset,uintptr_t unit){if(!reset||!unit)return false;__try{reset(unit);return true;}__except(1){return false;}}
template<class Read,class Apply,class Valid> bool ResetCooldownFlow131(Read read,Apply apply,Valid valid){
 RemoveUnitSource131 before,current;if(!read(before)||!read(current)||!SameRemoveUnit131(before,current)||!valid(current))return false;
 if(!apply(current))return false;
 // Native callbacks can release the original unit; validate registry before any access.
 return valid(current);
}
template<class Selected> bool ResetSelectedCooldown131(uintptr_t base,unsigned build,Selected selected){
 const auto* p=CooldownResetProfileFor131(build);if(!p||!CooldownResetCode131(base,*p))return false;
 return ResetCooldownFlow131([&](RemoveUnitSource131& out){return ReadRemoveUnit131(base,build,selected,out);},[&](const RemoveUnitSource131& value){return InvokeCooldownReset131(reinterpret_cast<CooldownResetNative131>(base+p->reset),value.unit);},[&](const RemoveUnitSource131& value){return ValidRemoveUnit131(base,build,value)&&selected()==value.unit;});
}
struct NoCooldown131 {
 bool enabled=false;uint64_t due=0,generation=0;
 void Reset(){enabled=false;due=0;++generation;}
 template<class Apply> bool Command(const std::wstring& name,const std::wstring& args,uint64_t now,Apply apply,std::string& report){
  if((name!=L"#nocd"&&name!=L"#yescd")||!UnitInfoArgs131(args)){report="Usage: #nocd or #yescd (no arguments)";return false;}
  Reset();if(name==L"#yescd"){report="No Cooldown: OFF";return true;}
  const auto token=generation;const bool ok=apply();
  if(token!=generation){report="No Cooldown: state changed during native reset";return false;}
  if(!ok){report="No Cooldown: select a stable registered unit; native reset unavailable or interrupted";return false;}
  enabled=true;due=now+100;report="No Cooldown: ON (selected unit)";return true;
 }
 // Returns true only for a new failure requiring one notification.
 template<class Apply> bool Tick(bool allowed,bool ready,uint64_t now,Apply apply){
  if(!allowed){if(enabled)Reset();return false;}
  if(!enabled||!ready||now<due)return false;
  due=now+100;const auto token=generation;
  if(!apply()&&token==generation){Reset();return true;}return false;
 }
};
}
