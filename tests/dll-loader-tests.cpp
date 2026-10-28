#include <filesystem>
#include "../native/dll_loader131.hpp"
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
int wmain(int argc,wchar_t** argv){try{
 Check(argc==2,"fixture argument");
 const auto file=std::filesystem::absolute(argv[1]);const auto folder=file.parent_path().wstring();const auto path=file.wstring();
 Check(wht::CompatibleDll131(path),"real test DLL has matching PE architecture");
 Check(!wht::CompatibleDll131(folder),"directory cannot be a plugin");
 Check(wht::DllPath131(folder,L" \""+file.filename().wstring()+L"\" ")==path,"relative quoted paths resolve beside tool");
 for(auto input:{L"",L" \t",L"a.dll,b.dll",L"a.dll\n",L"a\"b.dll",L"C:relative.dll"})Check(wht::DllPath131(folder,input).empty(),"invalid paths refused");
 Check(wht::DllPath131(folder,std::wstring(L"a\0b",3)).empty(),"NUL path rejected");
 Check(!wht::DllMachine131(IMAGE_FILE_MACHINE_I386,IMAGE_NT_OPTIONAL_HDR64_MAGIC,IMAGE_FILE_DLL)&&!wht::DllMachine131(IMAGE_FILE_MACHINE_AMD64,IMAGE_NT_OPTIONAL_HDR32_MAGIC,IMAGE_FILE_DLL),"PE machine and optional header must agree");
 Check(!wht::DllMachine131(IMAGE_FILE_MACHINE_I386,IMAGE_NT_OPTIONAL_HDR32_MAGIC,0)&&!wht::DllMachine131(IMAGE_FILE_MACHINE_AMD64,IMAGE_NT_OPTIONAL_HDR64_MAGIC,0),"executable without DLL flag refused");
 wht::DllLoader131 loader;std::string report;std::wstring saved;unsigned writes=0,reloads=0;
 auto save=[&](const std::wstring& value){saved=value;++writes;return true;};auto reload=[&](){++reloads;};
 Check(loader.Command(folder,L"",saved,save,reload,report)&&loader.loaded.empty()&&!writes,"bare command lists without loading");
 Check(!loader.Command(folder,L"missing.dll",saved,save,reload,report)&&!writes&&loader.loaded.empty(),"missing file never persisted");
 Check(loader.Command(folder,L"\""+path+L"\"",saved,save,reload,report)&&loader.loaded.size()==1&&writes==1&&reloads==1&&saved==path,"real DLL loads then persists exact canonical path");
 const auto module=loader.loaded.begin()->second;Check(module!=nullptr,"owned plugin module");
 auto identity=wht::DllIdentity131(path);Check(loader.Command(folder,identity,saved,save,reload,report)&&loader.loaded.size()==1&&writes==1&&reloads==1,"case-insensitive duplicate neither loads nor saves twice");
 Check(wht::DllListContains131(folder,file.filename().wstring(),path),"relative stored path deduplicates absolute command");
 loader.loading=true;std::wstring resolved;Check(!loader.Load(folder,path,resolved,report),"reentrant initialization refused");loader.loading=false;
 unsigned logged=0;loader.Start(folder,saved+L",missing.dll",[&](const std::string&){++logged;});loader.Start(folder,saved,[&](const std::string&){++logged;});Check(logged==2&&loader.loaded.size()==1,"startup list processed once and tolerates failed entry");
 Check(!loader.Command(folder,path,L"",[](const std::wstring&){return false;},reload,report)&&reloads==1&&report.find("this session")!=std::string::npos,"save failure reports already-loaded session without claiming persistence");
 Check(loader.Command(folder,path,L"",save,reload,report)&&writes==2&&reloads==2&&loader.loaded.size()==1,"save retry never reloads DLL");
 wchar_t temp[MAX_PATH]{},ini[MAX_PATH]{};Check(GetTempPathW(MAX_PATH,temp)&&GetTempFileNameW(temp,L"dll",0,ini),"isolated INI");
 Check(wht::SaveSetting(ini,L"Event",L"LoadDll",saved)&&wht::LoadConfig(ini).loadDll==path,"real INI preserves paths with spaces");DeleteFileW(ini);
 std::puts("PASS DLL validation, native load, path normalization, dedup, persistence and startup");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
