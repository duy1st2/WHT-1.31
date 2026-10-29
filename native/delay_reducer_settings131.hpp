#pragma once
#include "common.hpp"
#include <cwchar>
namespace wht {
inline bool ParseDelayReducer131(const std::wstring& args,unsigned& value){if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;const auto first=args.find_first_not_of(L" \t"),last=args.find_last_not_of(L" \t");if(first==args.npos)return false;const auto text=args.substr(first,last-first+1);unsigned n=0;for(auto c:text){if(c<L'0'||c>L'9')return false;n=n*10+unsigned(c-L'0');if(n>1000)return false;}if(!n)return false;value=n;return true;}
inline unsigned DelayReducerMenuScope131(const wchar_t* section,const wchar_t* key){if(!section||!key||wcscmp(section,L"Event"))return 0;return !wcscmp(key,L"Local Game Delay Reducer")?1:!wcscmp(key,L"Net Game Delay Reducer")?2:0;}
inline std::wstring DelayReducerMenuValue131(const Config& c,unsigned scope){if(scope!=1&&scope!=2)return {};return !(c.delayReducerScopes&scope)?L"100":std::to_wstring(scope==1?c.localDelay:c.netDelay);}
}
