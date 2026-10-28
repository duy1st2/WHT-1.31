#pragma once
#include "common.hpp"
#include <sstream>
#include <cstdio>
namespace wht {
struct HotkeyUse131 {unsigned key,scope;std::string name;};
struct HotkeyConflict131 {unsigned key;std::string first,second;};
inline std::vector<HotkeyUse131> ConfiguredHotkeys131(const Config& c){
 std::vector<HotkeyUse131> uses;
 auto add=[&](unsigned key,const std::string& name,unsigned scope=3){if(key&&key<256)uses.push_back({key,scope,name});};
 add(c.menuKey,"Tools menu");add(c.commandKey,"Command settings");
 add(c.multiboardKey,"Multiboard");if(c.showResource)add(VK_TAB,"Resources");
 for(size_t i=0;i<c.heroes.size();++i)add(c.heroes[i],"Hero "+std::to_string(i+1));
 add(c.setIdTargetKey,"Pinned target");add(c.sellFastKey,"Sell item");
 add(c.autoClickLeftKey,"Auto click left");add(c.autoClickRightKey,"Auto click right");
 if(c.macroEnabled){const char* labels[]={"Macro play","Macro stop","Macro record","Macro edit"};for(size_t i=0;i<4;++i)add(c.macroKeys[i],labels[i]);}
 if(c.autoSkillsEnabled)add(c.autoSkillsKey,"Auto skills");
 if(c.quickChatEnabled)for(size_t i=0;i<c.quickChat.size();++i)if(!c.quickChat[i].text.empty())add(c.quickChat[i].vk,"Quick Chat "+std::to_string(i+1));
 if(c.inventoryEnabled)for(size_t i=0;i<c.inventory.size();++i)if(c.inventory[i].enabled)add(c.inventory[i].vk,"Inventory "+std::to_string(i+1));
 // Normal command and alternate/shop grids are mutually exclusive contexts.
 if(c.commandEnabled)for(size_t i=0;i<c.command.size();++i)if(c.command[i].enabled)add(c.command[i].vk,"Skill "+std::to_string(i+1),1);
 if(c.shopEnabled)for(size_t i=0;i<c.shop.size();++i)if(c.shop[i].enabled)add(c.shop[i].vk,"Shop "+std::to_string(i+1),2);
 if(c.mth49Enable||c.g17Enable){
  const unsigned keys[]={c.shieldHotkey,c.iceHotkey,c.wildHotkey,c.frozenHotkey,c.phoenixHotkey,c.pickGokuHotkey,c.chayGa1Hotkey,c.chayGa2Hotkey,c.chayGa3Hotkey,c.chaySoi1Hotkey,c.chaySoi2Hotkey,c.chaySoi3Hotkey};
  const char* labels[]={"Shield","Ice","Wild","Frozen","Phoenix","Goku","Chicken route 1","Chicken route 2","Chicken route 3","Wolf route 1","Wolf route 2","Wolf route 3"};
  for(size_t i=0;i<12;++i)add(keys[i],labels[i]);
  if(c.autoCourierEnable){add(c.courierHotkey,"Courier delivery");add(c.courierBackHotkey,"Courier return");}
 }
 return uses;
}
inline std::vector<HotkeyConflict131> HotkeyConflicts131(const Config& c){
 const auto uses=ConfiguredHotkeys131(c);std::vector<HotkeyConflict131> conflicts;
 for(size_t i=0;i<uses.size();++i)for(size_t j=i+1;j<uses.size();++j)if(uses[i].key==uses[j].key&&(uses[i].scope&uses[j].scope))conflicts.push_back({uses[i].key,uses[i].name,uses[j].name});
 return conflicts;
}
inline std::string DiagnosticKey131(unsigned key){
 if((key>='A'&&key<='Z')||(key>='0'&&key<='9'))return std::string(1,char(key));
 if(key>=VK_F1&&key<=VK_F12)return "F"+std::to_string(key-VK_F1+1);
 if(key>=VK_NUMPAD0&&key<=VK_NUMPAD9)return "NUMPAD"+std::to_string(key-VK_NUMPAD0);
 if(key==VK_TAB)return "TAB";if(key==VK_SPACE)return "SPACE";
 char text[24]{};std::snprintf(text,sizeof(text),"VK 0x%02X",key);return text;
}
inline bool HotkeyCheckText131(const Config& c,const std::wstring& args,std::string& output){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring token,extra;unsigned page=1;
 if(in>>token){if((in>>extra)||token.size()>4||token.find_first_not_of(L"0123456789")!=std::wstring::npos)return false;page=0;for(auto digit:token)page=page*10+unsigned(digit-L'0');}
 const auto conflicts=HotkeyConflicts131(c);const size_t pages=conflicts.empty()?1:(conflicts.size()+3)/4;
 if(!page||page>pages)return false;
 std::string text="[WHT] Hotkeys: ";
 if(conflicts.empty())text+="no configured conflicts.";
 else {text+=std::to_string(conflicts.size())+" potential conflicts ("+std::to_string(page)+"/"+std::to_string(pages)+")";
  const size_t begin=(page-1)*4;for(size_t i=begin;i<conflicts.size()&&i<begin+4;++i){const auto& item=conflicts[i];text+="\n"+DiagnosticKey131(item.key)+": "+item.first+" / "+item.second;}
  if(page<pages)text+="\n#hotkeycheck "+std::to_string(page+1)+" - next page";
 }
 output=std::move(text);return true;
}
}
