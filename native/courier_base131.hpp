#pragma once
#include "courier_route131.hpp"
namespace wht {
struct CourierBase131 {
 uintptr_t ui=0,session=0;float x=0,y=0;SelfCastSource131 owner{};bool pending=false;
 void Reset(){*this={};}
 void Cancel(){pending=false;owner={};}
 bool Save(const SelfCastSource131& source){if(!source.ui||!source.session||!source.unit||!source.id.Valid()||!std::isfinite(source.x)||!std::isfinite(source.y))return false;ui=source.ui;session=source.session;x=source.x;y=source.y;return true;}
 bool Queue(const SelfCastSource131& source){if(!ui||ui!=source.ui||!session||session!=source.session||!source.unit||!source.id.Valid())return false;owner=source;pending=true;return true;}
 template<class Read,class Send> bool Tick(bool enabled,bool ready,Read read,Send send){
  if(!pending)return false;if(!enabled){pending=false;owner={};return true;}if(!ready)return false;
  const auto captured=owner;pending=false;owner={};SelfCastSource131 current;
  if(!read(captured.unit,current)||!SameSelfCastSource131(current,captured)||current.ui!=ui||current.session!=session)return true;
  send(current,x,y);return true;
 }
};
}
