#pragma once
#include "mth_notice131.hpp"
inline void CheckMthNotice131(){
 wht::MthNotice131 notice;using Kind=wht::MthNoticeKind131;
 notice.Event(Kind::On,true,100);check(notice.Visible(true,true,false,100)&&notice.until==4100,"MTH event displays for four seconds");
 check(!notice.Visible(true,true,true,200)&&notice.kind==Kind::On,"menu masks notification without changing action state");
 check(notice.Visible(true,true,false,300),"closing menu restores unexpired notification");
 check(!notice.Visible(true,true,false,4100)&&notice.kind==Kind::None,"notification expires without replay");
 notice.Event(Kind::Acquired,true,5000);check(!notice.Visible(true,false,false,5100)&&!notice.until,"disabling notification clears existing text");
 notice.Event(Kind::Off,false,6000);check(notice.kind==Kind::None,"disabled preference does not queue future notification");
 notice.Event(Kind::On,true,7000);check(!notice.Visible(false,true,false,7100)&&notice.kind==Kind::None,"lost session clears notification");
 notice.Reset();check(!notice.frame&&!notice.label&&!notice.shown&&notice.applied.empty(),"UI teardown forgets all owned frame pointers");
 check(wht::MthNoticeText131(Kind::On,true)=="Auto Pick Goku: ON"&&!wht::MthNoticeText131(Kind::Acquired,false).empty()&&wht::MthNoticeText131(Kind::None,false).empty(),"bounded notification kinds support both languages");
 ViewIniTest131 ini;auto config=wht::LoadConfig(ini.file);unsigned writes=0,reloads=0;
 auto save=[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){++writes;return wht::SaveSetting(ini.file,section,key,value);};
 auto reload=[&](){++reloads;config=wht::LoadConfig(ini.file);};
 auto run=[&](const std::wstring& args){return wht::ApplyGokuDraw131(args,config.pickGokuDraw,save,reload);};
 check(run(L"on")&&config.pickGokuDraw&&run(L"FALSE")&&!config.pickGokuDraw&&run(L"")&&config.pickGokuDraw,"draw command uses reference boolean aliases and persists actual config");
 const unsigned before=writes;for(auto arg:{L"2",L"1 extra",L"1\n"})check(!run(arg),"malformed draw command refused");
 check(!run(std::wstring(L"1\0x",3))&&writes==before&&reloads==before,"invalid command cannot persist or reload");
 check(!wht::ApplyGokuDraw131(L"0",true,[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&reloads==before,"failed save cannot publish new button setting");
}
