#pragma once
#include "kill_unit_profile131.hpp"
#include "reselect131.hpp"
namespace wht {
inline const KillUnitProfile131* KillUnitProfileFor131(unsigned build){for(const auto& p:killUnitProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool KillUnitCode131(uintptr_t base,const KillUnitProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};if(!ResourceTextGuard131(base,code))return false;__try{return *reinterpret_cast<uintptr_t*>(base+p.unit+(sizeof(uintptr_t)==8?0x248:0x124))==base+p.kill&&!*reinterpret_cast<uint32_t*>(base+p.zero);}__except(1){return false;}}
using KillUnitNative131=void(__thiscall*)(uintptr_t,const float*);
inline bool InvokeKillUnit131(KillUnitNative131 kill,uintptr_t unit,const float* zero){if(!kill||!unit||!zero)return false;__try{if(*reinterpret_cast<const uint32_t*>(zero))return false;kill(unit,zero);return true;}__except(1){return false;}}
struct KillUnitSource131 {uintptr_t unit=0,world=0;DropObjectId id;unsigned local=24;float life=NAN;};
inline bool SameKillUnit131(const KillUnitSource131& a,const KillUnitSource131& b){return a.unit&&a.world&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.world==b.world&&a.local==b.local&&a.id==b.id;}
template<class Selected,class Life> bool ReadKillUnit131(uintptr_t base,unsigned build,Selected selected,Life life,KillUnitSource131& out){
 const auto* p=KillUnitProfileFor131(build);const auto* rp=ResourceProfileFor131(build);if(!p||!rp||!KillUnitCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{KillUnitSource131 value;value.unit=selected();if(!InventoryIdentity131(value.unit,base,p->unit))return false;value.id=DropIdentity131(value.unit);if(!value.id.Valid())return false;
  value.world=ResourceWorld131(base,*rp,value.local);if(!value.world||!life(value.unit,value.life)||!std::isfinite(value.life)||value.life<=.405f)return false;
  unsigned local=24;if(selected()!=value.unit||!(DropIdentity131(value.unit)==value.id)||!InventoryIdentity131(value.unit,base,p->unit)||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local)return false;
  out=value;return true;
 }__except(1){return false;}
}
template<class Read,class Kill> bool KillUnitCommand131(const std::wstring& args,Read read,Kill kill,std::string& report){
 if(!UnitInfoArgs131(args)){report="Usage: select a unit, then #kill";return false;}
 KillUnitSource131 before,current;if(!read(before)||!read(current)||!SameKillUnit131(before,current)||!std::isfinite(current.life)||current.life<=.405f){report="KillUnit: select a stable living unit";return false;}
 if(!kill(current.unit)){report="KillUnit: native call unavailable or failed";return false;}
 // Native death handling can destroy/change selection; do not dereference it afterwards.
 report="KillUnit invoked for selected unit";return true;
}
inline bool KillSelectedNative131(uintptr_t base,unsigned build,uintptr_t unit){const auto* p=KillUnitProfileFor131(build);return p&&KillUnitCode131(base,*p)&&InvokeKillUnit131(reinterpret_cast<KillUnitNative131>(base+p->kill),unit,reinterpret_cast<const float*>(base+p->zero));}
}
