#pragma once
#include "drop_all.hpp"
#include <sstream>
namespace wht {
inline bool DropCommand131(const std::wstring& name){return name==L"#drop"||name==L"#dropall"||name==L"#dropitem";}
inline bool DropCommandSlot131(const std::wstring& name,const std::wstring& args,int& slot){
 if(!DropCommand131(name)||args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;
 if(name==L"#dropitem"){if(!(in>>value)||value.size()!=1||value[0]<L'1'||value[0]>L'6'||(in>>extra))return false;slot=int(value[0]-L'1');}
 else {if(in>>extra)return false;slot=-1;}return true;
}
}
