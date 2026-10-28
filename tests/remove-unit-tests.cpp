#include "../native/remove_unit131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
unsigned prepared=0,removed=0;
#ifdef _WIN64
void Prepare(uintptr_t unit){Check(unit==123,"prepare this");++prepared;}
void Remove(uintptr_t unit,unsigned first,unsigned second){
#else
void __fastcall Prepare(uintptr_t unit,void*){Check(unit==123,"prepare this");++prepared;}
void __fastcall Remove(uintptr_t unit,void*,unsigned first,unsigned second){
#endif
 Check(unit==123&&first==1&&second==1,"native RemoveUnit this and flags ABI");++removed;
}
int main(){try{
 for(const auto& p:wht::removeUnitProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=wht::InventoryProfile(p.build)->unitTable+0x200;for(size_t i=0;i<p.count;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.count;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 *reinterpret_cast<uintptr_t*>(base+wht::InventoryProfile(p.build)->unitTable+(sizeof(uintptr_t)==8?0xf8:0x7c))=base+p.prepare;
 Check(wht::RemoveUnitCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::RemoveUnitCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}

 }
 Check(!wht::RemoveUnitProfileFor131(129),"unknown build refused");Check(wht::InvokePrepareRemoveUnit131(reinterpret_cast<wht::PrepareRemoveUnit131>(Prepare),123)&&prepared==1,"native preparation ABI");Check(wht::InvokeRemoveUnit131(reinterpret_cast<wht::RemoveUnitNative131>(Remove),123)&&removed==1,"native final removal ABI");Check(!wht::InvokeRemoveUnit131(nullptr,123)&&!wht::InvokePrepareRemoveUnit131(reinterpret_cast<wht::PrepareRemoveUnit131>(Prepare),0)&&prepared==1&&removed==1,"invalid native calls refused");
 wht::RemoveUnitSource131 target;target.unit=123;target.world=456;target.agent=789;target.id={1,2};target.local=0;unsigned reads=0,valids=0,prepares=0,removes=0;bool unstable=false,callbackRemoved=false,prepareFails=false,removeFails=false,finished=false;std::string report;
 auto read=[&](wht::RemoveUnitSource131& out){Check(!finished,"no unit reads after final native removal");out=target;if(unstable&&++reads==2)++out.id.low;else if(!unstable)++reads;return true;};
 auto valid=[&](const wht::RemoveUnitSource131& value){Check(!finished&&value.agent==789,"validate original identity only before removal");++valids;return !(callbackRemoved&&prepares);};
 auto prepare=[&](const wht::RemoveUnitSource131&){++prepares;return !prepareFails;};auto remove=[&](const wht::RemoveUnitSource131&){++removes;finished=true;return !removeFails;};
 auto run=[&](const std::wstring& args){reads=valids=prepares=removes=0;finished=false;return wht::RemoveUnitCommand131(args,read,valid,prepare,remove,report);};
 Check(run(L"")&&reads==2&&valids==2&&prepares==1&&removes==1&&report=="RemoveUnit invoked for selected unit","prepare revalidate remove and no post-removal read");unstable=true;Check(!run(L"")&&prepares==0&&removes==0,"changed selected identity no mutation");unstable=false;callbackRemoved=true;Check(!run(L"")&&prepares==1&&removes==0,"map callback removal blocks second native call");callbackRemoved=false;prepareFails=true;Check(!run(L"")&&removes==0,"preparation failure stops dispatch");prepareFails=false;removeFails=true;Check(!run(L"")&&removes==1,"native failure not success");removeFails=false;Check(!run(L"extra")&&reads==0&&prepares==0,"extra args no access");Check(!run(std::wstring(L"\0",1))&&reads==0,"NUL args refused");target.agent=0;Check(!run(L"")&&prepares==0,"unregistered unit refused");
 puts("remove unit contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
