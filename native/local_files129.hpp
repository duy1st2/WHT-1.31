#pragma once
#include "common.hpp"
namespace wht {
inline bool WriteLocalFiles129(bool enabled){
 HKEY key=nullptr;
 auto result=RegCreateKeyExW(HKEY_CURRENT_USER,L"Software\\Blizzard Entertainment\\Warcraft III",0,nullptr,0,KEY_SET_VALUE,nullptr,&key,nullptr);
 if(result!=ERROR_SUCCESS)return false;
 const DWORD value=enabled?1:0;
 result=RegSetValueExW(key,L"Allow Local Files",0,REG_DWORD,reinterpret_cast<const BYTE*>(&value),sizeof(value));
 RegCloseKey(key);return result==ERROR_SUCCESS;
}
using LocalFilesWriter129=bool(*)(bool);
inline bool SetLocalFiles129(const std::wstring& ini,bool previous,bool enabled,LocalFilesWriter129 writer){
 if(!writer||!SaveSetting(ini,L"Event",L"Allow Local Files",enabled?L"1":L"0"))return false;
 if(writer(enabled))return true;
 SaveSetting(ini,L"Event",L"Allow Local Files",previous?L"1":L"0");return false;
}
}
