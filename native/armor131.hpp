#pragma once
#include "armor_profile131.hpp"
#include "reselect131.hpp"
#include <sstream>
#include <cstdio>
namespace wht {
inline const ArmorProfile131* ArmorProfileFor131(unsigned build){for(const auto& p:armorProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool ArmorCode131(uintptr_t base,const ArmorProfile131& p){
 ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};if(!ResourceTextGuard131(base,code))return false;
 __try{return *reinterpret_cast<uintptr_t*>(base+p.floatTable)==base+p.set&&*reinterpret_cast<uintptr_t*>(base+p.panelTable+(sizeof(uintptr_t)==8?0xe0:0x70))==base+p.panel;}__except(1){return false;}
}
inline bool ParseArmor131(const std::wstring& args,float& armor){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring extra;float value=50;in>>std::ws;if(!in.eof()&&(!(in>>value)||(in>>extra)))return false;
 if(!std::isfinite(value)||std::abs(value)>1000000.f)return false;armor=value;return true;
}
using ArmorAdd131=void(__thiscall*)(uintptr_t,const float*);
inline bool InvokeArmorAdd131(ArmorAdd131 add,uintptr_t unit,float delta){if(!add||!unit||!std::isfinite(delta))return false;__try{add(unit,&delta);return true;}__except(1){return false;}}
inline bool ReadArmorFields131(uintptr_t base,const ArmorProfile131& p,uintptr_t unit,float& armor){
 const auto* inv=InventoryProfile(p.build);if(!inv)return false;
 __try{if(!InventoryIdentity131(unit,base,inv->unitTable)||!InventoryIdentity131(unit+p.property,base,p.floatTable))return false;const float value=*reinterpret_cast<float*>(unit+p.property+sizeof(uintptr_t));if(!std::isfinite(value))return false;armor=value;return true;}__except(1){return false;}
}
struct ArmorSource131 {uintptr_t unit=0,world=0;DropObjectId id;unsigned local=24;float armor=NAN;};
inline bool SameArmor131(const ArmorSource131& a,const ArmorSource131& b){return a.unit&&a.world&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.world==b.world&&a.local==b.local&&a.id==b.id;}
template<class Selected> bool ReadArmor131(uintptr_t base,unsigned build,Selected selected,ArmorSource131& out){
 const auto* p=ArmorProfileFor131(build);const auto* rp=ResourceProfileFor131(build);if(!p||!rp||!ArmorCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{ArmorSource131 value;value.unit=selected();if(!ReadArmorFields131(base,*p,value.unit,value.armor))return false;value.id=DropIdentity131(value.unit);if(!value.id.Valid())return false;
  value.world=ResourceWorld131(base,*rp,value.local);if(!value.world)return false;
  unsigned local=24;float check=NAN;if(selected()!=value.unit||!(DropIdentity131(value.unit)==value.id)||!ReadArmorFields131(base,*p,value.unit,check)||check!=value.armor||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local)return false;
  out=value;return true;
 }__except(1){return false;}
}
template<class Read,class Add> bool ArmorCommand131(const std::wstring& args,Read read,Add add,std::string& report){
 float requested=0;if(!ParseArmor131(args,requested)){report="Usage: select a unit, then #armor [-1000000..1000000] (default 50)";return false;}
 ArmorSource131 before,current,after;
 if(!read(before)||!read(current)||!SameArmor131(before,current)||!std::isfinite(before.armor)||before.armor!=current.armor){report="Unit Armor: select a stable unit";return false;}
 const float delta=requested-current.armor;if(!std::isfinite(delta)){report="Unit Armor: invalid native delta";return false;}
 if(delta&&!add(before.unit,delta)){report="Unit Armor: native update failed";return false;}
 if(!read(after)||!SameArmor131(before,after)||!std::isfinite(after.armor)||std::abs(after.armor-requested)>=.01f){report="Unit Armor: game did not confirm requested armor";return false;}
 char text[80]{};std::snprintf(text,sizeof(text),"Unit Armor: %.2f",after.armor);report=text;return true;
}
inline bool AddArmorNative131(uintptr_t base,unsigned build,uintptr_t unit,float delta){const auto* p=ArmorProfileFor131(build);return p&&ArmorCode131(base,*p)&&InvokeArmorAdd131(reinterpret_cast<ArmorAdd131>(base+p->add),unit,delta);}
}
