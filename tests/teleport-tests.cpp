#include "../native/teleport131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
unsigned stops=0,moves=0;
#ifdef _WIN64
void Stop(uintptr_t unit,unsigned flag){Check(unit==123&&flag==1,"native order stop ABI");++stops;}
void Move(uintptr_t unit,const float* x,const float* y,unsigned first,unsigned second,int third,uintptr_t fourth,uintptr_t fifth,unsigned sixth,int seventh,unsigned eighth){
#else
void __fastcall Stop(uintptr_t unit,void*,unsigned flag){Check(unit==123&&flag==1,"native order stop ABI");++stops;}
void __fastcall Move(uintptr_t unit,void*,const float* x,const float* y,unsigned first,unsigned second,int third,uintptr_t fourth,uintptr_t fifth,unsigned sixth,int seventh,unsigned eighth){
#endif
 Check(unit==123&&x&&y&&*x==12.5f&&*y==-34.25f&&first==1&&second==1&&third==-1&&!fourth&&!fifth&&!sixth&&seventh==-1&&!eighth,"native movement pointer ABI and all ten arguments");++moves;
}
int main(){try{
 for(const auto& p:wht::teleportProfiles131){
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
 *reinterpret_cast<uintptr_t*>(base+wht::InventoryProfile(p.build)->unitTable+(sizeof(uintptr_t)==8?0x300:0x180))=base+p.move;
 Check(wht::TeleportCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::TeleportCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}

 }

 Check(!wht::TeleportProfileFor131(129),"unknown profile refused");const auto stop=reinterpret_cast<wht::TeleportStop131>(Stop);const auto move=reinterpret_cast<wht::TeleportMove131>(Move);
 Check(wht::InvokeTeleportStop131(stop,123)&&stops==1,"stop thiscall");Check(wht::InvokeTeleportMove131(move,123,12.5f,-34.25f)&&moves==1,"movement thiscall all arguments");Check(!wht::InvokeTeleportMove131(move,123,NAN,1)&&!wht::InvokeTeleportMove131(move,123,100001,1)&&!wht::InvokeTeleportMove131(move,0,12.5f,-34.25f)&&!wht::InvokeTeleportStop131(nullptr,123)&&moves==1,"invalid native args never invoke");
 wht::RemoveUnitSource131 target;target.unit=123;target.world=456;target.agent=789;target.id={1,2};target.local=0;unsigned reads=0,stopCalls=0,moveCalls=0,positionCalls=0;unsigned changedAt=0;bool cursorMissing=false,stopFail=false,moveFail=false,positionFail=false;std::string report;
 auto cursor=[&](wht::MouseWorldPosition131& out){out={12.5f,-34.25f,0};return !cursorMissing;};
 auto read=[&](wht::RemoveUnitSource131& out){out=target;if(++reads==changedAt)++out.id.low;return true;};
 auto stopCall=[&](const wht::RemoveUnitSource131& value){Check(value.unit==123,"stop original unit");++stopCalls;return !stopFail;};
 auto moveCall=[&](const wht::RemoveUnitSource131& value,float x,float y){Check(value.unit==123&&x==12.5f&&y==-34.25f,"move exact cursor target after stop");++moveCalls;return !moveFail;};
 auto position=[&](const wht::RemoveUnitSource131& value,wht::MouseWorldPosition131& out){Check(value.unit==123,"read position only of revalidated identity");++positionCalls;out={10,-30,0};return !positionFail;};
 auto run=[&](const std::wstring& args){reads=stopCalls=moveCalls=positionCalls=0;return wht::TeleportCommand131(args,cursor,read,stopCall,moveCall,position,report);};
 Check(run(L"")&&reads==4&&stopCalls==1&&moveCalls==1&&positionCalls==1&&report=="Unit Position: X: 10.00, Y: -30.00","stop then move and report native adjusted coordinates");changedAt=2;Check(!run(L"")&&!stopCalls&&!moveCalls,"changed unit before stop refused");changedAt=3;Check(!run(L"")&&stopCalls==1&&!moveCalls,"callback change during stop blocks movement");changedAt=4;Check(!run(L"")&&moveCalls==1&&!positionCalls,"change after move blocks stale position read");changedAt=0;cursorMissing=true;Check(!run(L"")&&!reads&&!stopCalls,"missing cursor projection no unit access");cursorMissing=false;stopFail=true;Check(!run(L"")&&!moveCalls,"stop failure no movement");stopFail=false;moveFail=true;Check(!run(L"")&&!positionCalls,"native movement failure no readback");moveFail=false;positionFail=true;Check(!run(L""),"failed position read not success");positionFail=false;Check(!run(L"extra")&&!reads&&!stopCalls,"extra args no mutation");Check(!run(std::wstring(L"\0",1))&&!reads,"embedded NUL refused");target.agent=0;Check(!run(L"")&&!stopCalls,"unregistered selection refused");
 puts("teleport contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
