#include "../native/mpq_loader131.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned calls=0;
#ifdef _WIN64
uintptr_t FakeOpen(wht::MpqSpan131 span,unsigned type,unsigned flags,unsigned priority,uintptr_t parent,bool unknown){const auto path=span.path;const auto length=span.length;
#else
uintptr_t __cdecl FakeOpen(const char* path,size_t length,unsigned type,unsigned flags,unsigned priority,uintptr_t parent,int unknown){
#endif
 Check(std::string(path,length)=="test.mpq"&&type==2&&!flags&&priority==10&&!parent&&!unknown,"native MPQ ABI and reference priority");++calls;return 0x1234;
}
int wmain(int argc,wchar_t** argv){try{
 Check(argc==2,"archive fixture argument");const auto file=std::filesystem::absolute(argv[1]);const auto folder=file.parent_path().wstring();const auto path=file.wstring();
 Check(wht::InvokeCustomMpq131(FakeOpen,"test.mpq",8)==0x1234&&calls==1,"typed native ABI returns opaque provider");
 Check(!wht::InvokeCustomMpq131(nullptr,"test.mpq",8)&&!wht::InvokeCustomMpq131(FakeOpen,"",0)&&calls==1,"invalid native call never dispatched");
 Check(!wht::MpqOpenProfileFor131(129),"unverified build refused");
 for(const auto& profile:wht::mpqOpenProfiles131){
#ifdef _WIN64
  const auto capture=profile.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const auto capture=profile.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(capture,std::ios::binary);Check(bool(input),"native captured fixture available");input.seekg(profile.open);std::array<unsigned char,32> code{};input.read(reinterpret_cast<char*>(code.data()),code.size());Check(input.gcount()==code.size()&&code==profile.code,"full native guard matches captured profile");
  const auto base=reinterpret_cast<uintptr_t>(code.data())-profile.open;Check(wht::MpqOpenCode131(base,profile),"native profile guard accepts verified bytes");code[31]^=1;Check(!wht::MpqOpenCode131(base,profile)&&!wht::MpqOpenCode131(0,profile),"altered or missing native code refused");
 }
 wht::MpqLoader131 loader;std::string report;std::wstring saved;unsigned mounted=0,writes=0,reloads=0;
 auto mount=[&](const std::wstring& actual)->uintptr_t{Check(actual==path,"native mount receives canonical path");++mounted;return 0x1234;};
 auto save=[&](const std::wstring& value){saved=value;++writes;return true;};auto reload=[&](){++reloads;};
 Check(loader.Command(folder,L"",saved,mount,save,reload,report)&&!mounted&&!writes,"bare MPQ command lists without mounting");
 Check(!loader.Command(folder,L"missing.mpq",saved,mount,save,reload,report)&&!mounted&&!writes,"missing archive not mounted or persisted");
 Check(!loader.Command(folder,path,saved,[](const std::wstring&)->uintptr_t{return 0;},save,reload,report)&&!writes&&loader.loaded.empty(),"native failure never persisted");
 Check(loader.Command(folder,L"\""+path+L"\"",saved,mount,save,reload,report)&&mounted==1&&writes==1&&reloads==1&&saved==path,"successful custom MPQ mounted then persisted");
 Check(loader.Command(folder,file.filename().wstring(),saved,mount,save,reload,report)&&mounted==1&&writes==1,"relative duplicate does not mount twice");
 unsigned logged=0;loader.Start(folder,saved+L",missing.mpq",mount,[&](const std::string&){++logged;});loader.Start(folder,saved,mount,[&](const std::string&){++logged;});Check(logged==2&&mounted==1,"startup list once and duplicate mounts skipped");
 Check(!loader.Command(folder,path,L"",mount,[](const std::wstring&){return false;},reload,report)&&reloads==1&&mounted==1&&report.find("this session")!=std::string::npos,"save failure reports session-only mount");
 Check(loader.Command(folder,path,L"",mount,save,reload,report)&&mounted==1&&writes==2&&reloads==2,"save retry does not remount");
 loader.loading=true;std::wstring resolved;Check(!loader.Load(folder,path,mount,resolved,report),"reentrant mount refused");loader.loading=false;
 wchar_t temp[MAX_PATH]{},ini[MAX_PATH]{};Check(GetTempPathW(MAX_PATH,temp)&&GetTempFileNameW(temp,L"mpq",0,ini),"isolated INI");Check(wht::SaveSetting(ini,L"Event",L"LoadMPQ",saved)&&wht::LoadConfig(ini).loadMpq==path,"MPQ INI roundtrip preserves spaced path");DeleteFileW(ini);
 std::puts("PASS native MPQ ABI, all captured guards, priority, persistence, dedup and failure policy");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
