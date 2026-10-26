#pragma once
#include "mth_notice131.hpp"
inline void CheckMthNotice131(){
 check(wht::MthNoticeText131(wht::MthNoticeKind131::Acquired,true)=="[MTH] Da co Goku trong tui, Auto Pick Goku: Off","Goku acquired notification matches 1.29");
 check(wht::MthNoticeText131(wht::MthNoticeKind131::CourierAssigned,true)=="Set Ga: selected unit assigned","courier assignment matches latest 1.29");
 check(wht::MthNoticeText131(wht::MthNoticeKind131::BaseSaved,true)=="Courier Base: position saved","base saved matches latest 1.29");
 check(wht::MthNoticeText131(wht::MthNoticeKind131::ReturnQueued,true)=="Courier Return: queued","return acknowledgement matches latest 1.29");
 wht::MthNotice131 notice;using Kind=wht::MthNoticeKind131;
 notice.Event(Kind::On,true,100);check(notice.Visible(true,true,false,100)&&notice.until==4100,"MTH event displays for four seconds");
 check(!notice.Visible(true,true,true,200)&&notice.kind==Kind::On,"menu masks notification without changing action state");
 check(notice.Visible(true,true,false,300),"closing menu restores unexpired notification");
 check(!notice.Visible(true,true,false,4100)&&notice.kind==Kind::None,"notification expires without replay");
 notice.Event(Kind::Acquired,true,5000);check(!notice.Visible(true,false,false,5100)&&!notice.until,"disabling notification clears existing text");
 notice.Event(Kind::Off,false,6000);check(notice.kind==Kind::None,"disabled preference does not queue future notification");
 notice.Event(Kind::On,true,7000);check(!notice.Visible(false,true,false,7100)&&notice.kind==Kind::None,"lost session clears notification");
 notice.Reset();check(!notice.frame&&!notice.label&&!notice.shown&&notice.applied.empty(),"UI teardown forgets all owned frame pointers");
 check(wht::MthNoticeText131(Kind::On,true)=="[MTH] Auto Pick Goku: On"&&!wht::MthNoticeText131(Kind::Acquired,false).empty()&&wht::MthNoticeText131(Kind::None,false).empty(),"bounded notification kinds support both languages");
 for(auto kind:{Kind::CourierAssigned,Kind::BaseSaved,Kind::BaseRefused,Kind::CourierRequired,Kind::ReturnQueued,Kind::ReturnSent,Kind::RouteQueued,Kind::RouteSent,Kind::OrderRefused}){
  const auto en=wht::MthNoticeText131(kind,true),vi=wht::MthNoticeText131(kind,false);check(!en.empty()&&!vi.empty()&&en!=vi&&vi.find('?')==std::string::npos,"courier status has distinct English and Vietnamese text");
  notice.Event(kind,true,8000);check(notice.Visible(true,true,false,8001)&&notice.until==12000,"courier events use existing bounded notice lifetime");notice.Event(kind,false,8002);check(!notice.Visible(true,true,false,8003),"disabled Noti never displays courier feedback");
 }
 check(wht::MthNoticeText131(Kind::CourierRequired,true).find("#setga")!=std::string::npos&&wht::MthNoticeText131(Kind::CourierRequired,false).find("#gabase")!=std::string::npos,"missing courier/base message gives usable commands in both languages");
 notice.Event(Kind::RouteQueued,true,10000);notice.Event(Kind::RouteSent,true,10001);check(notice.kind==Kind::RouteSent&&notice.until==14001,"actual dispatch status replaces request status without accumulating frames");
 ViewIniTest131 ini;auto config=wht::LoadConfig(ini.file);unsigned writes=0,reloads=0;
 auto save=[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){++writes;return wht::SaveSetting(ini.file,section,key,value);};
 auto reload=[&](){++reloads;config=wht::LoadConfig(ini.file);};
 auto run=[&](const std::wstring& args){return wht::ApplyGokuDraw131(args,config.pickGokuDraw,save,reload);};
 check(run(L"on")&&config.pickGokuDraw&&run(L"FALSE")&&!config.pickGokuDraw&&run(L"")&&config.pickGokuDraw,"draw command uses reference boolean aliases and persists actual config");
 const unsigned before=writes;for(auto arg:{L"2",L"1 extra",L"1\n"})check(!run(arg),"malformed draw command refused");
 check(!run(std::wstring(L"1\0x",3))&&writes==before&&reloads==before,"invalid command cannot persist or reload");
 check(!wht::ApplyGokuDraw131(L"0",true,[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&reloads==before,"failed save cannot publish new button setting");
}
