#include "../native/quit_match131.hpp"
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstdio>
void Check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
unsigned calls=0;uint32_t score=1;
void __cdecl End(uint32_t value){++calls;score=value;}
int main(){try{
 wht::QuitMatchState131 state;
 Check(!state.Request(0,2,true)&&!state.Request(1,0,true)&&!state.Request(1,2,false),"invalid context/native refused");
 Check(state.Request(1,2,true)&&!state.Request(1,2,true),"one pending request only");
 Check(state.Process(1,2,true,[&](){Check(!state.pending&&!state.root&&!state.world,"consumed before native reentry");return wht::InvokeQuitMatch131(End);})&&calls==1&&score==0,"native ABI false score flag");
 Check(!state.Process(1,2,true,[](){Check(false,"no repeated native");return true;}),"consumed once");
 for(int reason=0;reason<4;++reason){Check(state.Request(1,2,true),"request");if(reason==3)state.Reset();Check(!state.Process(reason==0?3:1,reason==1?4:2,reason!=2,[](){Check(false,"canceled request invokes no native");return true;})&&!state.pending,"changed UI/world/authorization/teardown canceled");}
 Check(state.Request(1,2,true)&&!state.Process(1,2,true,[](){return false;})&&!state.pending,"failed invocation not retried");Check(!wht::InvokeQuitMatch131(nullptr),"null invocation refused");
 Check(!wht::QuitMatchNative131(0,12164),"null base refused");
 for(const auto& p:wht::quitMatchProfiles131){
#ifdef _WIN64
 const char* path=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* path=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream in(path,std::ios::binary);Check(bool(in),"capture exists");std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());Check(wht::QuitMatchNative131(base,p.build)==reinterpret_cast<wht::EndMatch131>(base+p.entry),"verified entry from independent capture");
 for(unsigned i=0;i<sizeof(p.bytes);++i){image[p.entry+i]^=1;Check(!wht::QuitMatchNative131(base,p.build),"any changed guarded byte refuses");image[p.entry+i]^=1;}
 Check(!wht::QuitMatchNative131(base,99999),"unknown build refused");
 }
 puts("Quit match lifecycle, ABI and profile contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
