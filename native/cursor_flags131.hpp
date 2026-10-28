#pragma once
#include "unit_info131.hpp"
namespace wht {
// Same agent flags already read by world_present131 / Auto Skills.
inline constexpr unsigned cursorUnitFlagsOffset131=sizeof(uintptr_t)==8?0x38:0x20;
inline bool ReadCursorUnitFlags131(uintptr_t base,uintptr_t table,uintptr_t unit,uint32_t& flags){
 if(!base||!table||!InventoryIdentity131(unit,base,table))return false;
 __try{flags=*reinterpret_cast<uint32_t*>(unit+cursorUnitFlagsOffset131);return true;}__except(1){return false;}
}
template<class Selected,class Read> bool CaptureCursorFlags131(Selected selected,Read read,uint32_t& flags){
 const auto before=selected();if(!before.unit||!before.id.Valid())return false;
 uint32_t value=0;if(!read(before.unit,value))return false;
 const auto after=selected();if(after.unit!=before.unit||!(after.id==before.id))return false;
 flags=value;return true;
}
template<class Selected> bool SelectedCursorFlags131(uintptr_t base,unsigned build,Selected selected,uint32_t& flags){
 const auto* p=BossProfileFor131(build);if(!p||!BossNative131(base,*p))return false;
 auto source=[&](){InventoryInfoSource131 value;const auto unit=selected();if(InventoryIdentity131(unit,base,p->unit)){value.unit=unit;value.id=DropIdentity131(unit);}return value;};
 return CaptureCursorFlags131(source,[&](uintptr_t unit,uint32_t& out){return ReadCursorUnitFlags131(base,p->unit,unit,out);},flags);
}
inline std::string CursorFlagsText131(const POINT& screen,const POINT& client,bool hasUnit,uint32_t flags){
 char text[192]{};std::snprintf(text,sizeof(text),"MouseXY: [%ld, %ld], ClientXY: [%ld, %ld]",screen.x,screen.y,client.x,client.y);
 std::string result=text;if(hasUnit){std::snprintf(text,sizeof(text)," | Unit Flags: 0x%08X",unsigned(flags));result+=text;}return result;
}
template<class Cursor,class Flags> bool CursorFlagsCommand131(const std::wstring& args,Cursor cursor,Flags flags,std::string& report){
 if(!UnitInfoArgs131(args))return false;
 POINT screen{},client{};if(!cursor(screen,client)){report="Mouse Position: game window or cursor unavailable";return false;}
 uint32_t value=0;const bool known=flags(value);report=CursorFlagsText131(screen,client,known,value);return true;
}
inline bool ReadCursorPoints131(HWND window,POINT& screen,POINT& client){
 if(!window||!IsWindow(window)||!GetCursorPos(&screen))return false;
 client=screen;return ScreenToClient(window,&client)!=FALSE;
}
}
