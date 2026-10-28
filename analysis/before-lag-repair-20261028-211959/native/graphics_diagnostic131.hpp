#pragma once
#include "ui_refresh131.hpp"
#include "life_mana_hook131.hpp"
#include "console_ui131.hpp"
#include "black_bar131.hpp"
#include "hp_player_color131.hpp"
#include "invulnerable_bar131.hpp"
#include "mouse_tooltip131.hpp"
namespace wht {
struct GraphicsDiagnostic131 {
 uint64_t due=0;unsigned count=0;
 template<class Find> void Update(uintptr_t base,unsigned build,uintptr_t ui,const Config& c,bool allowed,const std::wstring& folder,const ConsoleUi131& consoleState,const MouseTooltip131& tooltipState,Find find){
  if(count>=24||GetTickCount64()<due||!(c.lifeManaCommas||c.lifeManaNoHide||c.statsCommas||c.hideConsoleUI||c.mouseTooltip||c.colorPlayer||c.hideInvulnerableBars))return;
  due=GetTickCount64()+5000;++count;
  auto life=LifeProfileFor131(build);auto console=ConsoleProfileFor131(build);auto tip=MouseTooltipProfileFor131(build);auto hp=HpColorProfileFor131(build);auto inv=InvulnerableProfileFor131(build);auto black=BlackBarProfileFor131(build);
  auto portrait=life?find(life->portrait):0;auto source=life?ReadLifeSource131(base,*life,ui,portrait):LifeSource131{};
  char text[640]{};std::snprintf(text,sizeof(text),"WHT graphics diagnostic: build=%u arch=%u allowed=%u config=%u%u%u%u%u%u%u hook=%u uiThread=%u flags=%u portrait=%u selected=%u console=%u tooltip=%u guards[life,stats,console,layout,hp,invu]=%u%u%u%u%u%u",build,unsigned(sizeof(void*)*8),unsigned(allowed),unsigned(c.lifeManaCommas),unsigned(c.lifeManaNoHide),unsigned(c.statsCommas),unsigned(c.hideConsoleUI),unsigned(c.mouseTooltip),unsigned(c.colorPlayer),unsigned(c.hideInvulnerableBars),unsigned(lifeInstalled131),unsigned(GetCurrentThreadId()==lifeThread131),lifeFlags131,unsigned(portrait!=0),unsigned(source.unit!=0),unsigned(console&&ConsoleComponent131(ui,base,console->table)),unsigned(tip&&find(tip->table)),unsigned(life&&(lifeInstalled131||LifeCode131(base,*life))),unsigned(statsProfile131!=nullptr),unsigned(console&&ConsoleCode131(base,*console)),unsigned(black&&BlackBarCode131(base,*black)),unsigned(hp&&HpColorCode131(base,*hp)),unsigned(inv&&InvulnerableCode131(base,*inv)));
  Log(folder,text);Log(folder,"WHT graphics presentation: native UI updates="+std::to_string(uiRefreshCalls131)+" bar show/bind/update="+std::to_string(statPaintShows131)+"/"+std::to_string(statPaintBinds131)+"/"+std::to_string(statPaintUpdates131));Log(folder,"WHT graphics state: console stage="+std::to_string(consoleState.diagnostic)+" active="+std::to_string(consoleState.active)+" tooltip active="+std::to_string(tooltipState.active));Log(folder,"WHT font hook stage="+std::to_string(lifeHookStage131)+" status="+std::to_string(lifeHookError131)+" winerror="+std::to_string(lifeHookWinError131));
 }
};
}
