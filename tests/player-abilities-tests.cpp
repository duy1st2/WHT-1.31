#include "../native/player_units131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};

bool __cdecl Visit(uintptr_t unit,void* context){auto& ids=*static_cast<std::vector<uintptr_t>*>(context);ids.push_back(unit);return ids.size()<3;}
void __cdecl MockEnumerate(uint32_t type,wht::PlayerUnitVisit131 visit,void* context,bool children){Check(type==0x2b773375&&!children,"exact CUnit class and no derived traversal");for(uintptr_t i=1;i<=10;++i)if(!visit(i,context))break;}

int main(){try{
 for(const auto& p:wht::playerUnitsProfiles131){
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
 Check(wht::PlayerUnitsCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::PlayerUnitsCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }




 std::vector<uintptr_t> seen;Check(wht::InvokePlayerUnits131(MockEnumerate,Visit,&seen)&&seen==std::vector<uintptr_t>({1,2,3}),"cdecl native enum and callback ABI including early stop");Check(!wht::InvokePlayerUnits131(nullptr,Visit,&seen)&&!wht::InvokePlayerUnits131(MockEnumerate,nullptr,&seen),"invalid API no call");
 std::vector<wht::RemoveUnitSource131> units;for(unsigned i=0;i<3;++i){wht::RemoveUnitSource131 u;u.unit=100+i;u.world=1;u.agent=2+i;u.id={3+i,4};u.local=0;units.push_back(u);}
 unsigned checks=0,writes=0,failCheck=0,failWrite=0;std::string report;
 auto valid=[&](const auto&){return ++checks!=failCheck;};auto apply=[&](const auto&,std::string& detail){++writes;if(writes==failWrite){detail="native result not confirmed";return false;}return true;};
 auto run=[&](){checks=writes=0;return wht::PlayerAbilitiesBatch131(units,valid,apply,report);};
 Check(run()&&writes==3&&report.find("3/3")!=report.npos,"all snapshotted units confirmed");
 failCheck=2;Check(!run()&&!writes&&report.find("no action taken")!=report.npos,"whole roster preflight before first write");failCheck=5;Check(!run()&&writes==1&&report.find("0/3")!=report.npos,"ownership/world invalidated after callback stops without false confirmation");failCheck=0;
 failWrite=2;Check(!run()&&writes==2&&report.find("1/3")!=report.npos&&report.find("may have applied")!=report.npos,"partial failure no retry no rollback and truthful count");failWrite=0;
 units[1].id=units[0].id;Check(!run()&&!writes,"duplicate identity refused before mutation");units.clear();Check(run()&&!writes&&report.find("0/0")!=report.npos,"verified empty owned snapshot succeeds with zero");units.resize(8193);Check(!run()&&!checks&&!writes,"oversized snapshot cannot partially modify");
 wht::playerAbilitiesBusy131.test_and_set();Check(!wht::PlayerAbilitiesCommand131(0,12164,true,L"AHhb",report)&&report.find("already running")!=report.npos,"batch reentry rejected before native access");wht::playerAbilitiesBusy131.clear();Check(!wht::PlayerAbilitiesCommand131(0,12164,true,L"AHhb extra",report)&&report.find("Usage")!=report.npos,"bad rawcode args before access");Check(!wht::playerAbilitiesBusy131.test_and_set(),"bad args do not take batch flag");wht::playerAbilitiesBusy131.clear();
 puts("player unit ability batch contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
