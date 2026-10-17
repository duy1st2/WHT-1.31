#pragma once
#include "self_cast131.hpp"
namespace wht {
template<class Alive> bool ReadCastTarget131(uintptr_t base,unsigned build,uintptr_t ui,uintptr_t unit,Alive alive,SelfCastSource131& out){
 const auto* p=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(!ui||!p||!np)return false;
 const auto api=DropNetworkNative131(base,*np,*p);if(!api||!InventoryIdentity131(unit,base,p->unitTable)||!alive(unit))return false;
 __try{
  const auto id=DropIdentity131(unit);const auto session=DropSession131(api);if(!id.Valid()||!session)return false;
  float xyz[3]{};api.position(unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);
  if(!std::isfinite(xyz[0])||!std::isfinite(xyz[1]))return false;
  out={ui,unit,session,id,xyz[0],xyz[1]};return true;
 }__except(1){return false;}
}
template<class ReadSource,class ReadTarget,class Target,class Click,class Order,class Finish>
bool ActivateTargetCast131(ReadSource source,ReadTarget destination,Target target,Click click,Order order,Finish finish){
 if(target())return false;
 SelfCastSource131 before{},after{},pinned{},current{};
 if(!source(before)||!destination(pinned)||before.ui!=pinned.ui||before.session!=pinned.session||!click())return false;
 if(!source(after)||!destination(current)||!SameSelfCastSource131(before,after)||!SameSelfCastSource131(pinned,current))return false;
 const auto mode=target();if(!mode)return true;
 const auto saved=order(mode);return saved&&finish(current,mode,saved);
}
}
