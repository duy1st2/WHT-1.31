#include "../native/alliance_spam131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool b,const char* s){if(!b)throw std::runtime_error(s);}
unsigned submissions=0;
uintptr_t __cdecl Context(unsigned){return 1;}
void __cdecl Submit(wht::DropBuffer131* b,unsigned mode){Check(b&&b->table==7&&!b->position&&!b->reserved&&b->size==6&&b->capacity==6&&!mode,"native datastore ABI");const auto* bytes=static_cast<const unsigned char*>(b->data);Check(bytes[0]==0x50&&bytes[1]==1,"borrowed payload");++submissions;}
int main(){try{
 wht::DropNetworkApi131 native;native.context=Context;native.send=Submit;native.position=reinterpret_cast<decltype(native.position)>(Context);native.owner=reinterpret_cast<decltype(native.owner)>(Context);native.table=7;Check(wht::SubmitAllianceSpam131(native,1,0xff0000ffu)&&wht::SubmitAllianceSpam131(native,1,0)&&submissions==2,"sender ABI");Check(!wht::SubmitAllianceSpam131(native,16,0)&&!wht::SubmitAllianceSpam131(native,1,4)&&submissions==2,"invalid no submission");
 int slot=99;Check(wht::ParseAllianceSpam131(L"",slot)&&slot==-1,"stop");Check(wht::ParseAllianceSpam131(L"16",slot)&&slot==15,"last slot");for(auto s:{L"0",L"17",L"-1",L"1 extra",L"1\n",L"99999999999"})Check(!wht::ParseAllianceSpam131(s,slot),"bad args");Check(!wht::ParseAllianceSpam131(std::wstring(L"1\0",2),slot),"nul");
 const auto bytes=wht::AllianceSpamPacket131(15,0xff0000ff);Check(bytes==std::array<unsigned char,6>{{0x50,15,255,0,0,255}},"wire flags");Check(wht::AllianceSpamPacket131(15,0)[5]==0,"off wire");
 wht::AllianceSpamContext131 context;context.session=1;context.players.source.world=2;context.players.source.player=3;context.players.source.local=0;context.players.source.physical=0;context.players.target=context.players.source;context.players.target.player=4;context.players.target.physical=1;
 bool valid=true;auto read=[&](unsigned target,wht::AllianceSpamContext131& out){out=context;return valid&&target==1;};std::vector<uint32_t> sent;auto send=[&](unsigned target,uint32_t flags){Check(target==1,"target");sent.push_back(flags);return true;};std::string report;wht::AllianceSpam131 state;
 Check(state.Command(L"2",true,read,report),"arm");Check(sent.empty(),"command no mutation");Check(state.Tick(true,true,1,read,send,report)&&sent.size()==2&&sent[0]==0xff0000ffu&&sent[1]==0,"native pair");Check(state.Tick(true,true,1000,read,send,report)&&sent.size()==2,"cadence");Check(state.Tick(true,false,10000,read,send,report)&&sent.size()==2,"paused");Check(state.Tick(true,true,20000,read,send,report)&&sent.size()==4,"no catchup");context.session=5;Check(!state.Tick(true,true,22000,read,send,report)&&state.slot==-1&&sent.size()==4,"changed session stops");
 Check(state.Command(L"2",true,read,report),"rearm");unsigned attempts=0;auto failSecond=[&](unsigned,uint32_t){return ++attempts<2;};Check(!state.Tick(true,true,23000,read,failSecond,report)&&attempts==2&&state.slot==-1,"uncertain pair stops");state.Tick(true,true,25000,read,failSecond,report);Check(attempts==2,"no retry");Check(state.Command(L"2",true,read,report),"arm again");state.Tick(false,true,30000,read,send,report);Check(state.slot==-1,"authorization reset");Check(state.Command(L"",false,read,report),"stop regardless settings");valid=false;Check(!state.Command(L"2",true,read,report),"inactive refused");
 for(const auto& p:wht::allianceSpamProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream in(file,std::ios::binary);Check(bool(in),"capture");std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());Check(wht::AllianceSpamCode131(base,p.build),"native capture guard");image[p.code[0].rva]^=1;Check(!wht::AllianceSpamCode131(base,p.build),"tamper refusal");
 }
 puts("Alliance spam contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
