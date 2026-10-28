#include "../native/unit_model131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
int main(){try{
 for(const auto& p:wht::unitModelProfiles131){
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
 Check(wht::UnitModelCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::UnitModelCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }


 for(const auto& p:wht::unitModelProfiles131){
  alignas(uintptr_t) unsigned char unit[128]{},model[256]{},name[128]{};char path[]="units\\human\\Footman\\Footman.mdx";const uintptr_t base=0x100000;
  auto put=[](unsigned char* dst,uintptr_t value){memcpy(dst,&value,sizeof(value));};
  put(unit+p.modelOffset,reinterpret_cast<uintptr_t>(model));put(model,base+p.modelTable);put(model+p.nameOffset,reinterpret_cast<uintptr_t>(name));put(name,base+p.stringTable);put(name+p.stringOffset,reinterpret_cast<uintptr_t>(path));
  uintptr_t m=0,n=0,t=0;char output[512]{};
  Check(wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output)&&!strcmp(output,path),"exact model and STRINGREF chain");
  put(name,base+p.stringTable+8);Check(!wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output),"adjacent/unrelated string class refused");put(name,base+p.stringTable);
  put(model,base+p.modelTable+8);Check(!wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output),"unverified sprite class refused");put(model,base+p.modelTable);
  char longPath[512];memset(longPath,'a',sizeof(longPath));put(name+p.stringOffset,reinterpret_cast<uintptr_t>(longPath));Check(!wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output),"unterminated filename refused");
 }
 Check(wht::UnitModelPath131("units\\hero.MDX")&&wht::UnitModelPath131("x.mdl"),"native asset suffixes");for(const auto* bad:{"", "units\\hfoo", "x.blp", "x|r.mdx", "x\n.mdx"})Check(!wht::UnitModelPath131(bad),"invalid/not-model notification path refused");
 wht::UnitModelSource131 value;value.owner.unit=1;value.owner.world=2;value.owner.agent=3;value.owner.local=0;value.owner.id={4,5};value.model=6;value.name=7;value.text=8;value.path="units\\Footman.mdx";unsigned reads=0;bool change=false;std::string report;
 auto read=[&](wht::UnitModelSource131& out){out=value;if(++reads==2&&change)out.path="units\\Other.mdx";return true;};
 Check(wht::UnitModelCommand131(L"",read,report)&&report=="Get Model: units\\Footman.mdx","confirmed native path only");reads=0;change=true;Check(!wht::UnitModelCommand131(L"",read,report),"changed selection/model filename refused");reads=0;Check(!wht::UnitModelCommand131(L" extra",read,report)&&!reads,"invalid args no game access");
 puts("unit model contracts passed (read-only, no fabricated rawcode filename)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
