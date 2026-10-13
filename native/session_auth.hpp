#pragma once
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include "update_progress.hpp"
namespace wht {
inline bool ValidSessionId(const std::string& value){return value.size()==32&&value.find_first_not_of("0123456789abcdefABCDEF")==std::string::npos;}
inline std::string ReadSessionId(){
 wchar_t folder[32768]{};auto length=GetTempPathW(32768,folder);if(!length||length>=32768)return {};
 std::ifstream file(std::filesystem::path(std::wstring(folder)+L"LoadFileMapWar3-session.txt"),std::ios::binary);
 char bytes[128]{};file.read(bytes,sizeof(bytes));if(file.gcount()>=sizeof(bytes))return {};
 std::string value(bytes,size_t(file.gcount()));if(value.rfind("\xef\xbb\xbf",0)==0)value.erase(0,3);
 while(!value.empty()&&isspace(static_cast<unsigned char>(value.back())))value.pop_back();
 return ValidSessionId(value)?value:std::string{};
}
inline bool MainTitleMatches(const std::wstring& title,const std::wstring& username){auto prefix=L"Main - "+username;return title==prefix||title.rfind(prefix+L" - ",0)==0;}
struct MainWindowSearch {std::wstring username;bool found=false;};
inline BOOL CALLBACK FindSessionMain(HWND window,LPARAM argument){
 auto& search=*reinterpret_cast<MainWindowSearch*>(argument);
 DWORD pid=0;GetWindowThreadProcessId(window,&pid);
 // GetWindowText sends WM_GETTEXT for windows in our own process. The game
 // thread may be busy loading a map, so never wait on a Warcraft window here.
 if(!pid||pid==GetCurrentProcessId())return TRUE;
 HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);if(!process)return TRUE;
 wchar_t path[32768]{};DWORD length=32768;
 bool loader=QueryFullProcessImageNameW(process,0,path,&length)&&_wcsicmp(std::filesystem::path(path).filename().c_str(),L"LoadFileMapWar3.exe")==0;
 CloseHandle(process);if(!loader)return TRUE;
 wchar_t title[512]{};GetWindowTextW(window,title,512);
 search.found=MainTitleMatches(title,search.username);return search.found?FALSE:TRUE;
}
struct SessionIdentity {std::string id;std::wstring username;ULONGLONG until=0;};
enum class SessionCheck {Verified,Denied,Unavailable,Revoked};
inline bool RetainSession(SessionCheck result,const SessionIdentity& previous,ULONGLONG now){return result==SessionCheck::Unavailable&&!previous.username.empty()&&now<previous.until;}
inline std::mutex sessionMutex;
inline SessionIdentity sessionIdentity;
inline bool SessionReady(){
 SessionIdentity identity;{std::lock_guard<std::mutex> guard(sessionMutex);identity=sessionIdentity;}
 if(identity.username.empty()||GetTickCount64()>=identity.until||ReadSessionId()!=identity.id)return false;
 MainWindowSearch search{identity.username};EnumWindows(FindSessionMain,reinterpret_cast<LPARAM>(&search));return search.found;
}
inline std::wstring EmbeddedSessionHelper(){
 struct TemporaryHelper {
  std::wstring path,folder;
  ~TemporaryHelper(){if(!path.empty())DeleteFileW(path.c_str());if(!folder.empty())RemoveDirectoryW(folder.c_str());}
 };
 static TemporaryHelper helper;static std::mutex mutex;std::lock_guard<std::mutex> guard(mutex);
 if(!helper.path.empty())return helper.path;
 HMODULE module=nullptr;
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&EmbeddedSessionHelper),&module))return {};
 auto resource=FindResourceW(module,MAKEINTRESOURCEW(102),RT_RCDATA);if(!resource)return {};
 DWORD size=SizeofResource(module,resource);auto loaded=LoadResource(module,resource);auto bytes=LockResource(loaded);
 if(!bytes||!size)return {};
 wchar_t temp[32768]{},unique[32768]{};
 if(!GetTempPathW(32768,temp)||!GetTempFileNameW(temp,L"WHA",0,unique))return {};
 DeleteFileW(unique);if(!CreateDirectoryW(unique,nullptr))return {};
 std::wstring folder=unique,path=folder+L"\\WHT.SessionAuth.exe";
 HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_TEMPORARY,nullptr);
 DWORD written=0;bool ok=file!=INVALID_HANDLE_VALUE&&WriteFile(file,bytes,size,&written,nullptr)&&written==size;
 if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
 if(!ok){DeleteFileW(path.c_str());RemoveDirectoryW(folder.c_str());return {};}
 helper.folder=folder;helper.path=path;return path;
}
inline SessionIdentity VerifySession(const std::wstring& directory,SessionCheck* result=nullptr){
 if(result)*result=SessionCheck::Unavailable;
 SessionIdentity identity;if(ReadSessionId().empty()){if(result)*result=SessionCheck::Denied;return identity;}
 auto path=EmbeddedSessionHelper();if(path.empty())return identity;
 SECURITY_ATTRIBUTES security{sizeof(security),nullptr,TRUE};HANDLE read=nullptr,write=nullptr;
 if(!CreatePipe(&read,&write,&security,0))return identity;SetHandleInformation(read,HANDLE_FLAG_INHERIT,0);
 std::wstring command=L"\""+path+L"\"";
 STARTUPINFOW startup{};startup.cb=sizeof(startup);startup.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;startup.hStdOutput=write;startup.hStdError=write;
 PROCESS_INFORMATION process{};BOOL launched=CreateProcessW(path.c_str(),command.data(),nullptr,nullptr,TRUE,CREATE_NO_WINDOW,nullptr,directory.c_str(),&startup,&process);CloseHandle(write);
 if(!launched){CloseHandle(read);return identity;}
 const bool timedOut=WaitForSingleObject(process.hProcess,7000)!=WAIT_OBJECT_0;
 if(timedOut){TerminateProcess(process.hProcess,2);WaitForSingleObject(process.hProcess,1000);}
 DWORD code=2;if(!timedOut)GetExitCodeProcess(process.hProcess,&code);CloseHandle(process.hThread);CloseHandle(process.hProcess);
 char bytes[2048]{};DWORD count=0;ReadFile(read,bytes,sizeof(bytes)-1,&count,nullptr);CloseHandle(read);
 if(code){if(result)*result=code==3?SessionCheck::Revoked:code==1?SessionCheck::Denied:SessionCheck::Unavailable;return identity;}
 std::istringstream input(std::string(bytes,count));std::string id,user;std::getline(input,id);std::getline(input,user);
 if(!id.empty()&&id.back()=='\r')id.pop_back();if(!user.empty()&&user.back()=='\r')user.pop_back();
 if(ValidSessionId(id)&&id==ReadSessionId()&&!user.empty()){identity={id,Wide(user),GetTickCount64()+30000};if(result)*result=SessionCheck::Verified;}
 return identity;
}
struct StartupUpdateState {
 HANDLE ready=nullptr,process=nullptr;
 HANDLE progressMapping=nullptr;UpdateProgressData* progress=nullptr;
 ~StartupUpdateState(){if(progress)UnmapViewOfFile(progress);if(progressMapping)CloseHandle(progressMapping);if(ready)CloseHandle(ready);if(process)CloseHandle(process);}
 bool OpenProgress(const std::wstring& eventName){
  if(progressMapping)return false;
  progressMapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(UpdateProgressData),(eventName+L".Progress").c_str());
  if(!progressMapping)return false;
  if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(progressMapping);progressMapping=nullptr;return false;}
  progress=static_cast<UpdateProgressData*>(MapViewOfFile(progressMapping,FILE_MAP_READ|FILE_MAP_WRITE,0,0,sizeof(UpdateProgressData)));
  if(!progress){CloseHandle(progressMapping);progressMapping=nullptr;return false;}
  progress->percent=0;progress->phase=0;return true;
 }
 std::string ProgressText(bool english)const{
  if(!progress)return {};
  LONG phase=progress->phase,percent=progress->percent;
  if(phase>=1&&phase<=3&&process&&WaitForSingleObject(process,0)==WAIT_OBJECT_0)phase=5;
  return UpdateProgressText(phase,percent,english);
 }
 bool Pending()const{return ready&&process&&WaitForSingleObject(ready,0)==WAIT_TIMEOUT&&WaitForSingleObject(process,0)==WAIT_TIMEOUT;}
};
inline StartupUpdateState startupUpdateState;
inline bool StartupUpdatePending(){return startupUpdateState.Pending();}
inline bool StartUpdateWatcher(const std::wstring& directory){
 auto path=EmbeddedSessionHelper();if(path.empty())return false;
 auto eventName=L"Local\\WHT.Update.Startup."+std::to_wstring(GetCurrentProcessId())+L"."+std::to_wstring(GetTickCount64());
 HANDLE ready=CreateEventW(nullptr,TRUE,FALSE,eventName.c_str());if(!ready)return false;
 startupUpdateState.OpenProgress(eventName);
 std::wstring command=L"\""+path+L"\" --watch-update \""+directory+L"\" "+std::to_wstring(GetCurrentProcessId())+L" \""+eventName+L"\"";
 STARTUPINFOW startup{};startup.cb=sizeof(startup);startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
 PROCESS_INFORMATION process{};
 if(!CreateProcessW(path.c_str(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,directory.c_str(),&startup,&process)){CloseHandle(ready);return false;}
 startupUpdateState.ready=ready;startupUpdateState.process=process.hProcess;
 CloseHandle(process.hThread);return true;
}
inline bool RestrictedMainAction(unsigned action){return action==1||action==2||action==3||action==5;}
}
