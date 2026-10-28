#include "../native/repeat_chat131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* what){if(!ok)throw std::runtime_error(what);}
unsigned calls=0;
#ifdef _WIN64
void Original(uintptr_t o,unsigned s,const char* t,int c){
#else
void __fastcall Original(uintptr_t o,void*,unsigned s,const char* t,int c){
#endif
 Check(o==123&&s==2&&std::string(t)=="hello"&&c==1,"native ABI preserved");++calls;
}
int main(){try{
 for(const auto& p:wht::repeatChatProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);std::array<unsigned char,48> code{};input.seekg(p.address);input.read(reinterpret_cast<char*>(code.data()),48);Check(input.gcount()==48&&code==p.code,"captured chat entry matches profile");
 std::vector<unsigned char> image(p.address+64);memcpy(image.data()+p.address,code.data(),48);auto base=reinterpret_cast<uintptr_t>(image.data());
#ifndef _WIN64
 *reinterpret_cast<uintptr_t*>(image.data()+p.address+10)=base+p.cookie;
#endif
 Check(wht::RepeatChatCode131(base,p),"relocated native guard accepted");image[p.address+18]^=1;Check(!wht::RepeatChatCode131(base,p),"mutated native guard rejected");
 }
 wht::RepeatChat131 state;std::string report;
 Check(!state.Command(L"all",true,report),"no session refused");state.Publish(true,0);
 Check(!state.Command(L"1",true,report)&&!state.Command(L"25",true,report)&&!state.Command(L"2 extra",true,report),"self range and extra args rejected");
 Check(!state.Command(L"all",false,report)&&state.Command(L"3",true,report),"requires hook then chooses slot");
 state.Observe(0,"self",0);state.Observe(1,"other",0);state.Observe(2,"#command",0);state.Observe(2,"line\nbreak",0);state.Observe(2,"text",3);state.Observe(~0u,"system",0);state.Observe(2,std::string(129,'a').c_str(),0);Check(state.pending.empty(),"filter sender channel command length controls");
 state.Observe(2,"hello",1);unsigned sent=0;auto send=[&](const std::string& s){Check(s=="hello","text preserved");++sent;return true;};
 Check(!state.Tick(false,0,send)&&!state.Tick(true,0,[](const std::string&){return false;})&&!state.Tick(true,249,send)&&state.Tick(true,250,send)&&sent==1,"idle refusal backoff and cadence");
 Check(state.Command(L"all",true,report),"all mode");for(unsigned i=0;i<70;++i)state.Observe(1,"hello",0);Check(state.pending.size()==64,"bounded queue");
 state.Publish(false,24);Check(state.pending.empty()&&!state.all&&state.slot==-1,"leave clears all work");
 wht::originalChatHistory131=reinterpret_cast<wht::ChatHistoryNative131>(Original);
#ifdef _WIN64
 wht::ChatHistoryHook131(123,2,"hello",1);
#else
 wht::ChatHistoryHook131(123,nullptr,2,"hello",1);
#endif
 Check(calls==1,"observer calls original exactly once");puts("repeat chat contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
