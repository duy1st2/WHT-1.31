#include "../native/common.hpp"
#include <filesystem>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int wmain(int argc,wchar_t** argv){try{
 if(argc==3&&std::wstring(argv[1])==L"--child"){
  Check(GetPrivateProfileIntW(L"Event",L"Unlock Map Size",9,argv[2])==0,"child fixture really contains saved OFF");
  Check(wht::LoadConfig(argv[2]).unlockMap,"fresh process resets Map Size ON despite saved OFF");return 0;
 }
 const auto root=std::filesystem::absolute(L"../runtime-test");Check(std::filesystem::is_directory(root),"project runtime-test root exists");
 const auto directory=root/(L"mapsize-startup-owned-"+std::to_wstring(GetCurrentProcessId()));Check(std::filesystem::create_directory(directory),"unique owned test directory");
 const auto ini=(directory/L"settings.ini").wstring();
 Check(WritePrivateProfileStringW(L"Event",L"Unlock Map Size",L"0",ini.c_str())!=FALSE,"legacy OFF fixture");
 Check(wht::LoadConfig(ini).unlockMap,"launch default ignores old persisted OFF");
 Check(wht::SaveSetting(ini,L"Event",L"Unlock Map Size",L"0")&&!wht::LoadConfig(ini).unlockMap,"explicit OFF survives reload within current process");
 Check(WritePrivateProfileStringW(L"Event",L"Unlock Map Size",L"1",ini.c_str())!=FALSE&&!wht::LoadConfig(ini).unlockMap,"periodic reload cannot reset an explicit session OFF");
 Check(!wht::SaveSetting((directory/L"missing"/L"bad.ini").wstring(),L"Event",L"Unlock Map Size",L"1")&&!wht::LoadConfig(ini).unlockMap,"failed save does not enable session state");
 Check(wht::SaveSetting(ini,L"event",L"unlock map size",L"1")&&wht::LoadConfig(ini).unlockMap,"case-insensitive explicit ON survives reload");
 Check(wht::SaveSetting(ini,L"Event",L"Unlock Map Size",L"0")&&!wht::LoadConfig(ini).unlockMap,"prepare saved OFF for real restart test");
 wchar_t executable[32768]{};Check(GetModuleFileNameW(nullptr,executable,32768)>0,"own test executable path");
 std::wstring command=L"\""+std::wstring(executable)+L"\" --child \""+ini+L"\"";STARTUPINFOW startup{};startup.cb=sizeof(startup);PROCESS_INFORMATION child{};
 Check(CreateProcessW(executable,command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&child)!=FALSE,"launch independent child process");
 const auto waited=WaitForSingleObject(child.hProcess,10000);DWORD result=99;const bool got=GetExitCodeProcess(child.hProcess,&result)!=FALSE;CloseHandle(child.hThread);CloseHandle(child.hProcess);
 Check(waited==WAIT_OBJECT_0&&got&&result==0,"new process starts ON even after parent saved OFF");
 Check(!wht::LoadConfig(ini).unlockMap,"child restart does not alter parent session OFF");
 WritePrivateProfileStringW(nullptr,nullptr,nullptr,ini.c_str());Check(DeleteFileW(ini.c_str())&&RemoveDirectoryW(directory.c_str()),"owned fixture cleanup");
 puts("Map Size launch ON, saved OFF restart, session reload and failed-save contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
