#include "../native/mute_chat131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};

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

int main(){try{
 for(const auto& p:wht::muteChatProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=0;for(size_t i=0;i<p.count;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.count;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 Check(wht::MuteChatCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::MuteChatCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }


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
