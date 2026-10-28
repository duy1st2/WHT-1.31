#pragma once
#include "mute_chat_profile131.hpp"
#include "repeat_chat131.hpp"
#include "component_lookup131.hpp"
namespace wht {
inline const MuteChatProfile131* MuteChatProfileFor131(unsigned build){for(const auto& p:muteChatProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool MuteChatCode131(uintptr_t base,const MuteChatProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
using MuteDisplayNative131=void(__thiscall*)(uintptr_t,unsigned,const char*,int,float);
inline MuteDisplayNative131 originalMuteDisplay131=nullptr;
inline uintptr_t muteDisplayBase131=0,muteDisplayUiTable131=0;
inline bool muteDisplayReady131=false;
#ifdef _WIN64
inline void MuteDisplayHook131(uintptr_t object,unsigned sender,const char* text,int channel,float duration){
#else
inline void __fastcall MuteDisplayHook131(uintptr_t object,void*,unsigned sender,const char* text,int channel,float duration){
#endif
 const bool owner=mutedChat131.Hidden(sender,channel)&&ComponentIdentity131(object,muteDisplayBase131+muteDisplayUiTable131);
 DispatchMuteDisplay131(owner,object,sender,text,channel,duration,[](uintptr_t o,unsigned s,const char* t,int c,float d){originalMuteDisplay131(o,s,t,c,d);});
}
inline bool InstallMuteDisplay131(uintptr_t base,unsigned build){
 if(muteDisplayReady131)return true;const auto* p=MuteChatProfileFor131(build);if(!p||!MuteChatCode131(base,*p))return false;
 const auto status=MH_Initialize();if(status!=MH_OK&&status!=MH_ERROR_ALREADY_INITIALIZED)return false;
 auto target=reinterpret_cast<void*>(base+p->display);
 if(MH_CreateHook(target,reinterpret_cast<void*>(MuteDisplayHook131),reinterpret_cast<void**>(&originalMuteDisplay131))!=MH_OK)return false;
 muteDisplayBase131=base;muteDisplayUiTable131=p->uiTable;
 if(MH_EnableHook(target)==MH_OK){muteDisplayReady131=true;return true;}
 MH_RemoveHook(target);originalMuteDisplay131=nullptr;muteDisplayBase131=muteDisplayUiTable131=0;return false;
}
inline bool MuteChatCommand131(uintptr_t base,unsigned build,bool allowed,const std::wstring& name,const std::wstring& args,std::string& report){
 unsigned slot=0;if(!ParseMuteSlot131(name,args,slot)){report="Usage: #mute <1..24> or #unmute <1..24>";return false;}
 PublishRepeatChat131(base,build,allowed);if(!mutedChat131.Ready()){report="Mute Player: active Local match required";return false;}
 if(name==L"#mute"&&(!InstallMuteDisplay131(base,build)||!InstallRepeatChat131(base,build))){report="Mute Player: native display/history hooks unavailable";return false;}
 if(!mutedChat131.Set(slot,name==L"#mute")){report="Mute Player: cannot mute yourself or match changed";return false;}
 report="Mute Player "+std::to_string(slot)+(name==L"#mute"?": ON":": OFF");return true;
}
}
