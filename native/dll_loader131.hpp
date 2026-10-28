#pragma once
#include "common.hpp"
#include <algorithm>
#include <cwctype>
#include <sstream>
namespace wht {
inline std::wstring DllPath131(const std::wstring& folder,std::wstring path){
 if(path.find(L'\0')!=std::wstring::npos||path.find_first_of(L"\r\n,")!=std::wstring::npos)return {};
 const auto first=path.find_first_not_of(L" \t"),last=path.find_last_not_of(L" \t");if(first==std::wstring::npos)return {};
 path=path.substr(first,last-first+1);
 if(path.size()>1&&path.front()==L'"'&&path.back()==L'"')path=path.substr(1,path.size()-2);
 if(path.empty()||path.find(L'"')!=std::wstring::npos)return {};
 if(path.size()>1&&path[1]==L':'&&(path.size()<3||(path[2]!=L'\\'&&path[2]!=L'/')))return {};
 if(path.find(L':')==std::wstring::npos&&path.front()!=L'\\'&&path.front()!=L'/')path=folder+L"\\"+path;
 wchar_t absolute[32768]{};const auto length=GetFullPathNameW(path.c_str(),32768,absolute,nullptr);
 return length&&length<32768?std::wstring(absolute):std::wstring{};
}
inline std::wstring DllIdentity131(std::wstring path){std::transform(path.begin(),path.end(),path.begin(),[](wchar_t c){return wchar_t(towlower(c));});return path;}
inline bool DllMachine131(WORD machine,WORD magic,WORD characteristics){
#ifdef _WIN64
 return machine==IMAGE_FILE_MACHINE_AMD64&&magic==IMAGE_NT_OPTIONAL_HDR64_MAGIC&&(characteristics&IMAGE_FILE_DLL);
#else
 return machine==IMAGE_FILE_MACHINE_I386&&magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC&&(characteristics&IMAGE_FILE_DLL);
#endif
}
inline bool CompatibleDll131(const std::wstring& path){
 HANDLE file=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);if(file==INVALID_HANDLE_VALUE)return false;
 IMAGE_DOS_HEADER dos{};DWORD bytes=0;bool valid=false;
 if(ReadFile(file,&dos,sizeof(dos),&bytes,nullptr)&&bytes==sizeof(dos)&&dos.e_magic==IMAGE_DOS_SIGNATURE&&dos.e_lfanew>=sizeof(dos)&&dos.e_lfanew<16*1024*1024){
  if(SetFilePointer(file,dos.e_lfanew,nullptr,FILE_BEGIN)!=INVALID_SET_FILE_POINTER){DWORD signature=0;IMAGE_FILE_HEADER header{};WORD magic=0;
   if(ReadFile(file,&signature,sizeof(signature),&bytes,nullptr)&&bytes==sizeof(signature)&&signature==IMAGE_NT_SIGNATURE&&ReadFile(file,&header,sizeof(header),&bytes,nullptr)&&bytes==sizeof(header)&&header.SizeOfOptionalHeader>=sizeof(WORD)&&ReadFile(file,&magic,sizeof(magic),&bytes,nullptr)&&bytes==sizeof(magic))valid=DllMachine131(header.Machine,magic,header.Characteristics);
  }
 }
 CloseHandle(file);return valid;
}
inline bool DllListContains131(const std::wstring& folder,const std::wstring& list,const std::wstring& path){
 std::wistringstream in(list);std::wstring item;const auto wanted=DllIdentity131(path);
 while(std::getline(in,item,L','))if(DllIdentity131(DllPath131(folder,item))==wanted)return true;return false;
}
struct DllLoader131 {
 // Libraries intentionally stay loaded until the game exits, as in 1.29.
 std::map<std::wstring,HMODULE> loaded;bool starting=false,started=false,loading=false;
 bool Load(const std::wstring& folder,const std::wstring& requested,std::wstring& resolved,std::string& report){
  if(loading){report="LoadDll: another library is being initialized.";return false;}
  resolved=DllPath131(folder,requested);if(resolved.empty()){report="LoadDll: invalid path (use one DLL path).";return false;}
  const auto identity=DllIdentity131(resolved);
  if(loaded.count(identity)){report="LoadDll: already loaded: "+Utf8(resolved);return true;}
  if(!CompatibleDll131(resolved)){report="LoadDll: file missing, invalid, or wrong x86/x64 architecture: "+Utf8(resolved);return false;}
  loading=true;const auto module=LoadLibraryExW(resolved.c_str(),nullptr,LOAD_WITH_ALTERED_SEARCH_PATH);const auto error=GetLastError();loading=false;
  if(!module){report="LoadDll: failed (error "+std::to_string(error)+"): "+Utf8(resolved);return false;}
  loaded.emplace(identity,module);report="LoadDll: loaded: "+Utf8(resolved);return true;
 }
 template<class Log> void Start(const std::wstring& folder,const std::wstring& list,Log log){
  if(started||starting)return;started=true;starting=true;std::wistringstream in(list);std::wstring item;
  while(std::getline(in,item,L',')){if(item.find_first_not_of(L" \t")==std::wstring::npos)continue;std::wstring path;std::string report;Load(folder,item,path,report);log(report);}
  starting=false;
 }
 template<class Save,class Reload> bool Command(const std::wstring& folder,const std::wstring& args,const std::wstring& previous,Save save,Reload reload,std::string& report){
  if(args.find_first_not_of(L" \t")==std::wstring::npos){report="LoadDll: "+(previous.empty()?std::string("(none)"):Utf8(previous));return true;}
  std::wstring path;if(!Load(folder,args,path,report))return false;
  if(DllListContains131(folder,previous,path))return true;
  const auto list=previous.empty()?path:previous+L","+path;
  if(!save(list)){report="LoadDll: loaded for this session, but could not save automatic loading: "+Utf8(path);return false;}
  reload();return true;
 }
};
}
