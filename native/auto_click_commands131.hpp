#pragma once
#include "auto_click131.hpp"
#include "view_commands131.hpp"
namespace wht {
inline bool AutoClickCommand131(const std::wstring& name){return name==L"#autol"||name==L"#autoclickl"||name==L"#autor"||name==L"#autoclickr"||name==L"#clickdelay";}
template<class Save,class Reload> bool ApplyAutoClickCommand131(const std::wstring& name,const std::wstring& args,AutoClick131& state,Save save,Reload reload){
 if(!AutoClickCommand131(name))return false;
 if(name!=L"#clickdelay"){
  bool& enabled=(name==L"#autol"||name==L"#autoclickl")?state.left:state.right;bool next=false;
  if(!ViewToggle131(args,enabled,next))return false;enabled=next;state.due=0;return true;
 }
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;if(!(in>>value)||(in>>extra)||value.size()>4)return false;
 unsigned delay=0;for(auto ch:value){if(ch<L'0'||ch>L'9')return false;delay=delay*10+unsigned(ch-L'0');}
 if(delay<5||delay>1000||!save(L"HotKeyHero",L"AutoClickInterval",std::to_wstring(delay)))return false;
 reload();state.due=0;return true;
}
}
