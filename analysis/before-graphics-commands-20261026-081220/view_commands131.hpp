#pragma once
#include "common.hpp"
#include <sstream>
namespace wht {
// Only already ported display/settings features; no gameplay dispatch here.
struct ViewCommand131 {const wchar_t* command;const wchar_t* section;const wchar_t* key;bool Config::* field;};
inline constexpr ViewCommand131 viewCommands131[]={
 {L"#mthnoti",L"MTHKEY",L"Noti",&Config::mthNoti},
 {L"#colorplayer",L"Graphics",L"Color Player",&Config::colorPlayer},
 {L"#colorp",L"Graphics",L"Color Player",&Config::colorPlayer},
 {L"#barinvul",L"Graphics",L"BarInvul",&Config::hideInvulnerableBars},
 {L"#statetag",L"StateTag",L"Enable",&Config::stateTagEnable},
 {L"#hpmpvalue",L"StateTag",L"Enable",&Config::stateTagEnable},
 {L"#showmax",L"StateTag",L"ShowMaxValues",&Config::stateTagShowMax},
 {L"#hidemenu",L"Event",L"Hide Option Menu",&Config::hideOptionMenu},
 {L"#quickdrop",L"Event",L"Quick Drop Item",&Config::quickDropItem},
 {L"#timer",L"Event",L"Show Timer",&Config::showTimer},
 {L"#showtimer",L"Event",L"Show Timer",&Config::showTimer},
 {L"#res",L"Event",L"Show Resource",&Config::showResource},
 {L"#resource",L"Event",L"Show Resource",&Config::showResource},
 {L"#showhotkey",L"Event",L"Show Hotkey In Panel",&Config::showHotkeyInPanel},
 {L"#statstooltip",L"Graphics",L"Detailed Stats Tooltip",&Config::statsTooltip},
 {L"#statscommas",L"Graphics",L"Text Stats Commas",&Config::statsCommas},
 {L"#hpmpcommas",L"Graphics",L"Text Life/Mana Commas",&Config::lifeManaCommas},
 {L"#hpmpnohide",L"Graphics",L"Text Life/Mana No Hide",&Config::lifeManaNoHide},
 {L"#sound",L"Event",L"Sound Start Game",&Config::soundStartGame},
 {L"#showdropall",L"Event",L"Show Drop All",&Config::showDropAll},
 {L"#showautopick",L"Event",L"Show Auto Pick",&Config::showAutoPick},
 {L"#showsellall",L"Event",L"Show Sell All",&Config::showSellAll},
 {L"#console",L"Graphics",L"ConsoleUI",&Config::hideConsoleUI},
 {L"#consoleui",L"Graphics",L"ConsoleUI",&Config::hideConsoleUI},
 {L"#showskills",L"Event",L"Show Ins All Skills",&Config::showInsAllSkills},
 {L"#showlevel",L"StateTag",L"ShowLevelHero",&Config::stateTagShowLevel},
 {L"#blackbar",L"Graphics",L"BlackBarUI",&Config::hideBlackBar},
 {L"#mousetooltip",L"Graphics",L"MouseTooltipUI",&Config::mouseTooltip},
 {L"#hpvalue",L"StateTag",L"ShowHpValue",&Config::stateTagHpValue},
 {L"#manavalue",L"StateTag",L"ShowManaValue",&Config::stateTagManaValue}
};
inline const ViewCommand131* ViewCommandFor131(const std::wstring& name){for(const auto& c:viewCommands131)if(name==c.command)return &c;return nullptr;}
inline bool ViewToggle131(const std::wstring& args,bool current,bool& next){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;bool result=!current;
 if(in>>value){if((value!=L"0"&&value!=L"1")||(in>>extra))return false;result=value==L"1";}
 next=result;return true;
}
template<class Save,class Reload> bool ApplyViewCommand131(const ViewCommand131& target,const std::wstring& args,const Config& config,Save save,Reload reload){
 bool next=false;if(!ViewToggle131(args,config.*target.field,next))return false;
 if(!save(target.section,target.key,next?L"1":L"0"))return false;
 reload();return true;
}
// WHT-1.29 preset-camera policy: default 2400, clamp to 500..6000.
inline bool PresetCameraValue131(const std::wstring& args,float& value){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);float candidate=2400.f;std::wstring extra;
 in>>std::ws;if(!in.eof()&&(!(in>>candidate)||(in>>extra)||!std::isfinite(candidate)))return false;
 value=std::clamp(candidate,500.f,6000.f);return true;
}
template<class Save,class Reload> bool ApplyPresetCamera131(const std::wstring& args,Save save,Reload reload){
 float value=0;if(!PresetCameraValue131(args,value)||!save(L"Event",L"Camera",std::to_wstring(value)))return false;
 reload();return true;
}

template<class Reload> bool ReloadConfigCommand131(const std::wstring& args,Reload reload){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_not_of(L" \t")!=std::wstring::npos)return false;
 reload();return true;
}

inline const wchar_t* MthModeControl131(const std::wstring& section,const std::wstring& key){
 if(section!=L"MTHKEY")return nullptr;
 if(key==L"MTH49Enable")return L"#mth49";
 if(key==L"G17Enable")return L"#g17";
 return nullptr;
}
template<class Save,class Reload> bool ApplyMthMode131(const std::wstring& name,const std::wstring& args,const Config& config,Save save,Reload reload){
 if(name!=L"#mth49"&&name!=L"#g17")return false;
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;bool next=!(name==L"#mth49"?config.mth49Enable:config.g17Enable);
 if(in>>value){if(in>>extra)return false;if(value==L"1"||value==L"on"||value==L"true")next=true;else if(value==L"0"||value==L"off"||value==L"false")next=false;else return false;}
 // Reference save order: disable the other mode first, then save requested mode.
 if(next&&!save(L"MTHKEY",name==L"#mth49"?L"G17Enable":L"MTH49Enable",L"0"))return false;
 if(!save(L"MTHKEY",name==L"#mth49"?L"MTH49Enable":L"G17Enable",next?L"1":L"0"))return false;
 reload();return true;
}

template<class Save,class Reload> bool ApplyCastMode131(const std::wstring& args,const Config& config,Save save,Reload reload){
 bool quick=false;if(!ViewToggle131(args,config.modeCast==1,quick))return false;
 if(!save(L"Event",L"ModeCast",quick?L"1":L"0"))return false;
 reload();return true;
}

}
