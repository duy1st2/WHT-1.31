#pragma once
#include "mth_route_buttons131.hpp"
inline void CheckRouteButtons131(){
 ViewIniTest131 ini;auto c=wht::LoadConfig(ini.file);unsigned writes=0,reloads=0;
 auto save=[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){++writes;return wht::SaveSetting(ini.file,section,key,value);};
 auto reload=[&](){++reloads;c=wht::LoadConfig(ini.file);};
 auto run=[&](const wchar_t* name,const std::wstring& args){const auto* command=wht::RouteDrawCommand131(name);check(command!=nullptr,"reference route display alias exists");return wht::ApplyRouteDraw131(*command,args,c,save,reload);};
 check(run(L"#gadraw",L"ON")&&c.autoGaDraw1&&c.autoGaDraw2&&c.autoGaDraw3&&!c.autoSoiDraw1&&writes==3&&reloads==1,"group command persists all three Chicken buttons with one reload");
 check(run(L"#ga2draw",L"off")&&!c.autoGaDraw2&&c.autoGaDraw1,"individual command leaves other buttons unchanged");
 check(run(L"#gadraw",L"")&&!c.autoGaDraw1&&!c.autoGaDraw2&&!c.autoGaDraw3,"reference bare group toggles off when any member is on");
 check(run(L"#soidraw",L"true")&&c.autoSoiDraw1&&c.autoSoiDraw2&&c.autoSoiDraw3,"Wolf group independent");
 const unsigned before=writes,loads=reloads;for(auto arg:{L"2",L"1 extra",L"1\n"})check(!run(L"#soidraw",arg),"invalid draw syntax rejected before writes");
 check(!run(L"#soidraw",std::wstring(L"1\0x",3))&&writes==before&&reloads==loads,"NUL input has no persistence effects");
 const auto* group=wht::RouteDrawCommand131(L"#gadraw");check(!wht::ApplyRouteDraw131(*group,L"1",c,[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&reloads==loads,"failed persistence cannot publish reload");
 check(!wht::RouteDrawCommand131(L"#shielddraw")&&!wht::RouteDrawCommand131(L"#mthdraw"),"unported crafting buttons remain unavailable");
 for(const auto& setting:wht::routeDrawSettings131)c.*setting.field=true;c.pickGokuDraw=true;
 auto layout=wht::RouteButtonLayoutFor131(c);check(std::abs(layout[0].rect.right-.346f)<.000001f&&std::abs(layout[5].rect.right-.305f)<.000001f&&std::abs(layout[5].rect.bottom-.170f)<.000001f,"seven visible MTH buttons wrap at six reference columns");
 c.pickGokuDraw=false;c.autoGaDraw2=false;layout=wht::RouteButtonLayoutFor131(c);
 check(std::abs(layout[0].rect.right-.305f)<.000001f&&!layout[1].shown&&std::abs(layout[2].rect.right-.346f)<.000001f,"hidden buttons leave no layout holes");
 wht::RouteButtons131 toolbar;for(unsigned i=0;i<6;++i){toolbar.buttons[i].shown=layout[i].shown;toolbar.buttons[i].area=layout[i].rect;}
 const auto area=layout[2].rect;POINT center{LONG((area.right-area.width/2)*1000/.8f),LONG(900-(area.bottom+area.height/2)*900/.6f)};unsigned actions=0,index=99;
 auto action=[&](unsigned value){++actions;index=value;};
 check(toolbar.Mouse(WM_LBUTTONDOWN,center,1000,900,true,action)&&actions==1&&index==2,"route button dispatches the original logical route after compaction");
 check(toolbar.Mouse(WM_LBUTTONUP,{0,0},1000,900,false,action)&&actions==1,"release stays consumed outside button and after menu opens");
 check(toolbar.Mouse(WM_LBUTTONDBLCLK,center,1000,900,true,action)&&actions==1,"double click cannot start a second route");
 check(!toolbar.Mouse(WM_LBUTTONDOWN,center,1000,900,false,action)&&actions==1,"context gates suppress route button");
 toolbar.Reset();for(const auto& button:toolbar.buttons)check(!button.frame&&!button.label&&!button.shown,"UI teardown forgets all six button owners");
}
