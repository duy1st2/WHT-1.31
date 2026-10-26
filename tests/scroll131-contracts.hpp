#pragma once
#include "scroll131.hpp"
#include <fstream>
inline unsigned scrollAbiCalls131=0;
#ifdef _WIN64
inline void __fastcall ScrollMock131(uintptr_t camera,float value){check(camera==123&&value==2.5f,"native x64 camera scroll setter ABI");++scrollAbiCalls131;}
#else
inline void __fastcall ScrollMock131(uintptr_t camera,uintptr_t,float value){check(camera==123&&value==2.5f,"native x86 camera scroll setter ABI");++scrollAbiCalls131;}
#endif
inline void CheckScroll131(){
 check(wht::SetScroll131(123,reinterpret_cast<uintptr_t>(ScrollMock131),2.5f)&&scrollAbiCalls131==1,"raw native setter receives speed directly without legacy subtraction");
 wht::ScrollState131 state;float current=.75f;unsigned writes=0;bool refuse=false;
 auto read=[&](uintptr_t,float& value){value=current;return true;};auto set=[&](uintptr_t,float value){++writes;if(refuse)return false;current=value;return true;};
 check(state.Apply(1,200,true,read,set)&&current==2&&state.saved==.75f,"percent converts to native speed and saves original");
 check(state.Apply(1,200,true,read,set)&&writes==1,"unchanged updates avoid redundant native setters");
 check(state.Apply(1,700,true,read,set)&&current==7&&state.saved==.75f,"700 percent supported without replacing original baseline");
 check(state.Apply(1,0,true,read,set)&&current==1,"zero percent uses default speed like 1.29");
 check(state.Apply(1,0,false,read,set)&&current==.75f&&!state.active,"disable restores captured speed");
 state.Apply(1,200,true,read,set);current=3.25f;state.Apply(1,100,false,read,set);check(current==3.25f&&!state.active,"disable preserves external camera-option changes");
 state.Apply(1,200,true,read,set);current=4.5f;state.Apply(1,300,true,read,set);check(state.saved==4.5f&&current==3,"active external change becomes new restoration baseline");state.Apply(1,100,false,read,set);check(current==4.5f,"restores latest external baseline");
 current=2;state.Apply(2,100,true,read,set);check(state.owner==2&&state.saved==2,"new camera never inherits old owner's backup");const auto before=writes;check(!state.Apply(2,701,true,read,set)&&writes==before,"out-of-range setting cannot write native camera");
 current=NAN;check(!state.Apply(2,100,true,read,set)&&!state.active,"invalid native speed refused");current=1;refuse=true;check(!state.Apply(2,200,true,read,set)&&!state.active,"native setter failure cannot claim active override");refuse=false;
 state.Reset();check(state.Apply(0,100,false,read,set)&&!state.owner,"missing camera clears state");
 int value=123;for(auto arg:{L"",L"-1",L"701",L"1.5",L"20 extra",L"20\n"})check(!wht::ParseScrollPercent131(arg,value)&&value==123,"invalid scroll command leaves parsed output unchanged");check(wht::ParseScrollPercent131(L" 700 ",value)&&value==700&&wht::ParseScrollPercent131(L"0",value)&&value==0,"reference scroll boundaries parse");
 ViewIniTest131 ini;unsigned reloads=0;auto save=[&](const wchar_t* section,const wchar_t* key,const std::wstring& v){return wht::SaveSetting(ini.file,section,key,v);};auto reload=[&](){++reloads;};
 check(wht::ApplyScrollCommand131(L"#kscroll",L"250",save,reload)&&wht::LoadConfig(ini.file).keyboardScroll==250&&wht::LoadConfig(ini.file).mouseScroll==100,"keyboard command persists without changing mouse speed");check(wht::ApplyScrollCommand131(L"#mscroll",L"0",save,reload)&&wht::LoadConfig(ini.file).mouseScroll==0&&reloads==2,"mouse command preserves default sentinel");check(!wht::ApplyScrollCommand131(L"#kscroll",L"701",save,reload)&&!wht::ApplyScrollCommand131(L"#mscroll",L"100",[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&reloads==2,"malformed input/failed persistence cannot reload");
#ifdef _WIN64
 {std::ifstream input("../analysis/memory-6688.bin",std::ios::binary);check(bool(input),"captured native scroll fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 check(wht::ScrollCode131(base,12164),"full native keyboard/mouse setters and constants verified"); memory[0xc372e0]^=1;check(!wht::ScrollCode131(base,12164),"changed native setter refused");memory[0xc372e0]^=1;memory[0x1bc62e8]^=1;check(!wht::ScrollCode131(base,12164),"changed native scroll constants refused");}}
 {std::ifstream input("../analysis/memory-29928.bin",std::ios::binary);check(bool(input),"captured native scroll fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 check(wht::ScrollCode131(base,12173),"full native keyboard/mouse setters and constants verified"); memory[0xc7db60]^=1;check(!wht::ScrollCode131(base,12173),"changed native setter refused");memory[0xc7db60]^=1;memory[0x1bf52c8]^=1;check(!wht::ScrollCode131(base,12173),"changed native scroll constants refused");}}
#else
 {std::ifstream input("../analysis/memory-14740.bin",std::ios::binary);check(bool(input),"captured native scroll fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee85f)=base+0x1720718;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee869)=base+0x175ab60;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee875)=base+0x180747c;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee882)=base+0x1721140;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee8cf)=base+0x1720718;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee8d9)=base+0x175ab60;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee8e5)=base+0x180747c;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaee8f2)=base+0x1721140;
 check(wht::ScrollCode131(base,12164),"full native keyboard/mouse setters and constants verified"); memory[0xaee850]^=1;check(!wht::ScrollCode131(base,12164),"changed native setter refused");memory[0xaee850]^=1;memory[0x1720718]^=1;check(!wht::ScrollCode131(base,12164),"changed native scroll constants refused");}}
 {std::ifstream input("../analysis/memory-23884.bin",std::ios::binary);check(bool(input),"captured native scroll fixture available");std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});if(!memory.empty()){const auto base=reinterpret_cast<uintptr_t>(memory.data());
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad74f)=base+0x16d5728;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad759)=base+0x170fba0;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad765)=base+0x17bc4ac;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad772)=base+0x16d6150;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad7bf)=base+0x16d5728;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad7c9)=base+0x170fba0;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad7d5)=base+0x17bc4ac;
 *reinterpret_cast<uintptr_t*>(memory.data()+0xaad7e2)=base+0x16d6150;
 check(wht::ScrollCode131(base,12173),"full native keyboard/mouse setters and constants verified"); memory[0xaad740]^=1;check(!wht::ScrollCode131(base,12173),"changed native setter refused");memory[0xaad740]^=1;memory[0x16d5728]^=1;check(!wht::ScrollCode131(base,12173),"changed native scroll constants refused");}}
#endif
}
