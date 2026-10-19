#pragma once
#include "auto_click_commands131.hpp"
inline void CheckAutoClickCommands131(){
 ViewIniTest131 ini;wht::Config config=wht::LoadConfig(ini.file);wht::AutoClick131 state;unsigned saves=0,reloads=0;
 auto save=[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){++saves;return wht::SaveSetting(ini.file,section,key,value);};
 auto reload=[&](){++reloads;config=wht::LoadConfig(ini.file);};
 auto run=[&](const wchar_t* name,const std::wstring& args){return wht::ApplyAutoClickCommand131(name,args,state,save,reload);};
 check(run(L"#autol",L"")&&state.left&&!state.right,"left command toggles only left");
 state.due=900;check(run(L"#autoclickl",L"0")&&!state.left&&!state.due,"left alias disables and resets cadence");
 check(run(L"#autor",L"1")&&state.right&&run(L"#autoclickr",L"")&&!state.right,"right aliases share state");
 check(saves==0&&reloads==0,"runtime toggles do not persist active clicks");
 check(run(L"#clickdelay",L"5")&&config.autoClickInterval==5&&run(L"#clickdelay",L"1000")&&config.autoClickInterval==1000,"interval boundaries persist through real config");
 state.due=123;const unsigned writes=saves;
 for(auto arg:{L"",L"4",L"1001",L"-5",L"+5",L"5.0",L"5 10",L"5\n",L"999999999999999999"})check(!run(L"#clickdelay",arg),"invalid interval refused");
 for(auto arg:{L"on",L"2",L"1 0",L"1\n"})check(!run(L"#autol",arg),"invalid toggle refused");
 check(!run(L"#autor",std::wstring(L"1\0x",3))&&!run(L"#clickdelay",std::wstring(L"5\0x",3)),"NUL refused");
 check(saves==writes&&reloads==writes&&state.due==123&&!state.left&&!state.right,"invalid commands preserve settings and cadence");
 check(!wht::ApplyAutoClickCommand131(L"#clickdelay",L"10",state,[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&state.due==123&&reloads==writes,"failed save leaves running cadence intact");
 check(run(L"#autol",L"1")&&run(L"#autor",L"1")&&run(L"#clickdelay",L"25"),"both buttons and new interval configured");
 unsigned clicks=0;auto send=[&](UINT,WPARAM){++clicks;};
 state.Tick(config,false,100,send);check(clicks==0,"typing menu background gate suppresses command activated clicks");
 state.Tick(config,true,100,send);check(clicks==4&&state.due==125,"one down up pair per button");
 state.Tick(config,true,124,send);check(clicks==4,"configured interval enforced");
 state.Tick(config,true,1000,send);check(clicks==8&&state.due==1025,"late scheduler emits no catch up burst");
 state.Reset();state.Tick(config,true,2000,send);check(clicks==8&&!state.left&&!state.right,"match teardown cancels both runtime toggles");
 check(!run(L"#unknown",L"1"),"unknown commands refused");
}
