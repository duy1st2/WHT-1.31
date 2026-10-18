#pragma once
#include "quick_chat131.hpp"
inline wht::QuickChatView131 chatViewTest131{11,22,33,44,false};
inline std::vector<std::string> chatStepsTest131;
inline std::string chatTextTest131;
inline bool chatRefuseTest131=false,chatReplaceTest131=false;
#ifdef _WIN64
#define CHAT_EDX131
#else
#define CHAT_EDX131 ,void*
#endif
inline void __fastcall ChatOpenTest131(uintptr_t bar CHAT_EDX131,int mode){
 check(bar==22&&(mode==0||mode==1),"native quick chat mode and receiver");chatStepsTest131.push_back(mode?"open":"close");chatViewTest131.open=mode!=0;
}
inline void __fastcall ChatTextTest131(uintptr_t edit CHAT_EDX131,const char* text,int notify){
 check(edit==33&&notify==1&&chatViewTest131.open,"native edit text only after opening the original chat");chatStepsTest131.push_back("text");chatTextTest131=text;if(chatReplaceTest131)chatViewTest131.edit=55;
}
inline bool __fastcall ChatEnterTest131(uintptr_t edit CHAT_EDX131,const void* data){
 check(edit==33&&chatViewTest131.open,"native Enter uses original edit owner");auto event=static_cast<const unsigned char*>(data);unsigned id=0,key=0,modifiers=1;
 memcpy(&id,event+2*sizeof(void*),4);memcpy(&key,event+4*sizeof(void*),4);memcpy(&modifiers,event+4*sizeof(void*)+4,4);
 check(id==0x40060064&&key==VK_RETURN&&!modifiers,"verified native key event layout has unmodified Enter");chatStepsTest131.push_back("enter");if(!chatRefuseTest131)chatViewTest131.open=false;return !chatRefuseTest131;
}
#undef CHAT_EDX131
inline void CheckQuickChat131(){
 const wht::QuickChatApi131 api{reinterpret_cast<decltype(api.open)>(ChatOpenTest131),reinterpret_cast<decltype(api.text)>(ChatTextTest131),reinterpret_cast<decltype(api.enter)>(ChatEnterTest131)};
 auto reset=[](){chatViewTest131={11,22,33,44,false};chatStepsTest131.clear();chatTextTest131.clear();chatRefuseTest131=chatReplaceTest131=false;};
 auto read=[](wht::QuickChatView131& out){out=chatViewTest131;return true;};
 const auto unicode=wht::QuickChatText(L"Chào đồng đội");reset();
 check(wht::SendQuickChat131(unicode,api,read)&&chatTextTest131==unicode&&chatStepsTest131==std::vector<std::string>({"open","text","enter"})&&!chatViewTest131.open,"UTF-8 quick chat follows normal native open/text/Enter flow");
 for(unsigned i=0;i<256;++i){reset();check(wht::SendQuickChat131("hello",api,read),"native quick chat ABI stable across repeated calls");}
 reset();chatViewTest131.open=true;check(!wht::SendQuickChat131("hello",api,read)&&chatStepsTest131.empty(),"existing chat is never cleared or submitted");
 for(const auto& invalid:std::vector<std::string>{"","  ","hello\nworld",std::string(129,'x'),std::string("a\0b",3),std::string("\xc0\xaf",2)}){reset();check(!wht::SendQuickChat131(invalid,api,read)&&chatStepsTest131.empty(),"malformed, multiline and oversized chat refused before native writes");}
 reset();check(wht::SendQuickChat131(std::string(128,'x'),api,read),"reference 128-byte limit accepted without truncation");
 reset();chatRefuseTest131=true;check(!wht::SendQuickChat131("hello",api,read)&&chatStepsTest131==std::vector<std::string>({"open","text","enter","close"})&&!chatViewTest131.open,"failed native Enter only closes the chat opened here");
 reset();chatReplaceTest131=true;check(!wht::SendQuickChat131("hello",api,read)&&chatStepsTest131==std::vector<std::string>({"open","text"})&&chatViewTest131.open,"owner replacement aborts without submitting or closing another editor");
 reset();check(!wht::SendQuickChat131("hello",{},read)&&chatStepsTest131.empty(),"unavailable native API cannot change chat state");
 wht::Config config;config.quickChatEnabled=true;
 for(unsigned i=0;i<9;++i){config.quickChat[i]={unsigned('1'+i),"line"+std::to_string(i)};}
 unsigned sent=0;std::string actual;auto send=[&](const std::string& value){actual=value;++sent;return true;};
 for(unsigned i=0;i<9;++i)check(wht::QuickChatKey131('1'+i,false,true,false,config,send)&&actual=="line"+std::to_string(i),"all nine quick chat slots dispatch independently");
 check(sent==9&&!wht::QuickChatKey131('1',true,true,false,config,send)&&!wht::QuickChatKey131('1',false,false,false,config,send)&&!wht::QuickChatKey131('1',false,true,true,config,send)&&sent==9,"repeat, disabled context and modifiers cannot send chat");
 config.quickChatEnabled=false;check(!wht::QuickChatKey131('1',false,true,false,config,send)&&sent==9,"master checkbox disables quick chat");
 config.quickChatEnabled=true;config.quickChat[0].vk=VK_ESCAPE;config.quickChat[1].vk=VK_RETURN;check(!wht::QuickChatKey131(VK_ESCAPE,false,true,false,config,send)&&!wht::QuickChatKey131(VK_RETURN,false,true,false,config,send),"native Enter/Escape behavior stays available");
 wht::QuickChatEcho131 echo;const LPARAM scan=LPARAM(0x10)<<16;echo.Sent(scan,100);check(!echo.Character(LPARAM(0x11)<<16,101)&&echo.Character(scan,102)&&!echo.Character(scan,103),"consume only one matching generated shortcut character");echo.Sent(scan,100);check(!echo.Character(scan,351),"expired shortcut echo never consumes later typing");echo.Reset();check(!echo.Character(scan,101),"UI/focus reset clears shortcut echo");
 for(const auto& p:wht::quickChatProfiles131){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char ui[0x800]{},bar[0x420]{},edit[0x500]{};uintptr_t label[2]{};
  *reinterpret_cast<uintptr_t*>(ui)=base+p.ui;*reinterpret_cast<uintptr_t*>(bar)=base+p.bar;*reinterpret_cast<uintptr_t*>(edit)=base+p.edit;
  *reinterpret_cast<uintptr_t*>(ui+0x400)=reinterpret_cast<uintptr_t>(bar);*reinterpret_cast<uintptr_t*>(bar+wht::quickChatEdit131)=reinterpret_cast<uintptr_t>(edit);*reinterpret_cast<uintptr_t*>(bar+wht::quickChatLabel131)=reinterpret_cast<uintptr_t>(label);
  wht::QuickChatView131 view;check(wht::ReadQuickChat131(base,p.build,reinterpret_cast<uintptr_t>(ui),view)&&!view.open,"both profiles resolve exact initialized native chat/edit objects");bar[wht::quickChatMode131]=1;check(wht::ReadQuickChat131(base,p.build,reinterpret_cast<uintptr_t>(ui),view)&&view.open,"native chat-open byte uses verified architecture layout");
  *reinterpret_cast<uintptr_t*>(edit)=base+p.baseEdit;check(!wht::ReadQuickChat131(base,p.build,reinterpret_cast<uintptr_t>(ui),view),"generic CEditBox cannot substitute for actual CChatEditBox");*reinterpret_cast<uintptr_t*>(edit)=base+p.edit;
  bar[wht::quickChatMode131]=2;check(!wht::ReadQuickChat131(base,p.build,reinterpret_cast<uintptr_t>(ui),view),"unknown chat mode refused");
 }
 check(!wht::QuickChatProfileFor131(129)&&!wht::QuickChatNative131(0,99999),"no inherited 1.29 chat offsets enabled");
}
