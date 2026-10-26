#pragma once
#include "command_feedback131.hpp"
#include <fstream>
inline unsigned notificationCalls131=0;inline std::string notificationCaptured131;
#ifdef _WIN64
inline void __fastcall NotificationMock131(uintptr_t ui,float x,float y,const char* text,float duration,const unsigned* color){check(ui==123&&x==0&&y==0&&duration==4&&color&&*color==0xffffffffu,"native x64 local message ABI including color pointer");++notificationCalls131;notificationCaptured131=text;}
#else
inline void __fastcall NotificationMock131(uintptr_t ui,uintptr_t,float x,float y,const char* text,float duration,unsigned color){check(ui==123&&x==0&&y==0&&duration==4&&color==0xffffffffu,"native x86 local message ABI including color value");++notificationCalls131;notificationCaptured131=text;}
#endif
inline void CheckNotifications131(){
 check(wht::NotificationText("Auto Pick enabled")=="|cffffff00Auto Pick: |cff00ff00ON|cffffff00|r","latest reference normalizes enabled and colors ON");
 check(wht::NotificationText("Cooldown OFF")=="|cffffff00Cooldown: |cffff0000OFF|cffffff00|r","latest reference colors OFF red");
 check(wht::NotificationText("Set Camera: 2500")=="|cffffff00Set Camera: |cff00ff002500|r","latest reference colors setting value green");
 check(wht::NotificationText("|cffff0000[WHT] [WHT] Feature (#test) ON|r")=="|cffffff00[WHT] Feature: |cff00ff00ON|cffffff00|r","old colors, duplicate leading tags and command annotations normalized");
 const auto mock=reinterpret_cast<wht::LocalMessage131>(NotificationMock131);check(wht::DispatchNotification131(123,"Auto Pick: ON",mock)&&notificationCalls131==1&&notificationCaptured131==wht::NotificationText("Auto Pick: ON"),"native notification applies reference formatter once");
 check(!wht::DispatchNotification131(0,"test",mock)&&!wht::DispatchNotification131(123,"",mock)&&!wht::DispatchNotification131(123,std::string(1025,'x'),mock)&&!wht::DispatchNotification131(123,std::string("x\0y",3),mock)&&notificationCalls131==1,"invalid notification cannot enter native renderer");
 check(wht::MthRouteStarted131(0,5)=="[MTH]: Chay Ga 1 bat dau (5 luot qua lai)!"&&wht::MthRouteStarted131(5,20)=="[MTH]: Chay Soi 3 bat dau (20 luot qua lai)!"&&wht::MthRouteStarted131(6,1).empty(),"route/count feedback matches latest 1.29 exactly");
 wht::Config config;config.showTimer=true;auto feedback=[&](const std::wstring& name,const std::wstring& args,bool ok=true){return wht::CommandFeedback131(name,args,ok,config,false,true,false,false,true,false,false);};
 check(feedback(L"#timer",L"").find("ON")!=std::string::npos,"toggle feedback reads resulting configuration");check(feedback(L"#cam",L" 2500")=="Set Camera: 2500","camera feedback uses reference setting title/value");
 check(feedback(L"#unknown",L"",false).empty()&&feedback(L"#islot",L"").empty()&&feedback(L"#goku",L"").empty(),"unknown/refused or already handled commands do not emit duplicate generic success");
 size_t timer=0;for(size_t i=0;i<wht::kToolMenuControls.size();++i)if(std::wstring(wht::kToolMenuControls[i].key)==L"Show Timer")timer=i;
 check(feedback(L"#toolui",std::to_wstring(timer)+L" toggle").find("ON")!=std::string::npos,"menu checkbox feedback reports persisted state");
#ifdef _WIN64
 {std::ifstream input("../analysis/memory-6688.bin",std::ios::binary);check(bool(input),"captured native notification fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 check(wht::NotificationNative131(base,12164)==base+0xc5dc90,"captured complete native notification wrapper verified");memory[0xc5dc90]^=1;check(!wht::NotificationNative131(base,12164),"modified native notification bytes refused");}}
 {std::ifstream input("../analysis/memory-29928.bin",std::ios::binary);check(bool(input),"captured native notification fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 check(wht::NotificationNative131(base,12173)==base+0xca0ec0,"captured complete native notification wrapper verified");memory[0xca0ec0]^=1;check(!wht::NotificationNative131(base,12173),"modified native notification bytes refused");}}
#else
 {std::ifstream input("../analysis/memory-14740.bin",std::ios::binary);check(bool(input),"captured native notification fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 *reinterpret_cast<uintptr_t*>(memory.data()+0xb11b1b)=base+0x18253fc;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xb11b28)=base+0x1825404;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xb11b39)=base+0x1825400;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xb11b54)=base+0x181483c;
 check(wht::NotificationNative131(base,12164)==base+0xb11af0,"captured complete native notification wrapper verified");memory[0xb11af0]^=1;check(!wht::NotificationNative131(base,12164),"modified native notification bytes refused");}}
 {std::ifstream input("../analysis/memory-23884.bin",std::ios::binary);check(bool(input),"captured native notification fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 *reinterpret_cast<uintptr_t*>(memory.data()+0xad142b)=base+0x17da2dc;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xad1438)=base+0x17da2e4;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xad1449)=base+0x17da2e0;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xad1464)=base+0x17c970c;
 check(wht::NotificationNative131(base,12173)==base+0xad1400,"captured complete native notification wrapper verified");memory[0xad1400]^=1;check(!wht::NotificationNative131(base,12173),"modified native notification bytes refused");}}
#endif
}
