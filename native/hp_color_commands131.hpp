#pragma once
#include "common.hpp"
#include <sstream>
namespace wht {
inline bool HpColorCommand131(const std::wstring& name){return name==L"#chp"||name==L"#chpe";}
inline bool HpColorValue131(const std::wstring& args,std::wstring& result){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;if(!(in>>value)||(in>>extra))return false;
 if(value.size()==10&&value[0]==L'0'&&(value[1]==L'x'||value[1]==L'X'))value.erase(0,2);
 if(value.size()!=8||value.find_first_not_of(L"0123456789abcdefABCDEF")!=std::wstring::npos)return false;
 for(auto& c:value)if(c>=L'a'&&c<=L'f')c-=L'a'-L'A';result=L"0x"+value;return true;
}
template<class Save,class Reload> bool ApplyHpColorCommand131(const std::wstring& name,const std::wstring& args,Save save,Reload reload){
 std::wstring value;if(!HpColorCommand131(name)||!HpColorValue131(args,value))return false;
 if(!save(L"HPBar",name==L"#chpe"?L"Color Enemy":L"Color",value.c_str()))return false;
 reload();return true;
}
}
