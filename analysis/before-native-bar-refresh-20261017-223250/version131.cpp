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
#include "clock_fps131.hpp"
#include "runtime131.hpp"
#include "macro_recorder.hpp"
#include "memory_overlay.hpp"
#include "session_auth.hpp"
#include "cooldown131.hpp"
#include "hotkey_labels131.hpp"
#include "message_clear131.hpp"
#include "resource_board131.hpp"
#include "regen131.hpp"
#include "state_bars131.hpp"
#include <atomic>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace {
HMODULE module131=nullptr;uintptr_t base131=0;const wht::Profile131* profile131=nullptr;
HWND gameWindow131=nullptr;HHOOK messageHook131=nullptr;UINT_PTR timer131=0;
WNDPROC windowProc131=nullptr;constexpr UINT updateMessage131=WM_APP+0x131;
std::wstring folder131,ini131;wht::Config config131;
wht::ExtendedFrameApi frame131{};wht::ToolMenuOverlay menu131;wht::MacroRecorder macro131;
wht::CameraSetter131 originalCamera131=nullptr;wht::CameraOverride131 camera131;
wht::CooldownOverlay131 cooldown131;const wht::CooldownProfile131* cooldownProfile131=nullptr;
wht::HotkeyLabels131 hotkeys131;
wht::SlotRemap131 remap131;
wht::TimeOfDay131 timeOfDay131;
wht::ConsoleUi131 consoleUi131;
wht::BlackBar131 blackBar131;
wht::MouseTooltip131 mouseTooltip131;
wht::AutoClick131 autoClick131;
wht::SelfCastSource131 pinned131;
wht::MessageTimers131 messageTimers131;
wht::ResourceBoard131 resources131;
wht::GraphicsDiagnostic131 graphicsDiagnostic131;
wht::MatchStart131 matchStart131;
wht::ClockOverlay131 clock131;wht::FpsState131 fps131;
wht::QuickDropInput131 quickDrop131;
wht::Reveal131 reveal131;
wht::SellIcon131 sellIcon131;wht::SellAll131 sellAll131;
wht::AutoPickIcon131 pickIcon131;wht::AutoPick131 autoPick131;bool autoPickEnabled131=false;
wht::DropIcon131 dropIcon131;wht::DropAllState dropAllNetwork131;
wht::SkillsIcon131 skillsIcon131;wht::AllSkills131 allSkills131;
wht::RegenOverlay131 regen131;const wht::RegenProfile131* regenProfile131=nullptr;
wht::StateBars131 stateBars131;const wht::StateBarProfile131* stateBarProfile131=nullptr;
uintptr_t currentUi131=0;unsigned cameraSlot131=0;bool typed131=false;std::wstring text131;
int SkillPoints131(uintptr_t unit){const auto* p=wht::SkillProfile(profile131->build);if(!p||!regenProfile131||!stateBarProfile131)return -1;float current=0,maximum=0;
 return wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,current,maximum)&&current>.405f?wht::HeroPoints131(unit,base131,*p):-1;}
wht::SkillButton131 SkillData131(unsigned slot){return cooldownProfile131?wht::SkillButtonData131(wht::CooldownButton131(currentUi131,base131,*cooldownProfile131,slot)):wht::SkillButton131{};}
bool ReadPinned131(wht::SelfCastSource131& out){return wht::ReadCastTarget131(base131,profile131->build,currentUi131,pinned131.unit,[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},out)&&wht::SameSelfCastSource131(pinned131,out);}
bool PinTarget131(){pinned131={};return wht::ReadCastTarget131(base131,profile131->build,currentUi131,regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0,[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},pinned131);}
unsigned InventoryMask131(bool usableOnly=false){if(!profile131)return 0;const auto* p=wht::InventoryProfile(profile131->build);if(!p)return 0;return wht::InventoryMask131(base131,*p,wht::InventoryNative131(base131,*p),(regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0),usableOnly);}
bool AlternatePanel131(){return wht::AlternatePanel131([&](unsigned slot){const auto data=SkillData131(slot);return data.order?data.order:data.ability;});}
bool ClickSkill131(unsigned slot){const auto* p=wht::SkillProfile(profile131->build);return p&&cooldownProfile131&&wht::SkillClick131(wht::CooldownButton131(currentUi131,base131,*cooldownProfile131,slot),base131,*p);}
bool LearnSlot131(unsigned slot){return wht::LearnSlot131(slot,[&](wht::SelfCastSource131& out){return wht::ReadSelfCastSource131(base131,profile131->build,currentUi131,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},out);},SkillPoints131,SkillData131,ClickSkill131);}

std::atomic<bool> authorized131{false},ready131{false};bool settings131=true;
float cameraWritten131[2]{};unsigned cameraWrittenMask131=0;
bool menuReported131=false,cameraReported131=false;
bool ticking131=false,tickReported131=false,writeReported131=false;
ULONGLONG configNext131=0,updateNext131=0;void* memoryFont131=nullptr;
bool full131=false,lock131=false;LONG_PTR oldStyle131=0;RECT oldRect131{};
uintptr_t archive131=0;bool archiveAttempt131=false,archiveReady131=false;
bool ArchiveInfo131(const char* name,uintptr_t& owner){
 __try{return reinterpret_cast<bool(__cdecl*)(const char*,size_t,uintptr_t*)>(base131+profile131->fileInfo)(name,strlen(name),&owner);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}}
uintptr_t OpenAssets131(const char* path){
 __try{return reinterpret_cast<uintptr_t(__cdecl*)(const char*,size_t,unsigned,unsigned,unsigned,uintptr_t,int)>(base131+profile131->archiveOpen)(path,strlen(path),2,0,100,0,0);}
 __except(EXCEPTION_EXECUTE_HANDLER){return 0;}}
bool ArchiveIdentity131(uintptr_t value){__try{return value&&*reinterpret_cast<uintptr_t*>(value)==base131+profile131->mpqVtable;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
bool MountAssets131(){
 if(archiveReady131)return true;if(archiveAttempt131)return false;archiveAttempt131=true;
 const auto path=wht::Utf8(folder131+L"\\WHT.mix");archive131=OpenAssets131(path.c_str());
 if(!ArchiveIdentity131(archive131)){wht::Log(folder131,"WHT 1.31: native MPQ mount failed; menu withheld to avoid missing textures.");return false;}
 for(const char* name:{"fonts_VN.ttf","BTNWHT_Menu.blp","BTNWHT_Rec.blp","Transparent.tga","WHTUI_GreyBackground.tga"}){
  uintptr_t owner=0;if(!ArchiveInfo131(name,owner)||owner!=archive131){wht::Log(folder131,std::string("WHT 1.31: native provider lookup failed: ")+name);return false;}}
 archiveReady131=true;wht::Log(folder131,"WHT 1.31: native CArchiveMpq mounted; font/icon/transparency provider lookups verified.");return true;
}
struct Placement131{bool sized=false,absolute=false,relative=false;float width=0,x=0,y=0;int point=0,relativePoint=0,flag=1;void* owner=nullptr;};
std::unordered_map<void*,Placement131> placements131;float horizontalScale131=1;
void __fastcall Width131(void* object,DWORD,float value){auto& p=placements131[object];p.sized=true;p.width=value;
 reinterpret_cast<void(__thiscall*)(void*,float)>(base131+profile131->width)(object,value*horizontalScale131);}
void __fastcall Abs131(void* object,DWORD,int point,float x,float y,int flag){auto& p=placements131[object];p.absolute=true;p.relative=false;p.point=point;p.x=x;p.y=y;p.flag=flag;
 reinterpret_cast<void(__thiscall*)(void*,int,float,float,int)>(base131+profile131->absPoint)(object,point,.4f+(x-.4f)*horizontalScale131,y,flag);}
void __fastcall Rel131(void* object,DWORD,int point,void* owner,int relativePoint,float x,float y,int flag){auto& p=placements131[object];p.relative=true;p.absolute=false;p.point=point;p.owner=owner;p.relativePoint=relativePoint;p.x=x;p.y=y;p.flag=flag;
 reinterpret_cast<void(__thiscall*)(void*,int,void*,int,float,float,int)>(base131+profile131->relPoint)(object,point,owner,relativePoint,x*horizontalScale131,y,flag);}
void Viewport131(int width,int height){const int classic=std::min(width,height*4/3);const float scale=classic>0?float(width)/classic:1;
 if(std::abs(scale-horizontalScale131)<.00001f)return;horizontalScale131=scale;
 for(const auto& entry:placements131){const auto p=entry.second;if(p.sized)Width131(entry.first,0,p.width);
  if(p.absolute)Abs131(entry.first,0,p.point,p.x,p.y,p.flag);else if(p.relative)Rel131(entry.first,0,p.point,p.owner,p.relativePoint,p.x,p.y,p.flag);}}

uintptr_t CurrentUi131(){
 if(!profile131||!base131)return 0;
 __try{
  auto get=reinterpret_cast<uintptr_t(__cdecl*)(int,int)>(base131+profile131->uiGet);
  const auto ui=get(0,0);
  return wht::ComponentIdentity131(ui,base131+profile131->uiVtable)?ui:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
bool CameraLive131(uintptr_t ui){
 __try {return ui&&cameraSlot131&&camera131.owner&&
  *reinterpret_cast<uintptr_t*>(ui+cameraSlot131)==camera131.owner&&
  *reinterpret_cast<uintptr_t*>(camera131.owner)==base131+profile131->cameraVtable;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Allowed131(){return authorized131.load()&&config131.mode==wht::RunMode::Local&&currentUi131;}
bool CameraIdentity131(uintptr_t candidate){
 __try{return candidate&&*reinterpret_cast<uintptr_t*>(candidate)==base131+profile131->cameraVtable;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void FindCamera131(){
 if(!currentUi131)return;
 __try{for(unsigned off=0x200;off<0x4d4;off+=4){const auto candidate=*reinterpret_cast<uintptr_t*>(currentUi131+off);
  if(CameraIdentity131(candidate)){cameraSlot131=off;camera131.owner=candidate;return;}
 }}__except(EXCEPTION_EXECUTE_HANDLER){}
}
bool ReadCamera131(int field,float& value){
 if(!CameraLive131(currentUi131)||!CameraIdentity131(camera131.owner)||field<0||field>1)return false;
 __try {
  // The verified setter dispatches native fields 0/1 to CFloatVariable
  // +114/+138. Use its native getter; 1.31 encodes these values in memory.
  reinterpret_cast<void*(__thiscall*)(uintptr_t,float*)>(base131+profile131->readFloat)(camera131.owner+(field==0?0x114:0x138),&value);
  return std::isfinite(value)&&value>0&&value<1000000;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void ApplyCamera131(){
 if(!originalCamera131)return;if(!CameraLive131(currentUi131))FindCamera131();
 if(!CameraLive131(currentUi131))return;
 const bool enabled=Allowed131()&&settings131;
 for(int field=0;field<2;++field){float native=0;if(!ReadCamera131(field,native))continue;
  const unsigned bit=1u<<field;
  if(!cameraReported131){cameraReported131=true;wht::Log(folder131,"WHT 1.31: live camera identity and native float getter verified.");}
  if(!(cameraWrittenMask131&bit)||std::abs(native-cameraWritten131[field])>.01f){camera131.requested[field]=native;camera131.seen|=bit;}
  if(!enabled)continue;
  const float desired=field==0?config131.camera:config131.fogZ;
  originalCamera131(camera131.owner,field,desired,0,1);cameraWritten131[field]=desired;cameraWrittenMask131|=bit;camera131.active=true;
  if(field==0&&!writeReported131){float observed=0;if(ReadCamera131(field,observed)){
   writeReported131=true;wht::Log(folder131,"WHT 1.31: camera distance requested="+std::to_string(desired)+" readback="+std::to_string(observed));}}
 }
 if(!enabled){camera131.Restore(originalCamera131);cameraWrittenMask131=0;}
}
void Fullscreen131(bool value){
 if(!gameWindow131||value==full131)return;
 if(value){oldStyle131=GetWindowLongPtrW(gameWindow131,GWL_STYLE);GetWindowRect(gameWindow131,&oldRect131);
  MONITORINFO info{sizeof(info)};if(!GetMonitorInfoW(MonitorFromWindow(gameWindow131,MONITOR_DEFAULTTONEAREST),&info))return;
  SetWindowLongPtrW(gameWindow131,GWL_STYLE,oldStyle131&~(WS_CAPTION|WS_THICKFRAME));
  SetWindowPos(gameWindow131,nullptr,info.rcMonitor.left,info.rcMonitor.top,info.rcMonitor.right-info.rcMonitor.left,info.rcMonitor.bottom-info.rcMonitor.top,SWP_NOZORDER|SWP_FRAMECHANGED);
 }else{SetWindowLongPtrW(gameWindow131,GWL_STYLE,oldStyle131);SetWindowPos(gameWindow131,nullptr,oldRect131.left,oldRect131.top,oldRect131.right-oldRect131.left,oldRect131.bottom-oldRect131.top,SWP_NOZORDER|SWP_FRAMECHANGED);}
 full131=value;
}
bool ToggleArgument131(std::wistringstream& in,bool old,bool& value){
 std::wstring arg,extra;if(!(in>>arg)){value=!old;return true;}if(in>>extra)return false;
 wht::FoldCommandName(arg);if(arg==L"1"||arg==L"on"||arg==L"true")value=true;
 else if(arg==L"0"||arg==L"off"||arg==L"false")value=false;else return false;return true;
}
void Reload131(){config131=wht::LoadConfig(ini131);wht::toolEnglish.store(config131.english);menu131.nextRefresh=0;ApplyCamera131();}
bool BeginSell131(int selectedSlot=-1){
 if(!(Allowed131()&&!menu131.open&&!macro131.playing&&!macro131.recording&&!dropAllNetwork131.active&&GetForegroundWindow()==gameWindow131&&settings131))return false;
  autoPickEnabled131=false;autoPick131.Reset();wht::SellView131 view;
  const bool started=wht::ReadSellView131(base131,profile131->build,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},view)&&sellAll131.Start(view,GetTickCount64(),selectedSlot);
 wht::Log(folder131,started?"WHT: Sell All queued.":"WHT: Sell All unavailable (selection/pawnable items/active worker).");
 return started;
}
bool SellHovered131(){POINT point{};RECT client{};if(!GetCursorPos(&point)||!ScreenToClient(gameWindow131,&point)||!GetClientRect(gameWindow131,&client))return false;
 const int slot=wht::InventorySlotHit131(point,client.right,client.bottom,[&](unsigned slot){return cooldownProfile131?wht::CooldownButton131(currentUi131,base131,*cooldownProfile131,slot+12):0;});
 if(slot<0)return false;BeginSell131(slot);return true;
}
bool Command131(const std::wstring& text){
 std::wistringstream in(text);std::wstring command;if(!(in>>command))return false;wht::FoldCommandName(command);
 // The settings UI and diagnostics remain usable while a login is pending.
 if(command==L"#toolui"){
  size_t index=0;std::wstring op,value;if(!(in>>index>>op)||index>=wht::kToolMenuControls.size())return false;
  const auto& d=wht::kToolMenuControls[index];if(!d.supported||!wht::ControlAvailable131(d))return false;
  if(op==L"toggle")value=wht::ToolMenuOverlay::ReadValue(config131,index)==L"1"?L"0":L"1";
  else if(op==L"clear")value=L"";else if(op==L"set"){std::getline(in,value);if(!value.empty()&&value[0]==L' ')value.erase(0,1);}else return false;
  if(!wht::ValidMenuValue(d,value))return false;
  if(std::wstring(d.section)==L"Language"){
   const bool english=std::wstring(d.key)==L"English"?value==L"1":value!=L"1";
   wht::SaveSetting(ini131,L"Language",L"English",english?L"1":L"0");
   wht::SaveSetting(ini131,L"Language",L"Vietnamese",english?L"0":L"1");
  }else if(!wht::SaveSetting(ini131,d.section,d.key,value))return false;
  Reload131();return true;
 }
 if(command==L"#tool"||command==L"#menu"){menu131.Toggle();return true;}
 if(!Allowed131()){wht::Log(folder131,"WHT 1.31: command requires Local mode, active match and LoadFileMapWar3 session.");return false;}
 if(command==L"#speed"){std::wstring args;std::getline(in,args);const bool ok=wht::SetSpeed131(args);wht::Log(folder131,ok?"WHT: Game Speed "+std::to_string(wht::speedMultiplier131)+"x":"WHT: #speed requires 1..70; clock hooks unavailable or invalid argument.");return ok;}
 if(command==L"#tanghinh"||command==L"#invis"||command==L"#reveal"||command==L"#revealinvis"){std::wstring arg,extra;in>>arg;if(in>>extra)return false;wht::FoldCommandName(arg);bool next=!config131.revealInvisible;
  if(arg==L"on"||arg==L"1")next=true;else if(arg==L"off"||arg==L"0")next=false;else if(!arg.empty()&&arg!=L"toggle")return false;
  wht::SaveSetting(ini131,L"Event",L"Reveal Invisibles",next?L"1":L"0");Reload131();return true;
 }
 if(command==L"#sellslot"){int slot=0;std::wstring extra;if(!(in>>slot)||slot<1||slot>6||(in>>extra))return false;return BeginSell131(slot-1);}
 if(command==L"#sellall"){std::wstring extra;if(in>>extra)return false;return BeginSell131();}
 if(command==L"#cooldown"){bool next=false;if(!ToggleArgument131(in,config131.showCooldown,next))return false;
  if(!wht::SaveSetting(ini131,L"Event",L"ShowCooldown",next?L"1":L"0"))return false;Reload131();return true;}
 if(command==L"#regenhp"||command==L"#regenmp"){const bool mana=command==L"#regenmp";bool next=false;
  if(!ToggleArgument131(in,mana?config131.showRegenMana:config131.showRegenHP,next))return false;
  if(!wht::SaveSetting(ini131,L"Event",mana?L"ShowRegenMana":L"ShowRegenHP",next?L"1":L"0"))return false;Reload131();return true;}
 int field=0;float value=0;
 if(wht::CameraCommand131(text,field,value)){
  if(!wht::SaveSetting(ini131,L"Event",field==0?L"Camera":L"FogZ",std::to_wstring(value)))return false;
  wht::Log(folder131,"WHT 1.31: camera command accepted, field="+std::to_string(field)+" value="+std::to_string(value));
  Reload131();return true;
 }
 if(command==L"#commandsettings"){bool next=false;if(!ToggleArgument131(in,settings131,next))return false;settings131=next;ApplyCamera131();return true;}
 if(command==L"#full"||command==L"#lock"||command==L"#showmem"){
  bool next=false;if(!ToggleArgument131(in,command==L"#full"?full131:command==L"#lock"?lock131:config131.showMem,next))return false;
  if(command==L"#full")Fullscreen131(next);else if(command==L"#lock"){lock131=next;if(!next)ClipCursor(nullptr);}
  else{wht::SaveSetting(ini131,L"Event",L"Show Mem",next?L"1":L"0");Reload131();}return true;
 }
 std::wstring extra;if(in>>extra)return false;
 if(!config131.macroEnabled)return false;
 if(command==L"#macroplay")macro131.Play();else if(command==L"#macrorecord")macro131.Record();
 else if(command==L"#macrostop")macro131.Stop();else if(command==L"#macroscript")macro131.Edit();else return false;
 return true;
}
std::string AssetPath131(const char* path){
 if(!path||!*path)return path?path:"";
 if(archiveReady131)return path;
 const auto relative=std::filesystem::path(wht::Wide(path));
 if(relative.is_absolute())return path;
 std::error_code error;
 if(std::filesystem::is_regular_file(std::filesystem::path(folder131)/L"WHT-assets"/relative,error))return std::string("WHT-assets\\")+path;
 return path;
}
void __fastcall AssignString131(void* target,DWORD,const char* path){const auto asset=AssetPath131(path);
 reinterpret_cast<void(__thiscall*)(void*,const char*)>(base131+profile131->stringAssign)(target,asset.c_str());}
uint32_t __fastcall SetFont131(void* font,DWORD,const char* path,float height,int flags){
 auto call=reinterpret_cast<uint32_t(__thiscall*)(void*,const char*,float,int)>(base131+profile131->fontSet);
 const auto asset=AssetPath131(path);auto result=call(font,asset.c_str(),height,flags);
 static bool reported=false;if(result&&asset!=path&&!reported){reported=true;wht::Log(folder131,"WHT 1.31: custom font loaded: "+asset);}
 return result?result:call(font,"Fonts\\FRIZQT__.TTF",height,flags);
}
void __fastcall SetTexture131(void* texture,DWORD,const char* path,int flags){
 const auto asset=AssetPath131(path);
 reinterpret_cast<void(__thiscall*)(void*,const char*,int)>(base131+profile131->textureSet)(texture,asset.c_str(),flags);
}
wht::ExtendedFrameApi FrameApi131(){
 wht::nativeFrameLayout={0x120,0xd0,0xf4,0x90,0x9c,0xc0,0xc8,0x68,0x6c,profile131->simpleVtable,true};
 wht::ExtendedFrameApi api{};
#define API131(member,offset) api.member=reinterpret_cast<decltype(api.member)>(base131+profile131->offset)
 API131(alloc,alloc);API131(frameCtor,frameCtor);API131(backdropCtor,backdropCtor);API131(setBackdrop,backdropSet);
 API131(stringAssign,stringAssign);API131(fontCtor,fontCtor);API131(setText,fontText);API131(setTextAlign,fontAlign);
 API131(textureCtor,textureCtor);API131(setWidth,width);API131(setHeight,height);API131(setAbsPoint,absPoint);API131(setRelPoint,relPoint);
#undef API131
 api.setWidth=reinterpret_cast<decltype(api.setWidth)>(Width131);api.setAbsPoint=reinterpret_cast<decltype(api.setAbsPoint)>(Abs131);api.setRelPoint=reinterpret_cast<decltype(api.setRelPoint)>(Rel131);
 api.stringAssign=reinterpret_cast<decltype(api.stringAssign)>(AssignString131);
 api.setFont=reinterpret_cast<decltype(api.setFont)>(SetFont131);api.setTexture=reinterpret_cast<decltype(api.setTexture)>(SetTexture131);return api;
}
bool DestroyHost131(){
 __try {auto host=menu131.independentHost;if(host&&*reinterpret_cast<uintptr_t*>(host)==base131+profile131->simpleVtable){
  auto destructor=*reinterpret_cast<void(__thiscall**)(void*,unsigned)>(*reinterpret_cast<uintptr_t*>(host)+8);destructor(host,1);
 }return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void DestroyUi131(){
 wht::ForgetUiRefresh131();
 if(!DestroyHost131())wht::Log(folder131,"WHT 1.31: native menu cleanup exception.");
 wht::ResetSpeed131();wht::ResetLifeMana131();menu131.Reset();memoryFont131=nullptr;camera131.Reset();cameraSlot131=0;cameraWrittenMask131=0;typed131=false;text131.clear();macro131.Stop();
 placements131.clear();
 cooldown131.Reset();
 hotkeys131.Reset();remap131.Reset();timeOfDay131.Reset();consoleUi131.Reset();blackBar131.Reset();mouseTooltip131.Reset();autoClick131.Reset();pinned131={};
 messageTimers131.Reset();
 resources131.Reset();clock131.Reset();matchStart131.Reset();fps131.Reset();quickDrop131={};dropIcon131.Reset();pickIcon131.Reset();sellIcon131.Reset();reveal131.Reset();sellAll131.Reset();autoPick131.Reset();autoPickEnabled131=false;dropAllNetwork131.Reset();skillsIcon131.Reset();allSkills131.Reset();
 regen131.Reset();
 stateBars131.Reset();
 menuReported131=cameraReported131=writeReported131=false;if(lock131)ClipCursor(nullptr);
}
void PresentGraphics131(){
 if(wht::uiRefreshOwner131!=currentUi131||!currentUi131)return;
 if(config131.lifeManaCommas||config131.lifeManaNoHide||config131.statsCommas||wht::lifeFlags131||!wht::lifeUi131)if(const auto* p=wht::LifeProfileFor131(profile131->build))wht::UpdateLifeMana131(base131,profile131->build,currentUi131,wht::Component131(currentUi131,base131,p->portrait),wht::Component131(currentUi131,base131,cooldownProfile131->infoBar),(Allowed131()&&settings131)?((config131.lifeManaCommas?1u:0u)|(config131.lifeManaNoHide?2u:0u)|(config131.statsCommas?4u:0u)):0u,[](uintptr_t unit,bool mana,float& value,float& maximum){return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,mana,value,maximum);},true);
 for(const auto& item:wht::statPaintRecords131)wht::ApplyStatPaint131(item.first);
 if(Allowed131()&&settings131&&regenProfile131&&stateBarProfile131)stateBars131.Present(base131,*profile131,*regenProfile131,*stateBarProfile131,config131);
}
void UpdateUi131(){
 const auto ui=CurrentUi131();
 if(currentUi131!=ui){DestroyUi131();currentUi131=ui;}
 if(!Allowed131())wht::ResetSpeed131();
 if(!ui){if(lock131)ClipCursor(nullptr);return;}
 wht::InstallUiRefresh131(base131,profile131->build,ui,PresentGraphics131);
 graphicsDiagnostic131.Update(base131,profile131->build,ui,config131,Allowed131(),folder131,consoleUi131,mouseTooltip131,[&](uintptr_t table){return wht::Component131(ui,base131,table);}); wht::UpdateMatchStart131(matchStart131,base131,profile131->build,ui,Allowed131()&&settings131&&config131.soundStartGame,module131,folder131);
 ApplyCamera131();
 if(config131.lifeManaCommas||config131.lifeManaNoHide||config131.statsCommas||wht::lifeFlags131||!wht::lifeUi131)if(const auto* p=wht::LifeProfileFor131(profile131->build))wht::UpdateLifeMana131(base131,profile131->build,currentUi131,wht::Component131(currentUi131,base131,p->portrait),wht::Component131(currentUi131,base131,cooldownProfile131->infoBar),(Allowed131()&&settings131)?((config131.lifeManaCommas?1u:0u)|(config131.lifeManaNoHide?2u:0u)|(config131.statsCommas?4u:0u)):0u,[](uintptr_t unit,bool mana,float& value,float& maximum){return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,mana,value,maximum);});
 if(config131.mouseTooltip||mouseTooltip131.active){const auto* tip=wht::MouseTooltipProfileFor131(profile131->build);const auto* console=wht::ConsoleProfileFor131(profile131->build);
  if(tip&&console)wht::UpdateMouseTooltip131(mouseTooltip131,base131,profile131->build,currentUi131,wht::Component131(currentUi131,base131,tip->table),wht::ConsoleComponent131(currentUi131,base131,console->table),gameWindow131,Allowed131()&&settings131&&!menu131.open&&config131.mouseTooltip);
 }
 if(config131.hideBlackBar||blackBar131.Active())if(const auto* p=wht::BlackBarProfileFor131(profile131->build))blackBar131.Update(base131,profile131->build,currentUi131,wht::Component131(currentUi131,base131,p->worldTable),Allowed131()&&settings131&&config131.hideBlackBar);
 if(config131.hideConsoleUI||consoleUi131.active)if(const auto* p=wht::ConsoleProfileFor131(profile131->build))consoleUi131.Update(base131,profile131->build,wht::ConsoleComponent131(currentUi131,base131,p->table),Allowed131()&&settings131&&config131.hideConsoleUI);
 if(config131.hideTimeOfDay||timeOfDay131.active)if(const auto* p=wht::TimeOfDayProfileFor131(profile131->build))timeOfDay131.Update(base131,profile131->build,wht::Component131(currentUi131,base131,p->table),Allowed131()&&settings131&&config131.hideTimeOfDay);
 wht::UpdateAutoClick131(autoClick131,config131,gameWindow131,Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&!allSkills131.active&&!sellAll131.active&&!dropAllNetwork131.active);
 if(auto p=wht::RevealProfileFor131(profile131->build)){
  if(reveal131.Update(base131,profile131->build,wht::Component131(currentUi131,base131,p->worldTable),Allowed131()&&settings131&&config131.revealInvisible)&&!reveal131.reported){reveal131.reported=true;wht::Log(folder131,"WHT: native invisible-unit display enabled.");}
 }
 if(!MountAssets131())return;
 RECT r{};if(!GetClientRect(gameWindow131,&r)||r.right<=0||r.bottom<=0)return;
 menu131.SetViewport(r.right,r.bottom);
 Viewport131(r.right,r.bottom);
 // Keep the same eight-tab settings catalog and native mouse/key editing.
 menu131.Update(base131,ui,config131,true,&frame131);
 if(menu131.failed){if(!menuReported131){menuReported131=true;wht::Log(folder131,std::string("WHT 1.31: native menu unavailable at ")+menu131.initStage);}return;}
 if(menu131.initialized&&!menuReported131){menuReported131=true;wht::Log(folder131,"WHT 1.31: live native menu construction completed.");}
 if(menu131.initialized&&cooldownProfile131&&cooldown131.Update(base131,ui,*cooldownProfile131,Allowed131()&&settings131&&config131.showCooldown,menu131)&&!cooldown131.reported){
  cooldown131.reported=true;wht::Log(folder131,"WHT 1.31: live cooldown seconds queried from native CAbility virtual getter.");}
 if(menu131.initialized&&cooldownProfile131&&hotkeys131.Due(GetTickCount64(),Allowed131()&&settings131&&config131.showHotkeyInPanel)&&hotkeys131.Update(Allowed131()&&settings131&&config131.showHotkeyInPanel,menu131,[&](unsigned slot){return wht::CooldownButton131(ui,base131,*cooldownProfile131,slot);},&config131,(Allowed131()&&settings131&&config131.showHotkeyInPanel)?InventoryMask131(true):0,(Allowed131()&&settings131&&config131.showHotkeyInPanel)?AlternatePanel131():false)&&!hotkeys131.reported){
  hotkeys131.reported=true;wht::Log(folder131,"WHT 1.31: native panel hotkey labels displayed.");}
 if(menu131.initialized&&regenProfile131&&regen131.Update(base131,ui,*regenProfile131,config131,Allowed131()&&settings131,menu131)&&!regen131.reported){
  regen131.reported=true;wht::Log(folder131,"WHT 1.31: live selected-unit Float regeneration resolved and displayed.");}
 if(menu131.initialized&&regenProfile131&&stateBarProfile131){
  auto raw=frame131;
  raw.setWidth=reinterpret_cast<decltype(raw.setWidth)>(base131+profile131->width);
  raw.setAbsPoint=reinterpret_cast<decltype(raw.setAbsPoint)>(base131+profile131->absPoint);
  raw.setRelPoint=reinterpret_cast<decltype(raw.setRelPoint)>(base131+profile131->relPoint);
  const auto observed=stateBars131.Update(base131,menu131.independentHost,*profile131,*regenProfile131,*stateBarProfile131,config131,Allowed131()&&settings131,raw);
  if((observed&1)&&!stateBars131.manaReported){stateBars131.manaReported=true;wht::Log(folder131,"WHT 1.31: native overhead mana bar bound to live CUnit.");}
  if((observed&2)&&!stateBars131.stateReported){stateBars131.stateReported=true;wht::Log(folder131,"WHT 1.31: overhead HP/Mana native values displayed.");}
  if((observed&4)&&!stateBars131.levelReported){stateBars131.levelReported=true;wht::Log(folder131,"WHT 1.31: overhead hero level queried from native CAbilityHero and displayed.");}
 }
 if(menu131.initialized&&!memoryFont131){memoryFont131=wht::NativeCreateFontString(frame131,menu131.independentHost,.012f,"",4);
  if(memoryFont131){frame131.setWidth(memoryFont131,.36f);frame131.setHeight(memoryFont131,.018f);frame131.setAbsPoint(memoryFont131,0,.01f,.59f,1);}}
 if(memoryFont131){float mb=0,percent=0;std::string label="|cffffcc00WHT " WHT_VERSION_STRING " | 1.31.1."+std::to_string(profile131->build)+"|r";
  if(!authorized131.load())label+=" |cffff4444Login LoadFileMapWar3|r";
  else if(config131.showMem&&wht::GetCurrentProcessMemoryUsage(mb,percent))label+=" | "+std::to_string(int(mb))+" MB";
  frame131.setText(memoryFont131,label.c_str());}
 menu131.PlaybackVisual(macro131.playing);macro131.Tick(Allowed131()&&!typed131&&!menu131.open);
 if(menu131.initialized&&resources131.Update(base131,profile131->build,Allowed131()&&settings131&&config131.showResource,GetTickCount64(),menu131,folder131)&&!resources131.reported){resources131.reported=true;wht::Log(folder131,"WHT 1.31: native resource table displayed.");}
 if(clock131.Update(base131,profile131->build,Allowed131()&&settings131&&config131.showTimer,menu131)&&!clock131.reported){clock131.reported=true;wht::Log(folder131,"WHT 1.31: native simulation timer displayed.");}
 clock131.Diagnose(base131,profile131->build,Allowed131()&&settings131&&config131.showTimer,folder131);
 if(sellAll131.active){wht::SellView131 view;bool valid=Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&GetForegroundWindow()==gameWindow131&&wht::ReadSellView131(base131,profile131->build,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},view);
  if(valid&&sellAll131.scanning){auto p=wht::PickProfileFor131(profile131->build);valid=p&&wht::ScanSellShops131(base131,profile131->build,wht::Component131(currentUi131,base131,p->worldTable),sellAll131.scanCursor,view,[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;});}
  const auto result=sellAll131.Step(valid?&view:nullptr,GetTickCount64(),[&](const wht::DropSnapshot& source,const wht::DropItem& item,const wht::SellShop131& shop){return wht::SendSell131(base131,profile131->build,source,item,shop,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;});});
  if(result==wht::SellResult131::Submitted)wht::Log(folder131,"WHT: Sell All synchronized shop order submitted.");
  else if(result!=wht::SellResult131::Working&&result!=wht::SellResult131::Idle)wht::Log(folder131,std::string("WHT: Sell All ")+(result==wht::SellResult131::Complete?"completed":result==wht::SellResult131::NoShop?"no accessible purchase shop":result==wht::SellResult131::Cancelled?"cancelled":"remaining")+"; sold="+std::to_string(sellAll131.sold)+" skipped="+std::to_string(sellAll131.skipped)+" remaining="+std::to_string(sellAll131.remaining)+" gold delta="+std::to_string(sellAll131.goldDelta));
 }
 sellIcon131.latched=sellAll131.active;sellIcon131.Update(base131,Allowed131()&&settings131&&config131.showSellAll,menu131);
 if(!config131.showAutoPick)autoPickEnabled131=false;
 if(autoPickEnabled131&&(Allowed131()&&settings131&&config131.showAutoPick&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&GetForegroundWindow()==gameWindow131)&&!dropAllNetwork131.active){const auto now=GetTickCount64();
  if(now>=autoPick131.next){auto p=wht::PickProfileFor131(profile131->build);wht::PickSnapshot131 snapshot;
   const bool valid=p&&wht::ReadPickSnapshot131(base131,profile131->build,wht::Component131(currentUi131,base131,p->worldTable),[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},snapshot);
   const auto result=autoPick131.Tick(now,valid?&snapshot:nullptr,[&](const wht::DropSnapshot& source,const wht::PickCandidate131& item){return wht::SendPick131(base131,profile131->build,source,item,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;});});
   if(result==wht::PickResult131::Submitted)wht::Log(folder131,"WHT: Auto Pick native target order submitted.");
   if(result==wht::PickResult131::Cancelled){autoPickEnabled131=false;wht::Log(folder131,"WHT: Auto Pick stopped (selection/session/native validation).");}
  }
 }else autoPick131.Reset();
 pickIcon131.latched=autoPickEnabled131;pickIcon131.Update(base131,Allowed131()&&settings131&&config131.showAutoPick,menu131);
 dropIcon131.Update(base131,Allowed131()&&settings131&&config131.showDropAll,menu131);
 if(dropAllNetwork131.active){wht::DropSnapshot snapshot;const bool valid=Allowed131()&&settings131&&config131.showDropAll&&!sellAll131.active&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&GetForegroundWindow()==gameWindow131&&wht::DropSnapshot131(base131,profile131->build,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},snapshot);
  const auto result=dropAllNetwork131.Step(valid?&snapshot:nullptr,GetTickCount64(),[&](const wht::DropSnapshot& source,const wht::DropItem& item){return wht::SendDropSnapshot131(base131,profile131->build,source,item);});
  if(result==wht::DropResult::Complete)wht::Log(folder131,"WHT: synchronized drop all completed; original items absent from inventory.");
  else if(result==wht::DropResult::Cancelled)wht::Log(folder131,"WHT: synchronized drop all cancelled.");
  else if(result==wht::DropResult::Remaining)wht::Log(folder131,"WHT: synchronized drop all stopped; remaining "+std::to_string(dropAllNetwork131.remaining));
 }
 skillsIcon131.Update(base131,Allowed131()&&settings131&&config131.showInsAllSkills,menu131);
 if(allSkills131.Tick(GetTickCount64(),Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&GetForegroundWindow()==gameWindow131,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},SkillPoints131,SkillData131,ClickSkill131))wht::Log(folder131,"WHT 1.31: all skills native sequence finished; submitted "+std::to_string(allSkills131.submitted));
 if(fps131.Update(base131,ui,profile131->build,Allowed131()&&settings131,config131.showFps)&&!fps131.reported){fps131.reported=true;wht::Log(folder131,"WHT 1.31: native FPS toggle installed.");}
 const auto cleared=messageTimers131.Update(base131,ui,profile131->build,config131,Allowed131()&&settings131&&!typed131,GetTickCount64());
 const auto fresh=cleared&~messageTimers131.reported;
 if(fresh&1)wht::Log(folder131,"WHT 1.31: periodic native text clear executed.");
 if(fresh&2)wht::Log(folder131,"WHT 1.31: periodic native chat clear executed.");
 messageTimers131.reported|=cleared;
 if(lock131){if(GetForegroundWindow()==gameWindow131&&!menu131.open){POINT tl{0,0},br{r.right,r.bottom};ClientToScreen(gameWindow131,&tl);ClientToScreen(gameWindow131,&br);RECT clip{tl.x,tl.y,br.x,br.y};ClipCursor(&clip);}else ClipCursor(nullptr);}
}
void ReportUpdateException131(unsigned code){static unsigned errors=0;if(errors++<4)wht::Log(folder131,"WHT UI update exception="+std::to_string(code));}
void SafeUpdate131(){__try{UpdateUi131();}__except(EXCEPTION_EXECUTE_HANDLER){ReportUpdateException131(GetExceptionCode());menu131.failed=true;}}
void SafeReload131(){__try{Reload131();}__except(EXCEPTION_EXECUTE_HANDLER){menu131.failed=true;}}
void CALLBACK Tick131(HWND,UINT,UINT_PTR,DWORD){
 if(ticking131)return;ticking131=true;
 if(!tickReported131){tickReported131=true;wht::Log(folder131,"WHT 1.31: UI thread update pump active.");}
 const auto now=GetTickCount64();if(now>=configNext131){configNext131=now+2000;SafeReload131();}
 if(now>=updateNext131){updateNext131=now+33;SafeUpdate131();}
 ticking131=false;
}
bool Input131(MSG& msg){
 if(!ready131.load()||GetForegroundWindow()!=gameWindow131)return false;
 const bool keyDown=msg.message==WM_KEYDOWN||msg.message==WM_SYSKEYDOWN;
 const auto key=unsigned(msg.wParam);const bool repeat=bool(msg.lParam&(1L<<30));
 if((msg.message==WM_KEYUP||msg.message==WM_SYSKEYUP)&&remap131.Input(msg.message,key,repeat,false,false,config131,[](unsigned){return false;}))return true;
 auto queue=[](const wchar_t* command){Command131(command);};
 RECT r{};GetClientRect(gameWindow131,&r);
 if(skillsIcon131.Mouse(msg.message,{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))},r.right,r.bottom,Allowed131()&&settings131&&config131.showInsAllSkills&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording,[&](){
  const auto started=allSkills131.Start(GetTickCount64(),[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},SkillPoints131,SkillData131);
  wht::Log(folder131,started?"WHT 1.31: all skills sequence queued.":"WHT 1.31: all skills unavailable (hero/points/menu).");
 }))return true;
 if(pickIcon131.Mouse(msg.message,{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))},r.right,r.bottom,Allowed131()&&settings131&&config131.showAutoPick&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&GetForegroundWindow()==gameWindow131&&!dropAllNetwork131.active&&!sellAll131.active,[&](){autoPickEnabled131=!autoPickEnabled131;autoPick131.Reset();wht::Log(folder131,autoPickEnabled131?"WHT: Auto Pick enabled.":"WHT: Auto Pick disabled.");}))return true;
 if(sellIcon131.Mouse(msg.message,{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))},r.right,r.bottom,Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&GetForegroundWindow()==gameWindow131&&!dropAllNetwork131.active,[&](){
  BeginSell131();
 }))return true;
 if(dropIcon131.Mouse(msg.message,{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))},r.right,r.bottom,Allowed131()&&settings131&&config131.showDropAll&&!sellAll131.active&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording,[&](){
  const auto* p=wht::InventoryProfile(profile131->build);if(!p||!regenProfile131||!stateBarProfile131)return;
  wht::DropSnapshot snapshot;const char* diagnostic="unknown";
  const bool started=wht::DropSnapshot131(base131,profile131->build,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},snapshot,&diagnostic)&&dropAllNetwork131.Start(snapshot,GetTickCount64());
  wht::Log(folder131,started?"WHT: synchronized drop all queued.":std::string("WHT: drop all refused at ")+diagnostic+(dropAllNetwork131.active?" (already active)":""));
 }))return true;
 if(quickDrop131.Mouse(msg.message,Allowed131()&&settings131&&config131.quickDropItem&&!sellAll131.active&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000),[&](){
  if(!cooldownProfile131)return -1;POINT point{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))};
  return wht::InventorySlotHit131(point,r.right,r.bottom,[&](unsigned slot){return wht::CooldownButton131(currentUi131,base131,*cooldownProfile131,slot+12);});
 },[&](unsigned slot){
  const auto* p=wht::InventoryProfile(profile131->build);if(!p||!regenProfile131||!stateBarProfile131)return;
  const auto* networkProfile=wht::DropNetworkProfileFor131(profile131->build);if(!networkProfile)return;
  const auto network=wht::DropNetworkNative131(base131,*networkProfile,*p);
  const auto ok=wht::QuickDropNetwork131(base131,*p,wht::InventoryNative131(base131,*p),network,slot,[&](){return wht::SelectedUnit131(currentUi131,base131,*regenProfile131);},[&](uintptr_t unit){float value=0,maximum=0;return wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},[&](uintptr_t unit){return wht::DropOwned131(base131,profile131->build,network,unit);});
  wht::Log(folder131,ok?"WHT 1.31: quick drop synchronized action submitted.":"WHT 1.31: quick drop refused or slot empty.");
 }))return true;
 if(msg.message==WM_LBUTTONDOWN||msg.message==WM_LBUTTONUP||msg.message==WM_RBUTTONDOWN||msg.message==WM_RBUTTONUP){
  POINT p{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))};
  if(menu131.Mouse(msg.message,p,r.right,r.bottom,queue,&config131))return true;
 }
 if(keyDown&&menu131.ToggleKey(key,config131.menuKey,repeat,typed131))return true;
 if(menu131.visible&&menu131.open){
  if(keyDown&&!repeat&&key==VK_ESCAPE){menu131.Escape();return true;}
  if(keyDown&&menu131.Key(key,unsigned((msg.lParam>>16)&255),queue))return true;
  if(msg.message==WM_CHAR||keyDown||msg.message==WM_KEYUP)return true;
 }
 if(keyDown&&!repeat&&key==VK_ESCAPE){typed131=false;text131.clear();if(menu131.Escape())return true;}

 if(keyDown&&!repeat&&key==VK_RETURN){
  if(typed131){if(!text131.empty()&&text131[0]==L'#')Command131(text131);typed131=false;text131.clear();}
  else{typed131=true;text131.clear();}return false;
 }
 // Keep chat capture synchronized across transient session failures. The
 // command itself still checks authorization before changing gameplay.
 if(msg.message==WM_CHAR){const wchar_t ch=wchar_t(msg.wParam);
  if(ch==L'#'){typed131=true;text131=L"#";return false;}
  if(typed131){if(ch==8){if(!text131.empty())text131.pop_back();}else if(ch>=32&&text131.size()<1024)text131+=ch;return false;}}
 if(typed131)return false;
 if(!Allowed131())return false;
 if(keyDown&&!repeat&&key==VK_TAB&&settings131&&config131.showResource&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000))return resources131.Toggle();
 if(keyDown&&!repeat&&config131.macroEnabled){
  const bool chord=(GetKeyState(VK_CONTROL)&0x8000)&&(GetKeyState(VK_SHIFT)&0x8000);
  if((config131.macroKeys[0]&&key==config131.macroKeys[0])||(chord&&key==VK_F10)){macro131.Play();return true;}
  if((config131.macroKeys[1]&&key==config131.macroKeys[1])||(chord&&key==VK_F12)){macro131.Stop();return true;}
  if((config131.macroKeys[2]&&key==config131.macroKeys[2])||(chord&&key==VK_F9)){macro131.Record();return true;}
  if((config131.macroKeys[3]&&key==config131.macroKeys[3])||(chord&&key==VK_F11)){macro131.Edit();return true;}
 }
 if(keyDown&&!repeat&&key==config131.commandKey){settings131=!settings131;ApplyCamera131();return true;}
 if(keyDown&&!repeat&&key&&key==config131.multiboardKey&&Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&!allSkills131.active&&GetForegroundWindow()==gameWindow131&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const auto* p=wht::MultiboardProfileFor131(profile131->build);
  if(p&&wht::ToggleMultiboard131(base131,profile131->build,wht::Component131(currentUi131,base131,p->table))){if(key<256)remap131.pressed[key]=true;return true;}
 }
 if(keyDown&&!repeat&&Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&!allSkills131.active&&GetForegroundWindow()==gameWindow131&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const int slot=wht::HeroKeySlot131(unsigned(key),config131);const auto* p=wht::HeroProfileFor131(profile131->build);
  if(slot>=0&&p&&wht::ClickHero131(base131,profile131->build,wht::Component131(currentUi131,base131,p->bar),unsigned(slot))){if(key<256)remap131.pressed[key]=true;return true;}
 }
 if(keyDown&&!repeat&&Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&!allSkills131.active&&GetForegroundWindow()==gameWindow131&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)&&autoClick131.Key(unsigned(key),config131)){if(key<256)remap131.pressed[key]=true;wht::Log(folder131,std::string("WHT: Auto Click ")+(key==config131.autoClickLeftKey?"Left: ":"Right: ")+((key==config131.autoClickLeftKey?autoClick131.left:autoClick131.right)?"ON":"OFF"));return true;}
 if(keyDown&&!repeat&&key&&key==config131.sellFastKey&&Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&!allSkills131.active&&GetForegroundWindow()==gameWindow131&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  if(SellHovered131()){if(key<256)remap131.pressed[key]=true;return true;}
 }
 if(keyDown&&!repeat&&key&&key==config131.setIdTargetKey&&Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&!allSkills131.active&&GetForegroundWindow()==gameWindow131&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const bool ok=PinTarget131();if(key<256)remap131.pressed[key]=true;wht::Log(folder131,ok?"WHT: selected native unit pinned for target cast.":"WHT: no live target selected; pin cleared.");return true;
 }
 if(remap131.Input(msg.message,unsigned(key),repeat,Allowed131()&&settings131&&!typed131&&!menu131.open&&!macro131.playing&&!macro131.recording&&!allSkills131.active&&GetForegroundWindow()==gameWindow131,(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_MENU)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000),config131,[&](unsigned slot){
  const auto button=cooldownProfile131?wht::CooldownButton131(currentUi131,base131,*cooldownProfile131,slot):0;
  if(slot>=12&&!(InventoryMask131()&(1u<<(slot-12))))return false;
  const auto data=wht::SkillButtonData131(button);
  const bool shop=slot<12&&AlternatePanel131();if(shop)return (data.order||data.ability)&&ClickSkill131(slot);
  if(!data.order)return false;
  const auto& binding=slot>=12?config131.inventory[slot-12]:config131.command[slot];
  const auto target=[&](){const auto* p=wht::QuickCastProfile(profile131->build);return p?wht::QuickCastTarget131(base131,currentUi131,profile131->uiVtable,*p):uintptr_t(0);};
  if(binding.autoTargetMe||binding.autoTarget){
   const auto api=wht::SelfCastNative131(base131,profile131->build);if(!api)return false;
   const auto read=[&](wht::SelfCastSource131& source){return wht::ReadSelfCastSource131(base131,profile131->build,currentUi131,[&](){return regenProfile131?wht::SelectedUnit131(currentUi131,base131,*regenProfile131):0;},[&](uintptr_t unit){float value=0,maximum=0;return regenProfile131&&stateBarProfile131&&wht::ReadState131(unit,base131,*regenProfile131,*profile131,*stateBarProfile131,false,value,maximum)&&value>.405f;},source);};
   if(binding.autoTarget&&!binding.autoTargetMe)return wht::ActivateTargetCast131(read,ReadPinned131,target,[&](){return ClickSkill131(slot);},wht::SelfCastOrder131,[&](const wht::SelfCastSource131& destination,uintptr_t mode,unsigned order){return wht::FinishSelfCast131(api,destination,mode,order);});
   return wht::ActivateSelfCast131(read,target,[&](){return ClickSkill131(slot);},wht::SelfCastOrder131,[&](const wht::SelfCastSource131& source,uintptr_t mode,unsigned order){return wht::FinishSelfCast131(api,source,mode,order);});
  }
  return wht::ActivateQuickCast131(binding.quickCast,target,[&](){return ClickSkill131(slot);},[&](uintptr_t mode){return wht::QuickCastCursor131(gameWindow131,mode,target);});
 },AlternatePanel131,LearnSlot131))return true;
 macro131.Capture(msg,!menu131.open);return false;
}
LRESULT CALLBACK Message131(int code,WPARAM removed,LPARAM argument){
 if(code>=0&&removed==PM_REMOVE){auto& msg=*reinterpret_cast<MSG*>(argument);
  if(!timer131)timer131=SetTimer(nullptr,0,16,Tick131);
  // Warcraft can filter WM_TIMER rather than DispatchMessage its callback.
  // Run the throttled tick on the owning UI thread when a message is removed.
  Tick131(nullptr,0,0,0);
  if(msg.message==WM_TIMER&&!msg.hwnd&&msg.wParam==timer131){msg.message=WM_NULL;msg.wParam=msg.lParam=0;}
  if(Input131(msg)){msg.message=WM_NULL;msg.wParam=msg.lParam=0;}
 }
 return CallNextHookEx(messageHook131,code,removed,argument);
}
LRESULT CALLBACK Window131(HWND window,UINT message,WPARAM key,LPARAM data){
 if(message==updateMessage131){Tick131(nullptr,0,0,0);return 0;}
 MSG input{};input.hwnd=window;input.message=message;input.wParam=key;input.lParam=data;
 if(Input131(input))return 0;
 const auto result=CallWindowProcW(windowProc131,window,message,key,data);
 return result;
}
BOOL CALLBACK FindGame131(HWND window,LPARAM){
 DWORD pid=0;GetWindowThreadProcessId(window,&pid);if(pid!=GetCurrentProcessId()||GetWindow(window,GW_OWNER))return TRUE;
 wchar_t name[256]{};GetClassNameW(window,name,256);
 // 1.31 also uses Blizzard's OS-window class; exclude boot/error dialogs.
 RECT client{};GetClientRect(window,&client);
 if(wcsstr(name,L"Warcraft")||wcsstr(name,L"War3")||
    (wcscmp(name,L"#32770")&&wcscmp(name,L"ConsoleWindowClass")&&client.right>=640&&client.bottom>=480)){
  gameWindow131=window;return FALSE;
 }return TRUE;
}
bool ProfileReady131(){
 __try{
  const BYTE cam[]={0x55,0x8b,0xec,0xf3,0x0f,0x10,0x45,0x10,0x0f,0x57,0xd2,0xf3,0x0f,0x5c,0xc2};
  if(memcmp(reinterpret_cast<void*>(base131+profile131->cameraSet),cam,sizeof(cam)))return false;
  const auto table=base131+profile131->simpleVtable;
  if(*reinterpret_cast<uintptr_t*>(table+0xc)!=base131+profile131->width-0x40)return false;
  const BYTE getter[]={0x55,0x8b,0xec,0x83,0xec,0x0c,0x56,0xff,0x71,0x0c,0xff,0x71,0x08};
  if(memcmp(reinterpret_cast<void*>(base131+profile131->readFloat),getter,sizeof(getter)))return false;
  const BYTE abilityGetter[]={0x8b,0x81,0x74,0x2d,0,0,0xc3};
  if(!cooldownProfile131||memcmp(reinterpret_cast<void*>(base131+cooldownProfile131->abilityGetter),abilityGetter,sizeof(abilityGetter)))return false;
  int relative=0;memcpy(&relative,reinterpret_cast<void*>(base131+profile131->readFloat+14),4);
  if(!regenProfile131||base131+profile131->readFloat+18+relative!=base131+regenProfile131->resolve)return false;
  if(!stateBarProfile131||*reinterpret_cast<uintptr_t*>(base131+stateBarProfile131->bar+0x78)!=base131+stateBarProfile131->bind)return false;
  const BYTE bind[]={0x55,0x8b,0xec,0x83,0xec,0x1c,0x56,0x57,0x8b,0x7d,0x08,0x8b,0xf1};
  if(memcmp(reinterpret_cast<void*>(base131+stateBarProfile131->bind),bind,sizeof(bind)))return false;
  const BYTE maxRead[]={0x8b,0x88,0x88,0,0,0};
  if(memcmp(reinterpret_cast<void*>(base131+stateBarProfile131->maximum+0x2b),maxRead,sizeof(maxRead)))return false;
  const BYTE levelGetter[]={0x56,0x8b,0xf1,0x8b,0x8e,0,0,0,0,0x85,0xc9,0x74,0x0b,0x83,0xc1,0x70};
  BYTE expectedLevel[sizeof(levelGetter)];memcpy(expectedLevel,levelGetter,sizeof(levelGetter));memcpy(expectedLevel+5,&stateBarProfile131->heroMember,4);
  if(memcmp(reinterpret_cast<void*>(base131+stateBarProfile131->getLevel),expectedLevel,sizeof(expectedLevel)))return false;
  const BYTE textWidth[]={0x55,0x8b,0xec,0x51,0x56,0x8b,0xf1,0xe8};
  if(memcmp(reinterpret_cast<void*>(base131+stateBarProfile131->textWidth),textWidth,sizeof(textWidth)))return false;
  const BYTE solidColor[]={0x55,0x8b,0xec,0x53,0x56,0x8b,0xf1,0xb3,0x01,0x8b,0x86,0x98,0,0,0};
  if(memcmp(reinterpret_cast<void*>(base131+stateBarProfile131->solidColor),solidColor,sizeof(solidColor)))return false;
  for(auto rva:{profile131->uiGet,profile131->alloc,profile131->frameCtor,profile131->fontCtor,profile131->fontSet,profile131->fontText,profile131->textureCtor,profile131->textureSet,profile131->backdropCtor,profile131->backdropSet,profile131->stringAssign,profile131->width,profile131->height,profile131->absPoint,profile131->relPoint,profile131->archiveOpen,profile131->fileInfo})
   if(memcmp(reinterpret_cast<void*>(base131+rva),"\x55\x8b\xec",3))return false;
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
DWORD WINAPI Auth131(void*){
 wht::StartUpdateWatcher(folder131);
 bool first=true;
 while(true){wht::SessionCheck result;auto identity=wht::VerifySession(folder131,&result);
  {std::lock_guard<std::mutex> guard(wht::sessionMutex);wht::sessionIdentity=identity;}
  const bool ok=!identity.username.empty()&&wht::SessionReady()&&!wht::StartupUpdatePending();
  if(authorized131.exchange(ok)!=ok||first)wht::Log(folder131,ok?"WHT 1.31: session verified.":"WHT 1.31: session unavailable; gameplay suspended.");
  first=false;
  Sleep(5000);
 }
}
DWORD WINAPI Install131(void* parameter){
 module131=static_cast<HMODULE>(parameter);folder131=wht::Directory(module131);ini131=folder131+L"\\WHT.ini";
 wchar_t exe[32768]{};GetModuleFileNameW(nullptr,exe,32768);profile131=wht::FindProfile131(wht::HashFile(exe));
 if(!profile131){wht::Log(folder131,"WHT 1.31: unsupported executable hash; no hooks installed.");return 1;}
 cooldownProfile131=&wht::cooldownProfiles131[profile131->build==12164?0:1];
 regenProfile131=&wht::regenProfiles131[profile131->build==12164?0:1];
 stateBarProfile131=&wht::stateBarProfiles131[profile131->build==12164?0:1];
 base131=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));config131=wht::LoadConfig(ini131);wht::toolEnglish.store(config131.english);
 for(unsigned attempt=0;attempt<900&&!ProfileReady131();++attempt)Sleep(100);
 if(!ProfileReady131()){wht::Log(folder131,"WHT 1.31: exact native profile validation failed; no hooks installed.");return 2;}
 wht::Log(folder131,"WHT 1.31.1."+std::to_string(profile131->build)+": exact x86 camera/UI profile validated.");
 frame131=FrameApi131();menu131.controlAvailable=wht::ControlAvailable131;
 originalCamera131=reinterpret_cast<wht::CameraSetter131>(base131+profile131->cameraSet);
 for(unsigned attempt=0;attempt<3000&&!gameWindow131;++attempt){EnumWindows(FindGame131,0);if(!gameWindow131)Sleep(100);}
 if(!gameWindow131){wht::Log(folder131,"WHT 1.31: no game window; native profile validated, input pending.");return 4;}
 macro131.Configure(gameWindow131,folder131);
 SetLastError(0);
 windowProc131=reinterpret_cast<WNDPROC>(SetWindowLongPtrW(gameWindow131,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(Window131)));
 if(!windowProc131){
  wht::Log(folder131,"WHT 1.31: window callback install failed: win32="+std::to_string(GetLastError()));return 5;
 }
 ready131=true;PostMessageW(gameWindow131,WM_NULL,0,0);
 HANDLE auth=CreateThread(nullptr,0,Auth131,nullptr,0,nullptr);if(auth)CloseHandle(auth);
 wht::Log(folder131,"WHT 1.31.1."+std::to_string(profile131->build)+": window callback installed; native gameplay awaiting live UI update.");
 // SendMessage is delivered on the window's owning thread even when the
 // game's queue loop filters timers and bypasses WH_GETMESSAGE hooks.
 while(IsWindow(gameWindow131)){
  DWORD_PTR result=0;SendMessageTimeoutW(gameWindow131,updateMessage131,0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,100,&result);Sleep(33);
 }
 return 0;
}
}
extern "C" __declspec(dllexport) DWORD __stdcall WHT_RebuildVersion(){return (WHT_VERSION_MAJOR<<24)|(WHT_VERSION_MINOR<<16)|(WHT_VERSION_PATCH<<8)|WHT_VERSION_BUILD;}
extern "C" __declspec(dllexport) DWORD __stdcall WHT131_Status(){return (profile131?1u:0u)|(ready131.load()?2u:0u)|(authorized131.load()?4u:0u);}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,void*){
 if(reason==DLL_PROCESS_ATTACH){DisableThreadLibraryCalls(instance);HANDLE thread=CreateThread(nullptr,0,Install131,instance,0,nullptr);if(thread)CloseHandle(thread);}return TRUE;
}
