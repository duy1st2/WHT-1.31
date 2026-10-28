#pragma once
#include "teleport_profile131.hpp"
#include "remove_unit131.hpp"
#include "mouse_world131.hpp"
namespace wht {
inline const TeleportProfile131* TeleportProfileFor131(unsigned build){for(const auto& p:teleportProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool TeleportCode131(uintptr_t base,const TeleportProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};const auto* inv=InventoryProfile(p.build);if(!inv||!ResourceTextGuard131(base,code))return false;__try{return *reinterpret_cast<uintptr_t*>(base+inv->unitTable+(sizeof(uintptr_t)==8?0x300:0x180))==base+p.move;}__except(1){return false;}}
using TeleportStop131=void(__thiscall*)(uintptr_t,unsigned);
using TeleportMove131=void(__thiscall*)(uintptr_t,const float*,const float*,unsigned,unsigned,int,uintptr_t,uintptr_t,unsigned,int,unsigned);
inline bool InvokeTeleportStop131(TeleportStop131 stop,uintptr_t unit){if(!stop||!unit)return false;__try{stop(unit,1);return true;}__except(1){return false;}}
inline bool InvokeTeleportMove131(TeleportMove131 move,uintptr_t unit,float x,float y){if(!move||!unit||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>100000||std::abs(y)>100000)return false;
 __try{move(unit,&x,&y,1,1,-1,0,0,0,-1,0);return true;}__except(1){return false;}
}
inline bool StopTeleportNative131(uintptr_t base,unsigned build,const RemoveUnitSource131& source){const auto* p=TeleportProfileFor131(build);return p&&TeleportCode131(base,*p)&&ValidRemoveUnit131(base,build,source)&&InvokeTeleportStop131(reinterpret_cast<TeleportStop131>(base+p->stop),source.unit);}
inline bool MoveTeleportNative131(uintptr_t base,unsigned build,const RemoveUnitSource131& source,float x,float y){const auto* p=TeleportProfileFor131(build);return p&&TeleportCode131(base,*p)&&ValidRemoveUnit131(base,build,source)&&InvokeTeleportMove131(reinterpret_cast<TeleportMove131>(base+p->move),source.unit,x,y);}
inline bool ReadTeleportPosition131(uintptr_t base,unsigned build,const RemoveUnitSource131& source,MouseWorldPosition131& out){
 if(!ValidRemoveUnit131(base,build,source))return false;const auto* inv=InventoryProfile(build);const auto* p=DropNetworkProfileFor131(build);if(!inv||!p)return false;const auto api=DropNetworkNative131(base,*p,*inv);if(!api)return false;
 __try{float xyz[3]{};api.position(source.unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);if(!ValidRemoveUnit131(base,build,source))return false;const MouseWorldPosition131 value{xyz[0],xyz[1],xyz[2]};if(!MouseWorldPoint131(value))return false;out=value;return true;}__except(1){return false;}
}
template<class Cursor,class Read,class Stop,class Move,class Position> bool TeleportCommand131(const std::wstring& args,Cursor cursor,Read read,Stop stop,Move move,Position position,std::string& report){
 if(!UnitInfoArgs131(args)){report="Usage: select a unit, point at the map, then #tele";return false;}
 MouseWorldPosition131 target;if(!cursor(target)||!MouseWorldPoint131(target)){report="Teleport: point the cursor at the map";return false;}
 RemoveUnitSource131 before,current,after;
 if(!read(before)||!read(current)||!SameRemoveUnit131(before,current)){report="Teleport: select a stable registered unit";return false;}
 if(!stop(current)){report="Teleport: native order stop failed";return false;}
 if(!read(after)||!SameRemoveUnit131(before,after)){report="Teleport: selected unit or match changed while stopping its order";return false;}
 if(!move(after,target.x,target.y)){report="Teleport: native movement failed; update may have applied";return false;}
 if(!read(current)||!SameRemoveUnit131(before,current)){report="Teleport: unit or match changed during movement";return false;}
 MouseWorldPosition131 actual;if(!position(current,actual)||!MouseWorldPoint131(actual)){report="Teleport: cannot read resulting position";return false;}
 char text[192]{};std::snprintf(text,sizeof(text),"Unit Position: X: %.2f, Y: %.2f",actual.x,actual.y);report=text;return true;
}
}
