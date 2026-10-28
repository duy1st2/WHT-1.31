#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
#include <string>
#include <vector>
#pragma comment(lib,"Normaliz.lib")
namespace wht {
inline std::wstring MenuSearchFold131(const std::wstring& text){
 if(text.empty())return {};
 const int size=NormalizeString(NormalizationD,text.data(),int(text.size()),nullptr,0);
 if(size<=0)return {};
 std::wstring decomposed(size,L'\0');
 int written=NormalizeString(NormalizationD,text.data(),int(text.size()),decomposed.data(),size);
 // The size-only call is an estimate; Windows returns the required size negated.
 for(unsigned retry=0;written<0&&retry<3;++retry){const auto needed=-int64_t(written);if(needed>8192)return {};decomposed.resize(size_t(needed));written=NormalizeString(NormalizationD,text.data(),int(text.size()),decomposed.data(),int(decomposed.size()));}
 if(written<=0)return {};decomposed.resize(written);
 std::wstring result;
 for(auto ch:decomposed){if(ch>=0x300&&ch<=0x36f)continue;if(ch==0x110||ch==0x111)ch=L'd';if(ch>=L'A'&&ch<=L'Z')ch+=L'a'-L'A';
  if(ch==L' '||ch==L'\t'){if(!result.empty()&&result.back()!=L' ')result+=L' ';}else result+=ch;
 }
 while(!result.empty()&&result.back()==L' ')result.pop_back();return result;
}
inline bool MenuSearchQuery131(const std::wstring& args,std::wstring& folded){
 if(args.empty()||args.size()>128)return false;
 for(auto ch:args)if(ch<32&&ch!=L'\t')return false;
 // Reject lone UTF-16 surrogates through the existing text validator.
 auto plain=args;for(auto& ch:plain)if(ch==L'\t')ch=L' ';
 if(QuickChatText(plain).empty())return false;
 auto value=MenuSearchFold131(args);if(value.empty())return false;folded=std::move(value);return true;
}
struct MenuSearchResult131 {size_t index;int page;std::string label;};
template<class Available> std::vector<MenuSearchResult131> FindMenuControls131(const std::wstring& query,Available available){
 std::vector<MenuSearchResult131> hits;
 for(size_t i=0;i<kToolMenuControls.size();++i){const auto& d=kToolMenuControls[i];if(d.kind==MenuControlKind::Label||!d.supported||!available(d))continue;
  const auto label=std::string(d.label),english=EnglishControlLabel(d);
  const auto contains=[&](const std::wstring& value){return MenuSearchFold131(value).find(query)!=std::wstring::npos;};
  if(contains(Wide(label))||contains(Wide(english))||contains(d.section)||contains(d.key)){auto title=toolEnglish.load()?english:label;if(title.empty())title=english.empty()?Utf8(d.key):english;hits.push_back({i,d.page,title});}
 }
 return hits;
}
struct MenuSearch131 {
 std::wstring query;size_t cursor=0;
 void Reset(){query.clear();cursor=0;}
 template<class Available> bool Run(const std::wstring& args,ToolMenuOverlay& menu,Available available,std::string& notice){
  std::wstring folded;if(!MenuSearchQuery131(args,folded))return false;
  const auto hits=FindMenuControls131(folded,available);
  if(hits.empty()){notice="[WHT] Find: no matching menu option.";Reset();return true;}
  if(query==folded)cursor=(cursor+1)%hits.size();else{query=folded;cursor=0;}
  if(cursor>=hits.size())cursor=0;const auto& hit=hits[cursor];
  menu.CancelEdit();menu.open=true;menu.page=hit.page;menu.nextRefresh=0;
  notice="[WHT] Find "+std::to_string(cursor+1)+"/"+std::to_string(hits.size())+": "+ToolTabName(size_t(hit.page))+" - "+hit.label;
  return true;
 }
};
}
