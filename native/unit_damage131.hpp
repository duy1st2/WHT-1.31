#pragma once
#include "kill_unit131.hpp"
#include <sstream>
namespace wht {
inline bool ParseUnitDamage131(const std::wstring& args,float& amount){
 amount=100;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 if(args.find_first_not_of(L" \t")==std::wstring::npos)return true;std::wistringstream in(args);in.imbue(std::locale::classic());double value=0;std::wstring extra;
 if(!(in>>value)||in>>extra||!std::isfinite(value)||value<=0||value>1000000000.)return false;amount=float(value);return std::isfinite(amount)&&amount>0;
}
inline bool InvokeUnitLife131(KillUnitNative131 setter,uintptr_t unit,float life){if(!setter||!unit||!std::isfinite(life)||life<0)return false;__try{setter(unit,&life);return true;}__except(1){return false;}}
inline bool SetDamagedLifeNative131(uintptr_t base,unsigned build,uintptr_t unit,float life){const auto* p=KillUnitProfileFor131(build);return p&&KillUnitCode131(base,*p)&&InvokeUnitLife131(reinterpret_cast<KillUnitNative131>(base+p->kill),unit,life);}
template<class Read,class Set> bool UnitDamageCommand131(const std::wstring& args,Read read,Set set,std::string& report){
 float amount=100;if(!ParseUnitDamage131(args,amount)){report="Usage: #udmg [positive amount, up to 1000000000] (default 100)";return false;}
 KillUnitSource131 before,current;if(!read(before)||!read(current)||!SameKillUnit131(before,current)||!std::isfinite(current.life)||current.life<=.405f){report="Unit Damage: select a stable living unit";return false;}
 const float life=(std::max)(0.f,current.life-amount);if(life==current.life){report="Unit Damage: amount is too small to change current HP";return false;}
 if(!set(current.unit,life)){report="Unit Damage: native life update failed; update may have applied";return false;}
 // Death callbacks may free the unit: no post-call selection or unit read.
 report="Unit life update invoked: target HP "+std::to_string(life);return true;
}
}
