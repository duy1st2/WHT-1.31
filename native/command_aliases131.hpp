#pragma once
#include "common.hpp"
namespace wht {
// Names only: dispatch and feedback retain the existing 1.31 feature checks.
inline std::wstring CanonicalCommand131(std::wstring name){
 FoldCommandName(name);
 const std::pair<const wchar_t*,const wchar_t*> aliases[]={
  {L"#mem",L"#showmem"},{L"#memory",L"#showmem"},
  {L"#ram",L"#showmem"},{L"#usemem",L"#showmem"},
  {L"#trade",L"#res"},{L"#showregen",L"#regenhp"},{L"#regen",L"#regenhp"},
  {L"#showcooldown",L"#cooldown"},{L"#hotkeypanel",L"#showhotkey"},
  {L"#showresource",L"#res"},{L"#sell",L"#sellall"},{L"#quicksell",L"#sellall"},
  {L"#hideball",L"#hideb"},{L"#showball",L"#showb"}
 };
 for(const auto& alias:aliases)if(name==alias.first)return alias.second;
 return name;
}
}
