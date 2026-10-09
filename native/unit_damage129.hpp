#pragma once
#include "kill_unit129.hpp"
#include <cmath>
#include <sstream>
namespace wht {
struct UnitDamageApi129 {
 KillUnitApi129 life;
 float*(__thiscall* get)(uintptr_t,float*)=nullptr;
 explicit operator bool()const{return bool(life)&&get;}
};
inline UnitDamageApi129 ResolveUnitDamageApi129(uintptr_t base){
 const auto life=ResolveKillUnitApi129(base);if(!life)return {};
 __try {
  const unsigned char get[]={0x55,0x8b,0xec,0x51,0x8d,0x45,0xfc,0x81,0xc1,0xa0,0x01,0,0};
  if(memcmp((void*)(base+0x268e40),get,sizeof(get))||
     *reinterpret_cast<uintptr_t*>(base+0xa9c504+0x12c)!=base+0x268e40)return {};
  return {life,reinterpret_cast<decltype(UnitDamageApi129::get)>(base+0x268e40)};
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
inline bool ParseUnitDamage129(const std::wstring& argument,float& amount){
 amount=100.f;if(argument.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream input(argument);double value=0;std::wstring extra;
 if(!(input>>value)||input>>extra||!std::isfinite(value)||value<=0||value>1000000000.)return false;
 amount=float(value);return std::isfinite(amount)&&amount>0;
}
inline bool DamageUnit129(uintptr_t base,const SkillSelection129& selected,const UnitDamageApi129& api,
 float amount,float& requestedLife,const char** error=nullptr){
 requestedLife=0;if(error)*error=nullptr;
 __try {
  if(!base||!api||!selected.unit||!selected.identity||!std::isfinite(amount)||amount<=0||amount>1000000000.f)
   return KillUnitFailure129(error,"Select a unit and use #udmg [positive amount]");
  const auto current=SelectedSkill129(base);
  if(current.unit!=selected.unit||current.identity!=selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=base+0xa9c504)
   return KillUnitFailure129(error,"Selected unit changed or is unsupported");
  const auto agent=api.life.agent(uint32_t(selected.identity>>32),uint32_t(selected.identity));
  if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))
   return KillUnitFailure129(error,"Selected unit is no longer live");
  float before=0;
  if(api.get(selected.unit,&before)!=&before||!std::isfinite(before)||before<=0)
   return KillUnitFailure129(error,"Selected unit has no valid remaining life");
  requestedLife=(std::max)(0.f,before-amount);
  api.life.kill(selected.unit,&requestedLife);
  // A death callback may remove the unit. No unit access after setting its life.
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return KillUnitFailure129(error,"Native unit life access failed");}
}
inline bool DamageSelectedUnit129(uintptr_t base,float amount,float& requestedLife,const char** error=nullptr){
 const auto api=ResolveUnitDamageApi129(base);if(!api)return KillUnitFailure129(error,"Unit life function signature mismatch");
 return DamageUnit129(base,SelectedSkill129(base),api,amount,requestedLife,error);
}
}
