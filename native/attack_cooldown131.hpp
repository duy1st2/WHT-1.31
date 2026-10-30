#pragma once
#include "attack_cooldown_profile131.hpp"
#include "attack_speed131.hpp"
namespace wht {
inline const AttackCooldownProfile131* AttackCooldownProfileFor131(unsigned build){for(const auto& p:attackCooldownProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool AttackCooldownCode131(uintptr_t base,const AttackCooldownProfile131& p){ResourceTextProfile131 code{};code.code=p.code;code.count=p.count;return ResourceTextGuard131(base,code);}
inline bool ParseAttackCooldown131(const std::wstring& args,float& seconds,unsigned& weapon){
 if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;
 std::wistringstream in(args);float value=0;int number=1;std::wstring extra;
 if(!(in>>value)||!std::isfinite(value)||value<.01f||value>3600.f)return false;
 in>>std::ws;if(!in.eof()&&(!(in>>number)||number<1||number>2||(in>>extra)))return false;
 seconds=value;weapon=unsigned(number-1);return true;
}
using AttackCooldownGet131=float*(__thiscall*)(uintptr_t,float*,unsigned);
#ifdef _WIN64
using AttackCooldownSet131=void(*)(uintptr_t,const float*,unsigned);
#else
using AttackCooldownSet131=void(__thiscall*)(uintptr_t,float,unsigned);
#endif
inline bool InvokeAttackCooldownGet131(AttackCooldownGet131 get,uintptr_t attack,unsigned weapon,float& value){
 if(!get||!attack||weapon>1)return false;
 __try{float actual=NAN;if(get(attack,&actual,weapon)!=&actual||!std::isfinite(actual)||actual<0||actual>=1e6f)return false;value=actual;return true;}__except(1){return false;}
}
inline bool InvokeAttackCooldownSet131(AttackCooldownSet131 set,uintptr_t attack,unsigned weapon,float seconds){
 if(!set||!attack||weapon>1||!std::isfinite(seconds)||seconds<.01f||seconds>3600)return false;
 __try{
#ifdef _WIN64
  set(attack,&seconds,weapon);
#else
  set(attack,seconds,weapon);
#endif
  return true;
 }__except(1){return false;}
}
inline bool AttackCooldownProperty131(uintptr_t base,const AttackCooldownProfile131& p,uintptr_t attack,unsigned weapon){
 if(!base||!attack||weapon>1||!AttackCooldownCode131(base,p))return false;
 __try{const auto property=attack+(sizeof(uintptr_t)==8?0x1d8+weapon*16:0x158+weapon*8);return *reinterpret_cast<uintptr_t*>(property)==base+p.mini&&*reinterpret_cast<uintptr_t*>(base+p.mini)==base+p.propertySet;}__except(1){return false;}
}
struct AttackCooldownSource131 {AttackSpeedSource131 selected;unsigned weapon=2;float seconds=NAN;};
inline bool SameAttackCooldown131(const AttackCooldownSource131& a,const AttackCooldownSource131& b){return a.weapon<2&&a.weapon==b.weapon&&SameAttackSpeed131(a.selected,b.selected);}
template<class Selected> bool ReadAttackCooldown131(uintptr_t base,unsigned build,Selected selected,unsigned weapon,AttackCooldownSource131& out){
 const auto* p=AttackCooldownProfileFor131(build);if(!p||weapon>1)return false;
 AttackCooldownSource131 value;value.weapon=weapon;
 if(!ReadAttackSpeed131(base,build,selected,value.selected)||!AttackCooldownProperty131(base,*p,value.selected.attack,weapon)||!InvokeAttackCooldownGet131(reinterpret_cast<AttackCooldownGet131>(base+p->get),value.selected.attack,weapon,value.seconds))return false;
 AttackSpeedSource131 after;if(!ReadAttackSpeed131(base,build,selected,after)||!SameAttackSpeed131(value.selected,after))return false;out=value;return true;
}
inline bool SetAttackCooldownNative131(uintptr_t base,unsigned build,uintptr_t attack,unsigned weapon,float seconds){const auto* p=AttackCooldownProfileFor131(build);return p&&AttackCooldownProperty131(base,*p,attack,weapon)&&InvokeAttackCooldownSet131(reinterpret_cast<AttackCooldownSet131>(base+p->set),attack,weapon,seconds);}
template<class Read,class Set> bool AttackCooldownCommand131(const std::wstring& args,Read read,Set set,std::string& report){
 float seconds=0;unsigned weapon=0;if(!ParseAttackCooldown131(args,seconds,weapon)){report="Usage: #attackcd seconds [1|2] (0.01..3600 seconds; default weapon 1)";return false;}
 AttackCooldownSource131 before,current,after;
 if(!read(weapon,before)||!read(weapon,current)||!SameAttackCooldown131(before,current)){report="Attack Cooldown: select a stable unit with attack data";return false;}
 if(!set(current.selected.attack,weapon,seconds)){report="Attack Cooldown: native setter unavailable";return false;}
 if(!read(weapon,after)||!SameAttackCooldown131(before,after)||!std::isfinite(after.seconds)||std::abs(after.seconds-seconds)>.0001f){report="Attack Cooldown: game did not confirm the requested cooldown or selection changed";return false;}
 char text[128]{};std::snprintf(text,sizeof(text),"Attack Cooldown: weapon %u = %.3f seconds",weapon+1,after.seconds);report=text;return true;
}
}
