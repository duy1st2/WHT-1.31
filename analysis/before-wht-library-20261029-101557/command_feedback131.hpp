#pragma once
#include "quick_chat_settings131.hpp"
#include "common.hpp"
#include "command_aliases131.hpp"
#include "tool_menu.hpp"
#include "notification131.hpp"
#include "view_commands131.hpp"
namespace wht {
inline std::string CommandFeedback131(const std::wstring& inputName,const std::wstring& args,bool success,const Config& config,bool menuOpen,bool settings,bool full,bool lock,bool autoPick,bool autoLeft,bool autoRight){
 const auto name=CanonicalCommand131(inputName);
 // These handlers already publish their own result; never announce a second success.
 if(name==L"#sellall"||name==L"#sellslot"||name==L"#islot"||name==L"#goku"||name==L"#autopickgoku"||name==L"#setga"||name==L"#gabase"||name==L"#backga"||name==L"#backsoi"||name.rfind(L"#chicken",0)==0||name.rfind(L"#wolf",0)==0)return {};
 for(const auto* handled:{L"#csave",L"#n",L"#name",L"#item",L"#copyitem",L"#create",L"#copy",L"#find",L"#hotkeycheck",L"#ping",L"#kick",L"#ban",L"#openbl",L"#upos",L"#cname",L"#repeat",L"#shield",L"#ice",L"#wild",L"#frozen",L"#phoenix",L"#whp",L"#hhp",L"#autoheal",L"#healhp",L"#healitem",L"#automana",L"#healmp",L"#manaitem",L"#autoskilldelay",L"#autoskillrange",L"#autoskillswap",L"#autoskilllist",L"#autoskillclear",L"#autoskills",L"#ga",L"#soi",L"#courier",L"#chuyendo"})if(name==handled)return {};
 if(name==L"#stopga"||name==L"#stopmth")return {};
 if(!success)return {};
 if(name==L"#cs")return "|cFFff527dClear Set Id Target|r";
 if(name==L"#allowlocal"||name==L"#alf"||name==L"#localfiles")return LegacySettingNotice131(L"Allow Local Files",config.allowLocalFiles?L"ON":L"OFF")+"; restart Warcraft to apply.";
 if(name==L"#chp"||name==L"#chpe"){wchar_t text[16]{};swprintf_s(text,L"0x%08X",name==L"#chpe"?config.hpBarColorEnemy:config.hpBarColor);return LegacySettingNotice131(name==L"#chpe"?L"HP Bar Color Enemy":L"HP Bar Color",text);}
 if(name==L"#externalhud")return LegacySettingNotice131(L"External HUD",config.externalHud?L"ON":L"OFF");
 if(name==L"#workercores")return LegacySettingNotice131(L"Worker Core Preference",config.workerCorePreference?L"ON":L"OFF");
 if(name==L"#quickchat")return LegacySettingNotice131(L"Quick Chat",config.quickChatEnabled?L"ON":L"OFF");
 if(name==L"#mth49"||name==L"#g17")return std::string(name==L"#mth49"?"[MTH] MTH49: ":"[MTH] G17: ") + ((name==L"#mth49"?config.mth49Enable:config.g17Enable)?"ON":"OFF");
 if(name==L"#mc"||name==L"#modecast")return config.modeCast==1?"[WHT] Set Quick Cast":"[WHT] Set Normal";
 if(name==L"#mthnoti")return config.mthNoti?"[MTH] Noti: ON":"[MTH] Noti: OFF";
 if(name==L"#hn")return LegacySettingNotice131(L"Show Noti Kick/Ban",config.showNotiKickBan?L"ON":L"OFF");
 if(name==L"#showmem")return LegacySettingNotice131(L"Show Mem",config.showMem?L"ON":L"OFF");
 if(name==L"#vsync")return LegacySettingNotice131(L"Vsync",config.vsync?L"ON":L"OFF");
 if(name==L"#tools")return menuOpen?"Tools Menu: ON":"Tools Menu: OFF";
 if(name==L"#reloadconfig")return "Configuration reloaded";
 if(name==L"#pcam"||name==L"#pzoom")return "Camera: "+std::to_string(int(config.camera));
 if(name==L"#kscroll"||name==L"#mscroll")return LegacySettingNotice131(name==L"#kscroll"?L"Keyboard Scroll Speed":L"Mouse Scroll Speed",std::to_wstring(name==L"#kscroll"?config.keyboardScroll:config.mouseScroll));
 if(name==L"#toolui"){
  std::wistringstream in(args);size_t index=0;if(!(in>>index)||index>=kToolMenuControls.size())return {};const auto& c=kToolMenuControls[index];auto title=EnglishControlLabel(c);if(std::wstring(c.section)==L"AutoHeal")title=std::wstring(c.key)==L"Percent"?"Use below HP (%)":"Auto Heal";if(std::wstring(c.section)==L"AutoMana")title=std::wstring(c.key)==L"Percent"?"Use below mana (%)":"Auto Mana";if(std::wstring(c.key)==L"Hide Option Menu")title=c.label;if(title.empty())title=Utf8(c.key);const auto value=ToolMenuOverlay::ReadValue(config,index);
  if(c.kind==MenuControlKind::Check)return LegacySettingNotice131(Wide(title),value==L"1"?L"ON":L"OFF");
  if(c.kind==MenuControlKind::Clear)return title;
  return LegacySettingNotice131(Wide(title),c.kind==MenuControlKind::Key?Wide(MenuKeyName(value)):value);
 }
 if(name==L"#macro")return "Macro: "+Utf8(args.substr(args.find_first_not_of(L" \t")==std::wstring::npos?args.size():args.find_first_not_of(L" \t")));
 if(name==L"#clickdelay")return "Auto Click Interval: "+std::to_string(config.autoClickInterval)+" ms";
 if(name==L"#allskills"||name==L"#skills")return "All Skills";
 struct ReferenceToggle{const wchar_t* command;const char* title;bool value;};
 const ReferenceToggle reference[]={
 {L"#fogclick","Fog Click",config.fogClick},{L"#fc","Fog Click",config.fogClick},
 {L"#timer","Show Timer",config.showTimer},{L"#showtimer","Show Timer",config.showTimer},
 {L"#res","Show Resource",config.showResource},{L"#resource","Show Resource",config.showResource},
 {L"#mousetooltip","MouseTooltipUI",config.mouseTooltip},{L"#nfe","Remove Fog Effect",config.removeFogEffect},
 {L"#colorplayer","Color Player",config.colorPlayer},{L"#colorp","Color Player",config.colorPlayer},
 {L"#barinvul","BarInvul",config.hideInvulnerableBars},{L"#vertexcolor","VertexColor",config.vertexColor},
 {L"#regen","ShowRegenHP",config.showRegenHP},{L"#showregen","ShowRegenHP",config.showRegenHP},

 {L"#fastmenu","Fast Menu",config.fastMenu},{L"#unlockfps","Unlock Limit FPS",config.unlockFps},
 {L"#showhotkey","Show Hotkey In Panel",config.showHotkeyInPanel},{L"#showlevel","ShowLevelHero",config.stateTagShowLevel},
 {L"#blackbar","BlackBarUI",config.hideBlackBar},{L"#hpvalue","ShowHpValue",config.stateTagHpValue},
 {L"#manavalue","ShowManaValue",config.stateTagManaValue},{L"#statstooltip","Detailed Stats Tooltip",config.statsTooltip},
 {L"#statscommas","Text Stats Commas",config.statsCommas},{L"#hpmpcommas","Text Life/Mana Commas",config.lifeManaCommas},
 {L"#hpmpnohide","Text Life/Mana No Hide",config.lifeManaNoHide},{L"#autoclickl","Auto Click Left",autoLeft},{L"#autoclickr","Auto Click Right",autoRight}};
 for(const auto& item:reference)if(name==item.command){
  if(name==L"#fc"||name==L"#fogclick"||name==L"#autoclickl"||name==L"#autoclickr")return std::string(item.title)+": "+(item.value?"ON":"OFF");
  if(name==L"#unlockfps")return std::string("[WHT] Unlock Limit FPS: ")+(item.value?"ON":"OFF (native FPS limit restored)")+(item.value&&config.vsync?" (inactive while VSync is ON)":"");
  return LegacySettingNotice131(Wide(item.title),item.value?L"ON":L"OFF");
 }
 if(const auto* key=ClearDelayKey131(name))return LegacySettingNotice131(key,std::to_wstring(name==L"#cleartextdelay"?config.clearTextSeconds:config.clearChatSeconds)+L" s");
 if(QuickChatSettingCommand131(name)){QuickChatSetting131 value;if(!ParseQuickChatSetting131(name,args,value))return {};if(value.binding)return "[WHT] Quick Chat "+std::to_string(value.slot)+(value.code<0?": key cleared":": key "+Utf8(value.text));return "[WHT] Quick Chat "+std::to_string(value.slot)+(name==L"#qchatclear"?": cleared":": saved");}
 if(const auto* option=ViewCommandFor131(name)){for(const auto& c:kToolMenuControls)if(std::wstring(c.section)==option->section&&std::wstring(c.key)==option->key){auto title=EnglishControlLabel(c);if(title.empty())title=Utf8(c.key);return LegacySettingNotice131(option->key,config.*option->field?L"ON":L"OFF");}}
 struct Toggle{const wchar_t* name;const char* title;bool value;};
 const Toggle toggles[]={{L"#menu","Tools Menu",menuOpen},{L"#tool","Tools Menu",menuOpen},{L"#commandsettings","Command Settings",settings},{L"#full","Borderless",full},{L"#lock","Lock Cursor",lock},{L"#showmem","Show Memory",config.showMem},{L"#autopick","Auto Pick",autoPick},{L"#autol","Auto Click Left",autoLeft},{L"#autor","Auto Click Right",autoRight},{L"#cooldown","ShowCooldown",config.showCooldown},{L"#regenhp","ShowRegenHP",config.showRegenHP},{L"#regenmp","ShowRegenMana",config.showRegenMana},{L"#reveal","Reveal Invisibles",config.revealInvisible},{L"#tanghinh","Reveal Invisibles",config.revealInvisible},{L"#invis","Reveal Invisibles",config.revealInvisible},{L"#revealinvis","Reveal Invisibles",config.revealInvisible}};
 for(const auto& t:toggles)if(name==t.name){
  if(name==L"#full"||name==L"#lock"||name==L"#cooldown"||name==L"#regenhp"||name==L"#regenmp"||name==L"#showmem")return LegacySettingNotice131(Wide(t.title),t.value?L"ON":L"OFF");
  return std::string(t.title)+": "+(t.value?"ON":"OFF");
 }
 if(name==L"#cam"||name==L"#zoom"||name==L"#fogz")return LegacySettingNotice131(name==L"#fogz"?L"FogZ":L"Camera",args.substr(args.find_first_not_of(L" \t")));
 if(name==L"#sellall"||name==L"#sellslot")return "Sell All: finding a nearby shop";
 if(name==L"#drop"||name==L"#dropall")return "Drop All: requested";
 std::string label=Utf8(name);if(!label.empty()&&label[0]=='#')label.erase(0,1);return label+": "+(args.find_first_not_of(L" \t")==std::wstring::npos?"OK":Utf8(args.substr(args.find_first_not_of(L" \t"))));
}
}
