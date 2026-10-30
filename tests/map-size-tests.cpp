#include "../native/map_size131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
void PutByte(uintptr_t at,unsigned char value){DWORD old=0,ignored=0;Check(VirtualProtect(reinterpret_cast<void*>(at),1,PAGE_EXECUTE_READWRITE,&old)!=FALSE,"owned fixture writable");*reinterpret_cast<unsigned char*>(at)=value;Check(FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(at),1)!=FALSE,"owned fixture cache flush");Check(VirtualProtect(reinterpret_cast<void*>(at),1,old,&ignored)!=FALSE,"owned fixture protection restored");}
int main(){try{
 for(const auto& p:wht::mapSizeProfiles131){
#ifdef _WIN64
  const char* path=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
  const char* path=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
  std::ifstream file(path,std::ios::binary);Check(bool(file),"actual engine capture available");unsigned char header[4096]{};file.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(capturedBase));
  size_t length=0;for(size_t i=0;i<p.codeCount;++i)length=(std::max)(length,size_t(p.code[i].rva+p.code[i].length));std::vector<unsigned char> image(length+64);const auto base=reinterpret_cast<uintptr_t>(image.data());
  for(size_t i=0;i<p.codeCount;++i){const auto& code=p.code[i];std::vector<unsigned char> bytes(code.length);file.seekg(code.rva);file.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(file.gcount())==bytes.size(),"captured context read");
   for(size_t j=0;j<code.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+code.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+code.reloc[j],&value,4);}Check(!memcmp(bytes.data(),code.bytes,bytes.size()),"complete captured size-check contexts match independent profile");
   memcpy(image.data()+code.rva,bytes.data(),bytes.size());for(size_t j=0;j<code.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+code.reloc[j],4);value+=uint32_t(base);memcpy(image.data()+code.rva+code.reloc[j],&value,4);}
  }
  for(unsigned i=0;i<p.count;++i){unsigned char actual[6]{},expected[6]{};const auto n=wht::MapSizeCompare131(expected,wht::mapSizeNative131);file.seekg(p.sites[i]);file.read(reinterpret_cast<char*>(actual),n);Check(n==p.compareLength&&!memcmp(actual,expected,n),"captured compare is 256MiB, not a bitflag equality test");memcpy(image.data()+p.sites[i],actual,n);}
  wht::MapSizeService131 service;Check(service.Ready(base,p.build),"rebased complete profile ready");
  for(size_t i=0;i<p.codeCount;++i){const auto at=p.code[i].rva;image[at]^=1;Check(!service.Ready(base,p.build),"changed surrounding branch/allocation context refuses");image[at]^=1;}
  image[p.sites[p.count-1]]=0x90;Check(!service.Ready(base,p.build),"last compare mismatch fails whole-group preflight");
 }
 Check(!wht::MapSizeProfileFor131(129),"unsupported build refused");
 // Execute only purpose-written owned code, never bytes from engine captures.
 auto memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));Check(memory!=nullptr,"owned executable fixture allocation");unsigned pos=0;
#ifdef _WIN64
 const unsigned char argument[]={0x48,0x8b,0xc1};
#else
 const unsigned char argument[]={0x8b,0x44,0x24,0x04};
#endif
 memcpy(memory,argument,sizeof(argument));pos=sizeof(argument);const auto target=reinterpret_cast<uintptr_t>(memory+pos);pos+=wht::MapSizeCompare131(memory+pos,wht::mapSizeNative131);
 const unsigned char result[]={0x0f,0x97,0xc0,0x0f,0xb6,0xc0,0xc3};memcpy(memory+pos,result,sizeof(result));pos+=sizeof(result);DWORD old=0;Check(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&old)&&FlushInstructionCache(GetCurrentProcess(),memory,pos),"fixture RX");using Compare=unsigned(__cdecl*)(uintptr_t);const auto compare=reinterpret_cast<Compare>(memory);
 Check(!compare(wht::mapSizeNative131)&&compare(uintptr_t(wht::mapSizeNative131)+1),"stock inclusive size boundary");
 wht::MapSizeService131 native;auto& hook=native.hooks[0];Check(hook.Create(target)&&hook.Enable()&&hook.Current(),"actual MinHook enables compare stub");
 Check(!compare(0)&&!compare(wht::mapSizeNative131+1)&&!compare(wht::mapSizeRaised131)&&compare(uintptr_t(wht::mapSizeRaised131)+1),"stub CMP flags and indirect continuation preserve raised inclusive boundary");
#ifdef _WIN64
 Check(compare(uintptr_t(0x100000001ull))==1,"Win64 comparison keeps full64-bit file size");
#endif
 MEMORY_BASIC_INFORMATION info{};Check(VirtualQuery(memory,&info,sizeof(info))&&info.Protect==PAGE_EXECUTE_READ,"MinHook restores RX page protection");
 const auto owned=hook.owned;PutByte(target+4,owned[4]^1);Check(!hook.Disable()&&*reinterpret_cast<unsigned char*>(target+4)==(owned[4]^1),"foreign branch modification never overwritten");PutByte(target+4,owned[4]);
 std::string report;Check(native.Update(0,0,false,report)&&!hook.active&&compare(wht::mapSizeNative131+1),"OFF/auth loss restores stock compare without native profile probe");Check(hook.Enable()&&!compare(wht::mapSizeNative131+1)&&native.Reset(),"re-enable reuses owned stub and restores again");
 Check(MH_RemoveHook(reinterpret_cast<void*>(target))==MH_OK,"owned test hook cleanup");VirtualFree(reinterpret_cast<void*>(hook.stub),0,MEM_RELEASE);VirtualFree(memory,0,MEM_RELEASE);
 unsigned validations=0,enables=0,disables=0;
 Check(!wht::MapSizeTransition131(3,true,[&](unsigned i){++validations;return i!=2;},[&](unsigned){++enables;return true;},[&](unsigned){++disables;return true;})&&validations==3&&!enables&&!disables,"all size checks validated before any enable");
 Check(!wht::MapSizeTransition131(3,true,[](unsigned){return true;},[&](unsigned i){++enables;return i!=1;},[&](unsigned){++disables;return true;})&&enables==2&&disables==3,"partial group failure attempts rollback of all members");
 bool enabled=false;Check(wht::ParseMapSize131(L"",false,enabled)&&enabled&&wht::ParseMapSize131(L" 0 ",true,enabled)&&!enabled,"bare toggle and explicit OFF");
 for(const auto* bad:{L"2",L"0.5",L"1 extra",L"on",L"1\n",L"0\r"})Check(!wht::ParseMapSize131(bad,false,enabled),"strict boolean arguments");Check(!wht::ParseMapSize131(std::wstring(L"1\0",2),false,enabled),"NUL refused");
 wht::Config config;unsigned writes=0,applies=0;bool available=true,applyOk=true;auto save=[&](const wchar_t* value){++writes;config.unlockMap=*value==L'1';return true;};auto apply=[&](bool value){++applies;Check(value==config.unlockMap,"apply uses confirmed persisted preference");return applyOk;};auto ready=[&](){return available;};
 Check(wht::MapSizeCommand131(L"1",config,ready,apply,save,[](){},report)&&config.unlockMap&&applies==1,"checked persistence then immediate apply");available=false;const auto prior=writes;Check(!wht::MapSizeCommand131(L"1",config,ready,apply,save,[](){},report)&&writes==prior,"unavailable native no persistence");
 Check(wht::MapSizeCommand131(L"0",config,ready,apply,save,[](){},report)&&!config.unlockMap,"OFF remains available without native readiness");available=true;const auto priorApplies=applies;Check(!wht::MapSizeCommand131(L"1",config,ready,apply,[](auto){return true;},[](){},report)&&applies==priorApplies,"reload mismatch never applies native");
 applyOk=false;Check(!wht::MapSizeCommand131(L"1",config,ready,apply,save,[](){},report)&&config.unlockMap&&report.find("saved, but")!=report.npos,"saved preference with incomplete native apply reported honestly");
 MH_Uninitialize();puts("Map size captured profiles, live owned MinHook/CMP boundary, ownership, rollback and persistence contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
