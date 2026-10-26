#pragma once
#include "inventory_info131.hpp"
#include "time_boss131.hpp"
#include <cstdio>
namespace wht {
struct UnitInfo131 {uint32_t code=0;float x=0,y=0;};
inline bool UnitInfoArgs131(const std::wstring& args){return args.find(L'\0')==std::wstring::npos&&args.find_first_not_of(L" \t")==std::wstring::npos;}
template<class Selected,class Read> bool CaptureUnitInfo131(bool position,Selected selected,Read read,UnitInfo131& out){
 const auto before=selected();if(!before.unit||!before.id.Valid())return false;
 UnitInfo131 value;if(!read(before.unit,value)||!value.code||(position&&(!std::isfinite(value.x)||!std::isfinite(value.y))))return false;
 const auto after=selected();if(after.unit!=before.unit||!(after.id==before.id))return false;
 out=value;return true;
}
inline bool ReadUnitInfoFields131(uintptr_t base,unsigned build,uintptr_t unit,bool position,UnitInfo131& out){
 const auto* boss=BossProfileFor131(build);if(!boss||!BossNative131(base,*boss)||!InventoryIdentity131(unit,base,boss->unit))return false;
 __try{
  out.code=*reinterpret_cast<uint32_t*>(unit+bossCodeOffset131);if(!out.code)return false;
  if(position){const auto* inv=InventoryProfile(build);const auto* net=DropNetworkProfileFor131(build);if(!inv||!net)return false;
   const auto api=DropNetworkNative131(base,*net,*inv);if(!api)return false;float xyz[3]{};
   api.position(unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);out.x=xyz[0];out.y=xyz[1];
   if(!std::isfinite(out.x)||!std::isfinite(out.y))return false;
  }return true;
 }__except(1){return false;}
}
template<class Selected> bool ReadUnitInfo131(uintptr_t base,unsigned build,bool position,Selected selected,UnitInfo131& out){
 const auto* p=BossProfileFor131(build);if(!p)return false;
 auto source=[&](){InventoryInfoSource131 value;const auto unit=selected();if(InventoryIdentity131(unit,base,p->unit)){value.unit=unit;value.id=DropIdentity131(unit);}return value;};
 return CaptureUnitInfo131(position,source,[&](uintptr_t unit,UnitInfo131& value){return ReadUnitInfoFields131(base,build,unit,position,value);},out);
}
inline std::string UnitInfoText131(const UnitInfo131& info,bool position){
 char text[128]{};if(position)std::snprintf(text,sizeof(text),"X: %.5f, Y: %.5f",info.x,info.y);
 else{char raw[5]{};for(unsigned i=0;i<4;++i){const auto ch=(info.code>>(24-i*8))&255;raw[i]=ch>=32&&ch<=126?char(ch):'?';}std::snprintf(text,sizeof(text),"Unit ID: [%s] (0x%08X)",raw,unsigned(info.code));}
 return text;
}
}
