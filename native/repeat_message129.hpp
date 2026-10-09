#pragma once
#include <string>
#include <sstream>
#include <cstdint>
namespace wht {
struct RepeatMessage129 {
 std::string text;unsigned remaining=0;uint64_t due=0;
 void Reset(){text.clear();remaining=0;due=0;}
 bool Set(unsigned count,const std::string& value){
  if(!count||count>10000||value.empty()||value.size()>128||value.front()=='#'||value.find_first_of("\r\n")!=std::string::npos||value.find('\0')!=std::string::npos)return false;
  text=value;remaining=count;due=0;return true;
 }
 template<class Send> bool Tick(bool allowed,uint64_t now,Send send){
  if(!allowed||!remaining||now<due||!send(text))return false;
  --remaining;due=now+250;if(!remaining)text.clear();return true;
 }
};
inline bool ParseRepeatMessage129(const std::wstring& args,unsigned& count,std::wstring& text,bool& stop){
 std::wistringstream in(args);std::wstring token,extra;count=0;text.clear();stop=false;
 if(!(in>>token))return false;
 if(token==L"stop"){if(in>>extra)return false;stop=true;return true;}
 if(token.empty()||token.size()>5)return false;
 for(wchar_t ch:token){if(ch<L'0'||ch>L'9')return false;count=count*10+unsigned(ch-L'0');}
 if(!count||count>10000)return false;
 std::getline(in>>std::ws,text);return !text.empty()&&text.front()!=L'#'&&text.find_first_of(L"\r\n") ==std::wstring::npos;
}
}
