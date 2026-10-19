#pragma once
#include "view_commands131.hpp"
#include "tool_menu.hpp"
struct ViewIniTest131 {
 wchar_t file[MAX_PATH]{};
 ViewIniTest131(){wchar_t root[MAX_PATH]{};check(GetTempPathW(MAX_PATH,root)>0&&GetTempFileNameW(root,L"v13",0,file)!=0,"isolated temporary settings file");}
 ~ViewIniTest131(){if(file[0])DeleteFileW(file);}
};
inline void CheckViewCommands131(){
 ViewIniTest131 ini;wht::Config config=wht::LoadConfig(ini.file);unsigned writes=0,reloads=0;
 auto save=[&](const wchar_t* section,const wchar_t* key,const wchar_t* value){++writes;return wht::SaveSetting(ini.file,section,key,value);};
 auto reload=[&](){++reloads;config=wht::LoadConfig(ini.file);};
 auto run=[&](const wchar_t* name,const wchar_t* args){const auto* target=wht::ViewCommandFor131(name);check(target!=nullptr,"reference display alias exists");return wht::ApplyViewCommand131(*target,args,config,save,reload);};
 check(run(L"#timer",L"0")&&!config.showTimer&&wht::MenuRead(config,L"Event",L"Show Timer",L"1")==L"0","timer command persisted and menu agrees");
 check(run(L"#showtimer",L"")&&config.showTimer,"timer alias shares setting and bare command toggles");
 check(run(L"#res",L"1")&&config.showResource&&run(L"#resource",L"0")&&!config.showResource,"resource aliases persist same checkbox");
 check(run(L"#showhotkey",L"1")&&config.showHotkeyInPanel,"panel hotkey labels enabled through config");
 check(run(L"#statstooltip",L"0")&&!config.statsTooltip&&run(L"#statscommas",L"1")&&config.statsCommas,"graphics toggles independent");
 check(run(L"#hpmpcommas",L"1")&&config.lifeManaCommas&&run(L"#hpmpnohide",L"1")&&config.lifeManaNoHide,"portrait options persisted");
 check(run(L"#console",L"1")&&config.hideConsoleUI&&run(L"#consoleui",L"")&&!config.hideConsoleUI,"console aliases share setting");
 const unsigned beforeWrites=writes,beforeReloads=reloads;const auto* target=wht::ViewCommandFor131(L"#timer");
 for(auto args:{L"2",L"on",L"0 extra",L"1 0",L"1\n0"})check(!wht::ApplyViewCommand131(*target,args,config,save,reload),"invalid input cannot edit settings");
 check(writes==beforeWrites&&reloads==beforeReloads,"malformed commands never write or reload");
 bool next=true;check(!wht::ViewToggle131(std::wstring(L"1\0x",3),false,next)&&next,"embedded NUL rejected without changing output");
 check(!wht::ApplyViewCommand131(*target,L"0",config,[](const wchar_t*,const wchar_t*,const wchar_t*){return false;},reload)&&reloads==beforeReloads,"failed persistence cannot publish new config");
 check(writes==reloads,"each successful setting change reloads exactly once");
 check(!wht::ViewCommandFor131(L"#unlockfps")&&!wht::ViewCommandFor131(L"#autoskills"),"unported actions remain unavailable");
 check(!wht::ViewCommandFor131(L"#menu"),"configured tool menu command retains existing behavior");
}
