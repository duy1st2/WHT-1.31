#include "../native/fog_modifier131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};

uint32_t seenPlayer=0,seenState=0,seenRect=0,seenShared=0,seenAfter=0,seenAction=0;
uint32_t __cdecl MockZero(){return 77;}
uint32_t __cdecl MockCreate(uint32_t player,uint32_t state,uint32_t rect,uint32_t shared,uint32_t after){seenPlayer=player;seenState=state;seenRect=rect;seenShared=shared;seenAfter=after;return 88;}
void __cdecl MockAction(uint32_t handle){seenAction=handle;}
uintptr_t __cdecl MockResolve(uint32_t handle){return handle==88?uintptr_t(0x12345678):0;}
int main(){try{
 for(const auto& p:wht::fogNativeProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=0;for(size_t i=0;i<p.count;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));size=(std::max)(size,size_t((std::max)(p.fogTable,p.rectTable)+26*sizeof(uintptr_t)));const auto* rp=wht::ResourceProfileFor131(p.build);size=(std::max)(size,size_t(rp->playerTable+26*sizeof(uintptr_t)));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.count;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 for(const auto slot:std::vector<std::pair<uintptr_t,uintptr_t>>{{p.fogTable+24*sizeof(uintptr_t),p.configure},{p.fogTable+23*sizeof(uintptr_t),p.fogDelete},{p.rectTable+23*sizeof(uintptr_t),p.rectDelete},{rp->playerTable+25*sizeof(uintptr_t),p.ownerGetter}}){uintptr_t target=0;input.seekg(slot.first);input.read(reinterpret_cast<char*>(&target),sizeof(target));Check(target==capturedBase+slot.second,"actual captured native virtual slot");target=base+slot.second;memcpy(image.data+slot.first,&target,sizeof(target));}
 Check(wht::FogNativeCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::FogNativeCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }




 for(const auto& p:wht::fogNativeProfiles131){
  alignas(uintptr_t) unsigned char object[256]{};const auto ptr=reinterpret_cast<uintptr_t>(object);const uintptr_t base=0x100000;uintptr_t table=base+p.fogTable;memcpy(object,&table,sizeof(table));const uint32_t id[2]={11,22};memcpy(object+(sizeof(uintptr_t)==8?0x18:0xc),id,sizeof(id));uint32_t flags=0xcc,mask=4;memcpy(object+(sizeof(uintptr_t)==8?0x38:0x20),&flags,4);memcpy(object+(sizeof(uintptr_t)==8?0x3c:0x24),&mask,4);float coords[4]={-4096,-2048,4096,2048};memcpy(object+(sizeof(uintptr_t)==8?0x50:0x30),coords,sizeof(coords));
  wht::FogObjectFields131 fields;Check(wht::ReadFogObjectFields131(base,p,ptr,false,fields)&&fields.low==11&&fields.high==22&&fields.flags==flags&&fields.mask==4&&!memcmp(fields.coordinates,coords,sizeof(coords)),"native modifier identity/configuration offsets");
  ++table;memcpy(object,&table,sizeof(table));Check(!wht::ReadFogObjectFields131(base,p,ptr,false,fields),"foreign class refused");table=base+p.rectTable;memcpy(object,&table,sizeof(table));memcpy(object+(sizeof(uintptr_t)==8?0x40:0x24),coords,sizeof(coords));Check(wht::ReadFogObjectFields131(base,p,ptr,true,fields),"exact rectangle native coordinates");
  coords[0]=NAN;memcpy(object+(sizeof(uintptr_t)==8?0x40:0x24),coords,sizeof(coords));Check(!wht::ReadFogObjectFields131(base,p,ptr,true,fields),"invalid bounds refused");Check(wht::ReadFogObjectFields131(base,p,ptr,true,fields,false)&&fields.low==11&&fields.high==22,"invalid geometry still allows verified identity cleanup");
 }
 uint32_t result=0;Check(wht::InvokeFogZero131(MockZero,result)&&result==77,"cdecl zero-argument handle API");Check(wht::InvokeFogCreate131(MockCreate,11,22,result)&&result==88&&seenPlayer==11&&seenState==4&&seenRect==22&&seenShared==1&&seenAfter==0,"cdecl actual player/rect handles and visible shared rectangle arguments");Check(!wht::InvokeFogCreate131(MockCreate,0,22,result),"invalid input handle no call");Check(wht::InvokeFogAction131(MockAction,88)&&seenAction==88,"cdecl action handle ABI");Check(wht::InvokeFogResolve131(MockResolve,88)==uintptr_t(0x12345678),"cdecl typed native object result width");
 bool enabled=false;Check(wht::ParseFogToggle131(L"",false,enabled)&&enabled&&wht::ParseFogToggle131(L"",true,enabled)&&!enabled,"no-argument toggle");Check(wht::ParseFogToggle131(L" -1 ",false,enabled)&&enabled&&wht::ParseFogToggle131(L" 0 ",true,enabled)&&!enabled,"reference integer nonzero/zero semantics");for(const auto* bad:{L"on",L"1 extra",L"1\n",L"2147483648",L"1.0"})Check(!wht::ParseFogToggle131(bad,false,enabled),"invalid args reject instead of accidental toggle");Check(!wht::ParseFogToggle131(std::wstring(L"1\0",2),false,enabled),"embedded NUL refused");
 puts("fog modifier native profile and ABI contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
