#include "../native/resource_commands131.hpp"
#include "../native/quick_drop131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
uintptr_t expectedProperty=0;int expectedEncoded=0;unsigned propertyCalls=0,calls=0;
#ifdef _WIN64
void PropertyWrite(uintptr_t property,int value){
#else
void __fastcall PropertyWrite(uintptr_t property,void*,int value){
#endif
 Check(property==expectedProperty&&value==expectedEncoded,"native property address and encoded state");++propertyCalls;
}
#ifdef _WIN64
void Setter(uintptr_t player,unsigned state,int value){
#else
void __fastcall Setter(uintptr_t player,void*,unsigned state,int value){
#endif
 Check(player==123&&state==1&&value==200,"native player/state/value ABI");++calls;
}
int main(){try{
 for(const auto& p:wht::resourceCommandProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=wht::InventoryProfile(p.build)->unitTable+0x400;for(size_t i=0;i<p.count;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.count;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 Check(wht::ResourceCommandCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::ResourceCommandCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 unsigned char jump[16]{};size_t jumpSize=0;const uintptr_t callback=reinterpret_cast<uintptr_t>(PropertyWrite);
#ifdef _WIN64
 jump[0]=0x48;jump[1]=0xb8;memcpy(jump+2,&callback,8);jump[10]=0xff;jump[11]=0xe0;jumpSize=12;
#else
 jump[0]=0xb8;memcpy(jump+1,&callback,4);jump[5]=0xff;jump[6]=0xe0;jumpSize=7;
#endif
 memcpy(image.data+p.writer,jump,jumpSize);DWORD old=0;Check(bool(VirtualProtect(image.data,size+64,PAGE_EXECUTE_READ,&old)),"private arithmetic fixture RX");FlushInstructionCache(GetCurrentProcess(),image.data,size+64);
 alignas(uintptr_t) unsigned char player[1024]{};const auto raw=reinterpret_cast<uintptr_t>(player);const auto* rp=wht::ResourceProfileFor131(p.build);auto native=reinterpret_cast<wht::ResourceSetNative131>(base+p.set);
 for(unsigned state:{1u,2u,4u}){expectedProperty=raw+rp->stateBase+state*rp->stateStride;expectedEncoded=state<3?123450:12345;propertyCalls=0;Check(wht::InvokeResourceSet131(native,raw,state,12345)&&propertyCalls==1,"captured helper maps actual player property and tenfold currency encoding");}
 expectedProperty=raw+rp->stateBase+rp->stateStride;expectedEncoded=99999990;propertyCalls=0;Check(wht::InvokeResourceSet131(native,raw,1,9999999)&&propertyCalls==1,"native max safe currency encoding");
 }

 using Command=wht::ResourceCommand131;int amount=0;Check(wht::ParseResourceAmount131(Command::Gold,L"",amount)&&amount==100000,"gold default");Check(wht::ParseResourceAmount131(Command::Food,L"",amount)&&amount==200,"food default");Check(wht::ParseResourceAmount131(Command::Lumber,L"2147483647",amount)&&amount==INT_MAX,"integer maximum input");
 for(const auto* bad:{L"0",L"-1",L"1.5",L"2147483648",L"1 extra",L"1\n"})Check(!wht::ParseResourceAmount131(Command::Gold,bad,amount),"invalid resource args rejected");Check(!wht::ParseResourceAmount131(Command::Full,L"1",amount),"fullres no arguments");Check(!wht::ParseResourceAmount131(Command::Gold,std::wstring(L"1\0x",3),amount),"NUL refused");
 const auto native=reinterpret_cast<wht::ResourceSetNative131>(Setter);Check(wht::InvokeResourceSet131(native,123,1,200)&&calls==1,"native setter ABI");Check(!wht::InvokeResourceSet131(native,123,1,10000000)&&!wht::InvokeResourceSet131(native,123,3,200)&&!wht::InvokeResourceSet131(native,0,1,200)&&calls==1,"invalid native args no call");
 wht::ResourceCommandSource131 target;target.world=123;target.player=456;target.local=0;target.physical=0;target.values[0]=100;target.values[1]=200;target.values[2]=50;unsigned reads=0,writes=0;bool unstable=false,noOp=false,refused=false,changedAfter=false;std::string report;
 auto read=[&](wht::ResourceCommandSource131& out){out=target;++reads;if((unstable&&reads==2)||(changedAfter&&writes==1))++out.player;return true;};
 auto set=[&](const wht::ResourceCommandSource131& source,unsigned state,int value){Check(source.player==456,"local player only");++writes;if(refused)return false;if(!noOp)target.values[state==1?0:state==2?1:2]=value;return true;};
 auto run=[&](Command command,const std::wstring& args){reads=0;writes=0;return wht::ResourceCommandFlow131(command,args,read,set,report);};
 Check(run(Command::Gold,L"50")&&target.values[0]==150&&target.values[1]==200&&writes==1,"gold adds");Check(run(Command::Lumber,L"")&&target.values[1]==100200,"lumber default adds");Check(run(Command::Food,L"250")&&target.values[2]==250,"food sets rather than adds");Check(run(Command::Full,L"")&&target.values[0]==1000000&&target.values[1]==1000000&&target.values[2]==300&&writes==3,"full resources fixed values");Check(run(Command::Full,L"")&&!writes,"already full no writes");
 target.values[0]=9999999;Check(!run(Command::Gold,L"1")&&!writes,"native currency cap checked before writes");target.values[0]=100;unstable=true;Check(!run(Command::Gold,L"1")&&!writes,"changed player before write refused");unstable=false;noOp=true;Check(!run(Command::Gold,L"1")&&writes==1,"native no-op not success");noOp=false;refused=true;Check(!run(Command::Gold,L"1"),"native refusal propagated");refused=false;changedAfter=true;target.values[1]=100;Check(!run(Command::Full,L"")&&writes==1,"partial fullres stops when player changes");changedAfter=false;Check(!run(Command::Gold,L"bad")&&!reads&&!writes,"invalid input no game access");target.player=0;Check(!run(Command::Gold,L"1")&&!writes,"missing local player refused");
 puts("resource command contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
