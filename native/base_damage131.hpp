#pragma once
#include "base_damage_profile131.hpp"
#include "reselect131.hpp"
#include <sstream>
#include <climits>
namespace wht {
inline const BaseDamageProfile131* BaseDamageProfileFor131(unsigned build){for(const auto& p:baseDamageProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool BaseDamageCode131(uintptr_t base,const BaseDamageProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
inline bool ParseBaseDamage131(const std::wstring& args,int& damage){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring token,extra;if(!(in>>token)){damage=50;return true;}
 if((in>>extra)||token.size()>10||token.find_first_not_of(L"0123456789")!=std::wstring::npos)return false;
 uint64_t value=0;for(auto c:token)value=value*10+unsigned(c-L'0');if(value>1000000000)return false;damage=int(value);return true;
}
using BaseDamageGet131=int(__thiscall*)(uintptr_t,int);
using BaseDamageAdd131=void(__thiscall*)(uintptr_t,int,unsigned);
inline bool InvokeBaseDamageAdd131(BaseDamageAdd131 add,uintptr_t attack,int delta){if(!add||!attack)return false;__try{add(attack,delta,1);return true;}__except(1){return false;}}
struct BaseDamageSource131 {uintptr_t unit=0,attack=0,world=0;DropObjectId id;unsigned local=24;int damage=0;};
inline bool SameBaseDamage131(const BaseDamageSource131& a,const BaseDamageSource131& b){return a.unit&&a.attack&&a.world&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.attack==b.attack&&a.world==b.world&&a.local==b.local&&a.id==b.id;}
template<class Selected> bool ReadBaseDamage131(uintptr_t base,unsigned build,Selected selected,BaseDamageSource131& out){
 const auto* p=BaseDamageProfileFor131(build);const auto* inv=InventoryProfile(build);const auto* rp=ResourceProfileFor131(build);const DetailProfile131* detail=nullptr;for(const auto& d:detailProfiles131)if(d.build==build)detail=&d;
 if(!p||!inv||!rp||!detail||!BaseDamageCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{BaseDamageSource131 value;value.unit=selected();if(!InventoryIdentity131(value.unit,base,inv->unitTable))return false;value.id=DropIdentity131(value.unit);if(!value.id.Valid())return false;
  value.world=ResourceWorld131(base,*rp,value.local);if(!value.world)return false;
  const unsigned member=sizeof(uintptr_t)==8?0x570:build==12164?0x460:0x458,owner=sizeof(uintptr_t)==8?0x50:0x30;
  value.attack=*reinterpret_cast<uintptr_t*>(value.unit+member);if(!InventoryIdentity131(value.attack,base,detail->attack)||*reinterpret_cast<uintptr_t*>(value.attack+owner)!=value.unit)return false;
  value.damage=reinterpret_cast<BaseDamageGet131>(base+p->get)(value.attack,0);
  unsigned local=24;if(selected()!=value.unit||!(DropIdentity131(value.unit)==value.id)||*reinterpret_cast<uintptr_t*>(value.unit+member)!=value.attack||!InventoryIdentity131(value.attack,base,detail->attack)||*reinterpret_cast<uintptr_t*>(value.attack+owner)!=value.unit||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local)return false;
  out=value;return true;
 }__except(1){return false;}
}
template<class Read,class Add> bool BaseDamageCommand131(const std::wstring& args,Read read,Add add,std::string& report){
 int requested=0;if(!ParseBaseDamage131(args,requested)){report="Usage: select a unit, then #udmgb [0..1000000000] (default 50)";return false;}
 BaseDamageSource131 before,current,after;
 if(!read(before)||!read(current)||!SameBaseDamage131(before,current)||before.damage!=current.damage){report="Base Damage: select a stable unit with attack data";return false;}
 const int64_t delta=int64_t(requested)-current.damage;if(delta<INT_MIN||delta>INT_MAX){report="Base Damage: native delta would overflow";return false;}
 if(delta&&!add(before.attack,int(delta))){report="Base Damage: native update failed";return false;}
 if(!read(after)||!SameBaseDamage131(before,after)||after.damage!=requested){report="Base Damage: game did not confirm requested value";return false;}
 report="Unit Base Damage: "+std::to_string(after.damage);return true;
}
inline bool AddBaseDamageNative131(uintptr_t base,unsigned build,uintptr_t attack,int delta){const auto* p=BaseDamageProfileFor131(build);return p&&BaseDamageCode131(base,*p)&&InvokeBaseDamageAdd131(reinterpret_cast<BaseDamageAdd131>(base+p->add),attack,delta);}
}
