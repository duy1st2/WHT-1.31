#include "resource_text131.hpp"
#pragma once
#include "resource131.hpp"
#include "resource_board129.hpp"
namespace wht {
// The shared board only creates tool-owned frames. All game data comes from
// the independent, architecture-specific 1.31 reader above.
struct ResourceBoard131 {
 ResourceBoard129 board;ResourceTable cachedTable;bool expanded=true,reported=false,visible=false,titlePressed=false;uintptr_t checkedBase=0;unsigned checkedBuild=0;bool textReady=false,textReported=false;uint64_t next=0;std::string lastParticipants;
 void Reset(){board={};cachedTable={};expanded=true;reported=false;visible=false;titlePressed=false;next=0;lastParticipants.clear();checkedBase=0;checkedBuild=0;textReady=false;textReported=false;}
 void Hide(){board.Hide();visible=false;next=0;}
 bool Mouse(unsigned message,POINT point,int width,int height,bool enabled){
  const bool hit=enabled&&visible&&width>0&&height>0&&ResourceBoard129::HeaderRect().Hit(point,width,height);
  if(message==WM_LBUTTONUP){const bool consumed=titlePressed;titlePressed=false;if(consumed&&hit)Toggle();return consumed;}
  if(message!=WM_LBUTTONDOWN&&message!=WM_LBUTTONDBLCLK)return false;
  if(!hit)return false;titlePressed=true;return true;
 }
 bool Toggle(){expanded=!expanded;next=0;return true;}
 bool Update(uintptr_t base,unsigned build,bool enabled,uint64_t now,ToolMenuOverlay& menu,const std::wstring& folder){
  if(!enabled){Hide();return false;}if(checkedBase!=base||checkedBuild!=build){checkedBase=base;checkedBuild=build;const auto* p=ResourceTextProfileFor131(build);textReady=p&&ResourceTextGuard131(base,*p);Log(folder,textReady?"WHT: resource glyph projection profile verified.":"WHT: resource glyph projection profile refused.");}
  if(visible&&textReady){const auto* p=ResourceTextProfileFor131(build);unsigned repaired=0;if(board.title)repaired+=RepairResourceFont131(base,*p,board.title);for(auto& row:board.rows)if(row.shown)for(auto font:row.text)if(font)repaired+=RepairResourceFont131(base,*p,font);if(repaired&&!textReported){textReported=true;Log(folder,"WHT: resource glyph origins repaired: "+std::to_string(repaired));}}
  // Keep the collapsed header responsive without repeatedly sampling players.
  if(!expanded&&visible&&next)return false;
  if(now<next)return false;next=now+1000;
  if(expanded||!cachedTable.ready)cachedTable=ReadResources131(base,build);
  const auto& table=cachedTable;
  std::string participants;for(const auto& row:table.rows)participants+=" ["+std::to_string(row.slot+1)+":"+Utf8(row.name)+(row.left?":left":":playing")+"]";
  if(table.ready&&participants!=lastParticipants){lastParticipants=participants;Log(folder,"WHT resource participants:"+participants);}
  visible=board.Present(menu,base,table,expanded);return visible&&!table.rows.empty();
 }
};
}
