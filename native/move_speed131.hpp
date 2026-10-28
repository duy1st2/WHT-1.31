#pragma once
#include "move_speed_profile131.hpp"
#include "reselect131.hpp"
#include <sstream>
#include <cstdio>
namespace wht {
inline const MoveSpeedProfile131* MoveSpeedProfileFor131(unsigned build){for(const auto& p:moveSpeedProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool MoveSpeedCode131(uintptr_t base,const MoveSpeedProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
inline bool ParseMoveSpeed131(const std::wstring& args,float& speed){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring extra;float value=522;in>>std::ws;if(!in.eof()&&(!(in>>value)||(in>>extra)))return false;
 if(!std::isfinite(value))return false;speed=(std::max)(0.f,(std::min)(2000.f,value));return true;
}
using MoveSpeedGet131=float*(__thiscall*)(uintptr_t,float*);
using MoveSpeedSet131=void(__thiscall*)(uintptr_t,const float*);
inline bool InvokeMoveSpeed131(MoveSpeedSet131 set,uintptr_t unit,float speed){if(!set||!unit||!std::isfinite(speed)||speed<0||speed>2000)return false;__try{set(unit,&speed);return true;}__except(1){return false;}}
struct MoveSpeedSource131 {uintptr_t unit=0,movement=0,world=0;DropObjectId id;unsigned local=24;float speed=NAN;};
inline bool SameMoveSpeed131(const MoveSpeedSource131& a,const MoveSpeedSource131& b){return a.unit&&a.movement&&a.world&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.movement==b.movement&&a.world==b.world&&a.local==b.local&&a.id==b.id;}
template<class Selected> bool ReadMoveSpeed131(uintptr_t base,unsigned build,Selected selected,MoveSpeedSource131& out){
 const auto* p=MoveSpeedProfileFor131(build);const auto* inv=InventoryProfile(build);const auto* rp=ResourceProfileFor131(build);const DetailProfile131* detail=nullptr;for(const auto& d:detailProfiles131)if(d.build==build)detail=&d;
 if(!p||!inv||!rp||!detail||!MoveSpeedCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{MoveSpeedSource131 value;value.unit=selected();if(!InventoryIdentity131(value.unit,base,inv->unitTable))return false;value.id=DropIdentity131(value.unit);if(!value.id.Valid())return false;
  value.world=ResourceWorld131(base,*rp,value.local);if(!value.world)return false;
  value.movement=*reinterpret_cast<uintptr_t*>(value.unit+p->member);if(!InventoryIdentity131(value.movement,base,detail->move)||*reinterpret_cast<uintptr_t*>(value.movement+(sizeof(uintptr_t)==8?0x50:0x30))!=value.unit)return false;
  if(reinterpret_cast<MoveSpeedGet131>(base+p->get)(value.unit,&value.speed)!=&value.speed||!std::isfinite(value.speed)||value.speed<0||value.speed>1000000)return false;
  unsigned local=24;if(selected()!=value.unit||!(DropIdentity131(value.unit)==value.id)||*reinterpret_cast<uintptr_t*>(value.unit+p->member)!=value.movement||!InventoryIdentity131(value.movement,base,detail->move)||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local)return false;
  out=value;return true;
 }__except(1){return false;}
}
template<class Read,class Set> bool MoveSpeedCommand131(const std::wstring& args,Read read,Set set,std::string& report){
 float requested=0;if(!ParseMoveSpeed131(args,requested)){report="Usage: select a unit, then #ms [0..2000] (default 522)";return false;}
 MoveSpeedSource131 before,current,after;if(!read(before)||!read(current)||!SameMoveSpeed131(before,current)){report="Movement Speed: select a stable unit with movement data";return false;}
 if(!set(before.unit,requested)){report="Movement Speed: native update unavailable";return false;}
 if(!read(after)||!SameMoveSpeed131(before,after)||!std::isfinite(after.speed)||after.speed<0){report="Movement Speed: unit or match changed during update";return false;}
 char text[128]{};std::snprintf(text,sizeof(text),"Movement Speed: %.1f (requested %.1f)",after.speed,requested);report=text;return true;
}
inline bool SetMoveSpeedNative131(uintptr_t base,unsigned build,uintptr_t unit,float speed){const auto* p=MoveSpeedProfileFor131(build);return p&&MoveSpeedCode131(base,*p)&&InvokeMoveSpeed131(reinterpret_cast<MoveSpeedSet131>(base+p->set),unit,speed);}
}
