#include "../native/no_cooldown131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
int main(){try{
 for(const auto& p:wht::cooldownResetProfiles131){
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
 Check(wht::CooldownResetCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::CooldownResetCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }


 wht::NoCooldown131 state;std::string report;unsigned calls=0;bool ok=true;auto apply=[&](){++calls;return ok;};
 Check(state.Command(L"#nocd",L"",1000,apply,report)&&state.enabled&&calls==1&&state.due==1100,"enable invokes native once then schedules100ms");
 Check(!state.Tick(true,true,1099,apply)&&calls==1,"no early reset");Check(!state.Tick(true,true,1100,apply)&&calls==2&&state.due==1200,"100ms cadence");
 Check(!state.Tick(true,false,5000,apply)&&calls==2&&state.enabled,"menu/chat/focus pauses");Check(!state.Tick(true,true,5000,apply)&&calls==3&&state.due==5100,"resume once without catch up burst");
 Check(!state.Command(L"#nocd",L" extra",6000,apply,report)&&state.enabled&&calls==3,"bad args retain active state and invoke nothing");Check(!state.Command(L"#yescd",std::wstring(L"\0",1),6000,apply,report)&&state.enabled,"embedded NUL refused");
 Check(state.Command(L"#yescd",L"",6000,apply,report)&&!state.enabled&&calls==3,"disable never calls native");Check(!state.Tick(true,true,7000,apply)&&calls==3,"disabled no reset");
 ok=false;Check(!state.Command(L"#nocd",L"",8000,apply,report)&&!state.enabled&&calls==4,"failed initial reset never enables");ok=true;Check(state.Command(L"#nocd",L"",9000,apply,report),"re-enable");ok=false;Check(state.Tick(true,true,9100,apply)&&!state.enabled,"native failure stops and notifies once");const auto stoppedCalls=calls;Check(!state.Tick(true,true,9200,apply)&&calls==stoppedCalls,"failed reset not retried");
 ok=true;state.Command(L"#nocd",L"",10000,apply,report);const auto revokedCalls=calls;Check(!state.Tick(false,true,10100,apply)&&!state.enabled&&calls==revokedCalls,"authorization loss resets without native call");
 auto reentrant=[&](){state.Reset();return true;};Check(!state.Command(L"#nocd",L"",11000,reentrant,report)&&!state.enabled,"callback teardown cannot re-enable stale command");
 wht::RemoveUnitSource131 source;source.unit=1;source.world=2;source.agent=3;source.local=0;source.id={4,5};unsigned reads=0,writes=0,validations=0;bool changed=false,valid=true;
 auto read=[&](wht::RemoveUnitSource131& out){out=source;if(++reads==2&&changed)++out.id.high;return true;};auto reset=[&](const wht::RemoveUnitSource131& value){++writes;return value.unit==source.unit;};auto verify=[&](const wht::RemoveUnitSource131&){++validations;return valid;};
 Check(wht::ResetCooldownFlow131(read,reset,verify)&&writes==1&&validations==2,"stable registry checked before and after native");changed=true;reads=writes=validations=0;Check(!wht::ResetCooldownFlow131(read,reset,verify)&&!writes&&!validations,"changed selection identity before reset refused");changed=false;valid=false;reads=writes=validations=0;Check(!wht::ResetCooldownFlow131(read,reset,verify)&&!writes,"invalid registry no native call");
 Check(!wht::InvokeCooldownReset131(nullptr,1)&&!wht::InvokeCooldownReset131(nullptr,0),"null native refused");
 puts("no cooldown contracts passed (no live cooldown changed)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
