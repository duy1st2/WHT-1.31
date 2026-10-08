#include "common.hpp"
#include "native_ui129.hpp"
#include "macro_recorder.hpp"
#include "memory_overlay.hpp"
#include "services129.hpp"
#include "resource129.hpp"
#include "resource_board129.hpp"
#include "skills129.hpp"
#include "life_mana129.hpp"
#include "stats129.hpp"
#include "hotkey_labels129.hpp"
#include "quickcast129.hpp"
#include "mouse_tooltip129.hpp"
#include "black_bar129.hpp"
#include "show_level129.hpp"
#include "hp_dimensions129.hpp"
#include "regen129.hpp"
#include "state_values129.hpp"
#include "mana_bar129.hpp"
#include "hp_color129.hpp"
#include "invulnerable_bar129.hpp"
#include "fog_effect129.hpp"
#include "scroll129.hpp"
#include "vertex_color129.hpp"
#include "local_files129.hpp"
#include "fast_menu129.hpp"
#include <intrin.h>
#include "match_timer.hpp"
#include <MinHook.h>
#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <mmsystem.h>
#include <set>
namespace {
using TextFn=void(__thiscall*)(uintptr_t,const char*);
TextFn original=nullptr;
wht::FontText129 originalLifeText129=nullptr;
wht::LifeMana129 lifeMana129;
wht::Stats129 stats129;
wht::HotkeyLabels129 hotkeyLabels129;
wht::MouseTooltip129 mouseTooltip129;
wht::BlackBar129 blackBar129;
wht::ShowLevel129 showLevel129;
wht::HpDimensions129 hpDimensions129;
wht::Regen129 regen129;
wht::StateValues129 stateValues129;
wht::ManaBar129 manaBar129;
wht::HpColor129 hpColor129;
wht::InvulnerableBar129 invulnerableBar129;
wht::BarVisibility129 originalBarShow129=nullptr;
bool invulnerableHookReady129=false;
bool levelHooksReady129=false;
using BarBind129=void(__thiscall*)(uintptr_t,uintptr_t);
using BarDestroy129=uintptr_t(__thiscall*)(uintptr_t,unsigned);
BarBind129 originalBarBind129=nullptr;
BarDestroy129 originalBarDestroy129=nullptr;
uintptr_t label=0;
HWND window=nullptr;
std::string last;
UINT_PTR timer=0;
BOOL CALLBACK FindWindow(HWND h,LPARAM){DWORD pid=0;GetWindowThreadProcessId(h,&pid);if(pid==GetCurrentProcessId()&&IsWindowVisible(h)&&!GetWindow(h,GW_OWNER)){window=h;return FALSE;}return TRUE;}
std::string Display(){
 if(!window||!IsWindow(window))EnumWindows(FindWindow,0);
 RECT r{};if(!GetClientRect(window,&r)||r.right<=0||r.bottom<=0)return {};
 return "|cffff0000Sorry|r|cffff69b4Love|r_|cffffff00WHT|r |cff00ff00("+std::to_string(r.right)+" x "+std::to_string(r.bottom)+")|r | 1.29.2.9231 |cff00ff00(1.29b)|r";
}
bool Matches(uintptr_t object,const char* expected){
 __try{const char* text=*reinterpret_cast<const char**>(object+0x1f8);return text&&strcmp(text,expected)==0;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

uintptr_t executableBase=0;
using VertexColorFn129=uintptr_t(__thiscall*)(uintptr_t);
VertexColorFn129 originalVertexColor129=nullptr;
HMODULE runtimeModule=nullptr;
wht::MatchStart129 matchStart;
decltype(&PlaySoundW) startSoundPlayer129=PlaySoundW;
wht::LocalFilesWriter129 localFilesWriter129=wht::WriteLocalFiles129;
wht::MenuTransition129 originalMenuTransition129=nullptr;
wht::FastMenuAnimation129 fastMenuAnimation129;

std::set<std::wstring> loadedLibraries129,mountedArchives129;
std::vector<HANDLE> extraArchives129;
bool pluginsStarted129=false,pluginsLoading129=false;
std::vector<HMODULE> extraLibraries129;

std::wstring runtimeIni,runtimeFolder;
wht::Config runtimeConfig;
float __cdecl MenuTransitionHook129(unsigned kind,unsigned sequence,unsigned a,unsigned b,uintptr_t callback,uintptr_t context){
 const bool enabled=runtimeConfig.mode==wht::RunMode::Local&&runtimeConfig.fastMenu;
 if(!enabled)fastMenuAnimation129.Apply(executableBase,false);
 const float duration=wht::RunMenuTransition129(enabled,originalMenuTransition129,kind,sequence,a,b,callback,context);
 fastMenuAnimation129.Apply(executableBase,enabled);
 return duration;
}
uintptr_t __fastcall VertexColorNative129(uintptr_t unit,DWORD){
 wht::ScopedVertexAlpha129 alpha(executableBase,unit,runtimeConfig.mode==wht::RunMode::Local&&runtimeConfig.vertexColor);
 return originalVertexColor129(unit);
}

wht::ToolMenuOverlay toolMenu;
wht::ExtendedFrameApi frameApi;
wht::MacroRecorder macroRecorder;
HHOOK inputHook=nullptr;
HANDLE resourceArchive=nullptr;bool resourceFailed=false;
std::wstring mixPath;
void* memoryFrame=nullptr;void* memoryFont=nullptr;ULONGLONG memoryNext=0;
void* clockFrame=nullptr;void* clockFont=nullptr;
wht::ClearFrame129 clearFrame=nullptr;
wht::MessageTimers129 messageTimers;
wht::AutoClick129 autoClick;
wht::WindowServices129 windowServices;
wht::FpsState129 fpsState;
wht::TimeOfDay129 timeOfDay;
wht::ConsoleState129 consoleState;
wht::ClearFrame129 showConsole=nullptr,hideConsole=nullptr;
wht::FpsMode129 fpsMode=nullptr;
wht::ClearFrame129 showFdf=nullptr,hideFdf=nullptr;
wht::ClickFn129 clickButton=nullptr;
wht::ClearFrame129 minimizeBoard=nullptr,maximizeBoard=nullptr;
wht::AllSkills129 allSkills;
wht::SmartCast129 smartCast129;
void* skillsIconFrame=nullptr;void* skillsIcon=nullptr;bool skillsIconShown=false,skillsIconPressed=false;
wht::ToolMenuRect skillsIconRect{.563f,.012f,.016f,.016f};
wht::QuickChatApi129 quickChatApi;
wht::ResourceApi129 resourceApi;
wht::ResourceBoard129 resourceBoard;ULONGLONG resourceNext=0;
bool resourceExpanded=true;
bool chatTyping=false;
std::array<bool,256> consumedKeys{};
void EnsureInput();
void UpdateTools();
void StartPlugins129();
bool ToolCommand(const std::wstring& command);
using CameraFn=void(__thiscall*)(uintptr_t,int,float,float,int);
CameraFn setCamera=nullptr;
wht::Scroll129 keyboardScroll129,mouseScroll129;
wht::ScrollSetter129 setKeyboardScroll129=nullptr,setMouseScroll129=nullptr;
bool commandSettingsActive=true;
uintptr_t requestedCameraOwner=0,requestedFogOwner=0;
std::array<float,2> requestedCamera{};unsigned requestedCameraMask=0;
float requestedFog=0;bool hasRequestedFog=false;
using FogRealFn=void(__cdecl*)(uintptr_t,int,float);
FogRealFn setFogReal=nullptr;
wht::FogEffect129 fogEffect129;
wht::FogEffectFn129 originalFogEffect129=nullptr;
bool fogEffectWriting129=false;
void __fastcall CameraNative(uintptr_t object,DWORD,int field,float value,float duration,int flag){
 if(field>=0&&field<2&&std::isfinite(value)){
  if(requestedCameraOwner!=object){requestedCameraOwner=object;requestedCameraMask=0;}
  requestedCamera[field]=value;requestedCameraMask|=1u<<field;
 }
 setCamera(object,field,value,duration,flag);
}
void __cdecl FogNative(uintptr_t parameters,int property,float value){
 if(!fogEffectWriting129&&property==4&&std::isfinite(value)){requestedFogOwner=parameters;requestedFog=value;hasRequestedFog=true;}
 if(!fogEffectWriting129&&fogEffect129.observed&&parameters==fogEffect129.parameters&&std::isfinite(value)){
  if(property==2)fogEffect129.density=value;else if(property==3)fogEffect129.start=value;else if(property==4)fogEffect129.end=value;
 }
 setFogReal(parameters,property,!fogEffectWriting129&&fogEffect129.active&&parameters==fogEffect129.parameters&&property>=2&&property<=4?0.f:value);
}
void WriteFogEffect129(uintptr_t scene,unsigned style,float start,float end,float density,const uint32_t* color){
 fogEffectWriting129=true;originalFogEffect129(scene,style,start,end,density,color);fogEffectWriting129=false;
}
void __fastcall FogEffectNative129(uintptr_t scene,DWORD,unsigned style,float start,float end,float density,const uint32_t* color){
 const bool captured=fogEffect129.Capture(scene,style,start,end,density,color);
 if(captured){requestedFogOwner=fogEffect129.parameters;requestedFog=end;hasRequestedFog=true;}
 const bool remove=captured&&runtimeConfig.mode==wht::RunMode::Local&&runtimeConfig.removeFogEffect;
 WriteFogEffect129(scene,remove?1:style,remove?0.f:start,remove?0.f:end,remove?0.f:density,color);fogEffect129.active=remove;
}
void ApplyFogEffect129(bool allowed){
 if(!originalFogEffect129)return;
 __try {const auto ui=*reinterpret_cast<uintptr_t*>(executableBase+0xd3d7f8);const auto scene=ui?*reinterpret_cast<uintptr_t*>(ui+0x3fc):0;
  if(scene&&allowed&&runtimeConfig.removeFogEffect&&(fogEffect129.scene!=scene||!fogEffect129.Valid()))fogEffect129.ReadCurrent(scene,reinterpret_cast<wht::FogParameterRead129>(executableBase+0x56c440));
  fogEffect129.Apply(scene,allowed&&runtimeConfig.removeFogEffect,WriteFogEffect129);
 }__except(EXCEPTION_EXECUTE_HANDLER){fogEffect129.Reset();}
}
using ChatFn=void(__thiscall*)(uintptr_t,uint32_t,const char*,int,float);
ChatFn originalChat=nullptr;
bool CameraContext(uintptr_t& camera,uint16_t& local){
 __try{
  auto world=*reinterpret_cast<uintptr_t*>(executableBase+0xd3b6f4);
  auto ui=*reinterpret_cast<uintptr_t*>(executableBase+0xd3d7f8);
  if(!world||!ui)return false;
  camera=*reinterpret_cast<uintptr_t*>(ui+0x294);
  local=*reinterpret_cast<uint16_t*>(world+0x28);
  return camera!=0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool ParseCameraCommand(const char* text,int& field,float& value){
 if(!text)return false;
 std::istringstream in(text);std::string command,extra;
 if(!(in>>command>>value)||in>>extra||!std::isfinite(value))return false;
 for(auto& c:command)c=static_cast<char>(tolower(static_cast<unsigned char>(c)));
 if(command=="#cam"||command=="#zoom"){field=0;return value>=500&&value<=10000;}
 if(command=="#fogz"){field=1;return value>=1&&value<=50000;}
 return false;
}

bool FogContext(uintptr_t& parameters){
 __try{
  const auto ui=*reinterpret_cast<uintptr_t*>(executableBase+0xd3d7f8);
  if(!ui)return false;
  const auto scene=*reinterpret_cast<uintptr_t*>(ui+0x3fc);
  if(!scene)return false;
  parameters=*reinterpret_cast<uintptr_t*>(scene+0x3e4);
  if(!parameters||*reinterpret_cast<unsigned*>(parameters+8)<=4)return false;
  const auto slots=*reinterpret_cast<uintptr_t*>(parameters+0xc);
  if(!slots)return false;
  const auto end=*reinterpret_cast<uintptr_t*>(slots+4*4);
  return end&&*reinterpret_cast<unsigned char*>(end+0xc)==6&&(*reinterpret_cast<unsigned char*>(end+0xd)&2)==0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void ApplyScroll129(){
 uintptr_t camera=0;uint16_t local=0;CameraContext(camera,local);
 const bool enabled=runtimeConfig.mode==wht::RunMode::Local&&commandSettingsActive;
 keyboardScroll129.Apply(camera,0x4f0,runtimeConfig.keyboardScroll,enabled,setKeyboardScroll129);
 mouseScroll129.Apply(camera,0x4ec,runtimeConfig.mouseScroll,enabled,setMouseScroll129);
}
void ApplyCamera(){
 ApplyScroll129();
 if(!setCamera||!executableBase||runtimeConfig.mode!=wht::RunMode::Local||!commandSettingsActive)return;
 uintptr_t camera=0;uint16_t local=0;
 if(!CameraContext(camera,local))return;
 setCamera(camera,0,runtimeConfig.camera,0,1);
 setCamera(camera,1,runtimeConfig.fogZ,0,1);
 uintptr_t fog=0;
 // SetTerrainFogEx uses parameter 4 for scene-fog end distance.
 // Re-resolve on every tick so map reloads cannot retain a stale object.
 if(setFogReal&&!fogEffect129.active&&FogContext(fog))setFogReal(fog,4,runtimeConfig.fogZ);
}
void ToggleCommandSettings(){
 commandSettingsActive=!commandSettingsActive;ApplyScroll129();
 if(commandSettingsActive){ApplyCamera();return;}
 uintptr_t camera=0,fog=0;uint16_t local=0;
 if(setCamera&&CameraContext(camera,local)&&camera==requestedCameraOwner){
  for(int field=0;field<2;++field)if(requestedCameraMask&(1u<<field))setCamera(camera,field,requestedCamera[field],0,1);
 }
 if(setFogReal&&!fogEffect129.active&&hasRequestedFog&&FogContext(fog)&&fog==requestedFogOwner)setFogReal(fog,4,requestedFog);
}
void CALLBACK Refresh(HWND,UINT,UINT_PTR,DWORD);
void __fastcall ChatDisplay(uintptr_t object,DWORD,uint32_t sender,const char* text,int channel,float duration){
 uintptr_t camera=0;uint16_t local=0;int field=0;float value=0;
 if(runtimeConfig.mode==wht::RunMode::Local&&channel>=0&&channel<=2&&CameraContext(camera,local)&&sender==local&&ParseCameraCommand(text,field,value)){
  if(field==0)runtimeConfig.camera=value;else runtimeConfig.fogZ=value;
  wht::SaveSetting(runtimeIni,L"Event",field==0?L"Camera":L"FogZ",std::to_wstring(value));
  ApplyCamera();
  wht::Log(runtimeFolder,"WHT 1.29: applied local camera command: "+std::string(text));
 }
 if(channel>=0&&channel<=2&&CameraContext(camera,local)&&sender==local){
  chatTyping=false;
  if(text&&ToolCommand(wht::Wide(text))&&!_strnicmp(text,"#nfe",4)){
   originalChat(object,sender,runtimeConfig.removeFogEffect?
    "|cffffff00[WHT] Scene fog removal: ON (#nfe 0 to restore)|r":
    "|cffffff00[WHT] Scene fog removal: OFF (#nfe 1 to enable)|r",channel,8.f);
  }
 }
 EnsureInput();
 if(!timer)timer=SetTimer(nullptr,0,16,Refresh);
 originalChat(object,sender,text,channel,duration);
}
void CALLBACK Refresh(HWND,UINT,UINT_PTR,DWORD){
 ApplyCamera();
 UpdateTools();
 if(!label)return;
 if(!Matches(label,last.c_str())){label=0;last.clear();return;}
 auto desired=Display();if(!desired.empty()&&desired!=last){original(label,desired.c_str());last=std::move(desired);}
}
void __fastcall SetText(uintptr_t object,DWORD,const char* text){
 if(text&&!strcmp(text,"1.29.2.9231")){
  EnsureInput();
  label=object;last=Display();original(object,last.empty()?text:last.c_str());
  // Native archives and DLL dependencies are ready at the stock main menu.
  // Mount configured assets before the map browser resolves map resources.
  if(executableBase&&!runtimeFolder.empty()&&runtimeConfig.mode==wht::RunMode::Local)StartPlugins129();
  if(!timer)timer=SetTimer(nullptr,0,16,Refresh);
  return;
 }
 if(object==label){label=0;last.clear();}
 original(object,text);
}
unsigned LifeFlags129(){return runtimeConfig.mode==wht::RunMode::Local?unsigned(runtimeConfig.lifeManaCommas)|(unsigned(runtimeConfig.lifeManaNoHide)<<1):0;}
void __fastcall LifeText129(uintptr_t font,DWORD,const char* text){
 const auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
 auto statCaller=caller-executableBase;
 if(statCaller==0x259210)statCaller=wht::StatsOuter129(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()))-executableBase;
 if(stats129.Handle(executableBase,font,text,wht::StatsKind129(statCaller),runtimeConfig.mode==wht::RunMode::Local&&runtimeConfig.statsCommas,originalLifeText129))return;
 if(!lifeMana129.Handle(executableBase,font,text,caller-executableBase,LifeFlags129(),originalLifeText129))originalLifeText129(font,text);
}
bool ControlAvailable129(const wht::ToolMenuControl& d){
 if(!wcscmp(d.section,L"Event")&&!wcscmp(d.key,L"Fast Menu"))return originalMenuTransition129!=nullptr;
 if(!wcscmp(d.section,L"Event")&&!wcscmp(d.key,L"ModeCast"))return clickButton!=nullptr;
 if(!wcscmp(d.section,L"StateTag")||!wcscmp(d.section,L"ManaBar"))return levelHooksReady129;
 if(!wcscmp(d.section,L"Language")||!wcscmp(d.section,L"MacroRecorder")||!wcscmp(d.section,L"DelayClear"))return true;
 if(!wcscmp(d.section,L"Quick Chat"))return true;
 if(!wcscmp(d.section,L"HotKeyOther"))return !wcscmp(d.key,L"hko_menu")||!wcscmp(d.key,L"hko_command")||!wcscmp(d.key,L"hko_MinimizeMultiboard");
 if(!wcscmp(d.section,L"SlotHotkey")||!wcscmp(d.section,L"SlotHotkeyInv")||!wcscmp(d.section,L"SlotHotkeyShop"))return wcsncmp(d.key,L"AutoTarget",10);
 if(!wcscmp(d.section,L"Graphics"))return !wcscmp(d.key,L"timeOfDayUI")||!wcscmp(d.key,L"ConsoleUI")||!wcscmp(d.key,L"Text Life/Mana Commas")||!wcscmp(d.key,L"Text Life/Mana No Hide")||!wcscmp(d.key,L"Text Stats Commas")||!wcscmp(d.key,L"MouseTooltipUI")||!wcscmp(d.key,L"BlackBarUI")||!wcscmp(d.key,L"Color Player")||(!wcscmp(d.key,L"BarInvul")&&invulnerableHookReady129)||(!wcscmp(d.key,L"VertexColor")&&originalVertexColor129);
 if(!wcscmp(d.section,L"HotKeyHero"))return !wcsncmp(d.key,L"HeroF",5)||!wcscmp(d.key,L"AutoClickLeft")||!wcscmp(d.key,L"AutoClickRight");
 return !wcscmp(d.section,L"Event")&&(!wcscmp(d.key,L"Show Resource")||!wcscmp(d.key,L"Show Mem")||!wcscmp(d.key,L"Show Timer")||!wcscmp(d.key,L"Show FPS")||!wcscmp(d.key,L"Show Ins All Skills")||!wcscmp(d.key,L"Hide Option Menu")||!wcscmp(d.key,L"Show Hotkey In Panel")||!wcscmp(d.key,L"ShowCooldown")||!wcscmp(d.key,L"ShowRegenHP")||!wcscmp(d.key,L"ShowRegenMana")||(!wcscmp(d.key,L"Keyboard Scroll Speed")&&setKeyboardScroll129)||(!wcscmp(d.key,L"Mouse Scroll Speed")&&setMouseScroll129));
}
std::wstring PluginPath129(std::wstring path){
 auto first=path.find_first_not_of(L" \t"),lastCharacter=path.find_last_not_of(L" \t");
 if(first==std::wstring::npos)return {};
 path=path.substr(first,lastCharacter-first+1);
 if(path.size()>1&&path.front()==L'"'&&path.back()==L'"')path=path.substr(1,path.size()-2);
 if(path.empty())return {};
 if(path.find(L':')==std::wstring::npos&&path.front()!=L'\\')path=runtimeFolder+L"\\"+path;
 wchar_t absolute[32768]{};auto length=GetFullPathNameW(path.c_str(),32768,absolute,nullptr);
 return length&&length<32768?std::wstring(absolute):std::wstring{};
}
bool Is32BitDll129(const std::wstring& path){
 const auto file=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(file==INVALID_HANDLE_VALUE)return false;
 IMAGE_DOS_HEADER dos{};DWORD bytes=0;bool valid=false;
 if(ReadFile(file,&dos,sizeof(dos),&bytes,nullptr)&&bytes==sizeof(dos)&&dos.e_magic==IMAGE_DOS_SIGNATURE&&dos.e_lfanew>=sizeof(dos)&&dos.e_lfanew<16*1024*1024){
  SetFilePointer(file,dos.e_lfanew,nullptr,FILE_BEGIN);DWORD signature=0;IMAGE_FILE_HEADER header{};
  if(ReadFile(file,&signature,4,&bytes,nullptr)&&bytes==4&&signature==IMAGE_NT_SIGNATURE&&ReadFile(file,&header,sizeof(header),&bytes,nullptr)&&bytes==sizeof(header))
   valid=header.Machine==IMAGE_FILE_MACHINE_I386&&(header.Characteristics&IMAGE_FILE_DLL)!=0;
 }
 CloseHandle(file);return valid;
}
bool LoadPlugin129(const std::wstring& requested,bool archive){
 const auto path=PluginPath129(requested);if(path.empty())return false;
 auto identity=path;std::transform(identity.begin(),identity.end(),identity.begin(),[](wchar_t c){return wchar_t(towlower(c));});
 auto& loaded=archive?mountedArchives129:loadedLibraries129;
 if(loaded.count(identity))return true;
 bool success=false;
 DWORD error=0;
 const auto attributes=GetFileAttributesW(path.c_str());
 if(attributes==INVALID_FILE_ATTRIBUTES||(attributes&FILE_ATTRIBUTE_DIRECTORY)){error=ERROR_FILE_NOT_FOUND;}
 else if(archive){HANDLE handle=nullptr;success=wht::MountCustom129(executableBase,path,handle);if(success)extraArchives129.push_back(handle);else error=GetLastError();}
 else if(!Is32BitDll129(path)){error=ERROR_BAD_EXE_FORMAT;}
 else {
  // Search dependencies next to the plugin, without changing process cwd.
  auto library=LoadLibraryExW(path.c_str(),nullptr,LOAD_WITH_ALTERED_SEARCH_PATH);
  success=library!=nullptr;if(success)extraLibraries129.push_back(library);else error=GetLastError();
 }
 wht::Log(runtimeFolder,std::string("WHT 1.29 ")+(archive?"LoadMPQ: ":"LoadDll: ")+(success?"loaded ":"failed ")+wht::Utf8(path)+(success?"":" (error "+std::to_string(error)+")"));
 if(success)loaded.insert(identity);return success;
}
void StartPlugins129(){
 if(pluginsStarted129||pluginsLoading129)return;pluginsLoading129=true;pluginsStarted129=true;
 for(bool archive:{false,true}){std::wistringstream list(archive?runtimeConfig.loadMpq:runtimeConfig.loadDll);std::wstring path;while(std::getline(list,path,L','))if(!path.empty())LoadPlugin129(path,archive);}
 pluginsLoading129=false;
}
bool ToolCommand(const std::wstring& command){
 if(runtimeConfig.mode!=wht::RunMode::Local)return false;
 std::wistringstream in(command);std::wstring name,action,extra;in>>name;
 if(name==L"#loaddll"||name==L"#loadmpq"){
  std::wstring path;std::getline(in,path);const bool archive=name==L"#loadmpq";
  if(!LoadPlugin129(path,archive))return false;
  const auto resolved=PluginPath129(path);auto previous=archive?runtimeConfig.loadMpq:runtimeConfig.loadDll;
  // Deduplicate persistent paths as well as loaded process handles.
  std::wistringstream paths(previous);std::wstring saved;bool found=false;
  while(std::getline(paths,saved,L','))if(!_wcsicmp(PluginPath129(saved).c_str(),resolved.c_str()))found=true;
  if(!found){if(!previous.empty())previous+=L",";previous+=resolved;if(!wht::SaveSetting(runtimeIni,L"Event",archive?L"LoadMPQ":L"LoadDll",previous))return false;runtimeConfig=wht::LoadConfig(runtimeIni);}
  return true;
 }
 if(name==L"#showregen"||name==L"#regen"||name==L"#regenhp"||name==L"#regenmp"){
  const bool mana=name==L"#regenmp";std::wstring value=(mana?runtimeConfig.showRegenMana:runtimeConfig.showRegenHP)?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Event",mana?L"ShowRegenMana":L"ShowRegenHP",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#colorplayer"||name==L"#colorp"){
  std::wstring value=runtimeConfig.colorPlayer?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"Color Player",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#barinvul"){
  if(!invulnerableHookReady129)return false;std::wstring value=runtimeConfig.hideInvulnerableBars?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"BarInvul",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#vertexcolor"){
  if(!originalVertexColor129)return false;
  std::wstring value=runtimeConfig.vertexColor?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"VertexColor",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#kscroll"||name==L"#mscroll"){
  const bool keyboard=name==L"#kscroll";
  if(!(keyboard?setKeyboardScroll129:setMouseScroll129))return false;
  int percent=0;if(!(in>>percent)||in>>extra||percent<0||percent>700)return false;
  if(!wht::SaveSetting(runtimeIni,L"Event",keyboard?L"Keyboard Scroll Speed":L"Mouse Scroll Speed",std::to_wstring(percent)))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);ApplyScroll129();toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#nfe"){
  if(!originalFogEffect129)return false;std::wstring value=runtimeConfig.removeFogEffect?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Event",L"Remove Fog Effect",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);ApplyFogEffect129(runtimeConfig.mode==wht::RunMode::Local);toolMenu.nextRefresh=0;
  wht::Log(runtimeFolder,std::string("WHT 1.29 #nfe: ")+(runtimeConfig.removeFogEffect?"ON":"OFF")+", scene override="+(fogEffect129.active?"ON":"OFF"));return true;
 }
 if(name==L"#statscommas"){
  std::wstring value=runtimeConfig.statsCommas?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"Text Stats Commas",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#hpmpcommas"||name==L"#hpmpnohide"){
  const bool commas=name==L"#hpmpcommas";const bool previous=commas?runtimeConfig.lifeManaCommas:runtimeConfig.lifeManaNoHide;
  std::wstring value=previous?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Graphics",commas?L"Text Life/Mana Commas":L"Text Life/Mana No Hide",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#fastmenu"){
  if(!originalMenuTransition129)return false;
  std::wstring value;in>>value;
  if(value.empty())value=runtimeConfig.fastMenu?L"0":L"1";
  if((value!=L"0"&&value!=L"1")||(in>>extra))return false;
  if(!wht::SaveSetting(runtimeIni,L"Event",L"Fast Menu",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#hpvalue"||name==L"#manavalue"){
  if(!levelHooksReady129)return false;
  const bool mana=name==L"#manavalue";
  std::wstring value;in>>value;
  if(value.empty())value=(mana?runtimeConfig.stateTagManaValue:runtimeConfig.stateTagHpValue)?L"0":L"1";
  if((value!=L"0"&&value!=L"1")||(in>>extra))return false;
  if(!wht::SaveSetting(runtimeIni,L"StateTag",mana?L"ShowManaValue":L"ShowHpValue",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#allowlocal"||name==L"#alf"||name==L"#localfiles"){
  std::wstring value;in>>value;
  if(value.empty())value=runtimeConfig.allowLocalFiles?L"0":L"1";
  if((value!=L"0"&&value!=L"1")||(in>>extra))return false;
  if(!wht::SetLocalFiles129(runtimeIni,runtimeConfig.allowLocalFiles,value==L"1",localFilesWriter129))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);
  wht::Log(runtimeFolder,std::string("WHT 1.29 Allow Local Files: ")+(runtimeConfig.allowLocalFiles?"ON":"OFF")+"; restart Warcraft to apply.");return true;
 }
 if(name==L"#chp"||name==L"#chpe"||name==L"#cmp"||name==L"#cmpe"){
  std::wstring value;if(!(in>>value)||(in>>extra))return false;
  if(value.size()==10&&value[0]==L'0'&&(value[1]==L'x'||value[1]==L'X'))value=value.substr(2);
  if(value.size()!=8||value.find_first_not_of(L"0123456789abcdefABCDEF")!=std::wstring::npos)return false;
  if(!levelHooksReady129)return false;
  const bool hp=name==L"#chp"||name==L"#chpe",enemy=name==L"#chpe"||name==L"#cmpe";
  if(!wht::SaveSetting(runtimeIni,hp?L"HPBar":L"ManaBar",enemy?L"Color Enemy":L"Color",L"0x"+value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);return true;
 }
 if(name==L"#mc"||name==L"#modecast"){
  std::wstring value;in>>value;
  if(value.empty())value=std::to_wstring((runtimeConfig.modeCast+1)%3);
  if((value!=L"0"&&value!=L"1"&&value!=L"2")||(in>>extra))return false;
  if(!wht::SaveSetting(runtimeIni,L"Event",L"ModeCast",value))return false;
  smartCast129.Reset();runtimeConfig=wht::LoadConfig(runtimeIni);return true;
 }
 if(name==L"#sound"){
  std::wstring value=runtimeConfig.soundStartGame?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Event",L"Sound Start Game",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);return true;
 }
 if(name==L"#allskills"||name==L"#skills"){
  if(in>>extra||toolMenu.open||macroRecorder.playing||macroRecorder.recording)return false;
  return allSkills.Start(executableBase,clickButton,GetTickCount64());
 }
 if(name==L"#console"||name==L"#consoleui"||name==L"#menu"||name==L"#hidemenu"||name==L"#showskills"){
  const bool console=name==L"#console"||name==L"#consoleui",skills=name==L"#showskills";
  const bool previous=console?runtimeConfig.hideConsoleUI:skills?runtimeConfig.showInsAllSkills:runtimeConfig.hideOptionMenu;
  std::wstring value=previous?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,console?L"Graphics":L"Event",console?L"ConsoleUI":skills?L"Show Ins All Skills":L"Hide Option Menu",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#whp"||name==L"#hhp"||name==L"#wmp"||name==L"#hmp"){
  float value=0;if(!levelHooksReady129||!(in>>value)||in>>extra||!std::isfinite(value)||value<0||value>100)return false;
  if(!wht::SaveSetting(runtimeIni,L"Event",name==L"#whp"?L"whp":name==L"#hhp"?L"hhp":name==L"#wmp"?L"wmp":L"hmp",std::to_wstring(value)))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);return true;
 }
 if(name==L"#cooldown"||name==L"#showlevel"){
  const bool level=name==L"#showlevel";
  if(level&&!levelHooksReady129)return false;
  std::wstring value=(level?runtimeConfig.stateTagShowLevel:runtimeConfig.showCooldown)?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,level?L"StateTag":L"Event",level?L"ShowLevelHero":L"ShowCooldown",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#blackbar"){
  std::wstring value=runtimeConfig.hideBlackBar?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"BlackBarUI",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#mousetooltip"){
  std::wstring value=runtimeConfig.mouseTooltip?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"MouseTooltipUI",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#showhotkey"){
  std::wstring value=runtimeConfig.showHotkeyInPanel?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Event",L"Show Hotkey In Panel",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#commandsettings"){if(in>>extra)return false;ToggleCommandSettings();return true;}
 if(name==L"#clear"||name==L"#cls"||name==L"#clearc"||name==L"#clsc"){
  if(in>>extra)return false;
  uintptr_t camera=0;uint16_t local=0;
  return CameraContext(camera,local)&&wht::ClearMessages129(executableBase,name==L"#clearc"||name==L"#clsc",!chatTyping,clearFrame);
 }
 if(name==L"#timer"||name==L"#showtimer"||name==L"#full"||name==L"#lock"||name==L"#res"||name==L"#resource"){
  const bool resource=name==L"#res"||name==L"#resource";
  const bool previous=resource?runtimeConfig.showResource:name==L"#full"?runtimeConfig.borderless:name==L"#lock"?runtimeConfig.lockCursor:runtimeConfig.showTimer;
  std::wstring value=previous?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  const auto section=name==L"#full"||name==L"#lock"?L"Window":L"Event";
  const auto key=resource?L"Show Resource":name==L"#full"?L"Borderless":name==L"#lock"?L"Lock Cursor":L"Show Timer";
  if(!wht::SaveSetting(runtimeIni,section,key,value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
 }
 if(name==L"#tools"){if(in>>extra)return false;toolMenu.Toggle();return true;}
 if(name==L"#macro"){
  if(!(in>>action)||in>>extra)return false;
  if(!runtimeConfig.macroEnabled&&action!=L"stop")return false;
  macroRecorder.Configure(window,runtimeFolder);
  if(action==L"record"){toolMenu.open=false;macroRecorder.Record();}
  else if(action==L"play"){toolMenu.open=false;macroRecorder.Play();}
  else if(action==L"stop"){if(macroRecorder.recording)macroRecorder.FinishRecording();macroRecorder.Stop();}
  else if(action==L"edit")macroRecorder.Edit();else return false;
  wht::Log(runtimeFolder,"WHT 1.29 macro: "+macroRecorder.GameStatus());return true;
 }
 if(name!=L"#toolui")return false;
 size_t index;std::wstring value;
 if(!(in>>index>>action)||index>=wht::kToolMenuControls.size())return false;
 const auto& control=wht::kToolMenuControls[index];
 if(!control.supported||!ControlAvailable129(control)||!wht::ToolMenuOverlay::Enabled(runtimeConfig,index))return false;
 if(action==L"toggle"&&control.kind==wht::MenuControlKind::Check){if(in>>extra)return false;value=wht::ToolMenuOverlay::ReadValue(runtimeConfig,index)==L"1"?L"0":L"1";}
 else if(action==L"clear"&&control.kind==wht::MenuControlKind::Clear){if(in>>extra)return false;}
 else if(action==L"set"&&control.kind!=wht::MenuControlKind::Check&&control.kind!=wht::MenuControlKind::Label&&control.kind!=wht::MenuControlKind::Clear){std::getline(in,value);if(!value.empty()&&value.front()==L' ')value.erase(0,1);}
 else return false;
 if(!wht::ValidMenuValue(control,value))return false;
 const bool language=std::wstring(control.section)==L"Language";
 if(language)value=L"1";
 if(!wht::SaveSetting(runtimeIni,control.section,control.key,value))return false;
 if(language&&!wht::SaveSetting(runtimeIni,L"Language",std::wstring(control.key)==L"English"?L"Vietnamese":L"English",L"0"))return false;
 runtimeConfig=wht::LoadConfig(runtimeIni);wht::toolEnglish.store(runtimeConfig.english);toolMenu.nextRefresh=0;return true;
}
bool SlotKey129(unsigned key){
 if(!clickButton)return false;
 const auto activate=[&](bool inventory,int slot,const wht::Binding& binding){
  smartCast129.Reset();
  return wht::ActivateSlot129(executableBase,inventory,slot,binding.quickCast&&!macroRecorder.playing&&!macroRecorder.recording,clickButton,[&](uintptr_t target){
   if(runtimeConfig.modeCast==1){const auto selected=wht::SelectedSkill129(executableBase);smartCast129.Arm(key,target,selected.unit,selected.identity);}
   else wht::CastAtCursor129(executableBase,window,target);
  });
 };
 for(size_t i=0;i<runtimeConfig.inventory.size();++i){const auto& b=runtimeConfig.inventory[i];if(runtimeConfig.inventoryEnabled&&b.enabled&&b.vk==key)return activate(true,int(i),b);}
 const bool shop=wht::IsShop129(executableBase);
 const auto& bindings=shop?runtimeConfig.shop:runtimeConfig.command;
 if(shop?runtimeConfig.shopEnabled:runtimeConfig.commandEnabled)
  for(size_t i=0;i<bindings.size();++i){const auto& b=bindings[i];if(b.enabled&&b.vk==key)return activate(false,int(i),b);}
 if(!shop&&runtimeConfig.shopEnabled&&!allSkills.active&&!macroRecorder.playing&&!macroRecorder.recording){
  for(int slot=0;slot<11;++slot){const auto& b=runtimeConfig.shop[slot];if(b.enabled&&b.vk==key)return wht::LearnSlot129(executableBase,slot,clickButton);}
 }
 return false;
}
bool ChatKey129(unsigned key){
 if(!runtimeConfig.quickChatEnabled)return false;
 for(const auto& b:runtimeConfig.quickChat)if(b.vk&&b.vk==key&&!b.text.empty())return wht::SendQuickChat129(executableBase,b.text,quickChatApi);
 return false;
}
bool HeroKey129(unsigned key){
 for(int i=0;i<7;++i)if(key&&runtimeConfig.heroes[i]==key)return wht::SelectHero129(executableBase,i,clickButton);
 return false;
}
LRESULT CALLBACK InputMessages(int code,WPARAM removed,LPARAM data){
 if(code>=0&&removed==PM_REMOVE&&data){
  auto& msg=*reinterpret_cast<MSG*>(data);
  if(msg.hwnd==window){
   bool nativeTyping=false;if(wht::ChatOpen129(executableBase,nativeTyping))chatTyping=nativeTyping;
   uintptr_t camera=0;uint16_t local=0;const bool allowed=runtimeConfig.mode==wht::RunMode::Local&&CameraContext(camera,local)&&GetForegroundWindow()==window;
   const auto queue=[](const wchar_t* command){return ToolCommand(command);};
   bool handled=false;
   const bool down=msg.message==WM_KEYDOWN||msg.message==WM_SYSKEYDOWN;
   const bool up=msg.message==WM_KEYUP||msg.message==WM_SYSKEYUP;
   const auto key=unsigned(msg.wParam);
   if((down||up)&&key<256){
    if(up&&consumedKeys[key]){
     const auto selected=wht::SelectedSkill129(executableBase);
     smartCast129.Release(key,allowed&&!chatTyping&&!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&runtimeConfig.modeCast==1,wht::TargetMode129(executableBase),selected.unit,selected.identity,[&](uintptr_t target){return wht::CastAtCursor129(executableBase,window,target);});
     consumedKeys[key]=false;handled=true;
    }
    else if(down&&consumedKeys[key])handled=true;
    else if(allowed&&down&&!(msg.lParam&(1L<<30))){
     if(!chatTyping&&runtimeConfig.hideOptionMenu&&key==VK_F10&&GetKeyState(VK_CONTROL)>=0&&GetKeyState(VK_SHIFT)>=0&&GetKeyState(VK_MENU)>=0)handled=true;
     else if(!chatTyping&&toolMenu.Key(key,(unsigned(msg.lParam)>>16)&255,queue))handled=true;
     else if(!chatTyping&&key==VK_ESCAPE&&toolMenu.Escape())handled=true;
     else if(!chatTyping){
      const bool ctrl=GetKeyState(VK_CONTROL)<0,shift=GetKeyState(VK_SHIFT)<0,alt=GetKeyState(VK_MENU)<0;
      int action=-1;
      if(runtimeConfig.macroEnabled&&!toolMenu.open){
       if(!ctrl&&!shift&&!alt)for(size_t n=0;n<4;++n)if(runtimeConfig.macroKeys[n]&&runtimeConfig.macroKeys[n]==key){action=int(n);break;}
       if(ctrl&&shift&&!alt&&key>=VK_F9&&key<=VK_F12){int fallback=key==VK_F9?2:key==VK_F10?0:key==VK_F11?3:1;if(!runtimeConfig.macroKeys[fallback])action=fallback;}
      }
      if(action>=0)handled=ToolCommand(wht::ToolMenuOverlay::MacroCommand(size_t(action)));
      else if(runtimeConfig.commandKey&&key==runtimeConfig.commandKey&&!ctrl&&!shift&&!alt){ToggleCommandSettings();handled=true;}
      else if(key==VK_TAB&&runtimeConfig.showResource&&!toolMenu.open&&!ctrl&&!shift&&!alt){resourceExpanded=!resourceExpanded;resourceNext=0;handled=true;}
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&autoClick.Key(key,runtimeConfig))handled=true;
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&SlotKey129(key))handled=true;
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&HeroKey129(key))handled=true;
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&runtimeConfig.multiboardKey&&key==runtimeConfig.multiboardKey)handled=wht::ToggleMultiboard129(executableBase,minimizeBoard,maximizeBoard);
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&ChatKey129(key))handled=true;
      else if(runtimeConfig.menuKey&&key==runtimeConfig.menuKey&&!ctrl&&!shift&&!alt){toolMenu.Toggle();handled=true;}
     }
     if(handled)consumedKeys[key]=true;
    }
    if(!handled&&allowed&&down&&key==VK_RETURN)chatTyping=!chatTyping;
    if(!handled&&down&&key==VK_ESCAPE)chatTyping=false;
   }
   if(allowed&&!chatTyping&&(msg.message==WM_LBUTTONDOWN||msg.message==WM_LBUTTONUP||msg.message==WM_LBUTTONDBLCLK)){
    RECT r{};GetClientRect(window,&r);POINT p{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))};
    auto canvas=wht::Canvas129(r.right,r.bottom);
    const auto projected=canvas.Project(p);
    if(!toolMenu.open&&skillsIconShown&&skillsIconRect.Hit(projected,canvas.width,canvas.height)){
     handled=true;
     if(msg.message==WM_LBUTTONDOWN){skillsIconPressed=true;ToolCommand(L"#allskills");}
    }
    if(msg.message==WM_LBUTTONUP)skillsIconPressed=false;
    const int classic=std::min(r.right,r.bottom*4/3);
    POINT menuPoint{p.x-(r.right-classic)/2,p.y};
    if(runtimeConfig.hideOptionMenu&&wht::ToolMenuRect{.178f,.576f,.09f,.024f}.Hit(menuPoint,classic,r.bottom))handled=true;
    if(!handled)handled=toolMenu.Mouse(msg.message,projected,canvas.width,canvas.height,queue,&runtimeConfig);
   }
   if(!handled)macroRecorder.Capture(msg,allowed&&!toolMenu.open&&runtimeConfig.macroEnabled);
   if(handled){msg.message=WM_NULL;msg.wParam=msg.lParam=0;}
  }
 }
 return CallNextHookEx(inputHook,code,removed,data);
}
void EnsureInput(){
 if(!window||!IsWindow(window))EnumWindows(FindWindow,0);
 if(!inputHook&&window){inputHook=SetWindowsHookExW(WH_GETMESSAGE,InputMessages,nullptr,GetWindowThreadProcessId(window,nullptr));}
}
void UpdateHelp(){RECT r{};POINT p{};GetClientRect(window,&r);GetCursorPos(&p);ScreenToClient(window,&p);auto canvas=wht::Canvas129(r.right,r.bottom);p=canvas.Project(p);auto help=toolMenu.HelpAt(p,canvas.width,canvas.height,runtimeConfig);if(skillsIconShown&&skillsIconRect.Hit(p,canvas.width,canvas.height))help=runtimeConfig.english?"All Skills\nSpend available hero skill points without manually opening the learn menu.":wht::Utf8(L"C\u1ed9ng to\u00e0n b\u1ed9 k\u1ef9 n\u0103ng\nD\u00f9ng \u0111i\u1ec3m k\u1ef9 n\u0103ng c\u00f2n l\u1ea1i c\u1ee7a t\u01b0\u1edbng, kh\u00f4ng c\u1ea7n t\u1ef1 m\u1edf d\u1ea5u +.");toolMenu.UpdateHelp(p,canvas.width,canvas.height,GetForegroundWindow()==window,help);}
void UpdateMemory(bool enabled){
 if(!toolMenu.initialized)return;
 if(enabled&&!memoryFrame){memoryFrame=toolMenu.Frame(executableBase,toolMenu.independentHost,{.47f,.535f,.23f,.026f});if(memoryFrame)memoryFont=toolMenu.TextIn(memoryFrame,{.47f,.535f,.23f,.026f},"",true,.015f);}
 if(!memoryFrame||!memoryFont)return;
 if(!enabled){wht::NativeFrameHide(memoryFrame);return;}
 wht::NativeFrameShow(memoryFrame);if(GetTickCount64()<memoryNext)return;memoryNext=GetTickCount64()+1000;
 float used=0,percent=0;if(wht::GetCurrentProcessMemoryUsage(used,percent)){const auto text=wht::FormatMemoryMain(used,percent);frameApi.setText(memoryFont,text.c_str());}
}
void UpdateClock(bool enabled){
 if(!toolMenu.initialized)return;
 if(enabled&&!clockFrame){clockFrame=toolMenu.Frame(executableBase,toolMenu.independentHost,{.34f,.548f,.12f,.025f});if(clockFrame)clockFont=toolMenu.TextIn(clockFrame,{.34f,.548f,.12f,.025f},"",true,.016f);}
 if(!clockFrame||!clockFont)return;
 float seconds=0;if(!enabled||!wht::ReadClock129(executableBase,seconds)){wht::NativeFrameHide(clockFrame);return;}
 frameApi.setAbsPoint(clockFrame,4,runtimeConfig.hideTimeOfDay?.4f:.34f,.548f,1);
 const auto text=wht::FormatMatchTimer(static_cast<uint32_t>(seconds));frameApi.setText(clockFont,text.c_str());wht::NativeFrameShow(clockFrame);
}
void UpdateResources(bool enabled){
 if(!toolMenu.initialized)return;
 if(!enabled){resourceBoard.Hide();resourceNext=0;return;}
 if(GetTickCount64()<resourceNext)return;resourceNext=GetTickCount64()+500;
 const auto table=wht::ReadResources129(executableBase,true,resourceApi);
 resourceBoard.Present(toolMenu,executableBase,table,resourceExpanded);
}
void UpdateSkillsIcon(bool enabled){
 if(!toolMenu.initialized)return;
 if(enabled&&!skillsIconFrame){
  skillsIconFrame=toolMenu.Frame(executableBase,toolMenu.independentHost,skillsIconRect);
  if(skillsIconFrame){skillsIcon=toolMenu.Texture(skillsIconFrame,"BTNAllSkills.blp",skillsIconRect.width,skillsIconRect.height);if(skillsIcon)frameApi.setRelPoint(skillsIcon,4,skillsIconFrame,4,0,0,1);}
 }
 skillsIconShown=enabled&&skillsIconFrame&&skillsIcon;
 if(!skillsIconShown){if(skillsIconFrame)wht::NativeFrameHide(skillsIconFrame);skillsIconPressed=false;return;}
 frameApi.setTexture(skillsIcon,skillsIconPressed?"BTNAllSkillsDow.blp":"BTNAllSkills.blp",0);wht::NativeFrameShow(skillsIconFrame);
}
void UpdateTooltip129(uintptr_t owner,bool allowed);
wht::NativeFrameApi LevelFrameApi129(){
 wht::NativeFrameApi api=frameApi;
 api.setWidth=reinterpret_cast<decltype(api.setWidth)>(executableBase+0x473b40);
 api.setRelPoint=reinterpret_cast<decltype(api.setRelPoint)>(executableBase+0x473a10);
 return api;
}
void ApplyLevels129(bool enabled){
 __try {auto api=LevelFrameApi129();showLevel129.Apply(levelHooksReady129&&enabled,api);}
 __except(EXCEPTION_EXECUTE_HANDLER){showLevel129.enabled=false;}
}
void ApplyStateValues129(bool enabled){
 __try {auto api=LevelFrameApi129();stateValues129.Apply(executableBase,runtimeConfig,enabled&&levelHooksReady129,api);manaBar129.Apply(executableBase,runtimeConfig,enabled&&levelHooksReady129,api);hpColor129.Apply(executableBase,enabled&&levelHooksReady129,runtimeConfig);if(invulnerableHookReady129)invulnerableBar129.Apply(executableBase,enabled&&runtimeConfig.hideInvulnerableBars,originalBarShow129,reinterpret_cast<wht::BarVisibility129>(executableBase+0x488960));}
 __except(EXCEPTION_EXECUTE_HANDLER){}
}
void ApplyHpDimensions129(bool enabled){
 __try{auto api=LevelFrameApi129();hpDimensions129.Apply(executableBase,enabled?runtimeConfig.barWidthHp:0.f,enabled?runtimeConfig.barHeightHp:0.f,api);}
 __except(EXCEPTION_EXECUTE_HANDLER){}
}
void __fastcall ShowBar129(uintptr_t bar,DWORD){
 __try {if(invulnerableHookReady129&&invulnerableBar129.Update(executableBase,bar,originalBarShow129,reinterpret_cast<wht::BarVisibility129>(executableBase+0x488960)))return;}
 __except(EXCEPTION_EXECUTE_HANDLER){}
 originalBarShow129(bar);
}
void __fastcall BindBar129(uintptr_t bar,DWORD,uintptr_t unit){
 originalBarBind129(bar,unit);
 __try {auto api=LevelFrameApi129();hpDimensions129.Update(executableBase,bar,api);showLevel129.Write(bar,wht::OverheadHeroLevel129(executableBase,bar),api);stateValues129.Write(executableBase,bar,runtimeConfig,stateValues129.active,api);manaBar129.Update(executableBase,bar,runtimeConfig,api);hpColor129.Update(executableBase,bar);if(invulnerableHookReady129)invulnerableBar129.Update(executableBase,bar,originalBarShow129,reinterpret_cast<wht::BarVisibility129>(executableBase+0x488960));}
 __except(EXCEPTION_EXECUTE_HANDLER){}
}
uintptr_t __fastcall DestroyBar129(uintptr_t bar,DWORD,unsigned flags){
 showLevel129.Forget(bar);stateValues129.Forget(bar);manaBar129.Forget(bar);hpColor129.Forget(bar);invulnerableBar129.Forget(bar);
 hpDimensions129.Forget(bar);
 return originalBarDestroy129(bar,flags);
}
bool SafeUpdateMenu(uintptr_t owner,bool allowed){
 __try {toolMenu.Update(executableBase,owner,runtimeConfig,allowed,&frameApi);regen129.Update(executableBase,runtimeConfig,allowed,toolMenu);UpdateMemory(allowed&&runtimeConfig.showMem);UpdateClock(allowed&&runtimeConfig.showTimer);UpdateResources(allowed&&runtimeConfig.showResource);UpdateSkillsIcon(allowed&&runtimeConfig.showInsAllSkills);UpdateHelp();UpdateTooltip129(owner,allowed);hotkeyLabels129.Update(executableBase,runtimeConfig,allowed,toolMenu);return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void UpdateTooltip129(uintptr_t owner,bool allowed){
 blackBar129.Apply(executableBase,owner,allowed&&runtimeConfig.hideBlackBar,[&](uintptr_t layout,int point,const wht::TooltipAnchor129& anchor){
  using Fn=void(__thiscall*)(uintptr_t,int,uintptr_t,int,float,float,int);
  reinterpret_cast<Fn>(executableBase+0x473a10)(layout,point,anchor.relative,anchor.point,anchor.x,anchor.y,1);
 });
 POINT cursor{};RECT client{};
 const bool input=GetForegroundWindow()==window&&!IsIconic(window)&&GetClientRect(window,&client)&&GetCursorPos(&cursor)&&ScreenToClient(window,&cursor);
 mouseTooltip129.Apply(executableBase,owner,allowed&&runtimeConfig.mouseTooltip,cursor,client.right,client.bottom,input,wht::ui129HorizontalScale,[&](uintptr_t layout,int point,const wht::TooltipAnchor129& anchor){
  using Fn=void(__thiscall*)(uintptr_t,int,uintptr_t,int,float,float,int);
  reinterpret_cast<Fn>(executableBase+0x473a10)(layout,point,anchor.relative,anchor.point,anchor.x,anchor.y,1);
 });
}
void UpdateTools(){
 const bool localMode=runtimeConfig.mode==wht::RunMode::Local;
 windowServices.Apply(window,localMode&&runtimeConfig.borderless,localMode&&runtimeConfig.lockCursor);
 if(!executableBase||!frameApi.alloc||!window)return;
 uintptr_t camera=0;uint16_t local=0;
 if(!CameraContext(camera,local)){
  keyboardScroll129.Reset();mouseScroll129.Reset();ApplyFogEffect129(false);fogEffect129.Reset();ApplyLevels129(false);ApplyStateValues129(false);
  ApplyHpDimensions129(false);
  if(macroRecorder.recording)macroRecorder.FinishRecording();macroRecorder.Stop();
  // Map teardown releases native frames. Forget pointers without dereferencing them.
  regen129.Reset();blackBar129.Reset();mouseTooltip129.Reset();hotkeyLabels129.Reset();wht::ClearUiLayout129();if(toolMenu.parent)toolMenu.Reset();memoryFrame=memoryFont=clockFrame=clockFont=nullptr;resourceBoard={};memoryNext=resourceNext=0;resourceExpanded=true;chatTyping=false;messageTimers={};allSkills.Reset();smartCast129.Reset();autoClick.Reset();stats129.Reset();lifeMana129.Reset();fpsState={};timeOfDay={};matchStart={};consoleState={};skillsIconFrame=skillsIcon=nullptr;skillsIconShown=skillsIconPressed=false;return;
 }
 const auto owner=*reinterpret_cast<uintptr_t*>(executableBase+0xd3d7f8);
 float startSeconds=0;
 const bool startClockReady=wht::ReadClock129(executableBase,startSeconds);
 if(matchStart.Observe(owner,startClockReady,startSeconds,localMode&&runtimeConfig.soundStartGame))
  if(!startSoundPlayer129(MAKEINTRESOURCEW(101),runtimeModule,SND_RESOURCE|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP))wht::Log(runtimeFolder,"WHT 1.29: start sound unavailable; skipped.");
 if(resourceFailed)return;
 if(!resourceArchive&&!wht::MountResources129(executableBase,mixPath,resourceArchive)){resourceFailed=true;toolMenu.failed=true;wht::Log(runtimeFolder,"WHT 1.29: embedded MPQ mount failed.");return;}
 if(toolMenu.parent&&toolMenu.parent!=owner){regen129.Reset();blackBar129.Reset();mouseTooltip129.Reset();hotkeyLabels129.Reset();wht::ClearUiLayout129();memoryFrame=memoryFont=clockFrame=clockFont=nullptr;resourceBoard={};memoryNext=resourceNext=0;resourceExpanded=true;messageTimers={};allSkills.Reset();smartCast129.Reset();autoClick.Reset();consoleState={};skillsIconFrame=skillsIcon=nullptr;skillsIconShown=skillsIconPressed=false;}
 RECT r{};GetClientRect(window,&r);wht::SetViewport129(r.right,r.bottom);auto canvas=wht::Canvas129(r.right,r.bottom);wht::ToolMenuOverlay::SetViewport(canvas.width,canvas.height);
 const bool allowed=runtimeConfig.mode==wht::RunMode::Local;
 ApplyFogEffect129(allowed);ApplyLevels129(allowed&&runtimeConfig.stateTagShowLevel);ApplyStateValues129(allowed);
 ApplyHpDimensions129(levelHooksReady129&&allowed);
 bool nativeTyping=false;if(wht::ChatOpen129(executableBase,nativeTyping))chatTyping=nativeTyping;
 const auto smartSelection=wht::SelectedSkill129(executableBase);
 if(!smartCast129.Valid(allowed&&!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&GetForegroundWindow()==window&&runtimeConfig.modeCast==1,wht::TargetMode129(executableBase),smartSelection.unit,smartSelection.identity))smartCast129.Reset();
 allSkills.Tick(executableBase,allowed&&!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&GetForegroundWindow()==window,clickButton,GetTickCount64());
 fpsState.Apply(executableBase,allowed&&runtimeConfig.showFps,fpsMode);
 stats129.Apply(executableBase,allowed&&runtimeConfig.statsCommas,originalLifeText129);
 lifeMana129.Apply(executableBase,LifeFlags129(),originalLifeText129);
 consoleState.Apply(executableBase,allowed&&runtimeConfig.hideConsoleUI,showConsole,hideConsole);
 timeOfDay.Apply(executableBase,allowed&&runtimeConfig.hideTimeOfDay,showFdf,hideFdf);
 messageTimers.Tick(executableBase,runtimeConfig,allowed&&!chatTyping,GetTickCount64(),clearFrame);
 const bool clickAllowed=allowed&&!toolMenu.open&&!chatTyping&&!macroRecorder.recording&&!macroRecorder.playing&&GetForegroundWindow()==window&&!IsIconic(window);
 POINT cursor{};const bool inside=GetCursorPos(&cursor)&&ScreenToClient(window,&cursor)&&cursor.x>=0&&cursor.y>=0&&cursor.x<r.right&&cursor.y<r.bottom;
 autoClick.Tick(runtimeConfig,clickAllowed&&inside,GetTickCount64(),[&](UINT message,WPARAM key){PostMessageW(window,message,key,MAKELPARAM(cursor.x,cursor.y));});
 if(!toolMenu.failed&&!SafeUpdateMenu(owner,allowed)){toolMenu.failed=true;wht::Log(runtimeFolder,"WHT 1.29: native menu exception at "+std::string(toolMenu.initStage));}
 macroRecorder.Configure(window,runtimeFolder);
 macroRecorder.Tick(allowed&&runtimeConfig.macroEnabled&&!toolMenu.open);
}
DWORD WINAPI Install(void* module){
 wchar_t path[32768]{};GetModuleFileNameW(nullptr,path,32768);
 const auto folder=wht::Directory(static_cast<HMODULE>(module));
 if(wht::HashFile(path)!="A1950F17905B9CD7D5461D45E6723AF36DDA5304E12DBA27A1FEA85593B15F3F"){
  wht::Log(folder,"WHT 1.29: executable hash unsupported; no hooks installed.");return 1;
 }
 executableBase=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
 runtimeModule=static_cast<HMODULE>(module);runtimeFolder=folder;runtimeIni=folder+L"\\WHT.ini";runtimeConfig=wht::LoadConfig(runtimeIni);
 wht::toolEnglish.store(runtimeConfig.english);
 frameApi=wht::FrameApi129(executableBase);toolMenu.controlAvailable=ControlAvailable129;
 showLevel129.textWidth=reinterpret_cast<decltype(showLevel129.textWidth)>(executableBase+0x486fa0);
 showLevel129.backgroundColor=reinterpret_cast<decltype(showLevel129.backgroundColor)>(executableBase+0x487ba0);
 wchar_t self[32768]{};GetModuleFileNameW(static_cast<HMODULE>(module),self,32768);mixPath=self;
 auto address=reinterpret_cast<unsigned char*>(executableBase)+0x47e380;
 const unsigned char signature[]={0x55,0x8b,0xec,0x53,0x56,0x57,0x8b,0x7d,0x08,0x8b,0xf1};
 if(memcmp(address,signature,sizeof(signature))||MH_Initialize()!=MH_OK||MH_CreateHook(address,SetText,reinterpret_cast<void**>(&original))!=MH_OK||MH_EnableHook(address)!=MH_OK){wht::Log(folder,"WHT 1.29: version hook installation failed.");return 2;}
 // 1893F0 takes six stack arguments, returns duration in x87 and uses
 // plain RET: cdecl, unlike the legacy fastcall entry. Root +1A0/+1A4
 // select its two model frames; native callbacks are always forwarded.
 const unsigned char menuTransitionSignature129[]={0x55,0x8b,0xec,0x83,0xec,0x10,0x53,0x56,0x57,0xe8};
 const auto menuTransition129=reinterpret_cast<void*>(executableBase+0x1893f0);
 if(!memcmp(menuTransition129,menuTransitionSignature129,sizeof(menuTransitionSignature129))&&
    MH_CreateHook(menuTransition129,MenuTransitionHook129,reinterpret_cast<void**>(&originalMenuTransition129))==MH_OK){
  if(MH_EnableHook(menuTransition129)!=MH_OK)originalMenuTransition129=nullptr;
 }
 const auto bind=reinterpret_cast<void*>(executableBase+0x25b7e0),destroy=reinterpret_cast<void*>(executableBase+0x2278e0);
 const unsigned char bindSignature[]={0x55,0x8b,0xec,0x51,0x56,0x57,0x8b,0x7d,0x08};
 const unsigned char destroySignature[]={0x55,0x8b,0xec,0x6a,0xff};
 if(!memcmp(bind,bindSignature,sizeof(bindSignature))&&!memcmp(destroy,destroySignature,sizeof(destroySignature))&&
    *reinterpret_cast<uintptr_t*>(executableBase+0xa95104+0x78)==reinterpret_cast<uintptr_t>(bind)&&
    *reinterpret_cast<uintptr_t*>(executableBase+0xa95104+8)==reinterpret_cast<uintptr_t>(destroy)&&
    MH_CreateHook(bind,BindBar129,reinterpret_cast<void**>(&originalBarBind129))==MH_OK&&
    MH_CreateHook(destroy,DestroyBar129,reinterpret_cast<void**>(&originalBarDestroy129))==MH_OK&&
    MH_EnableHook(destroy)==MH_OK&&MH_EnableHook(bind)==MH_OK)levelHooksReady129=true;
 else wht::Log(folder,"WHT 1.29: overhead hero level hooks unavailable.");
 // Raw camera speed setters, verified against the 1.29 camera serialization
 // and mouse/arrow movement consumers. Do not use legacy normalized setters.
 const unsigned char scrollSignature[]={0x55,0x8b,0xec,0xf3,0x0f,0x10,0x55,8};
 if(!memcmp(reinterpret_cast<void*>(executableBase+0x1e3a90),scrollSignature,sizeof(scrollSignature))&&
    !memcmp(reinterpret_cast<void*>(executableBase+0x1e3b00),scrollSignature,sizeof(scrollSignature))){
  setKeyboardScroll129=reinterpret_cast<wht::ScrollSetter129>(executableBase+0x1e3a90);
  setMouseScroll129=reinterpret_cast<wht::ScrollSetter129>(executableBase+0x1e3b00);
 }else wht::Log(folder,"WHT 1.29: camera scroll setters unavailable.");
 const auto vertexColor=reinterpret_cast<void*>(executableBase+0x26dca0);
 const unsigned char vertexSignature[]={0x55,0x8b,0xec,0x83,0xec,0x14,0x53,0x56,0x57,0x8b,0xf9};
 if(!memcmp(vertexColor,vertexSignature,sizeof(vertexSignature))&&
    *reinterpret_cast<uintptr_t*>(executableBase+0xa9c504+0xb4)==reinterpret_cast<uintptr_t>(vertexColor)&&
    MH_CreateHook(vertexColor,VertexColorNative129,reinterpret_cast<void**>(&originalVertexColor129))==MH_OK){
  if(MH_EnableHook(vertexColor)!=MH_OK)originalVertexColor129=nullptr;
 }
 if(!originalVertexColor129)wht::Log(folder,"WHT 1.29: vertex color hook unavailable.");
 const auto sceneFog=reinterpret_cast<void*>(executableBase+0x216ed0);
 const unsigned char sceneFogSignature[]={0x55,0x8b,0xec,0x56,0x8b,0xf1,0x8b,0x86,0xe4,3,0,0};
 if(!memcmp(sceneFog,sceneFogSignature,sizeof(sceneFogSignature))&&MH_CreateHook(sceneFog,FogEffectNative129,reinterpret_cast<void**>(&originalFogEffect129))==MH_OK){
  if(MH_EnableHook(sceneFog)!=MH_OK)originalFogEffect129=nullptr;
 }
 const auto barShow=reinterpret_cast<void*>(executableBase+0x489110);
 const unsigned char showSignature[]={0x57,0x8b,0xf9,0x83,0xbf,0x94,0,0,0,0};
 if(levelHooksReady129&&!memcmp(barShow,showSignature,sizeof(showSignature))&&
    *reinterpret_cast<uintptr_t*>(executableBase+0xa95104+0x6c)==reinterpret_cast<uintptr_t>(barShow)&&
    *reinterpret_cast<uintptr_t*>(executableBase+0xa95104+0x68)==executableBase+0x488960&&
    MH_CreateHook(barShow,ShowBar129,reinterpret_cast<void**>(&originalBarShow129))==MH_OK){
  invulnerableHookReady129=true;if(MH_EnableHook(barShow)!=MH_OK)invulnerableHookReady129=false;
 }
 auto lifeFont=reinterpret_cast<unsigned char*>(executableBase)+0x487a40;
 const unsigned char lifeSignature[]={0x55,0x8b,0xec,0x56,0x8b,0xf1,0x8b,0x86,0xa0,0,0,0};
 if(!memcmp(lifeFont,lifeSignature,sizeof(lifeSignature))&&MH_CreateHook(lifeFont,LifeText129,reinterpret_cast<void**>(&originalLifeText129))==MH_OK)MH_EnableHook(lifeFont);
 else wht::Log(folder,"WHT 1.29: HP/MP text hook unavailable.");
 auto chat=reinterpret_cast<unsigned char*>(executableBase)+0x1f3020;
 auto camera=reinterpret_cast<unsigned char*>(executableBase)+0x1e30e0;
 const unsigned char chatSignature[]={0x55,0x8b,0xec,0x81,0xec,0xac,0x02,0,0};
 const unsigned char cameraSignature[]={0x55,0x8b,0xec,0xf3,0x0f,0x10,0x45,0x10};
 if(memcmp(chat,chatSignature,sizeof(chatSignature))||memcmp(camera,cameraSignature,sizeof(cameraSignature))||
    MH_CreateHook(chat,ChatDisplay,reinterpret_cast<void**>(&originalChat))!=MH_OK||MH_EnableHook(chat)!=MH_OK){
  wht::Log(folder,"WHT 1.29: camera/chat signatures or hook failed; version label only.");return 3;
 }
 if(MH_CreateHook(camera,CameraNative,reinterpret_cast<void**>(&setCamera))!=MH_OK||MH_EnableHook(camera)!=MH_OK){wht::Log(folder,"WHT 1.29: native camera capture hook failed.");return 4;}
 const unsigned char clearSignature[]={0x55,0x8b,0xec,0x51,0x53,0x56,0x8b,0xf1};
 auto clear=reinterpret_cast<unsigned char*>(executableBase)+0x48b7b0;
 if(!memcmp(clear,clearSignature,sizeof(clearSignature)))clearFrame=reinterpret_cast<wht::ClearFrame129>(clear);
 const unsigned char fpsSignature[]={0x55,0x8b,0xec,0x56,0x57,0x8b,0x7d,0x08};
 if(!memcmp(reinterpret_cast<void*>(executableBase+0x24e810),fpsSignature,sizeof(fpsSignature)))fpsMode=reinterpret_cast<wht::FpsMode129>(executableBase+0x24e810);
 showFdf=reinterpret_cast<wht::ClearFrame129>(executableBase+0x470020);
 hideFdf=reinterpret_cast<wht::ClearFrame129>(executableBase+0x46d8c0);
 showConsole=reinterpret_cast<wht::ClearFrame129>(executableBase+0x489110);hideConsole=reinterpret_cast<wht::ClearFrame129>(executableBase+0x488960);
 clickButton=reinterpret_cast<wht::ClickFn129>(executableBase+0x474bd0);
 minimizeBoard=reinterpret_cast<wht::ClearFrame129>(executableBase+0x25efa0);
 maximizeBoard=reinterpret_cast<wht::ClearFrame129>(executableBase+0x25eec0);
 quickChatApi={reinterpret_cast<decltype(quickChatApi.open)>(executableBase+0x22e320),reinterpret_cast<decltype(quickChatApi.text)>(executableBase+0x481c70),reinterpret_cast<decltype(quickChatApi.submit)>(executableBase+0x1ee2f0)};
 resourceApi={reinterpret_cast<decltype(resourceApi.player)>(executableBase+0xa4240),reinterpret_cast<decltype(resourceApi.local)>(executableBase+0x93790),reinterpret_cast<decltype(resourceApi.ally)>(executableBase+0x99510),reinterpret_cast<decltype(resourceApi.observer)>(executableBase+0x99630),reinterpret_cast<decltype(resourceApi.slot)>(executableBase+0x94980),reinterpret_cast<decltype(resourceApi.state)>(executableBase+0x94a80),reinterpret_cast<decltype(resourceApi.color)>(executableBase+0x94540),reinterpret_cast<decltype(resourceApi.resolve)>(executableBase+0x82770),reinterpret_cast<decltype(resourceApi.name)>(executableBase+0xbf430)};
 manaBar129.playerApi=resourceApi;hpColor129.playerApi=resourceApi;
 auto fogSetter=reinterpret_cast<unsigned char*>(executableBase)+0x56cd30;
 const unsigned char fogSignature[]={0x55,0x8b,0xec,0x8b,0x55,0x08,0x8b,0x4d,0x0c};
 if(!memcmp(fogSetter,fogSignature,sizeof(fogSignature))){
  if(MH_CreateHook(fogSetter,FogNative,reinterpret_cast<void**>(&setFogReal))!=MH_OK||MH_EnableHook(fogSetter)!=MH_OK){setFogReal=nullptr;wht::Log(folder,"WHT 1.29: fog capture hook failed.");}
 }
 else wht::Log(folder,"WHT 1.29: scene-fog setter signature mismatch; fog end update unavailable.");
 wht::Log(folder,"WHT 1.29: camera, native menu/macro, normal slot hotkeys, quick chat, auto click, simulation clock, FPS, Time of Day and message clearing installed. Other gameplay ports remain incomplete.");return 0;
}
}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,void*){
 if(reason==DLL_PROCESS_ATTACH){DisableThreadLibraryCalls(instance);HANDLE t=CreateThread(nullptr,0,Install,instance,0,nullptr);if(t)CloseHandle(t);}
 return TRUE;
}
extern "C" DWORD __stdcall WHT_RebuildVersion(){return 0x00010000;}
