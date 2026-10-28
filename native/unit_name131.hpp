#pragma once
#include "unit_name_profile131.hpp"
#include "unit_find131.hpp"
namespace wht {
inline const UnitNameProfile131* UnitNameProfileFor131(unsigned build){for(const auto& p:unitNameProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool UnitNameCode131(uintptr_t base,const UnitNameProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
// GetUnitName's CUnit worker returns this inline cache. Its initializer copies
// at most 259 bytes plus NUL. Read only: never initialize or modify game memory.
inline bool ReadUnitNameCache131(uintptr_t unit,const UnitNameProfile131& p,char* out){
 if(!unit||!out)return false;
 __try{for(unsigned i=0;i<260;++i){out[i]=*reinterpret_cast<const char*>(unit+p.nameOffset+i);if(!out[i])return i>0;}return false;}__except(1){return false;}
}
inline bool DecodeUnitName131(const std::string& text,std::wstring& out){
 if(text.empty()||text.size()>259||text.find('\0')!=text.npos)return false;
 const int size=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),int(text.size()),nullptr,0);if(!size)return false;
 std::wstring value(size,L'\0');if(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),int(text.size()),&value[0],size)!=size)return false;
 for(auto c:value)if(c<32||c==127)return false;out=value;return true;
}
inline bool UnitNameHex131(wchar_t c){return (c>=L'0'&&c<=L'9')||(c>=L'a'&&c<=L'f')||(c>=L'A'&&c<=L'F');}
inline std::wstring PlainUnitName131(const std::wstring& name){
 std::wstring out;for(size_t i=0;i<name.size();++i){if(name[i]==L'|'&&i+1<name.size()){
  const auto c=name[i+1];if((c==L'c'||c==L'C')&&i+9<name.size()){bool hex=true;for(size_t n=i+2;n<i+10;++n)hex=hex&&UnitNameHex131(name[n]);if(hex){i+=9;continue;}}
  if(c==L'r'||c==L'R'){++i;continue;}if(c==L'n'||c==L'N'){out+=L' ';++i;continue;}
 }out+=name[i];}return out;
}
inline std::string UnitNameReport131(const std::wstring& name){
 std::wstring safe=name;for(auto& c:safe)if(c==L'|')c=L'/';
 const int size=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,safe.data(),int(safe.size()),nullptr,0,nullptr,nullptr);if(size<=0)return {};
 std::string out(size,'\0');if(WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,safe.data(),int(safe.size()),&out[0],size,nullptr,nullptr)!=size)return {};return out;
}
inline bool ParseUnitNameQuery131(const std::wstring& args,std::wstring& out){
 if(args.find(L'\0')!=args.npos||args.size()>520)return false;
 for(auto c:args)if((c<32&&c!=L'\t')||c==127||c==L'|')return false;
 const auto first=args.find_first_not_of(L" \t"),last=args.find_last_not_of(L" \t");if(first==args.npos)return false;
 const auto value=args.substr(first,last-first+1);const auto encoded=UnitNameReport131(value);std::wstring roundtrip;
 if(encoded.empty()||!DecodeUnitName131(encoded,roundtrip)||roundtrip!=value)return false;out=value;return true;
}
inline bool UnitNameContains131(const std::wstring& name,const std::wstring& query){
 if(query.empty()||query.size()>name.size())return false;
 for(size_t i=0;i+query.size()<=name.size();++i)if(CompareStringOrdinal(name.data()+i,int(query.size()),query.data(),int(query.size()),TRUE)==CSTR_EQUAL)return true;return false;
}
struct UnitNameSource131 {UnitFindSource131 selected;std::string name;};
inline bool SameUnitName131(const UnitNameSource131& a,const UnitNameSource131& b){return SameUnitFind131(a.selected,b.selected)&&a.name==b.name;}
template<class Selected> bool ReadUnitName131(uintptr_t base,unsigned build,Selected selected,UnitNameSource131& out){
 const auto* p=UnitNameProfileFor131(build);if(!p||!UnitNameCode131(base,*p))return false;
 UnitNameSource131 value;char name[260]{};if(!ReadUnitFind131(base,build,selected,value.selected)||!ReadUnitNameCache131(value.selected.source.unit,*p,name))return false;
 value.name=name;std::wstring decoded;if(!DecodeUnitName131(value.name,decoded)||PlainUnitName131(decoded).empty())return false;
 if(!ValidRemoveUnit131(base,build,value.selected.source)||selected()!=value.selected.source.unit)return false;
 char again[260]{};if(!ReadUnitNameCache131(value.selected.source.unit,*p,again)||value.name!=again)return false;out=value;return true;
}
template<class Read> bool UnitNameCommand131(const std::wstring& args,Read read,std::string& report){
 std::wstring query;if(!ParseUnitNameQuery131(args,query)){report="Usage: #ufinds name (active selection only)";return false;}
 UnitNameSource131 before,current;std::wstring name;
 if(!read(before)||!read(current)||!SameUnitName131(before,current)||!ValidUnitFindPosition131(current.selected.info)||!DecodeUnitName131(current.name,name)||(name=PlainUnitName131(name)).empty()){
  report="Unit Find: selected unit name/position unavailable or changed";return false;
 }
 const auto display=UnitNameReport131(name);if(display.empty()){report="Unit Find: invalid native name";return false;}
 if(!UnitNameContains131(name,query)){report="Unit Find: name not found in active selection; selected ["+display+"]";return false;}
 char coordinates[100]{};std::snprintf(coordinates,sizeof(coordinates)," at (%.1f, %.1f)",current.selected.info.x,current.selected.info.y);
 report="Found ["+display+"] in active selection"+coordinates;return true;
}
}
