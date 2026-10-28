#pragma once
#include "common.hpp"
#include <sstream>
namespace wht {
struct ModelPath131 {
 std::wstring path;
 void Reset(){path.clear();}
 bool Command(const std::wstring& name,const std::wstring& args,std::string& report){
  if(name!=L"#emodel"&&name!=L"#smodel")return false;
  const bool edit=name==L"#emodel";const std::string title=edit?"Edit Model":"Set Model";
  for(auto c:args)if((c<32&&c!=L'\t')||c==L'|'||c==127){report="Usage: "+Utf8(name)+" [model path without spaces]";return false;}
  std::wistringstream in(args);std::wstring value,extra;in>>value;
  if(value.size()>259||(in>>extra)){report="Usage: "+Utf8(name)+" [model path without spaces, max259 characters]";return false;}
  path=value;report=title+": "+(path.empty()?std::string("(cleared)"):Utf8(path))+" (path stored only)";return true;
 }
};
}
