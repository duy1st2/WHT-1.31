#pragma once
#include "resource131.hpp"
#include "resource_board129.hpp"
namespace wht {
// The shared board only creates tool-owned frames. All game data comes from
// the independent, architecture-specific 1.31 reader above.
struct ResourceBoard131 {
 ResourceBoard129 board;bool expanded=true,reported=false;uint64_t next=0;std::string lastParticipants;
 void Reset(){board={};expanded=true;reported=false;next=0;lastParticipants.clear();}
 void Hide(){board.Hide();next=0;}
 bool Toggle(){expanded=!expanded;next=0;return true;}
 bool Update(uintptr_t base,unsigned build,bool enabled,uint64_t now,ToolMenuOverlay& menu,const std::wstring& folder){
  if(!enabled){Hide();return false;}if(now<next)return false;next=now+1000;
  auto table=ReadResources131(base,build);
  std::string participants;for(const auto& row:table.rows)participants+=" ["+std::to_string(row.slot+1)+":"+Utf8(row.name)+(row.left?":left":":playing")+"]";
  if(table.ready&&participants!=lastParticipants){lastParticipants=participants;Log(folder,"WHT resource participants:"+participants);}
  return board.Present(menu,base,table,expanded)&&!table.rows.empty();
 }
};
}
