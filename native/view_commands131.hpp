#pragma once
#include "common.hpp"
#include <sstream>
namespace wht {
// Only already ported display/settings features; no gameplay dispatch here.
struct ViewCommand131 {const wchar_t* command;const wchar_t* section;const wchar_t* key;bool Config::* field;};
inline constexpr ViewCommand131 viewCommands131[]={
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
}
