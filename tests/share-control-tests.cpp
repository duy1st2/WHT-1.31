#include "../native/share_control131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
int main(){try{
 for(const auto& p:wht::shareControlProfiles131){
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
 Check(wht::ResourceTextGuard131(base,wht::ResourceTextProfile131{p.build,0,0,0,0,0,p.code,p.count}),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::ResourceTextGuard131(base,wht::ResourceTextProfile131{p.build,0,0,0,0,0,p.code,p.count}),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }

 wht::ShareControlSnapshot131 target;target.source.world=123;target.source.player=456;target.source.local=target.source.physical=0;target.alliance=1000;target.peers[1]={789,1};target.peers[3]={900,3};target.mask=1u<<8;
 unsigned reads=0,writes=0,changedAt=0,refuseAt=0;bool noOp=false,otherBit=false;std::vector<unsigned> order;std::string report;
 auto read=[&](wht::ShareControlSnapshot131& out){out=target;if(++reads==changedAt)++out.peers[1].player;return true;};auto set=[&](const auto& before,unsigned slot,bool enabled){Check(wht::SameShareRoster131(before,target)&&before.mask==target.mask,"same native roster/mask");order.push_back(slot);if(++writes==refuseAt)return false;if(!noOp){const uint32_t bit=1u<<target.peers[slot].physical;target.mask=enabled?target.mask|bit:target.mask&~bit;if(otherBit)target.mask^=1u<<10;}return true;};auto run=[&](const std::wstring& args){reads=writes=0;order.clear();return wht::ShareControlFlow131(args,read,set,report);};
 Check(run(L"")&&order==std::vector<unsigned>({1,3})&&target.mask==((1u<<8)|(1u<<1)|(1u<<3)),"default ON all active others preserves unrelated bits");Check(run(L"1")&&!writes,"already shared no callbacks");Check(run(L"0")&&writes==2&&target.mask==(1u<<8),"OFF only active peer control bits");for(const auto* bad:{L"2",L"on",L"1 extra",L"1\n"})Check(!run(bad)&&!reads&&!writes,"invalid args no game access");Check(!run(std::wstring(L"1\0x",3))&&!reads,"NUL refused");
 changedAt=2;Check(!run(L"1")&&!writes,"roster changed before mutation");changedAt=4;Check(!run(L"1")&&writes==1,"roster changed during callback stops later players");changedAt=0;target.mask=0;noOp=true;Check(!run(L"1")&&writes==1,"unconfirmed native step stops");noOp=false;target.mask=0;refuseAt=2;Check(!run(L"1")&&writes==2&&report.find("1 players confirmed")!=report.npos,"partial change counted no rollback");refuseAt=0;target.mask=0;otherBit=true;Check(!run(L"1")&&writes==1,"unexpected unrelated mask update stops");otherBit=false;target.source.player=0;Check(!run(L"1")&&!writes,"missing local player");
 Check(!wht::InvokeShareControl131(nullptr,100,1)&&!wht::InvokeShareControl131(nullptr,0,24),"invalid native args refused");puts("shared control contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
