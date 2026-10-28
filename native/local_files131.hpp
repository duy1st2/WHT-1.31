#pragma once
#include <algorithm>
#include "view_commands131.hpp"
namespace wht {
inline bool LocalFilesCommand131(const std::wstring& name){return name==L"#allowlocal"||name==L"#alf"||name==L"#localfiles";}
inline bool WriteLocalFiles131(bool enabled){
 HKEY key=nullptr;
 auto result=RegCreateKeyExW(HKEY_CURRENT_USER,L"Software\\Blizzard Entertainment\\Warcraft III",0,nullptr,0,KEY_SET_VALUE,nullptr,&key,nullptr);
 if(result!=ERROR_SUCCESS)return false;
 const DWORD value=enabled?1:0;
 result=RegSetValueExW(key,L"Allow Local Files",0,REG_DWORD,reinterpret_cast<const BYTE*>(&value),sizeof(value));
 RegCloseKey(key);return result==ERROR_SUCCESS;
}
// Match 1.29: change the Windows preference only on an explicit command.
// Tests inject a writer; they never change the user's registry.
template<class Save,class Writer,class Reload>
bool ApplyLocalFiles131(const std::wstring& args,bool previous,Save save,Writer writer,Reload reload){
 bool enabled=false;if(!ViewToggle131(args,previous,enabled))return false;
 if(!save(L"Event",L"Allow Local Files",enabled?L"1":L"0"))return false;
 if(!writer(enabled)){save(L"Event",L"Allow Local Files",previous?L"1":L"0");return false;}
 reload();return true;
}
}
