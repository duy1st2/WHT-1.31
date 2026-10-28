#include "../native/random_seed131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private captured fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
unsigned calls=0;uint32_t argument=0;
void __cdecl Native(uint32_t seed){++calls;argument=seed;}
int main(){try{
 for(const auto& p:wht::randomSeedProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=0;for(size_t i=0;i<p.count;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.count;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 Check(wht::RandomSeedCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::RandomSeedCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}
 // Only execute self-contained arithmetic on private stack state, never the game wrapper.
 DWORD old=0;Check(bool(VirtualProtect(image.data,size+64,PAGE_EXECUTE_READ,&old)),"private fixture RX");FlushInstructionCache(GetCurrentProcess(),image.data,size+64);
 using Init=void(__thiscall*)(wht::RandomSeedState131*,uint32_t);using Step=uint32_t(__cdecl*)(wht::RandomSeedState131*);
 for(uint32_t seed:{0u,1u,47u,53u,59u,61u,12345u,0x7fffffffu,0x80000000u,0xffffffffu}){
 struct StateCanary {uint32_t left;wht::RandomSeedState131 value;uint32_t right;} local{0x12345678,{},0x87654321};
 reinterpret_cast<Init>(base+p.init)(&local.value,seed);Check(local.value.seed==seed,"native initializer keeps full unsigned seed");
 const uint32_t generated=reinterpret_cast<Step>(base+p.step)(&local.value);const auto expected=wht::ExpectedRandomSeed131(seed,p.code[3].bytes);
 Check(wht::SameRandomState131(local.value,expected)&&generated==expected.seed,"independent prediction agrees with captured native init and first step");Check(local.left==0x12345678&&local.right==0x87654321,"native arithmetic only writes two state words");}
 }
 uint32_t seed=1;Check(wht::ParseRandomSeed131(L"",seed)&&seed==0,"default zero");Check(wht::ParseRandomSeed131(L"4294967295",seed)&&seed==0xffffffffu,"full uint32 input");Check(wht::ParseRandomSeed131(L" 12345 ",seed)&&seed==12345,"decimal with outer spaces");
 for(const auto* bad:{L"-1",L"+1",L"0x10",L"1.2",L"4294967296",L"00000000000",L"1 2",L"1\n",L"nan"})Check(!wht::ParseRandomSeed131(bad,seed),"malformed seed rejected");Check(!wht::ParseRandomSeed131(std::wstring(L"1\0x",3),seed),"embedded NUL rejected");
 Check(!wht::RandomSeedProfileFor131(129),"unknown build refused");Check(wht::InvokeRandomSeed131(Native,0xffffffffu)&&calls==1&&argument==0xffffffffu,"cdecl native ABI preserves full uint32");Check(!wht::InvokeRandomSeed131(nullptr,0)&&calls==1,"missing native not invoked");
 const auto& p=wht::randomSeedProfiles131[0];wht::RandomSeedSource131 target;target.world=123;target.rng=456;target.local=0;target.value={7,8};unsigned reads=0,writes=0;bool unstable=false,replaced=false,noOp=false,refused=false;std::string report;
 auto read=[&](wht::RandomSeedSource131& out){out=target;++reads;if(unstable&&reads==2)++out.value.state;if(replaced&&reads==3)++out.rng;return true;};
 auto apply=[&](uint32_t value,const wht::RandomSeedSource131& before){Check(before.rng==456,"stable native RNG");++writes;if(refused)return false;if(!noOp)target.value=wht::ExpectedRandomSeed131(value,p.code[3].bytes);return true;};
 auto run=[&](const std::wstring& args){reads=0;return wht::RandomSeedCommand131(args,p,read,apply,report);};
 Check(run(L"4294967295")&&writes==1&&reads==3&&report=="Random Seed: 4294967295","native update and exact advanced state confirmation");Check(run(L"")&&report=="Random Seed: 0","command default zero");auto old=writes;unstable=true;Check(!run(L"1")&&writes==old,"changing RNG before update refused");unstable=false;replaced=true;Check(!run(L"2"),"RNG replaced after update not success");replaced=false;noOp=true;Check(!run(L"12345"),"native no-op refused");noOp=false;refused=true;Check(!run(L"3"),"native failure not success");refused=false;
 old=writes;Check(!run(L"4294967296")&&writes==old&&reads==0,"invalid input no reads or writes");target.world=0;Check(!run(L"4")&&writes==old,"absent match no writes");
 puts("random seed contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
