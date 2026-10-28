#include "../native/alliance_spam_all131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <limits>
void Check(bool b,const char* s){if(!b)throw std::runtime_error(s);}
unsigned submitted=0;
uintptr_t __cdecl Context(unsigned){return 1;}
void __cdecl Submit(wht::DropBuffer131* b,unsigned mode){Check(b&&b->table==7&&!b->position&&!b->reserved&&b->size==13&&b->capacity==13&&!mode,"native buffer ABI");const auto* bytes=static_cast<const unsigned char*>(b->data);float x=0,y=0,duration=0;memcpy(&x,bytes+1,4);memcpy(&y,bytes+5,4);memcpy(&duration,bytes+9,4);Check(bytes[0]==0x68&&x==1.5f&&y==-2.f&&duration==-1.f,"ping payload");++submitted;}
int main(){try{
 std::array<unsigned char,13> bytes;Check(wht::PingActionPacket131(1.5f,-2.f,bytes)&&bytes==std::array<unsigned char,13>{{0x68,0,0,0xc0,0x3f,0,0,0,0xc0,0,0,0x80,0xbf}},"wire LE floats");Check(!wht::PingActionPacket131(std::numeric_limits<float>::quiet_NaN(),0,bytes)&&!wht::PingActionPacket131(0,10001,bytes),"bad coords");wht::DropNetworkApi131 api;api.context=Context;api.send=Submit;api.position=reinterpret_cast<decltype(api.position)>(Context);api.owner=reinterpret_cast<decltype(api.owner)>(Context);api.table=7;Check(wht::SubmitPingAction131(api,1.5f,-2.f)&&submitted==1,"native ABI call");Check(!wht::SubmitPingAction131(api,10001,0)&&submitted==1,"invalid no call");
 wht::AllianceSpamAllContext131 c;c.session=1;c.roster.source.world=2;c.roster.source.player=3;c.roster.source.local=0;c.roster.source.physical=0;c.roster.alliance=4;c.roster.peers[1]={5,1};c.roster.peers[15]={6,15};Check(wht::ValidSpamAllRoster131(c),"roster");auto bad=c;bad.roster.peers[1].physical=0;Check(!wht::ValidSpamAllRoster131(bad),"self alias refused");bad=c;bad.roster.peers[1].physical=2;Check(!wht::ValidSpamAllRoster131(bad),"noncanonical slot");
 auto read=[&](wht::AllianceSpamAllContext131& out){out=c;return true;};std::vector<std::string> calls;auto send=[&](unsigned slot,uint32_t flags){calls.push_back(std::to_string(slot)+":"+std::to_string(flags));return true;};unsigned pings=0;auto ping=[&](float x,float y){Check(std::isfinite(x)&&std::isfinite(y)&&std::abs(x)<=10000&&std::abs(y)<=10000,"random bounds");calls.push_back("ping");++pings;return true;};std::string report;wht::AllianceSpamAll131 state;
 Check(!state.Command(L"1",true,read,report)&&!state.Command(L"\n",true,read,report)&&!state.Command(std::wstring(1,L'\0'),true,read,report),"strict args");Check(state.Command(L"",true,read,report)&&state.active&&calls.empty(),"toggle arm without sends");Check(state.Tick(true,true,1,read,send,ping,report)&&calls==std::vector<std::string>{"1:4278190335","1:0","ping","ping","15:4278190335","15:0"},"WHT action order");Check(state.Tick(true,true,1000,read,send,ping,report)&&calls.size()==6,"cadence");state.Tick(true,false,9000,read,send,ping,report);Check(calls.size()==6,"pause");state.Tick(true,true,10000,read,send,ping,report);Check(calls.size()==12&&pings==4,"no catchup");Check(state.Command(L"",false,read,report)&&!state.active,"off despite settings");
 Check(state.Command(L"",true,read,report),"rearm");c.session=9;Check(!state.Tick(true,true,20000,read,send,ping,report)&&!state.active&&calls.size()==12,"changed session no send");Check(state.Command(L"",true,read,report),"rearm partial");unsigned attempts=0;auto failSecond=[&](unsigned,uint32_t){return ++attempts<2;};Check(!state.Tick(true,true,30000,read,failSecond,ping,report)&&attempts==2&&!state.active&&pings==4,"partial pair no ping");state.Tick(true,true,40000,read,failSecond,ping,report);Check(attempts==2,"no retry");Check(state.Command(L"",true,read,report),"rearm ping fail");auto failPing=[&](float,float){return false;};Check(!state.Tick(true,true,50000,read,send,failPing,report)&&!state.active,"ping failure stops batch");
 c.roster.peers={};Check(state.Command(L"",true,read,report),"empty roster arm");auto count=calls.size();state.Tick(true,true,60000,read,send,ping,report);Check(calls.size()==count&&pings==4,"no peers no pings");state.Tick(false,true,70000,read,send,ping,report);Check(!state.active,"authorization reset");
 for(const auto& p:wht::pingActionProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream in(file,std::ios::binary);Check(bool(in),"capture");std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());Check(wht::PingActionCode131(base,p.build),"native guard");for(size_t i=0;i<p.count;++i){image[p.code[i].rva]^=1;Check(!wht::PingActionCode131(base,p.build),"tamper refused");image[p.code[i].rva]^=1;}
 }
 puts("Spam all contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
