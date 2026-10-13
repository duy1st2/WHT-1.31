#ifdef _WIN64
#include "profile64.hpp"
constexpr auto ClientArch131=L"x86_64";
#else
#include "runtime131.hpp"
constexpr auto ClientArch131=L"x86";
#endif
#include <tlhelp32.h>
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs=std::filesystem;
uintptr_t RemoteModule131(DWORD pid,const wchar_t* name){
 HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid);
 if(snapshot==INVALID_HANDLE_VALUE)return 0;MODULEENTRY32W entry{};entry.dwSize=sizeof(entry);uintptr_t found=0;
 if(Module32FirstW(snapshot,&entry))do{if(!_wcsicmp(entry.szModule,name)){found=reinterpret_cast<uintptr_t>(entry.modBaseAddr);break;}}while(Module32NextW(snapshot,&entry));
 CloseHandle(snapshot);return found;
}
bool Inside131(const fs::path& child,const fs::path& root){
 const auto a=fs::weakly_canonical(child).wstring(),b=fs::weakly_canonical(root).wstring()+L"\\";
 return a.size()>b.size()&&!_wcsnicmp(a.c_str(),b.c_str(),b.size());
}
DWORD ExistingClient131(const fs::path& exe){
 HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
 if(snapshot==INVALID_HANDLE_VALUE)return 0;
 PROCESSENTRY32W entry{};entry.dwSize=sizeof(entry);DWORD found=0;
 if(Process32FirstW(snapshot,&entry))do{
  if(_wcsicmp(entry.szExeFile,L"Warcraft III.exe"))continue;
  HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,entry.th32ProcessID);
  wchar_t path[32768]{};DWORD size=32768;std::error_code error;
  if(process&&QueryFullProcessImageNameW(process,0,path,&size)&&fs::equivalent(path,exe,error)){
   if(found){CloseHandle(process);CloseHandle(snapshot);throw std::runtime_error("Multiple test clients running; select --attach PID.");}
   found=entry.th32ProcessID;
  }
  if(process)CloseHandle(process);
 }while(Process32NextW(snapshot,&entry));
 CloseHandle(snapshot);return found;
}
bool Inject131(HANDLE process,DWORD pid,const fs::path& dll){
 if(RemoteModule131(pid,L"WHT.mix")){
  std::wcout<<L"WHT is already loaded. F8 opens its menu in a match.\n";return true;
 }
 auto local=reinterpret_cast<uintptr_t>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"LoadLibraryW"));
 HMODULE owner=nullptr;if(!local||!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(local),&owner))return false;
 wchar_t ownerName[32768]{};GetModuleFileNameW(owner,ownerName,32768);
 const auto name=fs::path(ownerName).filename().wstring();uintptr_t remote=0;
 for(unsigned attempt=0;attempt<300&&!remote;++attempt){remote=RemoteModule131(pid,name.c_str());if(!remote)Sleep(100);}
 if(!remote){std::wcerr<<L"Game loader is not ready.\n";return false;}
 const auto path=fs::absolute(dll).wstring();const size_t size=(path.size()+1)*sizeof(wchar_t);
 auto memory=VirtualAllocEx(process,nullptr,size,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);if(!memory)return false;
 bool ok=WriteProcessMemory(process,memory,path.c_str(),size,nullptr)!=FALSE;
 HANDLE thread=ok?CreateRemoteThread(process,nullptr,0,reinterpret_cast<LPTHREAD_START_ROUTINE>(remote+local-reinterpret_cast<uintptr_t>(owner)),memory,0,nullptr):nullptr;
 if(!thread){VirtualFreeEx(process,memory,0,MEM_RELEASE);return false;}
 const auto wait=WaitForSingleObject(thread,30000);DWORD result=0;
 ok=wait==WAIT_OBJECT_0&&GetExitCodeThread(thread,&result)&&result;
 if(wait==WAIT_OBJECT_0)VirtualFreeEx(process,memory,0,MEM_RELEASE);
 CloseHandle(thread);
 if(wait!=WAIT_OBJECT_0)std::wcerr<<L"DLL load still pending; remote argument retained.\n";
 return ok;
}
int wmain(int argc,wchar_t** argv){try{
 std::wstring self(32768,L'\0');self.resize(GetModuleFileNameW(nullptr,self.data(),DWORD(self.size())));
 const auto ownFolder=fs::path(self).parent_path();
 if(argc<3){std::wcerr<<L"WHT131Launcher --client <runtime-test client root> [--attach PID] [--loadfile map]\n";return 1;}
 fs::path client;DWORD attach=0;std::wstring map;
 for(int i=1;i<argc;++i){std::wstring option=argv[i];if(i+1>=argc)return 1;
  if(option==L"--client")client=fs::absolute(argv[++i]);
  else if(option==L"--attach")attach=std::stoul(argv[++i]);
  else if(option==L"--loadfile")map=fs::absolute(argv[++i]).wstring();else return 1;
 }
 // Locate the project runtime-test root from either build/Release or an
 // installed client/x86. This launcher cannot touch or launch the originals.
 fs::path root;for(auto dir=ownFolder;!dir.empty()&&dir!=dir.root_path();dir=dir.parent_path()){
  if(dir.filename()==L"runtime-test"){root=dir;break;}
  if(fs::exists(dir/L"AGENTS.md")&&fs::exists(dir/L"runtime-test")){root=dir/L"runtime-test";break;}
 }
 if(root.empty()||!Inside131(client,root)){std::wcerr<<L"Only this project's runtime-test clients are allowed.\n";return 2;}
 const auto exe=client/ClientArch131/L"Warcraft III.exe",dll=client/ClientArch131/L"WHT.mix";
 if(!Inside131(exe,root)||!Inside131(dll,root)){std::wcerr<<L"Executable and tool must remain inside runtime-test.\n";return 2;}
#ifdef _WIN64
 const auto profile=wht::FindProfile64(wht::HashFile(exe.wstring()));
#else
 const auto profile=wht::FindProfile131(wht::HashFile(exe.wstring()));
#endif
 if(!profile||!fs::exists(dll)){std::wcerr<<L"Unsupported executable or missing WHT.mix.\n";return 3;}
 if(!attach){attach=ExistingClient131(exe);if(attach)std::wcout<<L"Loading WHT into the existing test client, PID "<<attach<<L".\n";}
 HANDLE process=nullptr;DWORD pid=attach;
 if(attach){process=OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,attach);
  wchar_t path[32768]{};DWORD length=32768;
  if(!process||!QueryFullProcessImageNameW(process,0,path,&length)||!fs::equivalent(fs::path(path),exe)){if(process)CloseHandle(process);std::wcerr<<L"Attach target must be the specified test client's x86 executable.\n";return 4;}
 }else{
  std::wstring command=L"\""+exe.wstring()+L"\" -launch -window";
#ifndef _WIN64
  command+=L" -graphicsapi Direct3D9";
#endif
  if(!map.empty()){if(!fs::exists(map))return 5;command+=L" -loadfile \""+map+L"\"";}
  STARTUPINFOW startup{};startup.cb=sizeof(startup);PROCESS_INFORMATION info{};
  if(!CreateProcessW(exe.c_str(),command.data(),nullptr,nullptr,FALSE,0,nullptr,exe.parent_path().c_str(),&startup,&info)){std::wcerr<<L"CreateProcess failed: "<<GetLastError()<<L"\n";return 6;}
  CloseHandle(info.hThread);process=info.hProcess;pid=info.dwProcessId;
 }
 const bool loaded=Inject131(process,pid,dll);CloseHandle(process);
 std::wcout<<L"WHT 1.31.1."<<profile->build<<L" "<<ClientArch131<<L" | PID "<<pid<<L" | "<<(loaded?L"DLL loaded; F8 opens the menu in a match. Inspect WHT.log for native/session state":L"DLL load failed")<<L"\n";
 return loaded?0:7;
}catch(const std::exception& error){std::cerr<<error.what()<<"\n";return 8;}}
