#include "../native/delay_reducer131.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
struct Image {
 uintptr_t base=0;wht::DelayReducer131& service;
 Image(const char* path,const wht::DelayProfile131& p,wht::DelayReducer131& s):service(s){std::ifstream in(path,std::ios::binary);Check(bool(in),"capture exists");std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(in)),{});base=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,bytes.size(),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));Check(base!=0,"owned image allocation");memcpy(reinterpret_cast<void*>(base),bytes.data(),bytes.size());for(const auto& e:p.entries){*reinterpret_cast<uintptr_t*>(base+e.table-sizeof(uintptr_t))=base+e.locator;*reinterpret_cast<uintptr_t*>(base+e.table+10*sizeof(uintptr_t))=base+e.getter;
#ifndef _WIN64
 *reinterpret_cast<uint32_t*>(base+e.locator+12)=uint32_t(base+e.type);
#endif
 }DWORD old=0;Check(VirtualProtect(reinterpret_cast<void*>(base),bytes.size(),PAGE_READONLY,&old)!=FALSE,"owned image read-only like engine");}
 ~Image(){service.Reset();if(base)VirtualFree(reinterpret_cast<void*>(base),0,MEM_RELEASE);}
 template<class T> void Put(uintptr_t rva,T value){DWORD old=0;auto at=reinterpret_cast<void*>(base+rva);Check(VirtualProtect(at,sizeof(value),PAGE_READWRITE,&old)!=FALSE,"owned edit protection");memcpy(at,&value,sizeof(value));DWORD ignored=0;Check(VirtualProtect(at,sizeof(value),old,&ignored)!=FALSE,"owned edit restores protection");}
 uintptr_t Slot(const wht::DelayEntry131& p)const{return *reinterpret_cast<uintptr_t*>(base+p.table+10*sizeof(uintptr_t));}
 unsigned Call(const wht::DelayEntry131& p,unsigned index)const{Check(Slot(p)==wht::DelayCallback131(index),"only invoke actual test callback, not captured code");using Fn=unsigned(__thiscall*)(uintptr_t);return reinterpret_cast<Fn>(Slot(p))(0x12345678);}
};
struct Ini {
 std::wstring path;Ini(const wchar_t* root){const auto folder=std::filesystem::canonical(root);path=(folder/(L"delay-reducer-contracts-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64())+L".ini")).wstring();Check(std::filesystem::path(path).is_absolute()&&std::filesystem::path(path).parent_path()==folder,"isolated runtime-test INI");HANDLE h=CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);Check(h!=INVALID_HANDLE_VALUE,"never replace existing test INI");CloseHandle(h);}~Ini(){DeleteFileW(path.c_str());}
};
int wmain(int argc,wchar_t** argv){try{
 Check(argc==2,"runtime-test argument");unsigned value=0;Check(wht::ParseDelayReducer131(L" 50 ",value)&&value==50&&wht::ParseDelayReducer131(L"1000",value)&&value==1000,"integer range");for(auto text:{L"",L"0",L"1001",L"-1",L"1.5",L"50 extra",L"50\n",L"999999999999"})Check(!wht::ParseDelayReducer131(text,value),"invalid delay arguments");Check(!wht::ParseDelayReducer131(std::wstring(L"5\0x",3),value),"embedded NUL rejected");Check(!wht::DelayProfileFor131(129),"unknown build refused");
 for(const auto& p:wht::delayProfiles131){
#ifdef _WIN64
 const char* path=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* path=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 wht::DelayReducer131 service;Image image(path,p,service);wht::Config config;std::string report;
 for(unsigned i=0;i<3;++i)Check(wht::DelayEntryCode131(image.base,p.entries[i],i)&&!wht::DelayEntryCode131(0,p.entries[i],i),"independent rebased RTTI and native getter guards");
 Check(service.Update(image.base,p.build,true,config,report),"default scopes0 no changes");for(const auto& e:p.entries)Check(image.Slot(e)==image.base+e.getter,"native100 preserved despite legacy config net250");
 config.delayReducerScopes=1;config.localDelay=config.lanDelay=25;Check(service.Update(image.base,p.build,true,config,report)&&image.Call(p.entries[0],0)==25&&image.Call(p.entries[1],1)==25&&image.Slot(p.entries[2])==image.base+p.entries[2].getter,"Local/LAN callbacks typed thiscall/Win64 ABI, net unchanged");
 config.delayReducerScopes=3;config.netDelay=37;Check(service.Update(image.base,p.build,true,config,report)&&image.Call(p.entries[2],2)==37,"BNET enabled independently");MEMORY_BASIC_INFORMATION info{};Check(VirtualQuery(reinterpret_cast<void*>(image.base+p.entries[0].table),&info,sizeof(info))&&info.Protect==PAGE_READONLY,"callback swap restores original read-only protection");for(const auto& e:p.entries)Check(!memcmp(reinterpret_cast<void*>(image.base+e.getter),"\xb8\x64\0\0\0\xc3",6),"no engine instruction patch");
 const auto owned=service.slots[0].address;Check(service.Update(image.base,p.build,true,config,report)&&service.slots[0].address==owned,"steady update does not recreate callback ownership");
 image.Put<unsigned char>(p.entries[0].getter,0x90);report.clear();Check(!service.Update(image.base,p.build,true,config,report)&&image.Slot(p.entries[0])==image.base+p.entries[0].getter&&image.Slot(p.entries[1])==image.base+p.entries[1].getter&&image.Call(p.entries[2],2)==37&&!report.empty(),"changed native code refuses Local/LAN and restores owned callbacks without disabling BNET");report.clear();Check(!service.Update(image.base,p.build,true,config,report)&&report.empty(),"failure logged once");image.Put<unsigned char>(p.entries[0].getter,0xb8);Check(service.Update(image.base,p.build,true,config,report),"native profile recovery");
 const auto foreign=uintptr_t(0x12345678);image.Put<uintptr_t>(p.entries[1].table+10*sizeof(uintptr_t),foreign);Check(!service.Ready(image.base,p.build,1)&&!service.Update(image.base,p.build,true,config,report)&&image.Slot(p.entries[1])==foreign&&image.Slot(p.entries[0])==image.base+p.entries[0].getter,"foreign callback never overwritten, Local/LAN fail closed");image.Put<uintptr_t>(p.entries[1].table+10*sizeof(uintptr_t),image.base+p.entries[1].getter);Check(service.Update(image.base,p.build,true,config,report)&&image.Call(p.entries[1],1)==25,"externally restored original clears stale tracker and can re-enable");
 Check(service.Update(image.base,p.build,false,config,report),"settings/authorization disable cleanup");for(const auto& e:p.entries)Check(image.Slot(e)==image.base+e.getter,"native slot restored");Check(config.delayReducerScopes==3,"disable does not erase saved scope intent");Check(service.Update(image.base,p.build,true,config,report),"saved scopes reapply");config.localDelay=config.lanDelay=config.netDelay=100;Check(service.Update(image.base,p.build,true,config,report),"100 restores engine ownership");for(const auto& e:p.entries)Check(image.Slot(e)==image.base+e.getter,"no callback needed for native100");
#ifdef _WIN64
 constexpr unsigned nameOffset=16;
#else
 constexpr unsigned nameOffset=8;
#endif
 image.Put<unsigned char>(p.entries[2].type+nameOffset,'!');Check(!service.Ready(image.base,p.build,2),"wrong provider RTTI rejected");image.Put<unsigned char>(p.entries[2].type+nameOffset,'.');
 wht::Config display;display.netDelay=250;Check(wht::DelayReducerMenuValue131(display,2)==L"100","inactive legacy Net250 displays native100");display.delayReducerScopes=1;display.localDelay=25;Check(wht::DelayReducerMenuValue131(display,1)==L"25"&&wht::DelayReducerMenuValue131(display,2)==L"100","menu scopes independent");display.delayReducerScopes=3;display.netDelay=37;Check(wht::DelayReducerMenuValue131(display,2)==L"37"&&wht::DelayReducerMenuScope131(L"Event",L"Net Game Delay Reducer")==2&&wht::DelayReducerMenuScope131(L"Graphics",L"Net Game Delay Reducer")==0,"active menu scope and exact key routing");
 Ini ini(argv[1]);config=wht::LoadConfig(ini.path);Check(config.delayReducerScopes==0&&config.netDelay==250,"existing delay defaults preserved but remain opt-in");unsigned writes=0,applies=0;bool failLan=false;
 auto save=[&](const wchar_t* key,const std::wstring& text){++writes;if(failLan&&std::wstring(key)==L"Lan Game Delay Reducer"&&text==L"40")return false;return wht::SaveSetting(ini.path,L"Event",key,text);};auto reload=[&](){config=wht::LoadConfig(ini.path);};auto available=[&](unsigned scope){return service.Ready(image.base,p.build,scope);};auto apply=[&](){++applies;std::string ignored;return service.Update(image.base,p.build,true,config,ignored);};
 Check(wht::DelayReducerCommand131(false,L"25",config,available,apply,save,reload,report)&&config.localDelay==25&&config.lanDelay==25&&config.delayReducerScopes==1&&image.Call(p.entries[0],0)==25,"real INI persistence and Local/LAN callback integration");Check(wht::DelayReducerCommand131(true,L"37",config,available,apply,save,reload,report)&&config.netDelay==37&&config.delayReducerScopes==3&&image.Call(p.entries[2],2)==37,"real INI BNET activation preserves Local/LAN");Check(wht::DelayReducerCommand131(false,L"100",config,available,apply,save,reload,report)&&image.Slot(p.entries[0])==image.base+p.entries[0].getter&&image.Call(p.entries[2],2)==37,"per-command restore to baseline");
 const auto priorApplies=applies;failLan=true;Check(!wht::DelayReducerCommand131(false,L"40",config,available,apply,save,reload,report)&&config.localDelay==100&&config.lanDelay==100&&config.delayReducerScopes==3&&applies==priorApplies&&report.find("restored")!=report.npos,"partial persistence rolls back both delay fields and scope before any apply");failLan=false;
 const auto priorWrites=writes;Check(!wht::DelayReducerCommand131(false,L"25 extra",config,available,apply,save,reload,report)&&writes==priorWrites,"invalid input no settings write");Check(!wht::DelayReducerCommand131(false,L"25",config,[](unsigned){return false;},apply,save,reload,report)&&writes==priorWrites,"unavailable profile no persistence");Check(!wht::DelayReducerCommand131(false,L"40",config,available,[](){return false;},save,reload,report)&&config.localDelay==40&&report.find("saved, but")!=report.npos,"saved setting with failed native apply is reported honestly");
 Check(wht::SaveSetting(ini.path,L"Event",L"Delay Reducer Scopes",L"9"),"invalid scope fixture");reload();Check(config.delayReducerScopes==0&&service.Update(image.base,p.build,true,config,report),"invalid persisted mask cannot activate providers");
 }
 puts("Delay reducer RTTI, native callback ABI, ownership/restoration and persistence contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
