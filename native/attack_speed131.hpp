#pragma once
#include "attack_speed_profile131.hpp"
#include "reselect131.hpp"
#include <sstream>
#include <cstdio>
namespace wht {
inline const AttackSpeedProfile131* AttackSpeedProfileFor131(unsigned build){for(const auto& p:attackSpeedProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool AttackSpeedCode131(uintptr_t base,const AttackSpeedProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
inline bool ParseAttackSpeed131(const std::wstring& args,float& factor){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring extra;float value=2;in>>std::ws;if(!in.eof()&&(!(in>>value)||(in>>extra)))return false;
 if(!std::isfinite(value))return false;factor=(std::clamp)(value,.1f,50.f);return true;
}
using AttackSpeedSet131=void(__thiscall*)(uintptr_t,const float*,int);
using AttackSpeedRefresh131=void(__thiscall*)(uintptr_t,uintptr_t);
inline bool InvokeAttackSpeedSet131(AttackSpeedSet131 set,uintptr_t property,float factor){if(!set||!property||!std::isfinite(factor)||factor<.1f||factor>50)return false;__try{set(property,&factor,1);return true;}__except(1){return false;}}
inline bool InvokeAttackSpeedRefresh131(AttackSpeedRefresh131 refresh,uintptr_t attack,uintptr_t unit){if(!refresh||!attack||!unit)return false;__try{refresh(attack,unit);return true;}__except(1){return false;}}
struct AttackSpeedSource131 {uintptr_t unit=0,attack=0,world=0;DropObjectId id;unsigned local=24;float factor=NAN,minimum=NAN,maximum=NAN;};
inline bool SameAttackSpeed131(const AttackSpeedSource131& a,const AttackSpeedSource131& b){return a.unit&&a.attack&&a.world&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.attack==b.attack&&a.world==b.world&&a.local==b.local&&a.id==b.id;}
inline bool ValidAttackSpeedValues131(const AttackSpeedSource131& value){return std::isfinite(value.factor)&&value.factor>0&&value.factor<1e6f&&std::isfinite(value.minimum)&&value.minimum>0&&std::isfinite(value.maximum)&&value.maximum>=value.minimum&&value.maximum<1e6f;}
template<class Selected> bool ReadAttackSpeed131(uintptr_t base,unsigned build,Selected selected,AttackSpeedSource131& out){
 const auto* p=AttackSpeedProfileFor131(build);const auto* inv=InventoryProfile(build);const auto* rp=ResourceProfileFor131(build);const DetailProfile131* detail=nullptr;for(const auto& d:detailProfiles131)if(d.build==build)detail=&d;
 if(!p||!inv||!rp||!detail||!AttackSpeedCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{AttackSpeedSource131 value;value.unit=selected();if(!InventoryIdentity131(value.unit,base,inv->unitTable))return false;value.id=DropIdentity131(value.unit);if(!value.id.Valid())return false;
  value.world=ResourceWorld131(base,*rp,value.local);if(!value.world)return false;
  const unsigned member=sizeof(uintptr_t)==8?0x570:build==12164?0x460:0x458;
  value.attack=*reinterpret_cast<uintptr_t*>(value.unit+member);if(!InventoryIdentity131(value.attack,base,detail->attack))return false;
  const auto property=value.attack+(sizeof(uintptr_t)==8?0x268:0x1b0);if(!InventoryIdentity131(property,base,detail->miniB)||*reinterpret_cast<uintptr_t*>(base+detail->miniB)!=base+p->set)return false;
  value.factor=*reinterpret_cast<float*>(property+sizeof(uintptr_t));value.minimum=*reinterpret_cast<float*>(base+detail->minimum);value.maximum=*reinterpret_cast<float*>(base+detail->maximum);
  if(!ValidAttackSpeedValues131(value))return false;
  unsigned local=24;if(selected()!=value.unit||!(DropIdentity131(value.unit)==value.id)||*reinterpret_cast<uintptr_t*>(value.unit+member)!=value.attack||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local)return false;
  out=value;return true;
 }__except(1){return false;}
}
template<class Read,class Set,class Refresh> bool AttackSpeedCommand131(const std::wstring& args,Read read,Set set,Refresh refresh,std::string& report){
 float requested=0;if(!ParseAttackSpeed131(args,requested)){report="Usage: #as [multiplier 0.1..50] (default 2)";return false;}
 AttackSpeedSource131 before,current,after,final;
 if(!read(before)||!read(current)||!SameAttackSpeed131(before,current)||!ValidAttackSpeedValues131(current)){report="Attack Speed: select a stable unit with attack data";return false;}
 if(!set(before.attack,requested)){report="Attack Speed: native setter unavailable";return false;}
 if(!read(after)||!SameAttackSpeed131(before,after)||!ValidAttackSpeedValues131(after)||std::abs(after.factor-requested)>.0001f){report="Attack Speed: game did not confirm requested multiplier";return false;}
 if(!refresh(before.attack,before.unit)){report="Attack Speed: native animation refresh failed";return false;}
 if(!read(final)||!SameAttackSpeed131(before,final)||!ValidAttackSpeedValues131(final)||std::abs(final.factor-requested)>.0001f){report="Attack Speed: unit or match changed during refresh";return false;}
 const float effective=(std::clamp)(final.factor,final.minimum,final.maximum);char text[128]{};std::snprintf(text,sizeof(text),"Attack Speed: %.2fx (effective map limit: %.2fx)",final.factor,effective);report=text;return true;
}
inline bool SetAttackSpeedNative131(uintptr_t base,unsigned build,uintptr_t attack,float factor){const auto* p=AttackSpeedProfileFor131(build);return attack&&p&&AttackSpeedCode131(base,*p)&&InvokeAttackSpeedSet131(reinterpret_cast<AttackSpeedSet131>(base+p->set),attack+(sizeof(uintptr_t)==8?0x268:0x1b0),factor);}
inline bool RefreshAttackSpeedNative131(uintptr_t base,unsigned build,uintptr_t attack,uintptr_t unit){const auto* p=AttackSpeedProfileFor131(build);return p&&AttackSpeedCode131(base,*p)&&InvokeAttackSpeedRefresh131(reinterpret_cast<AttackSpeedRefresh131>(base+p->refresh),attack,unit);}
}
