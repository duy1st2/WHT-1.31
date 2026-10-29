#pragma once
#include "dll_loader131.hpp"
#include <array>
namespace wht {
struct MpqOpenProfile131 {unsigned build;uintptr_t open;std::array<unsigned char,32> code;};
#ifdef _WIN64
inline constexpr MpqOpenProfile131 mpqOpenProfiles131[]={
{12164,0x3324b0,{{0x40,0x55,0x56,0x57,0x41,0x54,0x48,0x81,0xec,0x88,0x00,0x00,0x00,0x48,0x83,0x79,0x08,0x00,0x41,0x8b,0xe9,0x41,0x8b,0xf8,0x44,0x8b,0xe2,0x48,0x8b,0xf1,0x75,0x0f}}},
{12173,0x39ff30,{{0x40,0x55,0x56,0x57,0x41,0x54,0x48,0x81,0xec,0x88,0x00,0x00,0x00,0x48,0x83,0x79,0x08,0x00,0x41,0x8b,0xe9,0x41,0x8b,0xf8,0x44,0x8b,0xe2,0x48,0x8b,0xf1,0x75,0x0f}}},
};
#else
inline constexpr MpqOpenProfile131 mpqOpenProfiles131[]={
{12164,0x2d7ee0,{{0x55,0x8b,0xec,0x83,0xec,0x0c,0x83,0x7d,0x0c,0x00,0x53,0x56,0x57,0x75,0x09,0x33,0xc0,0x5f,0x5e,0x5b,0x8b,0xe5,0x5d,0xc3,0x8b,0x7d,0x1c,0x8b,0x75,0x18,0x85,0xff}}},
{12173,0x2c1d20,{{0x55,0x8b,0xec,0x83,0xec,0x0c,0x83,0x7d,0x0c,0x00,0x53,0x56,0x57,0x75,0x09,0x33,0xc0,0x5f,0x5e,0x5b,0x8b,0xe5,0x5d,0xc3,0x8b,0x7d,0x1c,0x8b,0x75,0x18,0x85,0xff}}},
};
#endif
inline const MpqOpenProfile131* MpqOpenProfileFor131(unsigned build){for(const auto& p:mpqOpenProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool MpqOpenCode131(uintptr_t base,const MpqOpenProfile131& p){__try{return base&&!memcmp(reinterpret_cast<void*>(base+p.open),p.code.data(),p.code.size());}__except(1){return false;}}
inline constexpr unsigned customMpqPriority131=10;
#ifdef _WIN64
struct MpqSpan131 {const char* path;size_t length;};
using MpqOpenFn131=uintptr_t(*)(MpqSpan131,unsigned,unsigned,unsigned,uintptr_t,bool);
#else
using MpqOpenFn131=uintptr_t(__cdecl*)(const char*,size_t,unsigned,unsigned,unsigned,uintptr_t,int);
#endif
inline uintptr_t InvokeCustomMpq131(MpqOpenFn131 open,const char* path,size_t length,unsigned priority=customMpqPriority131){
 if(!open||!path||!length)return 0;
 __try{
#ifdef _WIN64
  return open({path,length},2,0,priority,0,false);
#else
  return open(path,length,2,0,priority,0,0);
#endif
 }__except(1){return 0;}
}
inline uintptr_t OpenCustomMpq131(uintptr_t base,unsigned build,const std::wstring& path,unsigned priority=customMpqPriority131){
 const auto* p=MpqOpenProfileFor131(build);if(!p||!MpqOpenCode131(base,*p))return 0;
 const auto encoded=Utf8(path);return InvokeCustomMpq131(reinterpret_cast<MpqOpenFn131>(base+p->open),encoded.c_str(),encoded.size(),priority);
}
struct MpqLoader131 {
 // Native archive providers stay mounted until process exit, like 1.29.
 std::map<std::wstring,uintptr_t> loaded;bool started=false,loading=false;
 template<class Mount> bool Load(const std::wstring& folder,const std::wstring& args,Mount mount,std::wstring& path,std::string& report){
  if(loading){report="LoadMPQ: another archive is being mounted.";return false;}
  path=DllPath131(folder,args);if(path.empty()){report="LoadMPQ: invalid path (use one archive path).";return false;}
  const auto identity=DllIdentity131(path);if(loaded.count(identity)){report="LoadMPQ: already mounted: "+Utf8(path);return true;}
  const auto attributes=GetFileAttributesW(path.c_str());if(attributes==INVALID_FILE_ATTRIBUTES||(attributes&FILE_ATTRIBUTE_DIRECTORY)){report="LoadMPQ: file not found: "+Utf8(path);return false;}
  loading=true;const auto archive=mount(path);loading=false;
  if(!archive){report="LoadMPQ: native archive mount failed: "+Utf8(path);return false;}
  loaded.emplace(identity,archive);report="LoadMPQ: mounted: "+Utf8(path);return true;
 }
 template<class Mount,class Log> void Start(const std::wstring& folder,const std::wstring& list,Mount mount,Log log){
  if(started||loading)return;started=true;std::wistringstream in(list);std::wstring item;
  while(std::getline(in,item,L',')){if(item.find_first_not_of(L" \t")==std::wstring::npos)continue;std::wstring path;std::string report;Load(folder,item,mount,path,report);log(report);}
 }
 template<class Mount,class Save,class Reload> bool Command(const std::wstring& folder,const std::wstring& args,const std::wstring& previous,Mount mount,Save save,Reload reload,std::string& report){
  if(args.find_first_not_of(L" \t")==std::wstring::npos){report="LoadMPQ: "+(previous.empty()?std::string("(none)"):Utf8(previous));return true;}
  std::wstring path;if(!Load(folder,args,mount,path,report))return false;
  if(DllListContains131(folder,previous,path))return true;
  const auto list=previous.empty()?path:previous+L","+path;
  if(!save(list)){report="LoadMPQ: mounted for this session, but could not save automatic loading: "+Utf8(path);return false;}
  reload();return true;
 }
};
}
