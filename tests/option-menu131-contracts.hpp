#pragma once
#include "option_menu131.hpp"
inline void CheckOptionMenu131(){
 wht::OptionMenu131 input;
 check(input.Key(WM_KEYDOWN,VK_F10,true,false,true,false)&&input.f10,"enabled option blocks ordinary F10");check(input.Held(WM_SYSKEYDOWN,VK_F10)&&input.Held(WM_SYSKEYUP,VK_F10)&&!input.f10,"repeat and system-key release consumed as one press");
 for(bool enabled:{false,true})for(bool typing:{false,true})for(bool foreground:{false,true})for(bool modifiers:{false,true}){input.Reset();check(input.Key(WM_SYSKEYDOWN,VK_F10,enabled,typing,foreground,modifiers)==(enabled&&!typing&&foreground&&!modifiers),"system F10 respects feature/chat/focus/modifier gates");}
 input.Reset();check(!input.Key(WM_KEYDOWN,VK_F9,true,false,true,false)&&!input.Key(WM_KEYUP,VK_F10,true,false,true,false),"unrelated keys and unmatched release pass through");
 input.Key(WM_KEYDOWN,VK_F10,true,false,true,false);check(input.Held(WM_KEYUP,VK_F10),"owned release stays consumed after config or context changes");
 input.ToolToggle(VK_F10);check(input.Held(WM_KEYDOWN,VK_F10)&&input.Held(WM_KEYUP,VK_F10),"F10 used by configured tool-menu toggle/capture cannot leak repeats or key-up to game");input.ToolToggle(VK_F8);check(!input.f10,"other tool-menu keys unaffected");
 for(const auto size:{std::pair<int,int>{800,600},{1600,900},{2560,1080}}){const int w=size.first,h=size.second,classic=(std::min)(w,h*4/3);POINT p{(w-classic)/2+int(.13f/.8f*classic),int(.02f/.6f*h)};
  check(wht::OptionMenuHit131(p,w,h),"reference F10 button projects through centered classic canvas");check(input.Mouse(WM_LBUTTONDOWN,p,w,h,true,false,true)&&input.Held(WM_LBUTTONUP,0),"native menu-button down/up blocked");check(input.Mouse(WM_LBUTTONDBLCLK,p,w,h,true,false,true)&&input.Held(WM_LBUTTONUP,0),"double click cannot reopen game menu");check(input.Mouse(WM_RBUTTONDOWN,p,w,h,true,false,true)&&input.Held(WM_RBUTTONUP,0),"reference right-button region also paired");
  check(!input.Mouse(WM_LBUTTONDOWN,p,w,h,false,false,true)&&!input.Mouse(WM_LBUTTONDOWN,p,w,h,true,true,true)&&!input.Mouse(WM_LBUTTONDOWN,p,w,h,true,false,false),"menu-button filter honors preference/chat/focus");
  check(!input.Mouse(WM_LBUTTONDOWN,{w/2,h/2},w,h,true,false,true),"map clicks outside native button preserved");
 }check(!wht::OptionMenuHit131({0,0},0,600),"invalid canvas refused");
 input.Mouse(WM_LBUTTONDOWN,{130,20},800,600,true,false,true);check(input.Held(WM_LBUTTONUP,0),"release outside button stays consumed after drag");input.Reset();check(!input.mouse&&!input.f10,"match teardown clears input ownership");
 ViewIniTest131 ini;auto config=wht::LoadConfig(ini.file);const auto* command=wht::ViewCommandFor131(L"#hidemenu");check(command!=nullptr,"reference hidemenu command exposed");if(command){unsigned reloads=0;auto save=[&](const wchar_t* section,const wchar_t* key,const wchar_t* value){return wht::SaveSetting(ini.file,section,key,value);};auto reload=[&](){++reloads;config=wht::LoadConfig(ini.file);};
 check(wht::ApplyViewCommand131(*command,L"",config,save,reload)&&config.hideOptionMenu,"bare hidemenu toggles persisted reference option");check(wht::ApplyViewCommand131(*command,L"0",config,save,reload)&&!config.hideOptionMenu,"explicit zero restores native F10 behavior");check(!wht::ApplyViewCommand131(*command,L"1 extra",config,save,reload)&&reloads==2,"invalid command leaves preference untouched");}
}
