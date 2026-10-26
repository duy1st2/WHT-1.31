#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
#include "notification131.hpp"
#include "view_commands131.hpp"
namespace wht {
inline std::string CommandFeedback131(const std::wstring& name,const std::wstring& args,bool success,const Config& config,bool menuOpen,bool settings,bool full,bool lock,bool autoPick,bool autoLeft,bool autoRight){
 // These handlers already publish their own result; never announce a second success.
 if(name==L"#sellall"||name==L"#sellslot"||name==L"#islot"||name==L"#goku"||name==L"#autopickgoku"||name==L"#setga"||name==L"#gabase"||name==L"#backga"||name==L"#backsoi"||name.rfind(L"#chicken",0)==0||name.rfind(L"#wolf",0)==0)return {};
 if(name==L"#stopga"||name==L"#stopmth")return {};
 if(!success)return {};
 if(name==L"#mth49"||name==L"#g17")return std::string(name==L"#mth49"?"MTH49: ":"G17: ") + ((name==L"#mth49"?config.mth49Enable:config.g17Enable)?"ON":"OFF");
 if(name==L"#mc"||name==L"#modecast")return config.modeCast==1?"Casting Mode: Quick Cast":"Casting Mode: Normal";
 if(name==L"#reloadconfig")return "Configuration reloaded";
 if(name==L"#pcam"||name==L"#pzoom")return Utf8(SettingFeedback(L"Camera",std::to_wstring(int(config.camera))));
 if(name==L"#kscroll"||name==L"#mscroll")return Utf8(SettingFeedback(name==L"#kscroll"?L"Keyboard Scroll":L"Mouse Scroll",std::to_wstring(name==L"#kscroll"?config.keyboardScroll:config.mouseScroll)+L"%"));
 if(name==L"#toolui"){
  std::wistringstream in(args);size_t index=0;if(!(in>>index)||index>=kToolMenuControls.size())return {};const auto& c=kToolMenuControls[index];auto title=EnglishControlLabel(c);if(title.empty())title=Utf8(c.key);const auto value=ToolMenuOverlay::ReadValue(config,index);
  if(c.kind==MenuControlKind::Check)return title+": "+(value==L"1"?"ON":"OFF");
  if(c.kind==MenuControlKind::Clear)return title;
  return Utf8(SettingFeedback(Wide(title),c.kind==MenuControlKind::Key?Wide(MenuKeyName(value)):value));
 }
 if(const auto* option=ViewCommandFor131(name)){for(const auto& c:kToolMenuControls)if(std::wstring(c.section)==option->section&&std::wstring(c.key)==option->key){auto title=EnglishControlLabel(c);if(title.empty())title=Utf8(c.key);return title+": "+(config.*option->field?"ON":"OFF");}}
 struct Toggle{const wchar_t* name;const char* title;bool value;};
 const Toggle toggles[]={{L"#menu","Tools Menu",menuOpen},{L"#tool","Tools Menu",menuOpen},{L"#commandsettings","Command Settings",settings},{L"#full","Borderless",full},{L"#lock","Lock Cursor",lock},{L"#showmem","Show Memory",config.showMem},{L"#autopick","Auto Pick",autoPick},{L"#autol","Auto Click Left",autoLeft},{L"#autor","Auto Click Right",autoRight},{L"#cooldown","Show Cooldown",config.showCooldown},{L"#regenhp","HP Regen",config.showRegenHP},{L"#regenmp","Mana Regen",config.showRegenMana},{L"#reveal","Reveal Invisibles",config.revealInvisible},{L"#tanghinh","Reveal Invisibles",config.revealInvisible},{L"#invis","Reveal Invisibles",config.revealInvisible},{L"#revealinvis","Reveal Invisibles",config.revealInvisible}};
 for(const auto& t:toggles)if(name==t.name)return std::string(t.title)+": "+(t.value?"ON":"OFF");
 if(name==L"#cam"||name==L"#zoom"||name==L"#fogz")return Utf8(SettingFeedback(name==L"#fogz"?L"FogZ":L"Camera",args.substr(args.find_first_not_of(L" \t"))));
 if(name==L"#sellall"||name==L"#sellslot")return "Sell All: finding a nearby shop";
 if(name==L"#drop"||name==L"#dropall")return "Drop All: requested";
 std::string label=Utf8(name);if(!label.empty()&&label[0]=='#')label.erase(0,1);return label+": OK";
}
}
