#include "../native/add_ability131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
int main(){try{
 for(const auto& p:wht::addAbilityProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=wht::InventoryProfile(p.build)->unitTable+0x400;for(size_t i=0;i<p.count;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.count;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 Check(wht::AddAbilityCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::AddAbilityCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }

 wht::AddAbilitySource131 target;target.unit.unit=target.ability.unit=123;target.unit.world=target.ability.world=456;target.unit.agent=789;target.unit.id=target.ability.id={1,2};target.unit.local=target.ability.local=0;
 unsigned reads=0,creates=0,steps=0,refreshes=0,changedAt=0;bool createFail=false,noOp=false,refreshFail=false;int max=3;std::string report;
 auto read=[&](uint32_t code,wht::AddAbilitySource131& out){Check(code==0x41486862,"original rawcode");out=target;if(++reads==changedAt)++out.unit.agent;return true;};
 auto create=[&](uint32_t,const auto&){++creates;if(createFail)return false;target.ability.ability=1000;target.ability.abilityTable=2000;target.ability.abilityId={3,4};target.level=1;return true;};auto maximum=[&](const auto&,int& value){value=max;return true;};auto step=[&](const auto&,bool up){++steps;if(!noOp)target.level+=up?1:-1;return true;};auto refresh=[&](const auto&){++refreshes;return !refreshFail;};auto run=[&](const std::wstring& args){reads=creates=steps=refreshes=0;return wht::AddAbilityFlow131(args,read,create,maximum,step,refresh,report);};
 Check(run(L"AHhb")&&creates==1&&!steps&&target.level==1&&report.find("Added Ability")!=report.npos,"create absent ability default level one");Check(run(L"AHhb 10000")&&!creates&&steps==2&&target.level==3&&report.find("Level: 3")!=report.npos,"native max cap and actual level");Check(run(L"AHhb 2")&&steps==1&&target.level==2,"native decrease");Check(run(L"AHhb")&&!steps&&target.level==2,"reference default1 preserves existing level");Check(run(L"0x41486862 2")&&!steps&&refreshes==1,"hex idempotent level");
 for(const auto* bad:{L"",L"AHhb 0",L"AHhb 10001",L"AHhb 2 extra",L"AHhb 1.5",L"AHhb\n"})Check(!run(bad)&&!reads&&!creates&&!steps,"bad syntax no game access");Check(!run(std::wstring(L"AHhb\0x",6))&&!reads,"NUL rejected");changedAt=2;Check(!run(L"AHhb 3")&&!steps,"registry changes before mutation");changedAt=0;noOp=true;Check(!run(L"AHhb 3")&&steps==1,"native no-op stops loop");noOp=false;max=0;Check(!run(L"AHhb 3")&&!steps,"invalid native limit refused");max=3;refreshFail=true;Check(!run(L"AHhb 3")&&report.find("refresh")!=report.npos,"post callback failure explicit");refreshFail=false;
 target.ability.ability=target.ability.abilityTable=0;target.ability.abilityId={};target.level=0;createFail=true;Check(!run(L"AHhb 3")&&creates==1&&!steps,"unknown or restricted rawcode no level mutation");
 puts("add ability contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
