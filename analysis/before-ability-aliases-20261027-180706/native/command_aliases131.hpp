#pragma once
#include "common.hpp"
namespace wht {
// Names only: dispatch and feedback retain the existing 1.31 feature checks.
inline std::wstring CanonicalCommand131(std::wstring name){
 FoldCommandName(name);
 const std::pair<const wchar_t*,const wchar_t*> aliases[]={
  {L"#showcooldown",L"#cooldown"},{L"#hotkeypanel",L"#showhotkey"},
  {L"#showresource",L"#res"},{L"#sell",L"#sellall"},{L"#quicksell",L"#sellall"}
 };
 for(const auto& alias:aliases)if(name==alias.first)return alias.second;
 return name;
}
}
