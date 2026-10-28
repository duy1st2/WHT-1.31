from pathlib import Path
root=Path(__file__).resolve().parents[1]
src=(root/'tests/save-game-tests.cpp').read_text(encoding='utf-8');src=src[:src.index(' std::string name;')]
src=src.replace('save_game131.hpp','mute_chat131.hpp').replace('saveGameProfiles131','muteChatProfiles131').replace('SaveGameCode131','MuteChatCode131')
definitions=r'''
volatile unsigned nativeCalls=0,historyCalls=0;uintptr_t seenObject=0;unsigned seenSender=0;const char* seenText=nullptr;int seenChannel=0;float seenDuration=0;
#ifdef _WIN64
__declspec(noinline) void MockDisplay(uintptr_t o,unsigned s,const char* t,int c,float d){
#else
__declspec(noinline) void __fastcall MockDisplay(uintptr_t o,void*,unsigned s,const char* t,int c,float d){
#endif
 ++nativeCalls;seenObject=o;seenSender=s;seenText=t;seenChannel=c;seenDuration=d;
}
#ifdef _WIN64
void MockHistory(uintptr_t,unsigned,const char*,int){++historyCalls;}
#else
void __fastcall MockHistory(uintptr_t,void*,unsigned,const char*,int){++historyCalls;}
#endif
'''
src=src.replace('int main(){try{',definitions+'\nint main(){try{')
src+=r'''
 unsigned slot=0;for(unsigned i=1;i<=24;++i)Check(wht::ParseMuteSlot131(L"#mute",std::to_wstring(i),slot)&&slot==i,"all24 player slots");
 for(const auto* bad:{L"",L"0",L"25",L"-1",L"+1",L"one",L"1 2",L"1x",L"2\n"})Check(!wht::ParseMuteSlot131(L"#mute",bad,slot),"bad input refused");Check(!wht::ParseMuteSlot131(L"#unmute",std::wstring(L"1\0x",3),slot),"NUL refused");
 wht::MuteChatState131 state;Check(!state.Set(2,true),"no published context cannot mute");state.Publish(true,123,0);Check(!state.Set(1,true)&&state.Set(2,true)&&state.Set(24,true),"self protected and high slot supported");
 Check(state.Hidden(1,0)&&state.Hidden(1,1)&&state.Hidden(1,2)&&state.Hidden(23,0),"all allies observers channels filtered");Check(!state.Hidden(0,0)&&!state.Hidden(24,0)&&!state.Hidden(~0u,0)&&!state.Hidden(1,-1)&&!state.Hidden(1,3),"self and system messages always visible");
 state.Publish(true,123,0);Check(state.Hidden(1,0),"same match publish preserves mask");Check(state.Set(2,false)&&!state.Hidden(1,0),"unmute immediate");state.Publish(true,456,0);Check(!state.Hidden(23,0),"new world clears old slot masks");state.Set(24,true);state.Publish(true,456,2);Check(!state.Hidden(23,0),"local player change clears masks");state.Set(24,true);state.Publish(false,456,2);Check(!state.Ready()&&!state.Hidden(23,0),"authorization loss clears filter");
 // Exercise the real hook/trampoline ABI on an owned test function only.
 wht::mutedChat131.Publish(true,123,0);wht::mutedChat131.Set(2,true);uintptr_t objectTable=0x3000;const uintptr_t object=reinterpret_cast<uintptr_t>(&objectTable);wht::muteDisplayBase131=0x1000;wht::muteDisplayUiTable131=0x2000;
 const auto status=MH_Initialize();Check(status==MH_OK||status==MH_ERROR_ALREADY_INITIALIZED,"MinHook test init");void* target=reinterpret_cast<void*>(MockDisplay);
 Check(MH_CreateHook(target,reinterpret_cast<void*>(wht::MuteDisplayHook131),reinterpret_cast<void**>(&wht::originalMuteDisplay131))==MH_OK&&MH_EnableHook(target)==MH_OK,"owned native ABI hook installed");
 wht::MuteDisplayNative131 volatile invoke=reinterpret_cast<wht::MuteDisplayNative131>(target);const char text[]="native chat text";
 for(unsigned i=0;i<100;++i)invoke(object,1,text,0,3.25f);Check(nativeCalls==0,"muted screen chat skips native display with correct stack cleanup");
 invoke(object,0,text,0,3.25f);Check(nativeCalls==1&&seenObject==object&&seenSender==0&&seenText==text&&seenChannel==0&&seenDuration==3.25f,"unmuted trampoline preserves object sender pointer channel and duration");
 invoke(object,1,nullptr,-1,10.f);Check(nativeCalls==2&&seenText==nullptr&&seenChannel==-1&&seenDuration==10.f,"system native arguments preserved without text inspection");objectTable=0x3008;invoke(object,1,text,0,1.5f);Check(nativeCalls==3,"unverified owner always forwarded");objectTable=0x3000;
 wht::mutedChat131.Set(2,false);invoke(object,1,text,1,7.5f);Check(nativeCalls==4&&seenDuration==7.5f,"unmute forwards immediately");
 Check(MH_DisableHook(target)==MH_OK&&MH_RemoveHook(target)==MH_OK,"owned hook removed");MH_Uninitialize();wht::originalMuteDisplay131=nullptr;
 wht::originalChatHistory131=reinterpret_cast<wht::ChatHistoryNative131>(MockHistory);wht::mutedChat131.Set(2,true);wht::repeatChat131.Publish(true,0);std::string report;Check(wht::repeatChat131.Command(L"all",true,report),"observer test enabled");
#ifdef _WIN64
 wht::ChatHistoryHook131(object,1,text,0);
#else
 wht::ChatHistoryHook131(object,nullptr,1,text,0);
#endif
 Check(historyCalls==0&&wht::repeatChat131.pending.size()==1,"muted history suppressed after repeat observer sees message");wht::mutedChat131.Set(2,false);
#ifdef _WIN64
 wht::ChatHistoryHook131(object,1,text,0);
#else
 wht::ChatHistoryHook131(object,nullptr,1,text,0);
#endif
 Check(historyCalls==1,"unmuted history forwarded through original ABI");wht::mutedChat131.Reset();wht::repeatChat131.Reset();wht::originalChatHistory131=nullptr;
 puts("mute chat contracts passed (owned hooks only; no live game chat hidden)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
'''
(root/'tests/mute-chat-tests.cpp').write_text(src,encoding='utf-8')
p=root/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''
if(BUILD_TESTING)
 add_executable(WHT131MuteChatTests tests/mute-chat-tests.cpp)
 target_link_libraries(WHT131MuteChatTests PRIVATE wht_common minhook user32)
 target_compile_options(WHT131MuteChatTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME mute-chat-contracts COMMAND WHT131MuteChatTests)
endif()
''';p.write_text(s,encoding='utf-8')
