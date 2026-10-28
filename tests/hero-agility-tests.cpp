#include "../native/hero_agility131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
unsigned calls=0;
#ifdef _WIN64
void Setter(uintptr_t unit,int value,unsigned permanent){
#else
void __fastcall Setter(uintptr_t unit,void*,int value,unsigned permanent){
#endif
 Check(unit==123&&value==500&&permanent==1,"native Agility this/value/permanent ABI");++calls;
}
int main(){try{
 for(const auto& p:wht::heroAgilityProfiles131){
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
 Check(wht::HeroAgilityCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::HeroAgilityCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }
 for(const auto& p:wht::heroAgilityProfiles131){
 const auto& code=p.code[0];
#ifdef _WIN64
 const unsigned char field[]={0x88,0x01,0x00,0x00};const unsigned char baseField[]={0x10,0x01,0x00,0x00};
#else
 const unsigned char field[]={0xe8,0x00,0x00,0x00};const unsigned char baseField[]={0xac,0x00,0x00,0x00};
#endif
 Check(std::search(code.bytes,code.bytes+code.length,std::begin(field),std::end(field))!=code.bytes+code.length,"getter accesses independently identified Agility component");Check(std::search(code.bytes,code.bytes+code.length,std::begin(baseField),std::end(baseField))!=code.bytes+code.length,"getter reads Agility base field rather than Strength field");
 }
 int value=0;Check(wht::ParseHeroAgility131(L"",value)&&value==500,"default500");Check(wht::ParseHeroAgility131(L"1",value)&&value==1,"minimum");Check(wht::ParseHeroAgility131(L"10000",value)&&value==10000,"maximum");
 for(const auto* bad:{L"0",L"-1",L"10001",L"1.5",L"nan",L"1 extra",L"1\n"})Check(!wht::ParseHeroAgility131(bad,value),"invalid Agility input refused");Check(!wht::ParseHeroAgility131(std::wstring(L"1\0x",3),value),"NUL refused");Check(!wht::HeroAgilityProfileFor131(129),"unsupported build refused");
 const auto native=reinterpret_cast<wht::HeroAgilitySet131>(Setter);Check(wht::InvokeHeroAgility131(native,123,500)&&calls==1,"native setter ABI");Check(!wht::InvokeHeroAgility131(native,123,0)&&!wht::InvokeHeroAgility131(native,123,10001)&&!wht::InvokeHeroAgility131(native,0,500)&&!wht::InvokeHeroAgility131(nullptr,123,500)&&calls==1,"invalid native args no call");
 wht::HeroAgilitySource131 target;target.source.unit=123;target.source.world=456;target.source.agent=789;target.source.id={1,2};target.source.local=0;target.hero=1000;target.base=25;target.total=35;
 unsigned reads=0,writes=0;unsigned changedAt=0;bool noOp=false,refused=false;std::string report;
 auto read=[&](wht::HeroAgilitySource131& out){out=target;if(++reads==changedAt)++out.hero;return true;};
 auto set=[&](const wht::HeroAgilitySource131& before,int requested){Check(before.hero==1000&&before.source.unit==123,"stable original hero");++writes;if(refused)return false;if(!noOp){target.base=requested;target.total=requested+10;}return true;};
 auto run=[&](const std::wstring& args){reads=writes=0;return wht::HeroAgilityCommand131(args,read,set,report);};
 Check(run(L"")&&target.base==500&&target.total==510&&reads==3&&writes==1&&report=="Hero AGI: 500 (total: 510)","base setter with item bonus does not falsely fail");Check(run(L"500")&&!writes&&reads==2,"already requested base no mutation");Check(run(L"1")&&target.base==1,"minimum stat");changedAt=2;Check(!run(L"100")&&!writes,"changed hero before setter refused");changedAt=3;Check(!run(L"100")&&writes==1,"hero changed after setter no false success");changedAt=0;noOp=true;Check(!run(L"200")&&writes==1,"native no-op refused");noOp=false;refused=true;Check(!run(L"200"),"native failure propagated");refused=false;Check(!run(L"bad")&&!reads&&!writes,"invalid args no game access");target.hero=0;Check(!run(L"100")&&!writes,"nonhero selection refused");
 puts("hero Agility contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
