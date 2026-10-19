#include "time_boss131.hpp"
#include "repeat_message131.hpp"
#include "quick_chat131.hpp"
#include "ui_refresh131.hpp"
#include "graphics_diagnostic131.hpp"
#include "match_start131.hpp"
#include "life_mana_hook131.hpp"
#include "mouse_tooltip131.hpp"
#include "black_bar131.hpp"
#include "console_ui131.hpp"
#include "time_of_day131.hpp"
#include "multiboard131.hpp"
#include "hero_keys131.hpp"
#include "auto_click131.hpp"
#include "learn_slot131.hpp"
#include "target_cast131.hpp"
#include "self_cast131.hpp"
#include "quickcast131.hpp"
#include "slot_remap131.hpp"
#include "speed131.hpp"
#include "reveal131.hpp"
#include "sell_native131.hpp"
#include "auto_pick_native131.hpp"
#include "quick_drop_network131.hpp"
#include "all_skills131.hpp"
#include "drop_icon131.hpp"
#include "quick_drop131.hpp"
#include "profile64.hpp"
#include "clock_fps131.hpp"
#include "tool_menu.hpp"
#include "session_auth.hpp"
#include "gameplay64.hpp"
#include "hotkey_labels131.hpp"
#include "message_clear131.hpp"
#include "resource_board131.hpp"
#include "macro_recorder.hpp"
#include "memory_overlay.hpp"
#include <filesystem>
#include <sstream>
#include <unordered_map>
namespace {
HMODULE module64;uintptr_t base64=0,ui64=0,archive64=0;const wht::Profile64* profile64=nullptr;
HWND window64=nullptr;WNDPROC previous64=nullptr;std::wstring folder64,ini64;
wht::Config config64;wht::ToolMenuOverlay menu64;wht::ExtendedFrameApi api64;
std::atomic<bool> authorized64=false,ready64=false;bool menuLogged64=false;void* status64=nullptr;
constexpr UINT update64=WM_APP+0x164;ULONGLONG reloadAt64=0;float scale64=1;
struct Placement64{bool sized=false,absolute=false,relative=false;float width=0,x=0,y=0;int point=0,otherPoint=0,flag=1;void* other=nullptr;};
std::unordered_map<void*,Placement64> placements64;
wht::Gameplay64 gameplay64;wht::MacroRecorder macro64;bool settings64=true,typed64=false;std::wstring chat64;
wht::HotkeyLabels131 hotkeys64;
wht::SlotRemap131 remap64;
wht::QuickChatEcho131 quickChatEcho64;
wht::RepeatMessage131 repeatMessage64;
bool SendChat64(const std::string& text);
wht::TimeOfDay131 timeOfDay64;
wht::ConsoleUi131 consoleUi64;
wht::BlackBar131 blackBar64;
wht::MouseTooltip131 mouseTooltip64;
wht::AutoClick131 autoClick64;
wht::SelfCastSource131 pinned64;
wht::MessageTimers131 messageTimers64;
wht::ResourceBoard131 resources64;
wht::GraphicsDiagnostic131 graphicsDiagnostic64;
wht::MatchStart131 matchStart64;
wht::TimeBoss131 timeBoss64;
wht::ClockOverlay131 clock64;wht::FpsState131 fps64;
wht::QuickDropInput131 quickDrop64;
wht::Reveal131 reveal64;
wht::SellIcon131 sellIcon64;wht::SellAll131 sellAll64;
wht::AutoPickIcon131 pickIcon64;wht::AutoPick131 autoPick64;bool autoPickEnabled64=false;
wht::DropIcon131 dropIcon64;wht::DropAllState dropAllNetwork64;
wht::SkillsIcon131 skillsIcon64;wht::AllSkills131 allSkills64;
int SkillPoints64(uintptr_t unit){const auto* p=wht::SkillProfile(profile64->build);if(!p)return -1;float current=0,maximum=0,rate=0;
 return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f?wht::HeroPoints131(unit,base64,*p):-1;}
wht::SkillButton131 SkillData64(unsigned slot){return wht::SkillButtonData131(wht::Button64(ui64,base64,*profile64,slot));}
bool ReadPinned64(wht::SelfCastSource131& out){return wht::ReadCastTarget131(base64,profile64->build,ui64,pinned64.unit,[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},out)&&wht::SameSelfCastSource131(pinned64,out);}
bool PinTarget64(){pinned64={};return wht::ReadCastTarget131(base64,profile64->build,ui64,wht::Selected64(ui64,base64,*profile64),[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},pinned64);}
unsigned InventoryMask64(bool usableOnly=false){if(!profile64)return 0;const auto* p=wht::InventoryProfile(profile64->build);if(!p)return 0;return wht::InventoryMask131(base64,*p,wht::InventoryNative131(base64,*p),wht::Selected64(ui64,base64,*profile64),usableOnly);}
bool AlternatePanel64(){return wht::AlternatePanel131([&](unsigned slot){const auto data=SkillData64(slot);return data.order?data.order:data.ability;});}
bool ClickSkill64(unsigned slot){const auto* p=wht::SkillProfile(profile64->build);return p&&wht::SkillClick131(wht::Button64(ui64,base64,*profile64,slot),base64,*p);}
bool LearnSlot64(unsigned slot){return wht::LearnSlot131(slot,[&](wht::SelfCastSource131& out){return wht::ReadSelfCastSource131(base64,profile64->build,ui64,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},out);},SkillPoints64,SkillData64,ClickSkill64);}

uintptr_t camera64=0;std::array<float,2> cameraRequested64{},cameraWritten64{};unsigned cameraMask64=0;bool cameraLogged64=false;
bool Allowed64(){return authorized64&&settings64&&config64.mode==wht::RunMode::Local&&ui64;}
bool ReadCamera64(unsigned field,float& value){if(!wht::Object64(camera64,base64,profile64->cameraVtable))return false;__try{reinterpret_cast<void*(*)(uintptr_t,float*)>(base64+profile64->floatCurrent)(camera64+(field?0x1e8:0x1a8),&value);return std::isfinite(value)&&value>0&&value<1000000;}__except(1){return false;}}
void SetCamera64(unsigned field,float value){reinterpret_cast<void(*)(uintptr_t,unsigned,float,float,int)>(base64+profile64->cameraSet)(camera64,field,value,0,1);}
void Camera64(){if(!wht::Object64(camera64,base64,profile64->cameraVtable)){camera64=wht::Component64(ui64,base64,profile64->cameraVtable);cameraMask64=0;}if(!camera64)return;
 for(unsigned field=0;field<2;++field){float native=0;if(!ReadCamera64(field,native))continue;unsigned bit=1u<<field;
  if(!Allowed64()){if(cameraMask64&bit)SetCamera64(field,cameraRequested64[field]);cameraMask64&=~bit;continue;}
  if(!(cameraMask64&bit)||std::abs(native-cameraWritten64[field])>.01f)cameraRequested64[field]=native;
  const float desired=field?config64.fogZ:config64.camera;SetCamera64(field,desired);cameraWritten64[field]=desired;cameraMask64|=bit;
  if(!cameraLogged64){float observed=0;if(ReadCamera64(field,observed)&&std::abs(desired-observed)<.01f){cameraLogged64=true;wht::Log(folder64,"WHT x64: native camera setter/readback verified.");}}
 }}
void Width64(void* object,float value){auto& p=placements64[object];p.sized=true;p.width=value;reinterpret_cast<void(*)(void*,float)>(base64+profile64->width)(object,value*scale64);}
void Absolute64(void* object,int point,float x,float y,int flag){auto& p=placements64[object];p.absolute=true;p.relative=false;p.point=point;p.x=x;p.y=y;p.flag=flag;reinterpret_cast<void(*)(void*,int,float,float,int)>(base64+profile64->absolute)(object,point,.4f+(x-.4f)*scale64,y,flag);}
void Relative64(void* object,int point,void* other,int otherPoint,float x,float y,int flag){auto& p=placements64[object];p.absolute=false;p.relative=true;p.point=point;p.other=other;p.otherPoint=otherPoint;p.x=x;p.y=y;p.flag=flag;reinterpret_cast<void(*)(void*,int,void*,int,float,float,int)>(base64+profile64->relative)(object,point,other,otherPoint,x*scale64,y,flag);}
bool Identity64(uintptr_t object,uintptr_t table){__try{return wht::ComponentIdentity131(object,base64+table);}__except(1){return false;}}
uintptr_t CurrentUi64(){__try{auto ui=reinterpret_cast<uintptr_t(*)(bool,bool)>(base64+profile64->uiGet)(false,false);return Identity64(ui,profile64->uiVtable)?ui:0;}__except(1){return 0;}}
struct Span64{const char* data;size_t size;};
uintptr_t Mount64(const char* path){__try{return reinterpret_cast<uintptr_t(*)(Span64,unsigned,unsigned,unsigned,uintptr_t,bool)>(base64+profile64->archiveOpen)({path,strlen(path)},2,0,100,0,false);}__except(1){return 0;}}
uintptr_t Provider64(const char* path){__try{return reinterpret_cast<uintptr_t(*)(Span64)>(base64+profile64->archiveLookup)({path,strlen(path)});}__except(1){return 0;}}
bool Assets64(){if(archive64)return true;
 const auto path=wht::Utf8(folder64+L"\\WHT.mix");const auto value=Mount64(path.c_str());if(!value)return false;
 for(const char* name:{"fonts_VN.ttf","BTNWHT_Menu.blp","Transparent.tga","WHTUI_GreyBackground.tga"})if(Provider64(name)!=value)return false;
 archive64=value;wht::Log(folder64,"WHT x64: native MPQ mount and provider lookups verified.");
 for(const char* name:{"UI\\Widgets\\EscMenu\\Human\\human-options-button-border-up.blp","UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp"})
  wht::Log(folder64,std::string("WHT x64: backdrop provider ")+name+(Provider64(name)==value?" = WHT":" = game archive"));
 return true;
}
uint32_t Font64(void* font,const char* path,float size,int flags){return reinterpret_cast<bool(*)(void*,const char*,float,int)>(base64+profile64->fontSet)(font,path,size,flags)?1:0;}
wht::ExtendedFrameApi Api64(){wht::nativeFrameLayout={0x1d0,0x13c,0x150,0xe4,0xf0,0x114,0x120,0xd0,0xd8,profile64->simpleVtable,true};wht::ExtendedFrameApi api{};
#define P64(member,field) api.member=reinterpret_cast<decltype(api.member)>(base64+profile64->field)
 P64(alloc,alloc);P64(frameCtor,frameCtor);P64(fontCtor,fontCtor);P64(textureCtor,textureCtor);P64(backdropCtor,backdropCtor);P64(setBackdrop,backdropSet);P64(stringAssign,stringAssign);P64(setText,fontText);P64(setTextAlign,fontAlign);P64(setTexture,textureSet);P64(setHeight,height);
#undef P64
 api.setFont=Font64;api.setWidth=Width64;api.setAbsPoint=Absolute64;api.setRelPoint=Relative64;return api;}
bool Control64(const wht::ToolMenuControl& c){const std::wstring section=c.section,key=c.key;
 if(section==L"MTHKEY"&&(key==L"TimeBoss"||key==L"MTH49Enable"||key==L"G17Enable"))return true;
 if(section==L"Graphics"&&(key==L"timeOfDayUI"||key==L"ConsoleUI"||key==L"BlackBarUI"||key==L"MouseTooltipUI"||key==L"BarInvul"||key==L"Color Player"||key==L"Text Life/Mana Commas"||key==L"Text Life/Mana No Hide"||key==L"Text Stats Commas"))return true;
 if(section==L"Quick Chat")return true;
 if(section==L"DelayClear"||section==L"Language"||section==L"MacroRecorder"||section==L"StateTag")return true;
 if(section==L"Event")return key==L"Camera"||key==L"FogZ"||key==L"Show Mem"||key==L"ShowCooldown"||key==L"ShowRegenHP"||key==L"ShowRegenMana"||key==L"Show Hotkey In Panel"||key==L"Show Resource"||key==L"Show Timer"||key==L"Show FPS"||key==L"Sound Start Game"||key==L"Quick Drop Item"||key==L"Reveal Invisibles"||key==L"Show Sell All"||key==L"Show Auto Pick"||key==L"Show Drop All"||key==L"Show Ins All Skills";
 if(section==L"HotKeyHero"&&(key==L"SetIdTarget"||key==L"SellFast"||key==L"AutoClickLeft"||key==L"AutoClickRight"||key.rfind(L"HeroF",0)==0))return true;
 if(wht::RemapControl131(section,key))return true;
 if(section==L"HotKeyOther")return key==L"hko_menu"||key==L"hko_command"||key==L"hko_MinimizeMultiboard";return false;}
void Reload64(){config64=wht::LoadConfig(ini64);wht::toolEnglish=config64.english;menu64.nextRefresh=0;}
bool BeginSell64(int selectedSlot=-1){
 if(!(Allowed64()&&!menu64.open&&!macro64.playing&&!macro64.recording&&!dropAllNetwork64.active&&GetForegroundWindow()==window64))return false;
  autoPickEnabled64=false;autoPick64.Reset();wht::SellView131 view;
  const bool started=wht::ReadSellView131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},view)&&sellAll64.Start(view,GetTickCount64(),selectedSlot);
 wht::Log(folder64,started?"WHT: Sell All queued.":"WHT: Sell All unavailable (selection/pawnable items/active worker).");
 return started;
}
bool SellHovered64(){POINT point{};RECT client{};if(!GetCursorPos(&point)||!ScreenToClient(window64,&point)||!GetClientRect(window64,&client))return false;
 const int slot=wht::InventorySlotHit131(point,client.right,client.bottom,[&](unsigned slot){return wht::Button64(ui64,base64,*profile64,slot+12);});
 if(slot<0)return false;BeginSell64(slot);return true;
}
bool Command64(const std::wstring& text){std::wistringstream in(text);std::wstring cmd;if(!(in>>cmd))return false;wht::FoldCommandName(cmd);
 if(cmd==L"#menu"||cmd==L"#tool"){menu64.Toggle();return true;}
 if(cmd==L"#toolui"){size_t index;std::wstring op,value;if(!(in>>index>>op)||index>=wht::kToolMenuControls.size())return false;const auto& c=wht::kToolMenuControls[index];if(!Control64(c))return false;
  if(op==L"toggle")value=wht::ToolMenuOverlay::ReadValue(config64,index)==L"1"?L"0":L"1";else if(op==L"set"){std::getline(in,value);if(!value.empty()&&value[0]==L' ')value.erase(0,1);}else return false;
  if(!wht::ValidMenuValue(c,value))return false;
  if(std::wstring(c.section)==L"Language"){const bool english=std::wstring(c.key)==L"English"?value==L"1":value!=L"1";wht::SaveSetting(ini64,L"Language",L"English",english?L"1":L"0");wht::SaveSetting(ini64,L"Language",L"Vietnamese",english?L"0":L"1");}
  else if(!wht::SaveSetting(ini64,c.section,c.key,value))return false;Reload64();return true;}
 if(!authorized64||config64.mode!=wht::RunMode::Local||!ui64)return false;
 if(cmd==L"#repeat"){std::wstring args;std::getline(in,args);const bool ok=repeatMessage64.Command(args);wht::Log(folder64,ok?"WHT: Repeat updated; #repeat stop cancels.":"WHT: Usage #repeat <1..10000> <text> (1..128 UTF-8 bytes); #repeat stop");return ok;}
 if(cmd==L"#speed"){std::wstring args;std::getline(in,args);const bool ok=wht::SetSpeed131(args);wht::Log(folder64,ok?"WHT: Game Speed "+std::to_string(wht::speedMultiplier131)+"x":"WHT: #speed requires 1..70; clock hooks unavailable or invalid argument.");return ok;}
 if(cmd==L"#tanghinh"||cmd==L"#invis"||cmd==L"#reveal"||cmd==L"#revealinvis"){std::wstring arg,extra;in>>arg;if(in>>extra)return false;wht::FoldCommandName(arg);bool next=!config64.revealInvisible;
  if(arg==L"on"||arg==L"1")next=true;else if(arg==L"off"||arg==L"0")next=false;else if(!arg.empty()&&arg!=L"toggle")return false;
  wht::SaveSetting(ini64,L"Event",L"Reveal Invisibles",next?L"1":L"0");Reload64();return true;
 }
 if(cmd==L"#sellslot"){int slot=0;std::wstring extra;if(!(in>>slot)||slot<1||slot>6||(in>>extra))return false;return BeginSell64(slot-1);}
 if(cmd==L"#sellall"){std::wstring extra;if(in>>extra)return false;return BeginSell64();}
 if(cmd==L"#cam"||cmd==L"#zoom"||cmd==L"#fogz"){float value=0;std::wstring extra;if(!(in>>value)||(in>>extra)||!std::isfinite(value))return false;bool fog=cmd==L"#fogz";if(fog?(value<1||value>50000):(value<500||value>10000))return false;wht::SaveSetting(ini64,L"Event",fog?L"FogZ":L"Camera",std::to_wstring(value));Reload64();Camera64();return true;}
 if(cmd==L"#cooldown"||cmd==L"#regenhp"||cmd==L"#regenmp"||cmd==L"#showmem"||cmd==L"#commandsettings"){bool current=cmd==L"#cooldown"?config64.showCooldown:cmd==L"#regenhp"?config64.showRegenHP:cmd==L"#regenmp"?config64.showRegenMana:cmd==L"#showmem"?config64.showMem:settings64;std::wstring arg,extra;in>>arg;if(in>>extra)return false;wht::FoldCommandName(arg);bool next=!current;if(arg==L"on"||arg==L"1")next=true;else if(arg==L"off"||arg==L"0")next=false;else if(!arg.empty()&&arg!=L"toggle")return false;
  if(cmd==L"#commandsettings"){settings64=next;Camera64();return true;}auto key=cmd==L"#cooldown"?L"ShowCooldown":cmd==L"#regenhp"?L"ShowRegenHP":cmd==L"#regenmp"?L"ShowRegenMana":L"Show Mem";wht::SaveSetting(ini64,L"Event",key,next?L"1":L"0");Reload64();return true;}
 if(cmd==L"#macro"){std::wstring op;if(!(in>>op))return false;wht::FoldCommandName(op);cmd=op==L"play"?L"#macroplay":op==L"stop"?L"#macrostop":op==L"record"?L"#macrorecord":op==L"edit"?L"#macroscript":L"";}
 if(config64.macroEnabled){if(cmd==L"#macroplay"){macro64.Play();return true;}if(cmd==L"#macrostop"){macro64.Stop();return true;}if(cmd==L"#macrorecord"){macro64.Record();return true;}if(cmd==L"#macroscript"){macro64.Edit();return true;}}
 return false;
}
void Destroy64(){wht::ForgetUiRefresh131();wht::ResetSpeed131();wht::ResetLifeMana131();if(menu64.independentHost&&Identity64(reinterpret_cast<uintptr_t>(menu64.independentHost),profile64->simpleVtable)){
 auto table=*reinterpret_cast<uintptr_t*>(menu64.independentHost);reinterpret_cast<void(*)(void*,unsigned)>(*reinterpret_cast<uintptr_t*>(table+16))(menu64.independentHost,1);}
 menu64.Reset();placements64.clear();status64=nullptr;menuLogged64=false;gameplay64.Reset();hotkeys64.Reset();remap64.Reset();quickChatEcho64.Reset();repeatMessage64.Reset();timeOfDay64.Reset();consoleUi64.Reset();blackBar64.Reset();mouseTooltip64.Reset();autoClick64.Reset();pinned64={};messageTimers64.Reset();resources64.Reset();clock64.Reset();matchStart64.Reset();timeBoss64.Reset();fps64.Reset();quickDrop64={};dropIcon64.Reset();pickIcon64.Reset();sellIcon64.Reset();reveal64.Reset();sellAll64.Reset();autoPick64.Reset();autoPickEnabled64=false;dropAllNetwork64.Reset();skillsIcon64.Reset();allSkills64.Reset();camera64=0;cameraMask64=0;cameraLogged64=false;typed64=false;chat64.clear();macro64.Stop();}
void PresentGraphics64(){
 if(wht::uiRefreshOwner131!=ui64||!ui64)return;
if(config64.lifeManaCommas||config64.lifeManaNoHide||config64.statsCommas||wht::lifeFlags131||!wht::lifeUi131)if(const auto* p=wht::LifeProfileFor131(profile64->build))wht::UpdateLifeMana131(base64,profile64->build,ui64,wht::Component64(ui64,base64,p->portrait),wht::Component64(ui64,base64,profile64->infoVtable),(Allowed64()&&settings64)?((config64.lifeManaCommas?1u:0u)|(config64.lifeManaNoHide?2u:0u)|(config64.statsCommas?4u:0u)):0u,[](uintptr_t unit,bool mana,float& value,float& maximum){float rate=0;return wht::State64(unit,base64,*profile64,mana,value,maximum,rate);},true);
}
void PresentBar64(uintptr_t bar){
 timeBoss64.RememberBar(bar);
 if(!ui64||!menu64.initialized)return;
 auto raw=api64;raw.setWidth=reinterpret_cast<decltype(raw.setWidth)>(base64+profile64->width);raw.setAbsPoint=reinterpret_cast<decltype(raw.setAbsPoint)>(base64+profile64->absolute);raw.setRelPoint=reinterpret_cast<decltype(raw.setRelPoint)>(base64+profile64->relative);gameplay64.WriteBar(bar,base64,*profile64,config64,Allowed64()&&settings64,raw);
}
void Update64(){const auto ui=CurrentUi64();if(ui!=ui64){Destroy64();ui64=ui;}if(!Allowed64()||!settings64)repeatMessage64.Reset();if(!Allowed64()||!settings64||!config64.timeBoss||!(config64.mth49Enable||config64.g17Enable))timeBoss64.active=false;if(!Allowed64())wht::ResetSpeed131();if(!ui)return; wht::InstallUiRefresh131(base64,profile64->build,ui,PresentGraphics64);graphicsDiagnostic64.Update(base64,profile64->build,ui,config64,Allowed64(),folder64,consoleUi64,mouseTooltip64,[&](uintptr_t table){return wht::Component64(ui,base64,table);}); wht::UpdateMatchStart131(matchStart64,base64,profile64->build,ui,Allowed64()&&settings64&&config64.soundStartGame,module64,folder64); if(auto p=wht::RevealProfileFor131(profile64->build)){
  if(reveal64.Update(base64,profile64->build,wht::Component64(ui64,base64,p->worldTable),Allowed64()&&settings64&&config64.revealInvisible)&&!reveal64.reported){reveal64.reported=true;wht::Log(folder64,"WHT: native invisible-unit display enabled.");}
 }
if(config64.lifeManaCommas||config64.lifeManaNoHide||config64.statsCommas||wht::lifeFlags131||!wht::lifeUi131)if(const auto* p=wht::LifeProfileFor131(profile64->build))wht::UpdateLifeMana131(base64,profile64->build,ui64,wht::Component64(ui64,base64,p->portrait),wht::Component64(ui64,base64,profile64->infoVtable),(Allowed64()&&settings64)?((config64.lifeManaCommas?1u:0u)|(config64.lifeManaNoHide?2u:0u)|(config64.statsCommas?4u:0u)):0u,[](uintptr_t unit,bool mana,float& value,float& maximum){float rate=0;return wht::State64(unit,base64,*profile64,mana,value,maximum,rate);});
 if(config64.mouseTooltip||mouseTooltip64.active){const auto* tip=wht::MouseTooltipProfileFor131(profile64->build);const auto* console=wht::ConsoleProfileFor131(profile64->build);
  if(tip&&console)wht::UpdateMouseTooltip131(mouseTooltip64,base64,profile64->build,ui64,wht::Component64(ui64,base64,tip->table),wht::ConsoleComponent131(ui64,base64,console->table),window64,Allowed64()&&settings64&&!menu64.open&&config64.mouseTooltip);
 }
 if(config64.hideBlackBar||blackBar64.Active())if(const auto* p=wht::BlackBarProfileFor131(profile64->build))blackBar64.Update(base64,profile64->build,ui64,wht::Component64(ui64,base64,p->worldTable),Allowed64()&&settings64&&config64.hideBlackBar);
 if(config64.hideConsoleUI||consoleUi64.active)if(const auto* p=wht::ConsoleProfileFor131(profile64->build))consoleUi64.Update(base64,profile64->build,wht::ConsoleComponent131(ui64,base64,p->table),Allowed64()&&settings64&&config64.hideConsoleUI);
 if(config64.hideTimeOfDay||timeOfDay64.active)if(const auto* p=wht::TimeOfDayProfileFor131(profile64->build))timeOfDay64.Update(base64,profile64->build,wht::Component64(ui64,base64,p->table),Allowed64()&&settings64&&config64.hideTimeOfDay);
 wht::UpdateAutoClick131(autoClick64,config64,window64,Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!sellAll64.active&&!dropAllNetwork64.active);
Camera64();if(!Assets64())return;
 const auto cleared=messageTimers64.Update(base64,ui,profile64->build,config64,Allowed64()&&!typed64,GetTickCount64());
 const auto freshClear=cleared&~messageTimers64.reported;
 if(freshClear&1)wht::Log(folder64,"WHT x64: periodic native text clear executed.");
 if(freshClear&2)wht::Log(folder64,"WHT x64: periodic native chat clear executed.");
 messageTimers64.reported|=cleared;
 RECT r{};GetClientRect(window64,&r);if(r.right<=0||r.bottom<=0)return;const auto classic=std::min(r.right,r.bottom*4/3);const float scale=classic>0?float(r.right)/classic:1;
 if(std::abs(scale-scale64)>.00001f){scale64=scale;for(const auto& entry:placements64){const auto p=entry.second;if(p.sized)Width64(entry.first,p.width);if(p.absolute)Absolute64(entry.first,p.point,p.x,p.y,p.flag);else if(p.relative)Relative64(entry.first,p.point,p.other,p.otherPoint,p.x,p.y,p.flag);}}
 if(GetTickCount64()>=reloadAt64){reloadAt64=GetTickCount64()+2000;Reload64();}
 menu64.SetViewport(r.right,r.bottom);menu64.Update(base64,ui,config64,true,&api64);
 if(menu64.failed){if(!menuLogged64){menuLogged64=true;wht::Log(folder64,std::string("WHT x64: menu construction failed at ")+menu64.initStage);}return;}
 if(menu64.initialized&&!menuLogged64){menuLogged64=true;wht::Log(folder64,"WHT x64: native eight-tab menu constructed.");}
 if(menu64.initialized&&!status64){status64=wht::NativeCreateFontString(api64,menu64.independentHost,.012f,"",4);if(status64){api64.setWidth(status64,.45f);api64.setHeight(status64,.018f);api64.setAbsPoint(status64,0,.01f,.59f,1);}}
 if(menu64.initialized){auto raw=api64;raw.setWidth=reinterpret_cast<decltype(raw.setWidth)>(base64+profile64->width);raw.setAbsPoint=reinterpret_cast<decltype(raw.setAbsPoint)>(base64+profile64->absolute);raw.setRelPoint=reinterpret_cast<decltype(raw.setRelPoint)>(base64+profile64->relative);
  wht::statPaintPresentation131=PresentBar64;
  auto observed=gameplay64.Update(base64,ui,*profile64,config64,Allowed64(),menu64,raw);unsigned fresh=observed&~gameplay64.reported;
  if(resources64.Update(base64,profile64->build,Allowed64()&&config64.showResource,GetTickCount64(),menu64,folder64)&&!resources64.reported){resources64.reported=true;wht::Log(folder64,"WHT x64: native resource table displayed.");}
  if(clock64.Update(base64,profile64->build,Allowed64()&&config64.showTimer,menu64)&&!clock64.reported){clock64.reported=true;wht::Log(folder64,"WHT x64: native simulation timer displayed.");}
  clock64.Diagnose(base64,profile64->build,Allowed64()&&config64.showTimer,folder64);
  if(sellAll64.active){wht::SellView131 view;bool valid=Allowed64()&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64&&wht::ReadSellView131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},view);
  if(valid&&sellAll64.scanning){auto p=wht::PickProfileFor131(profile64->build);valid=p&&wht::ScanSellShops131(base64,profile64->build,wht::Component64(ui64,base64,p->worldTable),sellAll64.scanCursor,view,[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;});}
  const auto result=sellAll64.Step(valid?&view:nullptr,GetTickCount64(),[&](const wht::DropSnapshot& source,const wht::DropItem& item,const wht::SellShop131& shop){return wht::SendSell131(base64,profile64->build,source,item,shop,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;});});
  if(result==wht::SellResult131::Submitted)wht::Log(folder64,"WHT: Sell All synchronized shop order submitted.");
  else if(result!=wht::SellResult131::Working&&result!=wht::SellResult131::Idle)wht::Log(folder64,std::string("WHT: Sell All ")+(result==wht::SellResult131::Complete?"completed":result==wht::SellResult131::NoShop?"no accessible purchase shop":result==wht::SellResult131::Cancelled?"cancelled":"remaining")+"; sold="+std::to_string(sellAll64.sold)+" skipped="+std::to_string(sellAll64.skipped)+" remaining="+std::to_string(sellAll64.remaining)+" gold delta="+std::to_string(sellAll64.goldDelta));
 }
 sellIcon64.latched=sellAll64.active;sellIcon64.Update(base64,Allowed64()&&config64.showSellAll,menu64);
 if(!config64.showAutoPick)autoPickEnabled64=false;
 if(autoPickEnabled64&&(Allowed64()&&config64.showAutoPick&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64)&&!dropAllNetwork64.active){const auto now=GetTickCount64();
  if(now>=autoPick64.next){auto p=wht::PickProfileFor131(profile64->build);wht::PickSnapshot131 snapshot;
   const bool valid=p&&wht::ReadPickSnapshot131(base64,profile64->build,wht::Component64(ui64,base64,p->worldTable),[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},snapshot);
   const auto result=autoPick64.Tick(now,valid?&snapshot:nullptr,[&](const wht::DropSnapshot& source,const wht::PickCandidate131& item){return wht::SendPick131(base64,profile64->build,source,item,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;});});
   if(result==wht::PickResult131::Submitted)wht::Log(folder64,"WHT: Auto Pick native target order submitted.");
   if(result==wht::PickResult131::Cancelled){autoPickEnabled64=false;wht::Log(folder64,"WHT: Auto Pick stopped (selection/session/native validation).");}
  }
 }else autoPick64.Reset();
 pickIcon64.latched=autoPickEnabled64;pickIcon64.Update(base64,Allowed64()&&config64.showAutoPick,menu64);
 dropIcon64.Update(base64,Allowed64()&&config64.showDropAll,menu64);
 if(dropAllNetwork64.active){wht::DropSnapshot snapshot;const bool valid=Allowed64()&&config64.showDropAll&&!sellAll64.active&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64&&wht::DropSnapshot131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},snapshot);
  const auto result=dropAllNetwork64.Step(valid?&snapshot:nullptr,GetTickCount64(),[&](const wht::DropSnapshot& source,const wht::DropItem& item){return wht::SendDropSnapshot131(base64,profile64->build,source,item);});
  if(result==wht::DropResult::Complete)wht::Log(folder64,"WHT: synchronized drop all completed; original items absent from inventory.");
  else if(result==wht::DropResult::Cancelled)wht::Log(folder64,"WHT: synchronized drop all cancelled.");
  else if(result==wht::DropResult::Remaining)wht::Log(folder64,"WHT: synchronized drop all stopped; remaining "+std::to_string(dropAllNetwork64.remaining));
 }
  skillsIcon64.Update(base64,Allowed64()&&config64.showInsAllSkills,menu64);
  if(allSkills64.Tick(GetTickCount64(),Allowed64()&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64,[&](){return wht::Selected64(ui64,base64,*profile64);},SkillPoints64,SkillData64,ClickSkill64))wht::Log(folder64,"WHT x64: all skills native sequence finished; submitted "+std::to_string(allSkills64.submitted));
  if(fps64.Update(base64,ui,profile64->build,Allowed64(),config64.showFps)&&!fps64.reported){fps64.reported=true;wht::Log(folder64,"WHT x64: native FPS toggle installed.");}
  if(hotkeys64.Due(GetTickCount64(),Allowed64()&&config64.showHotkeyInPanel)&&hotkeys64.Update(Allowed64()&&config64.showHotkeyInPanel,menu64,[&](unsigned slot){return wht::Button64(ui,base64,*profile64,slot);},&config64,(Allowed64()&&config64.showHotkeyInPanel)?InventoryMask64(true):0,(Allowed64()&&config64.showHotkeyInPanel)?AlternatePanel64():false)&&!hotkeys64.reported){hotkeys64.reported=true;wht::Log(folder64,"WHT x64: native panel hotkey labels displayed.");}
  for(auto entry:{std::pair<unsigned,const char*>{1,"selected-unit HP/s MP/s"},{2,"cooldown seconds"},{8,"HP/Mana values"},{16,"hero level"}})if(fresh&entry.first)wht::Log(folder64,std::string("WHT x64: live ")+entry.second+" displayed.");gameplay64.reported|=observed;
 }
 if(status64){std::string text="|cffffcc00WHT " WHT_VERSION_STRING " | 1.31.1."+std::to_string(profile64->build)+" x64|r";if(!authorized64)text+=" |cffff4444Login LoadFileMapWar3|r";else if(config64.showMem){float mb=0,percent=0;if(wht::GetCurrentProcessMemoryUsage(mb,percent))text+=" | "+std::to_string(int(mb))+" MB";}api64.setText(status64,text.c_str());}
 timeBoss64.Update(base64,profile64->build,ui64,Allowed64()&&settings64&&config64.timeBoss&&(config64.mth49Enable||config64.g17Enable),GetTickCount64(),menu64,[&](uintptr_t unit,float& hp){float maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,hp,maximum,rate);});
 repeatMessage64.Tick(Allowed64()&&settings64,!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==window64,GetTickCount64(),SendChat64);
 menu64.PlaybackVisual(macro64.playing);macro64.Tick(Allowed64()&&!typed64&&!menu64.open);
}
void Safe64(){__try{Update64();}__except(1){menu64.failed=true;}}
bool SendChat64(const std::string& text){
 __try{return wht::SendQuickChat131(text,wht::QuickChatNative131(base64,profile64->build),[](wht::QuickChatView131& out){const auto ui=CurrentUi64();return ui&&ui==ui64&&wht::ReadQuickChat131(base64,profile64->build,ui,out);});}__except(1){return false;}
}
LRESULT CALLBACK Window64(HWND hwnd,UINT message,WPARAM key,LPARAM value){if(message==update64){Safe64();return 0;}
 if(message==WM_KILLFOCUS)quickChatEcho64.Reset();
 if(message==WM_KEYDOWN&&!(value&(1LL<<30)))quickChatEcho64.Reset();
 if(message==WM_CHAR&&quickChatEcho64.Character(value,GetTickCount64()))return 0;
 if((message==WM_KEYUP||message==WM_SYSKEYUP)&&remap64.Input(message,unsigned(key),false,false,false,config64,[](unsigned){return false;}))return 0;
 {RECT rect{};GetClientRect(hwnd,&rect);
 if(skillsIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&config64.showInsAllSkills&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd,[&](){
  const auto started=allSkills64.Start(GetTickCount64(),[&](){return wht::Selected64(ui64,base64,*profile64);},SkillPoints64,SkillData64);
  wht::Log(folder64,started?"WHT x64: all skills sequence queued.":"WHT x64: all skills unavailable (hero/points/menu).");
 }))return 0;}
 {RECT rect{};GetClientRect(hwnd,&rect);
 if(pickIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&config64.showAutoPick&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd&&!dropAllNetwork64.active&&!sellAll64.active,[&](){autoPickEnabled64=!autoPickEnabled64;autoPick64.Reset();wht::Log(folder64,autoPickEnabled64?"WHT: Auto Pick enabled.":"WHT: Auto Pick disabled.");}))return 0;
 if(sellIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&config64.showSellAll&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd&&!dropAllNetwork64.active,[&](){
  BeginSell64();
 }))return 0;
 if(dropIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&config64.showDropAll&&!sellAll64.active&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd,[&](){
  autoPickEnabled64=false;autoPick64.Reset();
  const auto* p=wht::InventoryProfile(profile64->build);if(!p)return;
  wht::DropSnapshot snapshot;
  const bool started=wht::DropSnapshot131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},snapshot)&&dropAllNetwork64.Start(snapshot,GetTickCount64());
  wht::Log(folder64,started?"WHT: synchronized drop all queued.":"WHT: drop all unavailable or already active.");
 }))return 0;}
 if(quickDrop64.Mouse(message,Allowed64()&&config64.quickDropItem&&!sellAll64.active&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd&&(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000),[&](){
  RECT rect{};GetClientRect(hwnd,&rect);POINT point{short(LOWORD(value)),short(HIWORD(value))};
  return wht::InventorySlotHit131(point,rect.right,rect.bottom,[&](unsigned slot){return wht::Button64(ui64,base64,*profile64,slot+12);});
 },[&](unsigned slot){
  autoPickEnabled64=false;autoPick64.Reset();
  const auto* p=wht::InventoryProfile(profile64->build);if(!p)return;
  const auto* networkProfile=wht::DropNetworkProfileFor131(profile64->build);if(!networkProfile)return;
  const auto network=wht::DropNetworkNative131(base64,*networkProfile,*p);
  const auto ok=wht::QuickDropNetwork131(base64,*p,wht::InventoryNative131(base64,*p),network,slot,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},[&](uintptr_t unit){return wht::DropOwned131(base64,profile64->build,network,unit);});
  wht::Log(folder64,ok?"WHT x64: quick drop synchronized action submitted.":"WHT x64: quick drop refused or slot empty.");
 }))return 0;
 if(message==WM_KEYDOWN){bool repeat=(value&(1LL<<30))!=0;if(menu64.ToggleKey(unsigned(key),config64.menuKey,repeat,typed64))return 0;if(!repeat&&key==VK_ESCAPE&&menu64.Escape())return 0;if(menu64.Key(unsigned(key),unsigned((value>>16)&255),Command64))return 0;
  if(!repeat&&key==VK_RETURN){if(typed64){Command64(chat64);typed64=false;chat64.clear();}else{typed64=true;chat64.clear();}}
  if(key==VK_ESCAPE){typed64=false;chat64.clear();}
  if(Allowed64()&&!typed64&&!repeat&&!menu64.open&&key==VK_TAB&&config64.showResource&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){resources64.Toggle();return 0;}
  if(Allowed64()&&!typed64&&!repeat){if(key==config64.commandKey){settings64=!settings64;Camera64();return 0;}
   bool chord=(GetKeyState(VK_CONTROL)&0x8000)&&(GetKeyState(VK_SHIFT)&0x8000);if(config64.macroEnabled){if(key==config64.macroKeys[0]||(chord&&key==VK_F10)){macro64.Play();return 0;}if(key==config64.macroKeys[1]||(chord&&key==VK_F12)){macro64.Stop();return 0;}if(key==config64.macroKeys[2]||(chord&&key==VK_F9)){macro64.Record();return 0;}if(key==config64.macroKeys[3]||(chord&&key==VK_F11)){macro64.Edit();return 0;}}}}
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&key&&key==config64.multiboardKey&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const auto* p=wht::MultiboardProfileFor131(profile64->build);
  if(p&&wht::ToggleMultiboard131(base64,profile64->build,wht::Component64(ui64,base64,p->table))){if(key<256)remap64.pressed[key]=true;return 0;}
 }
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const int slot=wht::HeroKeySlot131(unsigned(key),config64);const auto* p=wht::HeroProfileFor131(profile64->build);
  if(slot>=0&&p&&wht::ClickHero131(base64,profile64->build,wht::Component64(ui64,base64,p->bar),unsigned(slot))){if(key<256)remap64.pressed[key]=true;return 0;}
 }
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)&&autoClick64.Key(unsigned(key),config64)){if(key<256)remap64.pressed[key]=true;wht::Log(folder64,std::string("WHT: Auto Click ")+(key==config64.autoClickLeftKey?"Left: ":"Right: ")+((key==config64.autoClickLeftKey?autoClick64.left:autoClick64.right)?"ON":"OFF"));return 0;}
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&key&&key==config64.sellFastKey&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==hwnd&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  if(SellHovered64()){if(key<256)remap64.pressed[key]=true;return 0;}
 }
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&key&&key==config64.setIdTargetKey&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==hwnd&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const bool ok=PinTarget64();if(key<256)remap64.pressed[key]=true;wht::Log(folder64,ok?"WHT: selected native unit pinned for target cast.":"WHT: no live target selected; pin cleared.");return 0;
 }
 if(remap64.Input(message,unsigned(key),bool(value&(1LL<<30)),Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==hwnd,(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_MENU)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000),config64,[&](unsigned slot){
  const auto button=wht::Button64(ui64,base64,*profile64,slot);
  if(slot>=12&&!(InventoryMask64()&(1u<<(slot-12))))return false;
  const auto data=wht::SkillButtonData131(button);
  const bool shop=slot<12&&AlternatePanel64();if(shop)return (data.order||data.ability)&&ClickSkill64(slot);
  if(!data.order)return false;
  const auto& binding=slot>=12?config64.inventory[slot-12]:config64.command[slot];
  const auto target=[&](){const auto* p=wht::QuickCastProfile(profile64->build);return p?wht::QuickCastTarget131(base64,ui64,profile64->uiVtable,*p):uintptr_t(0);};
  if(binding.autoTargetMe||binding.autoTarget){
   const auto api=wht::SelfCastNative131(base64,profile64->build);if(!api)return false;
   const auto read=[&](wht::SelfCastSource131& source){return wht::ReadSelfCastSource131(base64,profile64->build,ui64,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},source);};
   if(binding.autoTarget&&!binding.autoTargetMe)return wht::ActivateTargetCast131(read,ReadPinned64,target,[&](){return ClickSkill64(slot);},wht::SelfCastOrder131,[&](const wht::SelfCastSource131& destination,uintptr_t mode,unsigned order){return wht::FinishSelfCast131(api,destination,mode,order);});
   return wht::ActivateSelfCast131(read,target,[&](){return ClickSkill64(slot);},wht::SelfCastOrder131,[&](const wht::SelfCastSource131& source,uintptr_t mode,unsigned order){return wht::FinishSelfCast131(api,source,mode,order);});
  }
  return wht::ActivateQuickCast131(binding.quickCast,target,[&](){return ClickSkill64(slot);},[&](uintptr_t mode){return wht::QuickCastCursor131(hwnd,mode,target);});
 },AlternatePanel64,LearnSlot64))return 0;
 if(message==WM_KEYDOWN&&wht::QuickChatKey131(unsigned(key),bool(value&(1LL<<30)),Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&GetForegroundWindow()==hwnd,(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000)||(GetKeyState(VK_MENU)&0x8000),config64,SendChat64)){remap64.pressed[key]=true;quickChatEcho64.Sent(value,GetTickCount64());return 0;}
 if(message==WM_CHAR){wchar_t ch=wchar_t(key);if(ch==L'#'){typed64=true;chat64=L"#";}else if(typed64){if(ch==8){if(!chat64.empty())chat64.pop_back();}else if(ch>=32&&chat64.size()<1024)chat64+=ch;}}
 if(message==WM_LBUTTONDOWN||message==WM_LBUTTONUP||message==WM_LBUTTONDBLCLK){POINT p{short(LOWORD(value)),short(HIWORD(value))};RECT r{};GetClientRect(hwnd,&r);if(menu64.Mouse(message,p,r.right,r.bottom,Command64,&config64))return 0;}
 MSG msg{hwnd,message,key,value};macro64.Capture(msg,!menu64.open&&!typed64);const auto result=CallWindowProcW(previous64,hwnd,message,key,value);
 return result;}
BOOL CALLBACK FindWindow64(HWND hwnd,LPARAM){DWORD pid=0;GetWindowThreadProcessId(hwnd,&pid);if(pid==GetCurrentProcessId()&&IsWindowVisible(hwnd)&&!GetWindow(hwnd,GW_OWNER)){window64=hwnd;return FALSE;}return TRUE;}
DWORD WINAPI Auth64(void*){while(true){auto identity=wht::VerifySession(folder64);{std::lock_guard<std::mutex> guard(wht::sessionMutex);wht::sessionIdentity=identity;}authorized64=!identity.username.empty()&&wht::SessionReady()&&!wht::StartupUpdatePending();Sleep(5000);}return 0;}
DWORD WINAPI Install64(void* module){module64=static_cast<HMODULE>(module);wchar_t path[32768]{};GetModuleFileNameW(module64,path,32768);folder64=std::filesystem::path(path).parent_path().wstring();ini64=folder64+L"\\WHT.ini";GetModuleFileNameW(nullptr,path,32768);profile64=wht::FindProfile64(wht::HashFile(path));if(!profile64)return 1;base64=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));Reload64();api64=Api64();menu64.controlAvailable=Control64;
 for(unsigned n=0;n<900&&!wht::ProfileCode64(base64,*profile64);++n)Sleep(100);if(!wht::ProfileCode64(base64,*profile64)){wht::Log(folder64,"WHT x64: native prologue verification failed.");return 2;}
 for(unsigned n=0;n<3000&&!window64;++n){EnumWindows(FindWindow64,0);if(!window64)Sleep(100);}if(!window64)return 2;
 macro64.Configure(window64,folder64);previous64=reinterpret_cast<WNDPROC>(SetWindowLongPtrW(window64,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(Window64)));if(!previous64)return 3;
 HANDLE auth=CreateThread(nullptr,0,Auth64,nullptr,0,nullptr);if(auth)CloseHandle(auth);ready64=true;wht::Log(folder64,"WHT x64: exact EXE profile selected; window update callback installed.");
 while(IsWindow(window64)){DWORD_PTR result=0;SendMessageTimeoutW(window64,update64,0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,100,&result);Sleep(33);}return 0;}
}
extern "C" __declspec(dllexport) const char* WHT_RebuildVersion(){return WHT_VERSION_STRING;}
extern "C" __declspec(dllexport) unsigned WHT131_Status(){return ready64?1:0;}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,void*){if(reason==DLL_PROCESS_ATTACH){DisableThreadLibraryCalls(module);HANDLE thread=CreateThread(nullptr,0,Install64,module,0,nullptr);if(thread)CloseHandle(thread);}return TRUE;}
