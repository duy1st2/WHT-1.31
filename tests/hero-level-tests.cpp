#include "../native/hero_level131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
int main(){try{
 for(const auto& p:wht::heroLevelProfiles131){
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
 Check(wht::HeroLevelCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::HeroLevelCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}
 // Execute only the self-contained integer-to-real leaf in private fixture memory.
 DWORD old=0;Check(VirtualProtect(image.data+p.real,128,PAGE_EXECUTE_READ,&old)!=0,"private native real leaf RX");FlushInstructionCache(GetCurrentProcess(),image.data+p.real,128);
 auto convert=reinterpret_cast<wht::HeroLevelReal131>(base+p.real);
 for(int n:{0,1,200,1000000,16777217,INT_MAX}){uint32_t result=0;Check(convert(&result,n)==&result,"native real ABI");unsigned width=0,v=unsigned(n);while(v){++width;v>>=1;}uint32_t expected=0;if(n){unsigned shift=width>24?width-24:0;float exact=float((unsigned(n)>>shift)<<shift);memcpy(&expected,&exact,4);}Check(result==expected,"native real truncation for large XP delta");}
 }

 int delta=0;Check(wht::HeroLevelDelta131(1000,200,delta)&&delta==800,"XP difference");Check(!wht::HeroLevelDelta131(INT_MAX,-1,delta)&&!wht::HeroLevelDelta131(1,1,delta)&&!wht::HeroLevelDelta131(1,2,delta),"invalid XP no mutation");
 wht::HeroLevelSource131 target;target.identity.source.unit=123;target.identity.source.world=456;target.identity.source.agent=789;target.identity.source.id={1,2};target.identity.source.local=0;target.identity.hero=1000;target.level=1;
 unsigned reads=0,writes=0,changedAt=0;bool noOp=false,refused=false;std::string report;
 auto read=[&](wht::HeroLevelSource131& out){out=target;if(++reads==changedAt)++out.identity.hero;return true;};
 auto set=[&](const wht::HeroLevelSource131& source,int value){Check(wht::SameHeroLevel131(source,target),"same original hero");++writes;if(refused)return false;if(!noOp)target.level=value;return true;};
 auto run=[&](const std::wstring& args){reads=writes=0;return wht::HeroLevelCommand131(args,read,set,report);};
 Check(run(L"")&&target.level==10&&writes==1,"default increase to ten");Check(run(L"10")&&!writes,"same level no mutation");Check(run(L"1")&&target.level==1,"decrease");Check(run(L"10000")&&target.level==10000,"maximum");
 for(const auto* bad:{L"0",L"10001",L"1.5",L"nan",L"2 extra",L"2\n"})Check(!run(bad)&&!reads&&!writes,"invalid input no native access");Check(!run(std::wstring(L"2\0x",3))&&!reads,"NUL rejected");
 changedAt=2;Check(!run(L"10")&&!writes,"hero changes before mutation");changedAt=3;Check(!run(L"10")&&writes==1,"hero changes in callback");changedAt=0;target.level=1;noOp=true;Check(!run(L"10")&&report.find("game reports 1")!=report.npos,"map cap or ignored update reported");noOp=false;refused=true;Check(!run(L"10"),"native refusal");
 puts("hero level contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
