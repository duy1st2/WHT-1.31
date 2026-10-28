#include "../native/map_trade131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
int main(){try{
 for(const auto& p:wht::mapTradeProfiles131){
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
 Check(wht::MapTradeCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::MapTradeCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }




 for(const auto& p:wht::mapTradeProfiles131){
  alignas(uintptr_t) unsigned char world[512]{},setup[256]{};const uintptr_t base=0x100000,wp=reinterpret_cast<uintptr_t>(world),sp=reinterpret_cast<uintptr_t>(setup);uintptr_t table=base+p.setupTable;memcpy(setup,&table,sizeof(table));memcpy(world+p.setupOffset,&sp,sizeof(sp));uint32_t flags=0xa5a5f7ff;memcpy(setup+p.flagsOffset,&flags,4);
  uintptr_t readSetup=0;uint32_t actual=0;Check(wht::ReadMapTradeFields131(base,p,wp,readSetup,actual)&&readSetup==sp&&actual==flags,"exact CMapSetup identity and native map flag offset");
  Check(wht::ClearMapTradeField131(base,p,sp,flags),"native flag bit cleared");memcpy(&actual,setup+p.flagsOffset,4);Check(actual==(flags&~0x100u)&&((actual^flags)==0x100u),"all other32-bit map flags preserved");
  Check(!wht::ClearMapTradeField131(base,p,sp,flags),"changed source compare/exchange refused");memcpy(&actual,setup+p.flagsOffset,4);Check(actual==(flags&~0x100u),"conflict cannot overwrite flags");
  ++table;memcpy(setup,&table,sizeof(table));Check(!wht::ReadMapTradeFields131(base,p,wp,readSetup,actual)&&!wht::ClearMapTradeField131(base,p,sp,actual),"wrong setup class refused for reads and writes");
 }
 wht::MapTradeSource131 value;value.world=1;value.setup=2;value.local=0;value.flags=0x40100;unsigned reads=0,writes=0,change=0;bool unavailable=false,clearOK=true;std::string report;
 auto read=[&](wht::MapTradeSource131& out){out=value;++reads;if(unavailable)return false;if(reads==2)switch(change){case 1:++out.world;break;case 2:++out.setup;break;case 3:++out.local;break;case 4:out.flags^=2;break;}if(reads==3){if(change==5)++out.world;if(change==6)out.flags|=0x100;if(change==7)out.flags^=2;}return true;};
 auto clear=[&](const wht::MapTradeSource131& before){++writes;Check(before.flags==value.flags,"exact source passed to writer");if(!clearOK)return false;value.flags&=~0x100u;return true;};
 auto run=[&](const std::wstring& args){reads=writes=0;return wht::MapTradeCommand131(args,read,clear,report);};
 Check(run(L" ")&&writes==1&&reads==3&&value.flags==0x40000&&report=="Trade restriction disabled","confirmed map restriction clear");Check(run(L"")&&!writes&&reads==2&&report.find("already disabled")!=report.npos,"already clear is read-only");
 for(const auto* args:{L"1",L"off",L"\n"})Check(!run(args)&&!reads&&!writes,"no extra args or newline allowed");Check(!run(std::wstring(L"\0",1))&&!reads,"NUL rejected");
 for(change=1;change<=4;++change){value.flags=0x40100;Check(!run(L"")&&!writes,"unstable world setup player or flags no action");}change=0;unavailable=true;Check(!run(L"")&&!writes&&reads==1,"unavailable native context no writes");unavailable=false;
 clearOK=false;Check(!run(L"")&&writes==1&&value.flags==0x40100,"compare/exchange refusal not reported as success");clearOK=true;
 for(change=5;change<=7;++change){value.flags=0x40100;Check(!run(L"")&&writes==1&&report.find("not confirmed")!=report.npos,"world change or unexpected post-write bits reported honestly");}
 puts("map trade native flag contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
