#pragma once
#include "common.hpp"
#include <sstream>
#include <cwctype>
namespace wht {
inline bool QuickChatSettingCommand131(const std::wstring& name){return name==L"#qchat"||name==L"#qchatclear"||name==L"#qchatkey";}
inline bool QuickChatKey131(std::wstring token,int& code){
 for(auto& ch:token)ch=wchar_t(std::towupper(ch));
 if(token==L"NONE"){code=-1;return true;}
 unsigned vk=0;
 if(token.size()==1&&((token[0]>=L'A'&&token[0]<=L'Z')||(token[0]>=L'0'&&token[0]<=L'9')))vk=unsigned(token[0]);
 else if(token.size()>=2&&token.size()<=3&&token[0]==L'F'){
  unsigned value=0;for(size_t i=1;i<token.size();++i){if(token[i]<L'0'||token[i]>L'9')return false;value=value*10+unsigned(token[i]-L'0');}
  if(value<1||value>12)return false;vk=VK_F1+value-1;
 }else if(token.size()==7&&token.substr(0,6)==L"NUMPAD"&&token[6]>=L'0'&&token[6]<=L'9')vk=VK_NUMPAD0+unsigned(token[6]-L'0');
 else if(token==L"SPACE")vk=VK_SPACE;
 else if(token==L"TAB")vk=VK_TAB;
 if(!vk)return false;
 for(int candidate=0;candidate<=0x30b;++candidate)if(EngineKeyToVirtualKey(candidate)==vk){code=candidate;return true;}
 return false;
}
struct QuickChatSetting131 {unsigned slot=0;std::wstring text;bool binding=false;int code=-1;};
inline bool ParseQuickChatSetting131(const std::wstring& name,const std::wstring& args,QuickChatSetting131& out){
 if(!QuickChatSettingCommand131(name)||args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring slot;
 if(!(in>>slot)||slot.size()!=1||slot[0]<L'1'||slot[0]>L'9')return false;
 if(name==L"#qchatkey"){
  std::wstring key,extra;int code=-1;if(!(in>>key)||(in>>extra)||!QuickChatKey131(key,code))return false;
  for(auto& ch:key)ch=wchar_t(std::towupper(ch));out={unsigned(slot[0]-L'0'),key,true,code};return true;
 }
 std::wstring text;std::getline(in,text);const auto start=text.find_first_not_of(L" \t");
 if(name==L"#qchatclear"){if(start!=std::wstring::npos)return false;text.clear();}
 else {if(start==std::wstring::npos)return false;text.erase(0,start);if(QuickChatText(text).empty())return false;}
 out={unsigned(slot[0]-L'0'),text};return true;
}
template<class Save,class Reload> bool ApplyQuickChatSetting131(const std::wstring& name,const std::wstring& args,Save save,Reload reload){
 QuickChatSetting131 value;if(!ParseQuickChatSetting131(name,args,value))return false;
 // Profile APIs remove one outer pair of quotes; protect message quotes/spaces.
 const std::wstring stored=value.binding?std::to_wstring(value.code):value.text.empty()?L"":L"\""+value.text+L"\"";
 if(!save(L"Quick Chat",((value.binding?L"QuickChatHotkey":L"QuickChatText")+std::to_wstring(value.slot)).c_str(),stored))return false;
 reload();return true;
}
}
