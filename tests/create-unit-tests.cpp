#include "../native/create_unit131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};

unsigned nativeOwner=0;uint32_t nativeCode=0;float nativeX=0,nativeY=0,nativeFacing=0;
uintptr_t __cdecl MockCreateUnit(unsigned owner,uint32_t code,const float* x,const float* y,const float* facing){nativeOwner=owner;nativeCode=code;nativeX=*x;nativeY=*y;nativeFacing=*facing;return 0x123456;}

int main(){try{
 for(const auto& p:wht::createUnitProfiles131){
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
 const auto* rp=wht::ResourceProfileFor131(p.build);const auto slot=rp->playerTable+(sizeof(uintptr_t)==8?0xc8:0x64);uintptr_t nativeGetter=0;input.seekg(slot);input.read(reinterpret_cast<char*>(&nativeGetter),sizeof(nativeGetter));Check(nativeGetter==capturedBase+p.ownerGetter&&p.ownerGetter==rp->idGetter,"native player virtual owner getter matches captured table and resource identity");nativeGetter=base+p.ownerGetter;memcpy(image.data+slot,&nativeGetter,sizeof(nativeGetter));
 float facing=0;Check(wht::CreateFacing131(base,p,facing)&&fabsf(facing-4.712389f)<.000001f,"native degree-to-radian scalar verified");
 Check(wht::CreateUnitCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::CreateUnitCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }


 uint32_t code=0;unsigned count=0;Check(wht::ParseCreateUnit131(L"hfoo",code,count)&&code==0x68666f6f&&count==1,"default fresh unit count");Check(wht::ParseCreateUnit131(L"0x68666F6F 50",code,count)&&count==50,"hex rawcode maximum50");Check(wht::ParseUnitCount131(L"",count)&&count==1&&wht::ParseUnitCount131(L" 3 ",count)&&count==3,"copy count/default");for(const auto* bad:{L"0",L"51",L"-1",L"+1",L"one",L"1 2",L"1x",L"2\n"})Check(!wht::ParseUnitCount131(bad,count),"malformed count refused");Check(!wht::ParseCreateUnit131(std::wstring(L"hfoo\0 1",8),code,count)&&!wht::ParseCreateUnit131(L"hfoo 1 extra",code,count),"NUL/extra args refused");
 Check(wht::InvokeCreateUnit131(MockCreateUnit,2,0x68666f6f,100,200,4.712389f)==0x123456&&nativeOwner==2&&nativeCode==0x68666f6f&&nativeX==100&&nativeY==200&&nativeFacing==4.712389f,"cdecl worker ABI physical owner rawcode and three real pointers");Check(!wht::InvokeCreateUnit131(nullptr,0,1,0,0,0)&&!wht::InvokeCreateUnit131(MockCreateUnit,24,1,0,0,0),"invalid native/owner no call");
 wht::ResourceCommandSource131 context;context.world=1;context.player=2;context.local=0;context.physical=2;unsigned reads=0,creates=0,confirms=0,changedAt=0,failAt=0;bool valid=true,confirmOK=true,duplicate=false;std::string report;
 auto read=[&](wht::ResourceCommandSource131& out){out=context;if(++reads==changedAt)++out.world;return true;};auto sourceValid=[&](const auto&){return valid;};
 auto create=[&](unsigned owner,uint32_t type,float x,float y)->uintptr_t{Check(owner==2&&type==0x68666f6f&&x==100+float(creates)*40&&y==200,"physical local player and frozen spacing");++creates;return creates==failAt?0:creates;};
 auto confirm=[&](uintptr_t unit,uint32_t type,const auto& expected,wht::CreatedUnit131& out){++confirms;Check(unit==creates&&type==0x68666f6f&&expected.world==context.world,"confirm exact returned native unit");out.source.id={duplicate?1u:unsigned(unit),5};return confirmOK;};
 auto run=[&](unsigned n=3,float x=100){reads=creates=confirms=0;return wht::CreateUnitBatch131(0x68666f6f,n,x,200,40,read,sourceValid,create,confirm,report);};
 Check(run()&&creates==3&&confirms==3&&report=="Create Unit: 3/3 created","confirmed bounded batch");changedAt=3;Check(!run()&&!creates,"world changed before native no spawn");changedAt=4;Check(!run()&&creates==1&&report.find("latest invocation")!=report.npos,"world changed during spawn stops without retry");changedAt=0;failAt=2;Check(!run()&&creates==2&&confirms==1&&report.find("1/3")!=report.npos,"partial batch reports only verified creations");failAt=0;confirmOK=false;Check(!run()&&creates==1,"wrong/unregistered returned unit stops");confirmOK=true;duplicate=true;Check(!run()&&creates==2&&report.find("1/3")!=report.npos,"duplicate registry identity cannot count twice");duplicate=false;valid=false;Check(!run()&&!creates,"changed copy source before batch no native write");valid=true;Check(!run(51)&&!reads&&!creates&&!run(2,99990)&&!reads&&!creates,"count and complete coordinate extent validated before game reads");
 unsigned accessed=0;auto selected=[&]()->uintptr_t{++accessed;return 0;};auto cursor=[&](wht::MouseWorldPosition131&){++accessed;return false;};Check(!wht::CreateOrCopyUnitCommand131(0,12164,L"#create",L"hfoo 0",selected,cursor,report)&&!accessed,"bad args no game access");Check(!wht::CreateOrCopyUnitCommand131(0,12164,L"#copy",L"1",selected,cursor,report)&&!accessed,"profile mismatch no game source reads");Check(!wht::createUnitBusy131.test_and_set(),"failed command releases batch guard");wht::createUnitBusy131.clear();
 puts("create/copy unit contracts passed (owned mock worker only, no live unit created)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
