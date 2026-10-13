#pragma once
#include "mth129.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
namespace wht {
// Resolve the cdecl wrappers from this build's native registration routine.
// 9A000..A2000 registers real natives; 8Bxxxx registers stub replacements.
inline uintptr_t NativeAddress129(uintptr_t base,const char* name){
 if(!base||!name)return 0;
 __try{
  for(uintptr_t r=0x9a000;r<0xa2000;++r){
   auto p=reinterpret_cast<const unsigned char*>(base+r);
   if(p[0]!=0x68||p[5]!=0x68||p[10]!=0x68||p[15]!=0xe8)continue;
   auto text=*reinterpret_cast<const uintptr_t*>(p+6),fn=*reinterpret_cast<const uintptr_t*>(p+11);
   if(text<base+0xa00000||text>=base+0xc00000||fn<base+0x1000||fn>=base+0x900000)continue;
   if(!strcmp(reinterpret_cast<const char*>(text),name))return fn;
  }
 }__except(EXCEPTION_EXECUTE_HANDLER){}
 return 0;
}
template<class T> T ParityNative129(uintptr_t base,const char* name){return reinterpret_cast<T>(NativeAddress129(base,name));}
struct NativeString129 {
 uintptr_t storage[8]{},object[3]{};
 explicit NativeString129(const char* text){storage[7]=reinterpret_cast<uintptr_t>(text);object[2]=reinterpret_cast<uintptr_t>(storage);}
};
inline std::wstring TrimParity129(std::wstring s){auto first=s.find_first_not_of(L" \t\r\n"),end=s.find_last_not_of(L" \t\r\n");return first==s.npos?L"":s.substr(first,end-first+1);}
inline bool ParityInteger129(const std::wstring& s,int low,int high,int& out){
 if(s.empty()||s.size()>10)return false;for(auto c:s)if(c<L'0'||c>L'9')return false;
 auto n=wcstoll(s.c_str(),nullptr,10);if(n<low||n>high)return false;out=int(n);return true;
}
inline bool ParityToggle129(std::wistringstream& in,bool previous,std::wstring& value){
 std::wstring extra;value=previous?L"0":L"1";if(in>>value){
  if(value==L"on"||value==L"true")value=L"1";if(value==L"off"||value==L"false")value=L"0";
  if((value!=L"0"&&value!=L"1")||in>>extra)return false;
 }return true;
}
inline std::wstring ParityAlias129(const std::wstring& n){
 const std::pair<const wchar_t*,const wchar_t*> aliases[]={
  {L"#showcooldown",L"#cooldown"},{L"#hotkeypanel",L"#showhotkey"},{L"#showresource",L"#res"},
  {L"#sell",L"#sellall"},{L"#quicksell",L"#sellall"},
  {L"#showball",L"#showb"},{L"#hideball",L"#hideb"}
 };for(const auto& a:aliases)if(n==a.first)return a.second;return n;
}
inline std::wstring RegistryPlayerName129(){
 wchar_t value[256]{};DWORD bytes=sizeof(value);
 if(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Blizzard Entertainment\\Warcraft III",L"userlocal",RRF_RT_REG_SZ,nullptr,value,&bytes)!=ERROR_SUCCESS)return {};
 return value;
}
inline bool StorePlayerName129(const std::wstring& name){
 HKEY key=nullptr;if(RegCreateKeyExW(HKEY_CURRENT_USER,L"Software\\Blizzard Entertainment\\Warcraft III",0,nullptr,0,KEY_SET_VALUE,nullptr,&key,nullptr)!=ERROR_SUCCESS)return false;
 auto result=RegSetValueExW(key,L"userlocal",0,REG_SZ,reinterpret_cast<const BYTE*>(name.c_str()),DWORD((name.size()+1)*sizeof(wchar_t)));RegCloseKey(key);return result==ERROR_SUCCESS;
}
inline bool RenameLocalPlayer129(uintptr_t base,const ResourceApi129& api,const char* name){
 const auto native=ParityNative129<void(__cdecl*)(uint32_t,void*)>(base,"SetPlayerName");if(!native||!api.Complete()||!name||!*name)return false;
 NativeString129 text(name);
 __try{if(!*reinterpret_cast<uintptr_t*>(base+0xd3b6f4))return false;auto local=ResourceLocal129(base,api);if(!local)return false;native(local,text.object);auto raw=api.resolve(local);auto now=raw?api.name(raw,1):nullptr;return now&&!strcmp(now,name);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ParityOwnedUnits129(uintptr_t base,uint32_t* units,unsigned capacity,unsigned& count){
 count=0;auto create=ParityNative129<uint32_t(__cdecl*)()>(base,"CreateGroup");
 auto destroy=ParityNative129<void(__cdecl*)(uint32_t)>(base,"DestroyGroup");
 auto enumerate=ParityNative129<void(__cdecl*)(uint32_t,uint32_t,uint32_t)>(base,"GroupEnumUnitsOfPlayer");
 auto first=ParityNative129<uint32_t(__cdecl*)(uint32_t)>(base,"FirstOfGroup");
 auto remove=ParityNative129<void(__cdecl*)(uint32_t,uint32_t)>(base,"GroupRemoveUnit");
 auto local=ParityNative129<uint32_t(__cdecl*)()>(base,"GetLocalPlayer");
 if(!create||!destroy||!enumerate||!first||!remove||!local)return false;
 uint32_t group=0;bool ok=false;
 __try{if(!*reinterpret_cast<uintptr_t*>(base+0xd3b6f4))return false;auto player=local();if(!player)return false;group=create();if(!group)return false;enumerate(group,player,0);
  for(;count<capacity;++count){auto unit=first(group);if(!unit){ok=true;break;}units[count]=unit;remove(group,unit);}
  if(count==capacity)ok=first(group)==0;destroy(group);group=0;
 }__except(EXCEPTION_EXECUTE_HANDLER){if(group){__try{destroy(group);}__except(EXCEPTION_EXECUTE_HANDLER){}}}
 return ok;
}
inline unsigned ChangeOwnedAbilities129(uintptr_t base,uint32_t code,bool add){
 uint32_t units[4096]{};unsigned count=0,changed=0;if(!ParityOwnedUnits129(base,units,4096,count))return 0;
 auto native=ParityNative129<int(__cdecl*)(uint32_t,uint32_t)>(base,add?"UnitAddAbility":"UnitRemoveAbility");if(!native)return 0;
 __try{for(unsigned i=0;i<count;++i)if(native(units[i],code))++changed;}
 __except(EXCEPTION_EXECUTE_HANDLER){}return changed;
}
struct MapReveal129 {
 uintptr_t world=0;uint32_t modifier=0;
 bool Set(uintptr_t base,bool enabled){
  auto create=ParityNative129<uint32_t(__cdecl*)(uint32_t,int,uint32_t,int,int)>(base,"CreateFogModifierRect");
  auto bounds=ParityNative129<uint32_t(__cdecl*)()>(base,"GetWorldBounds");
  auto local=ParityNative129<uint32_t(__cdecl*)()>(base,"GetLocalPlayer");
  auto start=ParityNative129<void(__cdecl*)(uint32_t)>(base,"FogModifierStart");
  auto stop=ParityNative129<void(__cdecl*)(uint32_t)>(base,"FogModifierStop");
  auto destroy=ParityNative129<void(__cdecl*)(uint32_t)>(base,"DestroyFogModifier");
  if(!create||!bounds||!local||!start||!stop||!destroy)return false;
  __try{auto current=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(current!=world){world=current;modifier=0;}if(!current)return false;
   if(enabled){if(modifier)return true;auto player=local(),rect=bounds();if(!player||!rect)return false;modifier=create(player,4,rect,1,0);if(!modifier)return false;start(modifier);}
   else if(modifier){stop(modifier);destroy(modifier);modifier=0;}return true;
  }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 void Observe(uintptr_t base){__try{auto current=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(current!=world){world=current;modifier=0;}}__except(EXCEPTION_EXECUTE_HANDLER){world=0;modifier=0;}}
};
struct TerrainWork129 {
 uintptr_t world=0;uint32_t code=0;int column=0;bool active=false;
 bool Start(uintptr_t base,uint32_t raw){if(!NativeAddress129(base,"SetTerrainType")||!raw)return false;world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);code=raw;column=0;active=world!=0;return active;}
 bool Tick(uintptr_t base,bool allowed){
  if(!active)return false;if(!allowed||world!=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4)){active=false;return false;}
  auto set=ParityNative129<void(__cdecl*)(const float*,const float*,uint32_t,int,int,int)>(base,"SetTerrainType");if(!set){active=false;return false;}
  __try{float x=-15000.f+float(column)*256.f;for(float y=15000.f;y>-15000.f;y-=256.f)set(&x,&y,code,-1,7,1);++column;
   if(-15000.f+float(column)*256.f>=15000.f){active=false;return true;}
  }__except(EXCEPTION_EXECUTE_HANDLER){active=false;}return false;
 }
};
struct IniWatch129 {
 FILETIME stamp{};DWORD size=0;bool initialized=false;uint64_t due=0;
 bool Poll(const std::wstring& path,uint64_t now){
  if(now<due)return false;due=now+1000;WIN32_FILE_ATTRIBUTE_DATA info{};
  if(!GetFileAttributesExW(path.c_str(),GetFileExInfoStandard,&info))return false;
  bool changed=initialized&&(CompareFileTime(&stamp,&info.ftLastWriteTime)!=0||size!=info.nFileSizeLow);
  stamp=info.ftLastWriteTime;size=info.nFileSizeLow;initialized=true;return changed;
 }
};
}
