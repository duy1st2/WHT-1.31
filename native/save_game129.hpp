#pragma once
#include "hero_skill_points129.hpp"
#include <sstream>
namespace wht {
inline bool ParseSaveName129(const std::wstring& args,std::string& name){
 std::wistringstream in(args);std::wstring token,extra;name.clear();if(!(in>>token))token=L"wht-save";
 if(in>>extra||token.size()>64)return false;
 for(auto c:token){if(!((c>=L'a'&&c<=L'z')||(c>=L'A'&&c<=L'Z')||(c>=L'0'&&c<=L'9')||c==L'_'||c==L'-'))return false;name.push_back(char(c));}
 return !name.empty();
}
using SaveGameFn129=void(__cdecl*)(void*);
inline SaveGameFn129 ResolveSaveGame129(uintptr_t base){
 if(!base)return nullptr;
 __try{const unsigned char sig[]={0x55,0x8b,0xec,0x81,0xec,0x08,0x01,0x00,0x00};
 if(memcmp((void*)(base+0xa5c50),sig,sizeof(sig))||*reinterpret_cast<unsigned char*>(base+0xa5c83)!=0xe8||base+0xa5c88+*reinterpret_cast<int32_t*>(base+0xa5c84)!=base+0x69010)return nullptr;
 return reinterpret_cast<SaveGameFn129>(base+0xa5c50);
 }__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
inline bool RequestSaveGame129(uintptr_t base,const char* name,SaveGameFn129 native,const char** error=nullptr){
 if(error)*error=nullptr;
 __try{
 if(!base||!native||!name||!*name)return HeroSkillPointsFailure129(error,"Save function unavailable");
 auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(!world)return HeroSkillPointsFailure129(error,"Match unavailable");
 if(*reinterpret_cast<uint32_t*>(world+0x6d4))return HeroSkillPointsFailure129(error,"Save already pending");
 // SaveGame uses String::GetCString: object+8 -> storage+0x1c.
 uintptr_t storage[8]{};storage[7]=reinterpret_cast<uintptr_t>(name);
 uintptr_t object[3]{};object[2]=reinterpret_cast<uintptr_t>(storage);
 native(object);
 if(!*reinterpret_cast<uint32_t*>(world+0x6d4))return HeroSkillPointsFailure129(error,"Game did not queue the save");
 return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return HeroSkillPointsFailure129(error,"Save request failed");}
}
}
