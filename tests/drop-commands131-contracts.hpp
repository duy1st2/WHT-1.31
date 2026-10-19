#pragma once
#include "drop_commands131.hpp"
inline void CheckDropCommands131(){
 int slot=99;check(wht::DropCommandSlot131(L"#drop",L"",slot)&&slot==-1&&wht::DropCommandSlot131(L"#dropall",L"  ",slot)&&slot==-1,"all drop aliases");
 for(int i=1;i<=6;++i)check(wht::DropCommandSlot131(L"#dropitem",std::to_wstring(i),slot)&&slot==i-1,"one based inventory slot");
 for(auto arg:{L"",L"0",L"7",L"-1",L"01",L"1 2",L"1\n",L"1.0"})check(!wht::DropCommandSlot131(L"#dropitem",arg,slot),"invalid slots refused");
 check(!wht::DropCommandSlot131(L"#drop",L"1",slot)&&!wht::DropCommandSlot131(L"#quickdrop",L"1",slot),"quickdrop retains its checkbox command semantics");
 check(!wht::DropCommandSlot131(L"#dropitem",std::wstring(L"1\0x",3),slot),"NUL refused");
 wht::QuickDropFlow131 flow;unsigned stopped=0,sent=0;bool selling=true,autoPickEnabled=true;
 auto stop=[&](){++stopped;selling=false;};
 auto send=[&](){check(!selling&&flow.pickupAfter==2100,"reference order: stop sell then delay pickup then submit");++sent;return true;};
 check(flow.Run(100,stop,send)&&stopped==1&&sent==1&&autoPickEnabled,"one direct send, Auto Pick enable state retained");
 check(2099<flow.pickupAfter&&2100>=flow.pickupAfter,"reference two second pickup pause");
 check(!flow.Run(300,stop,[&](){++sent;return false;})&&sent==2&&flow.pickupAfter==2300,"native refusal has no automatic retry, still pauses pickup");
 flow.Reset();check(flow.pickupAfter==0,"match teardown clears pause");
 ViewIniTest131 ini;auto config=wht::LoadConfig(ini.file);const auto* option=wht::ViewCommandFor131(L"#quickdrop");
 check(option&&wht::ApplyViewCommand131(*option,L"0",config,[&](const wchar_t* section,const wchar_t* key,const wchar_t* value){return wht::SaveSetting(ini.file,section,key,value);},[&](){config=wht::LoadConfig(ini.file);})&&!config.quickDropItem,"quickdrop toggles real checkbox configuration");
}
