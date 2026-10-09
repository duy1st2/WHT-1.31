#include "auto_skills129.hpp"
#include "self_cast129.hpp"
#include "create_item129.hpp"
#include "quick_drop129.hpp"
#include "save_game129.hpp"
#include "network_hooks129.hpp"
#include "common.hpp"
#include "native_ui129.hpp"
#include "macro_recorder.hpp"
#include "memory_overlay.hpp"
#include "services129.hpp"
#include "mute129.hpp"
#include "repeat_chat129.hpp"
#include "repeat_message129.hpp"
wht::RepeatMessage129 repeatMessage129;
uint64_t queuedChatDue129=0;
wht::RepeatChat129 repeatChat129;
wht::Mute129 mutedChat129;
#include "resource129.hpp"
#include "resource_board129.hpp"
#include "skills129.hpp"
#include "unit_info129.hpp"
#include "inventory_info129.hpp"
#include "ability_button129.hpp"
#include "unit_scale129.hpp"
#include "unit_invulnerability129.hpp"
#include "teleport129.hpp"
#include "remove_ability129.hpp"
#include "add_ability129.hpp"
#include "kill_unit129.hpp"
#include "hero_skill_points129.hpp"
#include "hero_attributes129.hpp"
#include "hero_level129.hpp"
#include "resource_commands129.hpp"
#include "move_speed129.hpp"
#include "armor129.hpp"
#include "attack_cooldown129.hpp"
#include "attack_speed129.hpp"
#include "no_cooldown129.hpp"
#include "game_speed129.hpp"
#include "speed_clock129.hpp"
#include "reselect129.hpp"
#include "create_unit129.hpp"
#include "copy_unit129.hpp"
#include "drop_items129.hpp"
#include "inventory_actions129.hpp"
#include "mth129.hpp"
#include "fog_click129.hpp"
#include "random_seed129.hpp"
#include "share_control129.hpp"
wht::InventoryActions129 inventoryActions129;
wht::Mth129 mth129;
wht::NoCooldown129 noCooldown129;
#include "remove_unit129.hpp"
#include "unit_damage129.hpp"
#include "base_damage129.hpp"
#include "pan_camera129.hpp"
#include "mouse_position129.hpp"
#include "life_mana129.hpp"
#include "stats129.hpp"
#include "stats_tooltip.hpp"
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
#include "fps_limit129.hpp"
#include "fps_timer.hpp"
#include "unlock_fps129.hpp"
#include "presentation129.hpp"
#include "notification129.hpp"
#include "time_boss129.hpp"
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
wht::FpsLimit129 fpsLimit129;
wht::FpsTimerState fpsTimer129;
unsigned loggedFpsInterval129=~0u;
wht::UnlockFps129 unlockFps129;
wht::FpsApi129 fpsApi129{};
wht::LocalMessage129 localMessage129=nullptr;
using PresentParameters129=void(__thiscall*)(uintptr_t,uint32_t*);
PresentParameters129 originalPresentParameters129=nullptr;

std::set<std::wstring> loadedLibraries129,mountedArchives129;
std::vector<HANDLE> extraArchives129;
bool pluginsStarted129=false,pluginsLoading129=false;
std::vector<HMODULE> extraLibraries129;

std::wstring runtimeIni,runtimeFolder;
using SpeedQpc129=BOOL(WINAPI*)(LARGE_INTEGER*);
using SpeedTick129=DWORD(WINAPI*)();
SpeedQpc129 originalSpeedQpc129=nullptr;
SpeedTick129 originalSpeedTime129=nullptr,originalSpeedTick129=nullptr;
std::mutex speedClockMutex129;
wht::ScaledClock129 speedQpcClock129;
wht::ScaledTick129 speedTimeClock129,speedTickClock129;
bool speedClocksReady129=false;
bool ApplySpeedMultiplier129(float speed){
 if(!speedClocksReady129||!std::isfinite(speed)||speed<1||speed>70)return false;
 std::lock_guard<std::mutex> lock(speedClockMutex129);
 LARGE_INTEGER now{};if(!originalSpeedQpc129(&now))return false;
 speedQpcClock129.Set(now.QuadPart,speed);
 speedTimeClock129.Set(originalSpeedTime129(),speed);speedTickClock129.Set(originalSpeedTick129(),speed);return true;
}
BOOL WINAPI SpeedQpcHook129(LARGE_INTEGER* result){
 if(!originalSpeedQpc129||!originalSpeedQpc129(result))return FALSE;
 std::lock_guard<std::mutex> lock(speedClockMutex129);
 result->QuadPart=static_cast<LONGLONG>(speedQpcClock129.Read(result->QuadPart));return TRUE;
}
DWORD WINAPI SpeedTimeHook129(){const auto now=originalSpeedTime129();std::lock_guard<std::mutex> lock(speedClockMutex129);return speedTimeClock129.Read(now);}
DWORD WINAPI SpeedTickHook129(){const auto now=originalSpeedTick129();std::lock_guard<std::mutex> lock(speedClockMutex129);return speedTickClock129.Read(now);}
void InstallSpeedClocks129(){
 auto kernel=GetModuleHandleW(L"kernel32.dll");auto multimedia=GetModuleHandleW(L"winmm.dll");
 if(!kernel||!multimedia)return;
 void* addresses[]={reinterpret_cast<void*>(GetProcAddress(kernel,"QueryPerformanceCounter")),reinterpret_cast<void*>(GetProcAddress(multimedia,"timeGetTime")),reinterpret_cast<void*>(GetProcAddress(kernel,"GetTickCount"))};
 void* hooks[]={reinterpret_cast<void*>(SpeedQpcHook129),reinterpret_cast<void*>(SpeedTimeHook129),reinterpret_cast<void*>(SpeedTickHook129)};
 void** originals[]={reinterpret_cast<void**>(&originalSpeedQpc129),reinterpret_cast<void**>(&originalSpeedTime129),reinterpret_cast<void**>(&originalSpeedTick129)};
 unsigned created=0,enabled=0;
 for(;created<3;++created)if(!addresses[created]||MH_CreateHook(addresses[created],hooks[created],originals[created])!=MH_OK)break;
 if(created==3)for(;enabled<3;++enabled)if(MH_EnableHook(addresses[enabled])!=MH_OK)break;
 speedClocksReady129=enabled==3;
 if(!speedClocksReady129){for(unsigned i=0;i<enabled;++i)MH_DisableHook(addresses[i]);for(unsigned i=0;i<created;++i)MH_RemoveHook(addresses[i]);originalSpeedQpc129=nullptr;originalSpeedTime129=originalSpeedTick129=nullptr;}
}
wht::Config runtimeConfig;
using SpeedDescription129=void(__cdecl*)(char*,unsigned,float,float);
SpeedDescription129 originalAttackDescription129=nullptr,originalMovementDescription129=nullptr;
bool detailedStatsHooksReady129=false;
bool AppendStatsDescription129(char* destination,unsigned capacity,const char* detail){
 __try {
  if(!destination||!detail||capacity!=512)return false;
  unsigned length=0;while(length<capacity&&destination[length])++length;
  const auto extra=strlen(detail);
  if(length==capacity||length+extra+3>capacity)return false;
  memcpy(destination+length,"|n",2);memcpy(destination+length+2,detail,extra+1);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool ReadAttackTooltip129(float& multiplier,float& cooldown,float& maximum){
 __try {
  const unsigned char signature[]={0x8b,0x86,0xb4,0x01,0,0};
  if(!executableBase||memcmp((void*)(executableBase+0x7206eb),signature,sizeof(signature)))return false;
  const auto selected=wht::SelectedSkill129(executableBase);
  if(!selected.unit||!selected.identity||*reinterpret_cast<uintptr_t*>(selected.unit)!=executableBase+0xa9c504)return false;
  const auto attack=*reinterpret_cast<uintptr_t*>(selected.unit+0x3f4);if(!attack)return false;
  multiplier=*reinterpret_cast<float*>(attack+0x1b4);
  cooldown=*reinterpret_cast<float*>(attack+0x15c);
  maximum=*reinterpret_cast<float*>(executableBase+0xdb4c6c);
  const auto after=wht::SelectedSkill129(executableBase);
  return after.unit==selected.unit&&after.identity==selected.identity&&*reinterpret_cast<uintptr_t*>(selected.unit+0x3f4)==attack;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void __cdecl AttackDescription129(char* destination,unsigned capacity,float first,float second){
 originalAttackDescription129(destination,capacity,first,second);
 if(runtimeConfig.mode!=wht::RunMode::Local||!runtimeConfig.statsTooltip)return;
 float multiplier=0,cooldown=0,maximum=0;
 if(ReadAttackTooltip129(multiplier,cooldown,maximum)){
  const auto detail=wht::AttackSpeedTooltip(multiplier,cooldown,maximum);
  if(!detail.empty())AppendStatsDescription129(destination,capacity,detail.c_str());
 }
}
void __cdecl MovementDescription129(char* destination,unsigned capacity,float first,float second){
 originalMovementDescription129(destination,capacity,first,second);
 if(runtimeConfig.mode!=wht::RunMode::Local||!runtimeConfig.statsTooltip)return;
 const auto detail=wht::MovementSpeedTooltip(first);
 if(!detail.empty())AppendStatsDescription129(destination,capacity,detail.c_str());
}
void __fastcall PresentParametersHook129(uintptr_t renderer,DWORD,uint32_t* parameters){
 originalPresentParameters129(renderer,parameters);
 if(wht::ConfigureVSync129(parameters,runtimeConfig.mode==wht::RunMode::Local,runtimeConfig.vsync))
  wht::Log(runtimeFolder,runtimeConfig.vsync?"WHT 1.29 Direct3D9: VSync ON":"WHT 1.29 Direct3D9: VSync OFF");
}
float __cdecl MenuTransitionHook129(unsigned kind,unsigned sequence,unsigned a,unsigned b,uintptr_t callback,uintptr_t context){
 const bool enabled=runtimeConfig.mode==wht::RunMode::Local&&runtimeConfig.fastMenu;
 fastMenuAnimation129.Apply(executableBase,enabled);
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
wht::AutoSkills129 autoSkills129;
wht::SkillSelection129 pinnedTarget129{};
wht::WindowServices129 windowServices;
using ClipCursorApi129=BOOL(WINAPI*)(const RECT*);
ClipCursorApi129 originalClipCursor129=nullptr;
BOOL WINAPI ClipCursorHook129(const RECT* rect){
 if(rect&&window&&GetForegroundWindow()==window&&runtimeConfig.mode==wht::RunMode::Local&&!runtimeConfig.lockCursor)rect=nullptr;
 return originalClipCursor129?originalClipCursor129(rect):FALSE;
}
wht::FpsState129 fpsState;
wht::TimeOfDay129 timeOfDay;
wht::ConsoleState129 consoleState;
wht::ClearFrame129 showConsole=nullptr,hideConsole=nullptr;
wht::FpsMode129 fpsMode=nullptr;
wht::ClearFrame129 showFdf=nullptr,hideFdf=nullptr;
wht::ClickFn129 clickButton=nullptr;
wht::ClearFrame129 minimizeBoard=nullptr,maximizeBoard=nullptr;
wht::AllSkills129 allSkills;
wht::TimeBoss129 timeBoss129;
wht::SmartCast129 smartCast129;
void* skillsIconFrame=nullptr;void* skillsIcon=nullptr;bool skillsIconShown=false,skillsIconPressed=false;
wht::ToolMenuRect skillsIconRect{.558f,.012f,.012f,.016f};
void* dropIconFrame=nullptr;void* dropIcon=nullptr;bool dropIconShown=false,dropIconPressed=false;
wht::ToolMenuRect dropIconRect{.558f,.068f,.012f,.016f};
void* inventoryIconFrames129[2]{};void* inventoryIconTextures129[2]{};bool inventoryIconsShown129[2]{},inventoryIconsPressed129[2]{};
wht::ToolMenuRect inventoryIconRects129[2]={{.558f,.096f,.012f,.016f},{.558f,.040f,.012f,.016f}};
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
  wht::ShowNotification129(executableBase,wht::Utf8(wht::SettingFeedback(field==0?L"Camera":L"FogZ",std::to_wstring(value))),localMessage129);
  wht::Log(runtimeFolder,"WHT 1.29: applied local camera command: "+std::string(text));
 }
 if(channel>=0&&channel<=2&&CameraContext(camera,local)&&sender==local){
  chatTyping=false;
  if(text)ToolCommand(wht::Wide(text));
 }
 if(runtimeConfig.mode==wht::RunMode::Local&&channel>=0&&channel<=2&&CameraContext(camera,local))repeatChat129.Observe(sender,local,text);
 EnsureInput();
 if(!timer)timer=SetTimer(nullptr,0,16,Refresh);
 if(runtimeConfig.mode!=wht::RunMode::Local||!mutedChat129.Hidden(sender,channel))originalChat(object,sender,text,channel,duration);
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
wht::FogClick129 fogClick129;
using SelectionDispatch129=int(__cdecl*)(void*);
SelectionDispatch129 originalSelectionDispatch129=nullptr;
int __cdecl SelectionDispatchHook129(void* packet){if(runtimeConfig.mode==wht::RunMode::Local&&runtimeConfig.fogClick)fogClick129.Capture(executableBase,packet);return originalSelectionDispatch129(packet);}
bool ControlAvailable129(const wht::ToolMenuControl& d){
 if(!wcscmp(d.section,L"Event")&&!wcscmp(d.key,L"Fog Click"))return originalSelectionDispatch129!=nullptr;
 if(!wcscmp(d.section,L"MTHKEY"))for(const auto* key:{L"Noti",L"PickGoku",L"PickGokuDraw",L"ChayGa1",L"ChayGa2",L"ChayGa3",L"ChaySoi1",L"ChaySoi2",L"ChaySoi3",L"ChayGa1_count",L"ChayGa2_count",L"ChayGa3_count",L"ChaySoi1_count",L"ChaySoi2_count",L"ChaySoi3_count",L"AutoGaDraw1",L"AutoGaDraw2",L"AutoGaDraw3",L"AutoSoiDraw1",L"AutoSoiDraw2",L"AutoSoiDraw3",L"Shield",L"Ice",L"Wild",L"Frozen",L"Phoenix",L"ShieldDraw",L"IceDraw",L"WildDraw",L"FrozenDraw",L"PhoenixDraw"})if(!wcscmp(d.key,key))return clickButton!=nullptr;
 if(!wcscmp(d.section,L"MTHKEY")&&(!wcscmp(d.key,L"TimeBoss")||!wcscmp(d.key,L"MTH49Enable")||!wcscmp(d.key,L"G17Enable")))return levelHooksReady129;
 if(!wcscmp(d.section,L"Event")&&(!wcscmp(d.key,L"Borderless")||!wcscmp(d.key,L"Lock Cursor")||!wcscmp(d.key,L"Sound Start Game")))return true;
 if(!wcscmp(d.section,L"Event")&&!wcscmp(d.key,L"Fast Menu"))return originalMenuTransition129!=nullptr;
 if(!wcscmp(d.section,L"Event")&&!wcscmp(d.key,L"ModeCast"))return clickButton!=nullptr;
 if(!wcscmp(d.section,L"StateTag")||!wcscmp(d.section,L"ManaBar"))return levelHooksReady129;
 if(!wcscmp(d.section,L"Language")||!wcscmp(d.section,L"MacroRecorder")||!wcscmp(d.section,L"DelayClear"))return true;
 if(!wcscmp(d.section,L"Quick Chat")||!wcscmp(d.section,L"AutoSkills"))return true;
 if(!wcscmp(d.section,L"HotKeyOther"))return !wcscmp(d.key,L"hko_menu")||!wcscmp(d.key,L"hko_command")||!wcscmp(d.key,L"hko_MinimizeMultiboard");
 if(!wcscmp(d.section,L"SlotHotkey")||!wcscmp(d.section,L"SlotHotkeyInv")||!wcscmp(d.section,L"SlotHotkeyShop"))return true;
 if(!wcscmp(d.section,L"Graphics")&&!wcscmp(d.key,L"VSync"))return originalPresentParameters129!=nullptr;
 if(!wcscmp(d.section,L"Graphics")&&!wcscmp(d.key,L"Unlock Limit FPS"))return unlockFps129.ready&&!unlockFps129.conflict;
 if(!wcscmp(d.section,L"Graphics"))return !wcscmp(d.key,L"timeOfDayUI")||!wcscmp(d.key,L"ConsoleUI")||!wcscmp(d.key,L"Text Life/Mana Commas")||!wcscmp(d.key,L"Text Life/Mana No Hide")||!wcscmp(d.key,L"Text Stats Commas")||(!wcscmp(d.key,L"Detailed Stats Tooltip")&&detailedStatsHooksReady129)||!wcscmp(d.key,L"MouseTooltipUI")||!wcscmp(d.key,L"BlackBarUI")||!wcscmp(d.key,L"Color Player")||(!wcscmp(d.key,L"BarInvul")&&invulnerableHookReady129)||(!wcscmp(d.key,L"VertexColor")&&originalVertexColor129);
 if(!wcscmp(d.section,L"HotKeyHero"))return !wcsncmp(d.key,L"HeroF",5)||!wcscmp(d.key,L"AutoClickLeft")||!wcscmp(d.key,L"AutoClickRight")||!wcscmp(d.key,L"SetIdTarget")||!wcscmp(d.key,L"SellFast");
 return !wcscmp(d.section,L"Event")&&(!wcscmp(d.key,L"Quick Drop Item")||!wcscmp(d.key,L"Show Resource")||!wcscmp(d.key,L"Show Mem")||!wcscmp(d.key,L"Show Timer")||!wcscmp(d.key,L"Show FPS")||!wcscmp(d.key,L"Show Ins All Skills")||!wcscmp(d.key,L"Show Drop All")||!wcscmp(d.key,L"Show Auto Pick")||!wcscmp(d.key,L"Show Sell All")||!wcscmp(d.key,L"Hide Option Menu")||!wcscmp(d.key,L"Show Hotkey In Panel")||!wcscmp(d.key,L"ShowCooldown")||!wcscmp(d.key,L"ShowRegenHP")||!wcscmp(d.key,L"ShowRegenMana")||(!wcscmp(d.key,L"Keyboard Scroll Speed")&&setKeyboardScroll129)||(!wcscmp(d.key,L"Mouse Scroll Speed")&&setMouseScroll129));
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
bool quickDropConsumed129=false;
bool ToolCommandImpl129(const std::wstring& command){
 if(runtimeConfig.mode!=wht::RunMode::Local)return false;
 std::wistringstream in(command);std::wstring name,action,extra;in>>name;
 if(name==L"#fc"||name==L"#fogclick"){
  if(!originalSelectionDispatch129)return false;std::wstring value=runtimeConfig.fogClick?L"0":L"1";
  if(in>>value){if((value!=L"0"&&value!=L"1")||in>>extra)return false;}
  if(!wht::SaveSetting(runtimeIni,L"Event",L"Fog Click",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;
  wht::ShowNotification129(executableBase,runtimeConfig.fogClick?"Fog Click: ON":"Fog Click: OFF",localMessage129);return true;
 }
 if(name==L"#gabase"||name==L"#backga"||name==L"#backsoi"){
  if(in>>extra)return false;
  const bool setting=name==L"#gabase";
  const bool ok=setting?mth129.SetBase(executableBase):mth129.QueueReturn(executableBase);
  wht::ShowNotification129(executableBase,setting?(ok?"Courier Base: position saved":"Courier Base: select a living unit"):(ok?"Courier Return: queued":"Courier Return: assign a unit with #setga and save a base with #gabase first; a recipe may be running"),localMessage129);return ok;
 }
 if(name==L"#setga"){
  if(in>>extra)return false;
  const bool ok=mth129.SetCourier(executableBase);
  wht::ShowNotification129(executableBase,ok?"Set Ga: selected unit assigned":"Set Ga: select a living owned unit",localMessage129);
  return ok;
 }
 const wchar_t* routeCommands[]={L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};
 for(int i=0;i<6;++i)if(name==routeCommands[i]){
  if(in>>extra)return false;
  if(!(runtimeConfig.mth49Enable||runtimeConfig.g17Enable))return false;
  const int counts[]={runtimeConfig.chayGa1Count,runtimeConfig.chayGa2Count,runtimeConfig.chayGa3Count,runtimeConfig.chaySoi1Count,runtimeConfig.chaySoi2Count,runtimeConfig.chaySoi3Count};
  const bool ok=mth129.QueueRoute(executableBase,i,counts[i]);
  if(runtimeConfig.mthNoti)wht::ShowNotification129(executableBase,std::string(wht::MthName129(i+6))+": "+(ok?"ON":"select a Chicken/Wolf unit or assign a living owned unit with #setga; another MTH action may be running"),localMessage129);return ok;
 }
 if(name==L"#goku"||name==L"#autopickgoku"){
  bool enabled=!inventoryActions129.autoPickGoku;
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;enabled=action==L"1";}
  if(enabled&&!(runtimeConfig.mth49Enable||runtimeConfig.g17Enable))return false;
  inventoryActions129.ToggleGoku(enabled);
  if(runtimeConfig.mthNoti)wht::ShowNotification129(executableBase,enabled?"Auto Pick Goku: ON":"Auto Pick Goku: OFF",localMessage129);
  return true;
 }
 for(int i=0;i<5;++i){
  const wchar_t* commands[]={L"#shield",L"#ice",L"#wild",L"#frozen",L"#phoenix"};
  if(name==commands[i]){
   if(in>>extra||!(runtimeConfig.mth49Enable||runtimeConfig.g17Enable)||!clickButton)return false;
   const bool queued=mth129.Queue(executableBase,i);
   if(runtimeConfig.mthNoti)wht::ShowNotification129(executableBase,std::string(wht::MthName129(i))+": "+(queued?"queued":"select a hero; another recipe may be running"),localMessage129);
   return queued;
  }
 }
 if(name==L"#copyitem"){
  std::wstring args;std::getline(in,args);unsigned slot=0,count=1;
  if(!wht::ParseCopyItem129(args,slot,count)){wht::ShowNotification129(executableBase,"Usage: #copyitem <slot 1..6> [1..50]",localMessage129);return false;}
  wht::InventoryInfo129 inventory;
  if(!wht::ReadInventoryInfo129(executableBase,inventory)||slot>=inventory.capacity||!inventory.codes[slot]){wht::ShowNotification129(executableBase,"Copy Item: select a unit with an item in that slot",localMessage129);return false;}
  wht::MousePosition129 cursor;if(!wht::ReadMousePosition129(executableBase,cursor)){wht::ShowNotification129(executableBase,"Copy Item: point the cursor at the map",localMessage129);return false;}
  auto made=wht::CreateItems129(executableBase,wht::ResolveCreateItem129(executableBase),inventory.codes[slot],count,cursor.x,cursor.y);
  wht::ShowNotification129(executableBase,"Copy Item: "+std::to_string(made)+"/"+std::to_string(count)+" created (default charges)",localMessage129);return made==count;
 }
 if(name==L"#item"){
  std::wstring args;std::getline(in,args);uint32_t code=0;unsigned count=0;
  if(!wht::ParseCreateUnit129(args,code,count)){wht::ShowNotification129(executableBase,"Usage: #item <rawcode> [1..50]",localMessage129);return false;}
  wht::MousePosition129 cursor;if(!wht::ReadMousePosition129(executableBase,cursor)){wht::ShowNotification129(executableBase,"Create Item: point the cursor at the map",localMessage129);return false;}
  auto made=wht::CreateItems129(executableBase,wht::ResolveCreateItem129(executableBase),code,count,cursor.x,cursor.y);
  wht::ShowNotification129(executableBase,"Create Item: "+std::to_string(made)+"/"+std::to_string(count)+(made?" created":" (invalid rawcode or unavailable native)"),localMessage129);return made==count;
 }
 if(name==L"#quickdrop"){
  bool enabled=!runtimeConfig.quickDropItem;
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra){wht::ShowNotification129(executableBase,"Usage: #quickdrop [0|1]",localMessage129);return false;}enabled=action==L"1";}
  if(!wht::SaveSetting(runtimeIni,L"Event",L"Quick Drop Item",enabled?L"1":L"0"))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;wht::ShowNotification129(executableBase,std::string("Quick Drop (Shift + Right Click): ")+(enabled?"ON":"OFF"),localMessage129);return true;
 }
 if(name==L"#dropitem"){
  if(!(in>>action)||action.size()!=1||action[0]<L'1'||action[0]>L'6'||in>>extra){wht::ShowNotification129(executableBase,"Usage: #dropitem <1..6>",localMessage129);return false;}
  const char* error=nullptr;const auto slot=unsigned(action[0]-L'1');
  inventoryActions129.selling=false;inventoryActions129.due=GetTickCount64()+2000;
  const bool ok=wht::DropItemSlot129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveDropItems129(executableBase),slot,&error);
  wht::ShowNotification129(executableBase,ok?"Dropped item from slot "+std::to_string(slot+1):std::string("Drop Item: ")+(error?error:"Unavailable"),localMessage129);return ok;
 }
 if(name==L"#save"){
  std::wstring args;std::getline(in,args);std::string filename;const char* error=nullptr;
  if(!wht::ParseSaveName129(args,filename)){wht::ShowNotification129(executableBase,"Usage: #save [name: letters, numbers, _ or -]",localMessage129);return false;}
  const bool ok=wht::RequestSaveGame129(executableBase,filename.c_str(),wht::ResolveSaveGame129(executableBase),&error);
  wht::ShowNotification129(executableBase,ok?"Save requested: "+filename:std::string("Save: ")+(error?error:"Unavailable"),localMessage129);return ok;
 }
 if(name==L"#shareall"){
  bool enabled=true;if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra){wht::ShowNotification129(executableBase,"Usage: #shareall [0|1]",localMessage129);return false;}enabled=action==L"1";}
  unsigned changed=0;const char* error=nullptr;
  const bool ok=wht::ShareControl129(wht::ResolveShareControl129(executableBase),enabled,changed,&error);
  wht::ShowNotification129(executableBase,std::string("Shared Control: ")+(enabled?"ON":"OFF")+" ("+std::to_string(changed)+" players changed)"+(ok?std::string{}:std::string(": ")+(error?error:"Unavailable")),localMessage129);return ok;
 }
 if(name==L"#srand"){
  std::wstring args;std::getline(in,args);uint32_t seed=0;const char* error=nullptr;
  if(!wht::ParseRandomSeed129(args,seed)){wht::ShowNotification129(executableBase,"Usage: #srand [0..4294967295] (default 0)",localMessage129);return false;}
  if(!wht::SetRandomSeed129(executableBase,seed,wht::ResolveRandomSeed129(executableBase),&error)){wht::ShowNotification129(executableBase,std::string("Random Seed: ")+(error?error:"Unavailable"),localMessage129);return false;}
  wht::ShowNotification129(executableBase,"Random Seed: "+std::to_string(seed),localMessage129);return true;
 }
 if(name==L"#autopick"){
  bool enabled=!inventoryActions129.autoPick;
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;enabled=action==L"1";}
  if(enabled&&!wht::ResolveInventoryActions129(executableBase).query){wht::ShowNotification129(executableBase,"Auto Pick: native signature mismatch",localMessage129);return false;}
  inventoryActions129.autoPick=enabled;inventoryActions129.due=0;
  wht::ShowNotification129(executableBase,enabled?"Auto Pick: ON":"Auto Pick: OFF",localMessage129);return true;
 }
 if(name==L"#sellslot"){
  unsigned slot=0;if(!(in>>slot)||slot<1||slot>6||in>>extra)return false;
  bool ok=inventoryActions129.StartSellSlot(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveDropItems129(executableBase),slot-1);
  wht::ShowNotification129(executableBase,ok?"Quick Sell: finding a nearby shop":"Quick Sell: select a unit with an item in this slot",localMessage129);return ok;
 }
 if(name==L"#sellall"){
  if(in>>extra)return false;
  if(!inventoryActions129.StartSell(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveDropItems129(executableBase))){wht::ShowNotification129(executableBase,"Sell All: select a live unit; native functions must be available",localMessage129);return false;}
  wht::ShowNotification129(executableBase,"Sell All: finding a nearby shop",localMessage129);return true;
 }
 if(name==L"#drop"||name==L"#dropall"){
  if(in>>extra){wht::ShowNotification129(executableBase,"Usage: #drop or #dropall (select a unit first)",localMessage129);return false;}
  inventoryActions129.selling=false;inventoryActions129.due=GetTickCount64()+2000;
  unsigned dropped=0;const char* error=nullptr;const auto api=wht::ResolveDropItems129(executableBase);
  const bool success=wht::DropItems129(executableBase,wht::SelectedSkill129(executableBase),api,dropped,&error);
  wht::ShowNotification129(executableBase,"Dropped "+std::to_string(dropped)+" items"+(success?std::string{}:std::string(": ")+(api?(error?error:"Native drop failed"):"Drop function signature mismatch")),localMessage129);
  return success;
 }
 if(name==L"#copy"){
  std::wstring args;std::getline(in,args);unsigned count=1;const char* error=nullptr;
  if(!wht::ParseCopyUnit129(args,count)){wht::ShowNotification129(executableBase,"Usage: #copy [1..50] (select a unit first)",localMessage129);return false;}
  const auto spawned=wht::CopyUnits129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveTeleportApi129(executableBase),wht::ResolveCreateUnit129(executableBase),count,&error);
  wht::ShowNotification129(executableBase,spawned?"Copy Unit: "+std::to_string(spawned)+"/"+std::to_string(count)+" created":std::string("Copy Unit: ")+(error?error:"Native function unavailable"),localMessage129);
  return spawned!=0;
 }
 if(name==L"#create"){
  std::wstring args;std::getline(in,args);uint32_t code=0;unsigned count=1;
  if(!wht::ParseCreateUnit129(args,code,count)){wht::ShowNotification129(executableBase,"Usage: #create rawcode [1..50]",localMessage129);return false;}
  wht::MousePosition129 cursor;
  if(!wht::ReadMousePosition129(executableBase,cursor)){wht::ShowNotification129(executableBase,"Create Unit: point the cursor at the map",localMessage129);return false;}
  const auto spawned=wht::CreateUnits129(executableBase,wht::ResolveCreateUnit129(executableBase),code,count,cursor.x,cursor.y);
  wht::ShowNotification129(executableBase,"Create Unit: "+std::to_string(spawned)+"/"+std::to_string(count)+(spawned?" created":" (invalid unit rawcode or native function unavailable)"),localMessage129);
  return spawned!=0;
 }
 if(name==L"#selc"){
  if(in>>extra){wht::ShowNotification129(executableBase,"Usage: #selc (select a unit first)",localMessage129);return false;}
  const char* error=nullptr;
  if(!wht::ReselectUnit129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveReselect129(executableBase),&error)){
   wht::ShowNotification129(executableBase,std::string("Reselect: ")+(error?error:"Native function unavailable"),localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Selected unit reselected",localMessage129);return true;
 }
 if(name==L"#speed"){
  std::wstring args;std::getline(in,args);float requested=1;unsigned actual=0;
  if(!wht::ParseSpeedMultiplier129(args,requested)){wht::ShowNotification129(executableBase,"Usage: #speed x (1..70, default 1)",localMessage129);return false;}
  if(!speedClocksReady129||!wht::SetGameSpeed129(executableBase,2,wht::ResolveGameSpeed129(executableBase),actual)||!ApplySpeedMultiplier129(requested)){
   wht::ShowNotification129(executableBase,"Game Speed: clock hooks unavailable, map speed locked, or native update failed",localMessage129);return false;
  }
  char text[80]{};std::snprintf(text,sizeof(text),"Game Speed: %.2fx",requested);wht::ShowNotification129(executableBase,text,localMessage129);return true;
 }
 if(name==L"#nocd"||name==L"#yescd"){
  if(in>>extra)return false;
  if(name==L"#yescd"){noCooldown129.Reset();wht::ShowNotification129(executableBase,"No Cooldown: OFF",localMessage129);return true;}
  const char* error=nullptr;
  if(!wht::ResetCooldown129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveCooldownReset129(executableBase),&error)){
   wht::ShowNotification129(executableBase,std::string("No Cooldown: ")+(error?error:"Native function unavailable"),localMessage129);return false;
  }
  noCooldown129.enabled=true;noCooldown129.due=GetTickCount64()+100;
  wht::ShowNotification129(executableBase,"No Cooldown: ON (selected unit)",localMessage129);return true;
 }
 if(name==L"#repeat"){
  std::wstring args,text;std::getline(in,args);unsigned count=0;bool stop=false;
  if(!wht::ParseRepeatMessage129(args,count,text,stop)){
   wht::ShowNotification129(executableBase,"Usage: #repeat <1..10000> <text>; #repeat stop",localMessage129);return false;
  }
  if(stop){repeatMessage129.Reset();wht::ShowNotification129(executableBase,"Repeat stopped",localMessage129);return true;}
  if(!repeatMessage129.Set(count,wht::Utf8(text))){wht::ShowNotification129(executableBase,"Repeat text: 1..128 UTF-8 bytes; no hash commands or line breaks",localMessage129);return false;}
  wht::ShowNotification129(executableBase,"Repeat queued: "+std::to_string(count)+" messages",localMessage129);return true;
 }
 if(name==L"#as"){
  std::wstring args;std::getline(in,args);float requested=2,actual=0,effective=0;const char* error=nullptr;
  if(!wht::ParseAttackSpeed129(args,requested)){wht::ShowNotification129(executableBase,"Usage: #as [multiplier 0.1..50] (default 2)",localMessage129);return false;}
  if(!wht::SetAttackSpeed129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveAttackSpeed129(executableBase),requested,actual,effective,&error)){
   wht::ShowNotification129(executableBase,std::string("Attack Speed: ")+(error?error:"Native function unavailable"),localMessage129);return false;
  }
  char text[128]{};std::snprintf(text,sizeof(text),"Attack Speed: %.2fx (effective map limit: %.2fx)",actual,effective);
  wht::ShowNotification129(executableBase,text,localMessage129);return true;
 }
 if(name==L"#attackcd"){
  std::wstring args;std::getline(in,args);float seconds=0,actual=0;unsigned weapon=0;const char* error=nullptr;
  if(!wht::ParseAttackCooldown129(args,seconds,weapon)){
   wht::ShowNotification129(executableBase,"Usage: #attackcd seconds(0.01..3600) [weapon 1|2]",localMessage129);return false;
  }
  if(!wht::SetAttackCooldown129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveAttackCooldown129(executableBase),seconds,weapon,actual,&error)){
   wht::ShowNotification129(executableBase,std::string("Attack Cooldown: ")+(error?error:"Native function unavailable"),localMessage129);return false;
  }
  char text[96]{};std::snprintf(text,sizeof(text),"Attack %u Cooldown: %.3f seconds",weapon+1,actual);wht::ShowNotification129(executableBase,text,localMessage129);return true;
 }
 if(name==L"#armor"){
  std::wstring args;std::getline(in,args);float requested=50,actual=0;const char* error=nullptr;
  if(!wht::ParseArmor129(args,requested)){
   wht::ShowNotification129(executableBase,"Usage: select a unit, then #armor [-1000000..1000000] (default 50)",localMessage129);return false;
  }
  if(!wht::SetArmor129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveArmor129(executableBase),requested,actual,&error)){
   wht::ShowNotification129(executableBase,std::string("Unit Armor: ")+(error?error:"Native function unavailable"),localMessage129);return false;
  }
  char text[80]{};std::snprintf(text,sizeof(text),"Unit Armor: %.2f",actual);wht::ShowNotification129(executableBase,text,localMessage129);return true;
 }
 if(name==L"#ms"){
  std::wstring args;std::getline(in,args);float requested=522,actual=0;const char* error=nullptr;
  if(!wht::ParseMoveSpeed129(args,requested)){
   wht::ShowNotification129(executableBase,"Usage: select a unit, then #ms [0..2000] (default 522)",localMessage129);return false;
  }
  if(!wht::ApplyMoveSpeed129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveMoveSpeed129(executableBase),requested,actual,&error)){
   wht::ShowNotification129(executableBase,std::string("Movement Speed: ")+(error?error:"Native function unavailable"),localMessage129);return false;
  }
  char text[128]{};std::snprintf(text,sizeof(text),"Movement Speed: %.1f (requested %.1f)",actual,requested);
  wht::ShowNotification129(executableBase,text,localMessage129);return true;
 }
 if(name==L"#udmgb"){
  std::wstring argument;std::getline(in,argument);int value=0,actual=0;const char* error=nullptr;
  if(!wht::ParseBaseDamage129(argument,value)){
   wht::ShowNotification129(executableBase,"Usage: select a unit, then #udmgb [0..1000000000]",localMessage129);return false;
  }
  if(!wht::SetSelectedBaseDamage129(executableBase,value,actual,&error)){
   const auto reason=std::string("Base Damage: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Unit Base Damage: "+std::to_string(actual),localMessage129);return true;
 }
 if(name==L"#udmg"){
  std::wstring argument;std::getline(in,argument);float amount=0,life=0;const char* error=nullptr;
  if(!wht::ParseUnitDamage129(argument,amount)){
   wht::ShowNotification129(executableBase,"Usage: select a unit, then #udmg [positive amount, up to 1000000000]",localMessage129);return false;
  }
  if(!wht::DamageSelectedUnit129(executableBase,amount,life,&error)){
   const auto reason=std::string("Unit Damage: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Unit life update invoked: target HP "+std::to_string(life),localMessage129);return true;
 }
 if(name==L"#udel"){
  if(in>>extra){wht::ShowNotification129(executableBase,"Usage: select a unit, then #udel",localMessage129);return false;}
  const char* error=nullptr;
  if(!wht::RemoveSelectedUnit129(executableBase,&error)){
   const auto reason=std::string("RemoveUnit: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"RemoveUnit invoked for selected unit",localMessage129);return true;
 }
 if(name==L"#ggl"||name==L"#gll"||name==L"#gg"||name==L"#gl"){
  std::wstring args;std::getline(in,args);unsigned slot=0;int gold=0,lumber=0;
  const bool single=name==L"#gg"||name==L"#gl";
  if(!(single?wht::ParseSingleResourceTransfer129(args,name==L"#gl",slot,gold,lumber):wht::ParseResourceTransfer129(args,name==L"#gll",slot,gold,lumber))){
   wht::ShowNotification129(executableBase,"Usage: #gg player(1..24) gold; #gl player(1..24) lumber; #ggl slot(0..23) gold lumber; #gll slot(0..23) lumber gold",localMessage129);return false;
  }
  const auto api=wht::ResolveResourceCommandApi129(executableBase);
  const auto source=wht::ResourceLocal129(executableBase,resourceApi);
  const auto target=api?wht::TransferTarget129(resourceApi,slot):0;bool partial=false;
  if(!wht::TransferResources129(source,target,api,gold,lumber,partial)){
   wht::ShowNotification129(executableBase,partial?"Resource transfer interrupted; some resource writes may have applied":"Transfer rejected: check player slot, balance and resource limits",localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Transferred Gold "+std::to_string(gold)+", Lumber "+std::to_string(lumber)+" to Player "+std::to_string(slot+1),localMessage129);return true;
 }
 if(name==L"#gold"||name==L"#lumber"||name==L"#food"||name==L"#fullres"){
  const auto kind=name==L"#gold"?wht::ResourceCommand129::Gold:name==L"#lumber"?wht::ResourceCommand129::Lumber:
   name==L"#food"?wht::ResourceCommand129::Food:wht::ResourceCommand129::Full;
  std::wstring argument;std::getline(in,argument);int amount=0;const char* error=nullptr;
  if(!wht::ParseResourceCommand129(kind,argument,amount)){
   wht::ShowNotification129(executableBase,"Usage: #gold/#lumber [positive amount], #food [cap], #fullres",localMessage129);return false;
  }
  const auto api=wht::ResolveResourceCommandApi129(executableBase);
  const auto player=wht::ResourceLocal129(executableBase,resourceApi);wht::ResourceCommandResult129 result;
  if(!wht::ApplyResourceCommand129(player,api,kind,amount,result,&error)){
   const auto reason=std::string("Resources: ")+(error?error:"Function signature mismatch");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Resources: Gold "+std::to_string(result.gold)+", Lumber "+std::to_string(result.lumber)+", Food cap "+std::to_string(result.food),localMessage129);return true;
 }
 if(name==L"#lvl"){
  std::wstring argument;std::getline(in,argument);int requested=0,actual=0;const char* error=nullptr;
  if(!wht::ParseHeroLevel129(argument,requested)){
   wht::ShowNotification129(executableBase,"Usage: select a hero, then #lvl [1..10000]",localMessage129);return false;
  }
  if(!wht::SetSelectedHeroLevel129(executableBase,requested,actual,&error)){
   const auto reason=std::string("Hero Level: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Hero Level: "+std::to_string(actual),localMessage129);return true;
 }
 if(name==L"#str"||name==L"#agi"||name==L"#int"||name==L"#fullatt"){
  std::wstring argument;std::getline(in,argument);int value=0;
  if(!wht::ParseHeroAttributeValue129(argument,value)){
   wht::ShowNotification129(executableBase,"Usage: select a hero, then #str/#agi/#int/#fullatt [1..10000]",localMessage129);return false;
  }
  const auto attribute=name==L"#str"?wht::HeroAttribute129::Strength:name==L"#agi"?wht::HeroAttribute129::Agility:
   name==L"#int"?wht::HeroAttribute129::Intelligence:wht::HeroAttribute129::All;
  int actual[3]{};const char* error=nullptr;
  if(!wht::SetSelectedHeroAttributes129(executableBase,attribute,value,actual,&error)){
   const std::string reason=std::string("Hero Attributes: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,name==L"#fullatt"?
   "Hero STR/AGI/INT: "+std::to_string(value):
   "Hero "+std::string(name==L"#str"?"STR":name==L"#agi"?"AGI":"INT")+": "+std::to_string(value),localMessage129);return true;
 }
 if(name==L"#sp"){
  std::wstring argument;std::getline(in,argument);int amount=0;
  if(!wht::ParseHeroSkillPoints129(argument,amount)){
   wht::ShowNotification129(executableBase,"Usage: select a hero, then #sp [1..10000]",localMessage129);return false;
  }
  int added=0,total=0;const char* error=nullptr;
  if(!wht::AddSelectedHeroSkillPoints129(executableBase,amount,added,total,&error)){
   const std::string reason=std::string("Hero Skill Points: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Hero Skill Points: +"+std::to_string(added)+" | Total: "+std::to_string(total),localMessage129);return true;
 }
 if(name==L"#kill"){
  if(in>>extra){wht::ShowNotification129(executableBase,"Usage: select a unit, then #kill",localMessage129);return false;}
  const char* error=nullptr;
  if(!wht::KillSelectedUnit129(executableBase,&error)){
   const std::string reason=std::string("KillUnit: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"KillUnit invoked for selected unit",localMessage129);return true;
 }
 if(name==L"#sabi"){
  std::wstring args;std::getline(in,args);uint32_t code=0;int level=1;
  if(!wht::ParseAddAbility129(args,code,level)){
   wht::ShowNotification129(executableBase,"Usage: #sabi AHhb [level 1..10000] (hex rawcode also accepted)",localMessage129);return false;
  }
  wht::AddAbilityResult129 state;const char* error=nullptr;
  if(!wht::AddSelectedAbility129(executableBase,code,level,state,&error)){
   const std::string reason=std::string(state.added?"Ability added, but level update failed: ":"Add Ability: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  char raw[5]{};for(unsigned i=0;i<4;++i){const auto ch=(code>>(24-8*i))&255;raw[i]=ch>=32&&ch<=126?char(ch):'?';}
  wht::ShowNotification129(executableBase,std::string(state.added?"Added Ability: ":"Existing Ability: ")+raw+" | Level: "+std::to_string(state.level),localMessage129);return true;
 }
 if(name==L"#rabi"){
  std::wstring args;std::getline(in,args);uint32_t code=0;
  if(!wht::ParseRemoveAbility129(args,code)){
   wht::ShowNotification129(executableBase,"Usage: select a unit, then #rabi AHhb (or #rabi 0x41486862)",localMessage129);return false;
  }
  bool removed=false;const char* error=nullptr;
  if(!wht::RemoveSelectedAbility129(executableBase,code,removed,&error)){
   const std::string reason=std::string("Remove Ability: ")+(error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  char raw[5]{};for(unsigned i=0;i<4;++i){const auto ch=(code>>(24-8*i))&255;raw[i]=ch>=32&&ch<=126?char(ch):'?';}
  wht::ShowNotification129(executableBase,std::string(removed?"Removed Ability: ":"Ability already absent: ")+raw,localMessage129);return true;
 }
 if(name==L"#tele"){
  if(in>>extra){wht::ShowNotification129(executableBase,"Usage: select a unit, point at the map, then #tele",localMessage129);return false;}
  wht::MousePosition129 cursor;wht::TeleportPosition129 position;const char* error=nullptr;
  const bool projected=wht::ReadMousePosition129(executableBase,cursor);
  if(!projected||!wht::TeleportSelected129(executableBase,cursor.x,cursor.y,position,&error)){
   const std::string reason=std::string("Teleport: ")+(!projected?"Point the cursor at the map":error?error:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Unit Position: "+wht::UnitPositionText129(position.x,position.y),localMessage129);return true;
 }
 if(name==L"#invul"||name==L"#vul"){
  if(in>>extra){wht::ShowNotification129(executableBase,"Usage: select a unit, then #invul or #vul",localMessage129);return false;}
  const char* failure=nullptr;wht::InvulnerabilityResult129 state;
  const bool enabled=name==L"#invul";
  if(!wht::SetUnitInvulnerability129(executableBase,enabled,state,&failure)){
   const std::string reason=std::string("Unit Invulnerability: ")+(failure?failure:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);return false;
  }
  const std::string status=enabled?"Unit Invulnerability: ON":state.effective?"Avul: OFF; another map effect keeps this unit invulnerable":"Unit Invulnerability: OFF";
  wht::ShowNotification129(executableBase,status,localMessage129);return true;
 }
 if(name==L"#size"){
  std::wstring args;std::getline(in,args);float scale=1;
  if(!wht::ParseUnitScale129(args,scale)){
   wht::ShowNotification129(executableBase,"Usage: #size 200 (2x) or #size 100 (1x)",localMessage129);return false;
  }
  const char* failure=nullptr;const bool applied=wht::SetUnitScale129(executableBase,scale,&failure,setFogReal);
  if(!applied){
   const std::string reason=std::string("Unit Scale: ")+(failure?failure:"Unknown failure");
   wht::Log(runtimeFolder,reason);wht::ShowNotification129(executableBase,reason,localMessage129);
  }
  return applied;
 }
 if(name==L"#hideb"||name==L"#showb"){
  uint32_t code=0;if(!(in>>action)||in>>extra||!wht::AbilityCode129(action,code))return false;
  return wht::SetAbilityButton129(executableBase,code,name==L"#hideb");
 }
 if(name==L"#ping"){return !(in>>extra);}
 if(name==L"#wsize"){
  int width=0,height=0;
  if(!(in>>width>>height)||in>>extra||width<320||height<240||width>16384||height>16384)return false;
  if(!window||!IsWindow(window)||IsIconic(window))return false;
  return SetWindowPos(window,nullptr,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED)!=0;
 }
 if(name==L"#uflag"){
  if(in>>extra)return false;POINT point{};return window&&IsWindow(window)&&GetCursorPos(&point)&&ScreenToClient(window,&point);
 }
 if(name==L"#islot"){
  if(in>>extra)return false;wht::InventoryInfo129 info;return wht::ReadInventoryInfo129(executableBase,info);
 }
 if(name==L"#mpos"){
  if(in>>extra)return false;wht::MousePosition129 position;return wht::ReadMousePosition129(executableBase,position);
 }
 if(name==L"#ppos"){
  std::wstring args;std::getline(in,args);float x=0,y=0;uintptr_t camera=0;uint16_t local=0;
  if(args.find_first_not_of(L" \t")==std::wstring::npos){wht::MousePosition129 position;return wht::ReadMousePosition129(executableBase,position);}
  return wht::ParsePan129(args,x,y)&&CameraContext(camera,local)&&wht::PanCamera129(executableBase,camera,x,y);
 }
 if(name==L"#focus"){
  if(in>>extra){wht::ShowNotification129(executableBase,"Usage: select a unit, then #focus",localMessage129);return false;}
  wht::UnitInfo129 info;uintptr_t camera=0;uint16_t local=0;
  if(!wht::ReadUnitInfo129(executableBase,true,info)||!CameraContext(camera,local)){
   wht::ShowNotification129(executableBase,"Camera Focus: no valid selected unit or camera",localMessage129);return false;
  }
  if(!wht::PanCamera129(executableBase,camera,info.x,info.y)){
   wht::ShowNotification129(executableBase,"Camera Focus: native camera call unavailable",localMessage129);return false;
  }
  wht::ShowNotification129(executableBase,"Camera Focus: "+wht::UnitPositionText129(info.x,info.y),localMessage129);return true;
 }
 if(name==L"#upos"||name==L"#cname"){
  if(in>>extra)return false;wht::UnitInfo129 info;
  return wht::ReadUnitInfo129(executableBase,name==L"#upos",info);
 }
 if(name==L"#nhailai"){
  uintptr_t camera=0;uint16_t local=0;if(!CameraContext(camera,local))return false;
  if(!(in>>action)){repeatChat129.Reset();return true;}
  if(in>>extra)return false;
  if(action==L"all"){repeatChat129.ToggleAll();return true;}
  std::wistringstream number(action);unsigned slot=0;wchar_t tail=0;
  if(!(number>>slot)||number>>tail)return false;
  return repeatChat129.Set(slot,local);
 }
 if(name==L"#mute"||name==L"#unmute"){
  unsigned slot=0;uintptr_t camera=0;uint16_t local=0;
  if(!(in>>slot)||in>>extra||!CameraContext(camera,local))return false;
  return mutedChat129.Set(slot,local,name==L"#mute");
 }
 if(name==L"#autol"||name==L"#autoclickl"||name==L"#autor"||name==L"#autoclickr"){
  const bool left=name==L"#autol"||name==L"#autoclickl";
  bool& enabled=left?autoClick.left:autoClick.right;
  bool next=!enabled;
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;next=action==L"1";}
  enabled=next;autoClick.due=0;return true;
 }
 if(name==L"#clickdelay"){
  unsigned delay=0;if(!(in>>delay)||in>>extra||delay<5||delay>1000)return false;
  if(!wht::SaveSetting(runtimeIni,L"HotKeyHero",L"AutoClickInterval",std::to_wstring(delay)))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);autoClick.due=0;return true;
 }
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
 if(name==L"#statstooltip"){
  if(!detailedStatsHooksReady129)return false;
  std::wstring value=runtimeConfig.statsTooltip?L"0":L"1";
  in>>std::ws;if(!in.eof()&&(!(in>>value)||(value!=L"0"&&value!=L"1")||in>>extra))return false;
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"Detailed Stats Tooltip",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
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
 if(name==L"#unlockfps"){
  if(!unlockFps129.ready||unlockFps129.conflict)return false;
  std::wstring value;in>>value;if(value.empty())value=runtimeConfig.unlockFps?L"0":L"1";
  if((value!=L"0"&&value!=L"1")||in>>extra)return false;
  if(!wht::SaveSetting(runtimeIni,L"Graphics",L"Unlock Limit FPS",value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);return unlockFps129.Apply(executableBase,wht::EffectiveUnlock129(runtimeConfig.mode==wht::RunMode::Local,runtimeConfig.unlockFps,runtimeConfig.vsync));
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
 if(name==L"#showdropall"||name==L"#showautopick"||name==L"#showsellall"){
  const auto key=name==L"#showdropall"?L"Show Drop All":name==L"#showautopick"?L"Show Auto Pick":L"Show Sell All";
  const bool previous=name==L"#showdropall"?runtimeConfig.showDropAll:name==L"#showautopick"?runtimeConfig.showAutoPick:runtimeConfig.showSellAll;
  std::wstring value=previous?L"0":L"1";
  if(in>>action){if((action!=L"0"&&action!=L"1")||in>>extra)return false;value=action;}
  if(!wht::SaveSetting(runtimeIni,L"Event",key,value))return false;
  runtimeConfig=wht::LoadConfig(runtimeIni);toolMenu.nextRefresh=0;return true;
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
 if(value==L"1"&&!wcscmp(control.section,L"MTHKEY")&&(!wcscmp(control.key,L"MTH49Enable")||!wcscmp(control.key,L"G17Enable")))if(!wht::SaveSetting(runtimeIni,L"MTHKEY",!wcscmp(control.key,L"MTH49Enable")?L"G17Enable":L"MTH49Enable",L"0"))return false;
 runtimeConfig=wht::LoadConfig(runtimeIni);if(!wcscmp(control.key,L"Keyboard Scroll Speed")||!wcscmp(control.key,L"Mouse Scroll Speed"))ApplyScroll129();wht::toolEnglish.store(runtimeConfig.english);toolMenu.nextRefresh=0;return true;
}
bool ToolCommand(const std::wstring& command){
 const bool result=ToolCommandImpl129(command);if(!result)return false;
 std::wistringstream input(command);std::wstring name,argument;input>>name>>argument;
 std::string feedback;
 if(name==L"#fc"||name==L"#fogclick"||name==L"#gabase"||name==L"#backga"||name==L"#backsoi"||name==L"#setga"||name==L"#chicken1"||name==L"#chicken2"||name==L"#chicken3"||name==L"#wolf1"||name==L"#wolf2"||name==L"#wolf3"||name==L"#goku"||name==L"#autopickgoku"||name==L"#shield"||name==L"#ice"||name==L"#wild"||name==L"#frozen"||name==L"#phoenix"||name==L"#sellslot"||name==L"#copyitem"||name==L"#item"||name==L"#quickdrop"||name==L"#dropitem"||name==L"#save"||name==L"#shareall"||name==L"#srand"||name==L"#autopick"||name==L"#sellall"||name==L"#drop"||name==L"#dropall"||name==L"#copy"||name==L"#create"||name==L"#attackcd"||name==L"#as"||name==L"#repeat"||name==L"#nocd"||name==L"#yescd"||name==L"#speed"||name==L"#selc")return true;
 if(name==L"#armor")return true;
 if(name==L"#ms")return true;
 if(name==L"#ggl"||name==L"#gll"||name==L"#gg"||name==L"#gl")return true;
 if(name==L"#focus")return true;
 if(name==L"#udmgb"||name==L"#udmg"||name==L"#udel"||name==L"#gold"||name==L"#lumber"||name==L"#food"||name==L"#fullres"||name==L"#lvl"||name==L"#str"||name==L"#agi"||name==L"#int"||name==L"#fullatt"||name==L"#sp"||name==L"#kill"||name==L"#sabi"||name==L"#rabi"||name==L"#tele"||name==L"#invul"||name==L"#vul")return true; // Native result/error is reported by the handler.
 if(name==L"#ping"){wht::ShowNotification129(executableBase,wht::Utf8(wht::network_ping::Status()),localMessage129);return true;}
 if(name==L"#toolui"){
  const auto index=wcstoul(argument.c_str(),nullptr,10);
  if(index<wht::kToolMenuControls.size()){
   const auto& control=wht::kToolMenuControls[index];
   // Native notification font lacks Vietnamese glyphs; use complete English labels here.
   auto title=wht::EnglishControlLabel(control);
   if(title.empty())title=wht::Utf8(control.key);
   auto value=wht::ToolMenuOverlay::ReadValue(runtimeConfig,index);
   if(control.kind==wht::MenuControlKind::Check)feedback=title+": "+(value==L"1"?"ON":"OFF");
   else if(control.kind==wht::MenuControlKind::Key)feedback=wht::Utf8(wht::SettingFeedback(wht::Wide(title),wht::Wide(wht::MenuKeyName(value))));
   else if(control.kind==wht::MenuControlKind::Clear)feedback=title;
   else feedback=wht::Utf8(wht::SettingFeedback(wht::Wide(title),value));
   if(!wcscmp(control.key,L"VSync"))feedback+=" (restart game to apply)";
  }
 }
 else if(name==L"#size"){
  float scale=1;if(wht::ParseUnitScale129(argument,scale)){
   char text[96]{};std::snprintf(text,sizeof(text),"Unit Scale: %.2fx",scale);feedback=text;
  }
 }
 else if(name==L"#hideb"||name==L"#showb")feedback=std::string(name==L"#hideb"?"Hide Ability: ":"Show Ability: ")+wht::Utf8(argument);
 else if(name==L"#wsize"){
  RECT outer{},client{};if(GetWindowRect(window,&outer)&&GetClientRect(window,&client))
   feedback="Window Size: "+std::to_string(outer.right-outer.left)+"x"+std::to_string(outer.bottom-outer.top)+" | Game: "+std::to_string(client.right)+"x"+std::to_string(client.bottom);
 }
 else if(name==L"#uflag"){
  POINT screen{},client{};if(GetCursorPos(&screen)){client=screen;if(ScreenToClient(window,&client)){
   uint32_t flags=0;const bool hasUnit=wht::ReadUnitFlags129(executableBase,flags);
   feedback=wht::CursorFlagsText129(screen,client,hasUnit,flags);
  }}
 }
 else if(name==L"#islot"){
  wht::InventoryInfo129 info;if(wht::ReadInventoryInfo129(executableBase,info))feedback=wht::InventoryInfoText129(info);
 }
 else if(name==L"#mpos"||(name==L"#ppos"&&argument.find_first_not_of(L" \t")==std::wstring::npos)){
  wht::MousePosition129 position;if(wht::ReadMousePosition129(executableBase,position)){
   RECT client{};GetClientRect(window,&client);feedback=wht::MousePositionText129(position,client.right,client.bottom);
  }
 }
 else if(name==L"#ppos")feedback="Camera Position: "+wht::Utf8(argument);
 else if(name==L"#upos"||name==L"#cname"){
  wht::UnitInfo129 info;if(wht::ReadUnitInfo129(executableBase,name==L"#upos",info))
   feedback=name==L"#upos"?wht::UnitPositionText129(info.x,info.y):wht::UnitCodeText129(info.code);
 }
 else if(name==L"#nhailai")feedback=std::string("Repeat Chat: ")+(repeatChat129.all?"ALL":repeatChat129.slot>=0?std::to_string(repeatChat129.slot+1):"OFF");
 else if(name==L"#mute"||name==L"#unmute")feedback=std::string("Mute Player ")+wht::Utf8(argument)+(name==L"#mute"?": ON":": OFF");
 else if(name==L"#clickdelay")feedback="Auto Click Interval: "+std::to_string(runtimeConfig.autoClickInterval)+" ms";
 else if(name==L"#fastmenu")feedback=std::string("Fast Menu: ")+(runtimeConfig.fastMenu?"ON":"OFF");
 else if(name==L"#macro")feedback="Macro: "+wht::Utf8(argument);
 else if(name==L"#allskills"||name==L"#skills")feedback="All Skills";
 else {
  struct ToggleFeedback129 {const wchar_t* command;const char* title;bool value;};
  const ToggleFeedback129 toggles[]={
   {L"#autol","Auto Click Left",autoClick.left},{L"#autoclickl","Auto Click Left",autoClick.left},
   {L"#autor","Auto Click Right",autoClick.right},{L"#autoclickr","Auto Click Right",autoClick.right},
   {L"#unlockfps","Unlock Limit FPS",runtimeConfig.unlockFps},
   {L"#full","Borderless",runtimeConfig.borderless},{L"#lock","Lock Cursor",runtimeConfig.lockCursor},
   {L"#timer","Show Timer",runtimeConfig.showTimer},{L"#showtimer","Show Timer",runtimeConfig.showTimer},
   {L"#res","Show Resource",runtimeConfig.showResource},{L"#resource","Show Resource",runtimeConfig.showResource},
   {L"#showhotkey","Show Hotkey",runtimeConfig.showHotkeyInPanel},{L"#cooldown","Show Cooldown",runtimeConfig.showCooldown},
   {L"#showlevel","Show Level",runtimeConfig.stateTagShowLevel},{L"#blackbar","Hide Black Bar",runtimeConfig.hideBlackBar},
   {L"#mousetooltip","Mouse Tooltip",runtimeConfig.mouseTooltip},{L"#nfe","Remove Fog Effect",runtimeConfig.removeFogEffect},
   {L"#colorplayer","Color Player",runtimeConfig.colorPlayer},{L"#colorp","Color Player",runtimeConfig.colorPlayer},
   {L"#barinvul","Hide Invulnerable Bars",runtimeConfig.hideInvulnerableBars},{L"#vertexcolor","Vertex Color",runtimeConfig.vertexColor},
   {L"#hpvalue","HP Value",runtimeConfig.stateTagHpValue},{L"#manavalue","Mana Value",runtimeConfig.stateTagManaValue},
   {L"#regenhp","HP Regen",runtimeConfig.showRegenHP},{L"#showregen","HP Regen",runtimeConfig.showRegenHP},
   {L"#regen","HP Regen",runtimeConfig.showRegenHP},{L"#regenmp","Mana Regen",runtimeConfig.showRegenMana},
   {L"#statstooltip","Detailed Damage/Armor",runtimeConfig.statsTooltip},{L"#statscommas","Stats Commas",runtimeConfig.statsCommas},{L"#hpmpcommas","HP/Mana Commas",runtimeConfig.lifeManaCommas},
   {L"#hpmpnohide","HP/Mana No Hide",runtimeConfig.lifeManaNoHide},
   {L"#commandsettings","Command Settings",commandSettingsActive},{L"#tools","Tools Menu",toolMenu.open}};
  for(const auto& item:toggles)if(name==item.command){feedback=std::string(item.title)+": "+(item.value?"ON":"OFF");break;}
  if(feedback.empty()){
   std::string commandLabel=wht::Utf8(name);if(!commandLabel.empty()&&commandLabel.front()=='#')commandLabel.erase(0,1);
   feedback=commandLabel+": "+(argument.empty()?"OK":wht::Utf8(argument));
  }
 }
 if(name==L"#unlockfps"&&runtimeConfig.unlockFps&&runtimeConfig.vsync)feedback+=" (inactive while VSync is ON)";
 if(!feedback.empty())wht::ShowNotification129(executableBase,feedback,localMessage129);
 return result;
}
bool SlotKey129(unsigned key){
 if(!clickButton)return false;
 const auto activate=[&](bool inventory,int slot,const wht::Binding& binding){
  smartCast129.Reset();
  if(binding.autoTarget&&!binding.autoTargetMe&&!macroRecorder.playing&&!macroRecorder.recording){
   const bool ok=pinnedTarget129.unit&&wht::TargetCast129(executableBase,inventory,slot,&pinnedTarget129);
   if(!ok)wht::ShowNotification129(executableBase,"Target Cast: select a target and press Set Id Target first",localMessage129);
   return true;
  }
  if(binding.autoTargetMe&&!macroRecorder.playing&&!macroRecorder.recording){
   const bool ok=wht::SelfCast129(executableBase,inventory,slot);
   if(!ok)wht::ShowNotification129(executableBase,"Self Cast: unavailable or game rejected target",localMessage129);
   return true;
  }
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
      if(runtimeConfig.autoSkillsEnabled&&ctrl&&alt&&!shift&&!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&wht::AutoSkillsState::Number(key)>=0){
       POINT p{};RECT r{};GetCursorPos(&p);ScreenToClient(window,&p);GetClientRect(window,&r);auto canvas=wht::Canvas129(r.right,r.bottom);
       bool bound=autoSkills129.Bind(executableBase,key,canvas.Project(p),canvas.width,canvas.height);handled=true;
       wht::ShowNotification129(executableBase,bound?"Auto Skills: sequence slot assigned":"Auto Skills: select a unit and hover a skill button",localMessage129);
      }
      else if(runtimeConfig.autoSkillsEnabled&&!ctrl&&!alt&&!shift&&!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&runtimeConfig.autoSkillsKey&&key==runtimeConfig.autoSkillsKey){
       handled=true;auto selected=wht::SelectedSkill129(executableBase);
       if(!autoSkills129.state.running&&!autoSkills129.state.Current(selected.identity))wht::ShowNotification129(executableBase,"Auto Skills: assign skills with Ctrl+Alt+1..9 first",localMessage129);
       else{autoSkills129.state.running=!autoSkills129.state.running;autoSkills129.state.next=0;wht::ShowNotification129(executableBase,autoSkills129.state.running?"Auto Skills: ON":"Auto Skills: OFF",localMessage129);}
      }
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&(runtimeConfig.mth49Enable||runtimeConfig.g17Enable)&&(key==runtimeConfig.shieldHotkey||key==runtimeConfig.iceHotkey||key==runtimeConfig.wildHotkey||key==runtimeConfig.frozenHotkey||key==runtimeConfig.phoenixHotkey)){
       const unsigned keys[]={runtimeConfig.shieldHotkey,runtimeConfig.iceHotkey,runtimeConfig.wildHotkey,runtimeConfig.frozenHotkey,runtimeConfig.phoenixHotkey};
       const wchar_t* commands[]={L"#shield",L"#ice",L"#wild",L"#frozen",L"#phoenix"};
       for(unsigned i=0;i<5;++i)if(key&&key==keys[i]){ToolCommand(commands[i]);handled=true;break;}
      }
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&(runtimeConfig.mth49Enable||runtimeConfig.g17Enable)&&runtimeConfig.pickGokuHotkey&&key==runtimeConfig.pickGokuHotkey){ToolCommand(L"#goku");handled=true;}
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&(runtimeConfig.mth49Enable||runtimeConfig.g17Enable)&&(key==runtimeConfig.chayGa1Hotkey||key==runtimeConfig.chayGa2Hotkey||key==runtimeConfig.chayGa3Hotkey||key==runtimeConfig.chaySoi1Hotkey||key==runtimeConfig.chaySoi2Hotkey||key==runtimeConfig.chaySoi3Hotkey)){
       const unsigned keys[]={runtimeConfig.chayGa1Hotkey,runtimeConfig.chayGa2Hotkey,runtimeConfig.chayGa3Hotkey,runtimeConfig.chaySoi1Hotkey,runtimeConfig.chaySoi2Hotkey,runtimeConfig.chaySoi3Hotkey};
       const wchar_t* commands[]={L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};
       for(unsigned i=0;i<6;++i)if(key&&key==keys[i]){ToolCommand(commands[i]);handled=true;break;}
      }
      else if(action>=0)handled=ToolCommand(wht::ToolMenuOverlay::MacroCommand(size_t(action)));
      else if(runtimeConfig.commandKey&&key==runtimeConfig.commandKey&&!ctrl&&!shift&&!alt){ToggleCommandSettings();handled=true;}
      else if(key==VK_TAB&&runtimeConfig.showResource&&!toolMenu.open&&!ctrl&&!shift&&!alt){resourceExpanded=!resourceExpanded;resourceNext=0;handled=true;}
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&autoClick.Key(key,runtimeConfig)){
       handled=true;const bool left=key==runtimeConfig.autoClickLeftKey;
       wht::ShowNotification129(executableBase,std::string(left?"Auto Click Left: ":"Auto Click Right: ")+((left?autoClick.left:autoClick.right)?"ON":"OFF"),localMessage129);
      }
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&runtimeConfig.setIdTargetKey&&key==runtimeConfig.setIdTargetKey){
       pinnedTarget129=wht::SelectedSkill129(executableBase);handled=true;
       wht::ShowNotification129(executableBase,pinnedTarget129.unit?"Target: selected unit pinned":"Target: no unit selected",localMessage129);
      }
      else if(!ctrl&&!shift&&!alt&&!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&!consoleState.active&&runtimeConfig.sellFastKey&&key==runtimeConfig.sellFastKey){
       POINT p{};RECT r{};if(GetCursorPos(&p)&&ScreenToClient(window,&p)&&GetClientRect(window,&r)){
        auto canvas=wht::Canvas129(r.right,r.bottom);int slot=wht::QuickDropSlot129(executableBase,canvas.Project(p),canvas.width,canvas.height);
        if(slot>=0){handled=true;ToolCommand(L"#sellslot "+std::to_wstring(slot+1));}
       }
      }
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
   if(msg.message==WM_RBUTTONUP&&quickDropConsumed129){quickDropConsumed129=false;handled=true;}
   if(msg.message==WM_RBUTTONDOWN||msg.message==WM_RBUTTONDBLCLK){
    quickDropConsumed129=false;
    if(allowed&&!chatTyping&&!toolMenu.open&&!consoleState.active&&!macroRecorder.playing&&!macroRecorder.recording&&runtimeConfig.quickDropItem&&GetForegroundWindow()==window&&(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
     RECT r{};GetClientRect(window,&r);auto canvas=wht::Canvas129(r.right,r.bottom);POINT p{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))};
     const int slot=wht::QuickDropSlot129(executableBase,canvas.Project(p),canvas.width,canvas.height);
     if(slot>=0){ToolCommand(L"#dropitem "+std::to_wstring(slot+1));handled=true;quickDropConsumed129=true;}
    }
   }
   if(allowed&&!chatTyping&&(msg.message==WM_LBUTTONDOWN||msg.message==WM_LBUTTONUP||msg.message==WM_LBUTTONDBLCLK)){
    RECT r{};GetClientRect(window,&r);POINT p{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))};
    auto canvas=wht::Canvas129(r.right,r.bottom);
    const auto projected=canvas.Project(p);
    const int mthHit=mth129.Hit(projected,canvas.width,canvas.height);
    if(!toolMenu.open&&!macroRecorder.playing&&!macroRecorder.recording&&mthHit>=0){
     handled=true;if(msg.message==WM_LBUTTONDOWN){mth129.Press(mthHit);const wchar_t* commands[]={L"#shield",L"#ice",L"#wild",L"#frozen",L"#phoenix",L"#goku",L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};ToolCommand(commands[mthHit]);}
    }
    if(!toolMenu.open&&skillsIconShown&&skillsIconRect.Hit(projected,canvas.width,canvas.height)){
     handled=true;
     if(msg.message==WM_LBUTTONDOWN){skillsIconPressed=true;ToolCommand(L"#allskills");}
    }
    if(!toolMenu.open&&dropIconShown&&dropIconRect.Hit(projected,canvas.width,canvas.height)){
     handled=true;
     if(msg.message==WM_LBUTTONDOWN){dropIconPressed=true;ToolCommand(L"#dropall");}
    }
    for(unsigned i=0;i<2;++i)if(!toolMenu.open&&inventoryIconsShown129[i]&&inventoryIconRects129[i].Hit(projected,canvas.width,canvas.height)){
     handled=true;if(msg.message==WM_LBUTTONDOWN){inventoryIconsPressed129[i]=true;ToolCommand(i==0?L"#autopick":L"#sellall");}
    }
    if(msg.message==WM_LBUTTONUP){mth129.Release();skillsIconPressed=false;dropIconPressed=false;inventoryIconsPressed129[0]=inventoryIconsPressed129[1]=false;}
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
void UpdateHelp(){RECT r{};POINT p{};GetClientRect(window,&r);GetCursorPos(&p);ScreenToClient(window,&p);auto canvas=wht::Canvas129(r.right,r.bottom);p=canvas.Project(p);auto help=toolMenu.HelpAt(p,canvas.width,canvas.height,runtimeConfig);if(skillsIconShown&&skillsIconRect.Hit(p,canvas.width,canvas.height))help=runtimeConfig.english?"All Skills\nSpend available hero skill points without manually opening the learn menu.":wht::Utf8(L"C\u1ed9ng to\u00e0n b\u1ed9 k\u1ef9 n\u0103ng\nD\u00f9ng \u0111i\u1ec3m k\u1ef9 n\u0103ng c\u00f2n l\u1ea1i c\u1ee7a t\u01b0\u1edbng, kh\u00f4ng c\u1ea7n t\u1ef1 m\u1edf d\u1ea5u +.");if(dropIconShown&&dropIconRect.Hit(p,canvas.width,canvas.height))help=runtimeConfig.english?"Drop All (#dropall)\nDrop all carried items on the ground.":wht::Utf8(L"Th\u1ea3 to\u00e0n b\u1ed9 \u0111\u1ed3 (#dropall)\nTh\u1ea3 t\u1ea5t c\u1ea3 v\u1eadt ph\u1ea9m trong t\u00fai xu\u1ed1ng \u0111\u1ea5t.");for(unsigned i=0;i<2;++i)if(inventoryIconsShown129[i]&&inventoryIconRects129[i].Hit(p,canvas.width,canvas.height))help=runtimeConfig.english?(i==0?"Auto Pick\nToggle automatic pickup of nearby items.":"Sell All\nSell carried items at a nearby allied shop."):wht::Utf8(i==0?L"T\u1ef1 nh\u1eb7t \u0111\u1ed3\nB\u1eadt ho\u1eb7c t\u1eaft t\u1ef1 nh\u1eb7t v\u1eadt ph\u1ea9m g\u1ea7n unit.":L"B\u00e1n to\u00e0n b\u1ed9 \u0111\u1ed3\nB\u00e1n v\u1eadt ph\u1ea9m t\u1ea1i shop g\u1ea7n unit.");const int mthHover=mth129.Hit(p,canvas.width,canvas.height);if(mthHover>=0)help=std::string(wht::MthName129(mthHover))+"\n"+(runtimeConfig.english?"Buy recipe components and combine at nearby MTH shops. Select your hero first.":wht::Utf8(L"Mua nguy\u00ean li\u1ec7u v\u00e0 \u00e9p t\u1ea1i shop MTH g\u1ea7n hero. Ch\u1ecdn hero tr\u01b0\u1edbc."));if(mthHover==5)help=runtimeConfig.english?"Auto Pick Goku\nPick up nearby Goku items (I016); stop when one is in inventory.":wht::Utf8(L"T\u1ef1 nh\u1eb7t Goku\nNh\u1eb7t item Goku (I016) g\u1ea7n unit; t\u1ef1 t\u1eaft khi \u0111\u00e3 v\u00e0o t\u00fai.");if(mthHover>=6)help=wht::MthName129(mthHover)+std::string("\n")+(runtimeConfig.english?"Move the selected owned Chicken/Wolf unit along this route for the configured number of laps.":wht::Utf8(L"Ch\u1ea1y unit G\u00e0/S\u00f3i \u0111ang ch\u1ecdn theo tuy\u1ebfn v\u00e0 s\u1ed1 l\u01b0\u1ee3t \u0111\u00e3 c\u1ea5u h\u00ecnh. \u0110\u1ed5i unit \u0111ang ch\u1ecdn \u0111\u1ec3 d\u1eebng."));toolMenu.UpdateHelp(p,canvas.width,canvas.height,GetForegroundWindow()==window,help);}
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
void UpdateDropIcon(bool enabled){
 if(!toolMenu.initialized)return;
 if(enabled&&!dropIconFrame){
  dropIconFrame=toolMenu.Frame(executableBase,toolMenu.independentHost,dropIconRect);
  if(dropIconFrame){dropIcon=toolMenu.Texture(dropIconFrame,"BTNDropItem.blp",dropIconRect.width,dropIconRect.height);if(dropIcon)frameApi.setRelPoint(dropIcon,4,dropIconFrame,4,0,0,1);}
 }
 dropIconShown=enabled&&dropIconFrame&&dropIcon;
 if(!dropIconShown){if(dropIconFrame)wht::NativeFrameHide(dropIconFrame);dropIconPressed=false;return;}
 frameApi.setTexture(dropIcon,dropIconPressed?"BTNDropItemDow.blp":"BTNDropItem.blp",0);wht::NativeFrameShow(dropIconFrame);
}
void UpdateInventoryIcons129(bool allowed){
 if(!toolMenu.initialized)return;
 const char* up[]={"BTNAutoPick.blp","BTNSellItem.blp"};const char* down[]={"BTNAutoPickDow.blp","BTNSellItemDow.blp"};
 for(unsigned i=0;i<2;++i){
  const bool enabled=allowed&&(i==0?runtimeConfig.showAutoPick:runtimeConfig.showSellAll);
  if(enabled&&!inventoryIconFrames129[i]){
   const auto rect=inventoryIconRects129[i];inventoryIconFrames129[i]=toolMenu.Frame(executableBase,toolMenu.independentHost,rect);
   if(inventoryIconFrames129[i]){inventoryIconTextures129[i]=toolMenu.Texture(inventoryIconFrames129[i],up[i],rect.width,rect.height);if(inventoryIconTextures129[i])frameApi.setRelPoint(inventoryIconTextures129[i],4,inventoryIconFrames129[i],4,0,0,1);}
  }
  inventoryIconsShown129[i]=enabled&&inventoryIconFrames129[i]&&inventoryIconTextures129[i];
  if(!inventoryIconsShown129[i]){if(inventoryIconFrames129[i])wht::NativeFrameHide(inventoryIconFrames129[i]);inventoryIconsPressed129[i]=false;continue;}
  const bool active=i==0?inventoryActions129.autoPick:inventoryActions129.selling;
  frameApi.setTexture(inventoryIconTextures129[i],active||inventoryIconsPressed129[i]?down[i]:up[i],0);wht::NativeFrameShow(inventoryIconFrames129[i]);
 }
}
void ResetInventoryIcons129(){for(unsigned i=0;i<2;++i){inventoryIconFrames129[i]=inventoryIconTextures129[i]=nullptr;inventoryIconsShown129[i]=inventoryIconsPressed129[i]=false;}}
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
 timeBoss129.Remember(executableBase,unit);
 __try {auto api=LevelFrameApi129();hpDimensions129.Update(executableBase,bar,api);showLevel129.Write(bar,wht::OverheadHeroLevel129(executableBase,bar),api);stateValues129.Write(executableBase,bar,runtimeConfig,stateValues129.active,api);manaBar129.Update(executableBase,bar,runtimeConfig,api);hpColor129.Update(executableBase,bar);if(invulnerableHookReady129)invulnerableBar129.Update(executableBase,bar,originalBarShow129,reinterpret_cast<wht::BarVisibility129>(executableBase+0x488960));}
 __except(EXCEPTION_EXECUTE_HANDLER){}
}
uintptr_t __fastcall DestroyBar129(uintptr_t bar,DWORD,unsigned flags){
 showLevel129.Forget(bar);stateValues129.Forget(bar);manaBar129.Forget(bar);hpColor129.Forget(bar);invulnerableBar129.Forget(bar);
 hpDimensions129.Forget(bar);
 return originalBarDestroy129(bar,flags);
}
bool SafeUpdateMenu(uintptr_t owner,bool allowed){
 __try {toolMenu.Update(executableBase,owner,runtimeConfig,allowed,&frameApi);regen129.Update(executableBase,runtimeConfig,allowed,toolMenu);UpdateMemory(allowed&&runtimeConfig.showMem);UpdateClock(allowed&&runtimeConfig.showTimer);UpdateResources(allowed&&runtimeConfig.showResource);UpdateSkillsIcon(allowed&&runtimeConfig.showInsAllSkills);UpdateDropIcon(allowed&&runtimeConfig.showDropAll);UpdateInventoryIcons129(allowed);mth129.Update(executableBase,owner,runtimeConfig,allowed,toolMenu);toolMenu.PlaybackVisual(macroRecorder.playing);mth129.GokuVisual(toolMenu,inventoryActions129.autoPickGoku);UpdateHelp();UpdateTooltip129(owner,allowed);hotkeyLabels129.Update(executableBase,runtimeConfig,allowed,toolMenu);return true;}
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
 const bool unlock=wht::EffectiveUnlock129(localMode,runtimeConfig.unlockFps,runtimeConfig.vsync);
 if(unlockFps129.ready)unlockFps129.Apply(executableBase,unlock);
 fpsTimer129.Apply(unlock,timeBeginPeriod,timeEndPeriod);
 if(fpsApi129.set){
  const bool confirmed=fpsLimit129.Apply(0,unlock,fpsApi129,true);
  if(confirmed&&fpsLimit129.active&&loggedFpsInterval129!=fpsLimit129.written){
   loggedFpsInterval129=fpsLimit129.written;
   wht::Log(runtimeFolder,"WHT 1.29 FPS scheduler: requested="+std::to_string(runtimeConfig.fpsLimit)+" interval-ms="+std::to_string(fpsLimit129.written)+" patch="+std::to_string(unlockFps129.active)+" timer="+std::to_string(fpsTimer129.active));
  }
  if(!unlock)loggedFpsInterval129=~0u;
 }
 windowServices.Apply(window,localMode&&runtimeConfig.borderless,localMode&&runtimeConfig.lockCursor);
 if(!executableBase||!frameApi.alloc||!window)return;
 uintptr_t camera=0;uint16_t local=0;
 if(!CameraContext(camera,local)){
  keyboardScroll129.Reset();mouseScroll129.Reset();ApplyFogEffect129(false);fogEffect129.Reset();ApplyLevels129(false);ApplyStateValues129(false);
  ApplyHpDimensions129(false);
  if(macroRecorder.recording)macroRecorder.FinishRecording();macroRecorder.Stop();
  // Map teardown releases native frames. Forget pointers without dereferencing them.
  mutedChat129.Reset();repeatChat129.Reset();repeatMessage129.Reset();queuedChatDue129=0;noCooldown129.Reset();ApplySpeedMultiplier129(1.f);timeBoss129.Reset();regen129.Reset();blackBar129.Reset();mouseTooltip129.Reset();hotkeyLabels129.Reset();wht::ClearUiLayout129();if(toolMenu.parent)toolMenu.Reset();memoryFrame=memoryFont=clockFrame=clockFont=nullptr;resourceBoard={};memoryNext=resourceNext=0;resourceExpanded=true;chatTyping=false;messageTimers={};allSkills.Reset();smartCast129.Reset();autoClick.Reset();autoSkills129.Reset();pinnedTarget129={};stats129.Reset();lifeMana129.Reset();fpsState={};timeOfDay={};matchStart={};consoleState={};skillsIconFrame=skillsIcon=nullptr;skillsIconShown=skillsIconPressed=false;dropIconFrame=dropIcon=nullptr;dropIconShown=dropIconPressed=false;inventoryActions129.Reset();mth129.Reset();ResetInventoryIcons129();return;
 }
 const auto owner=*reinterpret_cast<uintptr_t*>(executableBase+0xd3d7f8);
 float startSeconds=0;
 const bool startClockReady=wht::ReadClock129(executableBase,startSeconds);
 if(matchStart.Observe(owner,startClockReady,startSeconds,localMode&&runtimeConfig.soundStartGame))
  if(!startSoundPlayer129(MAKEINTRESOURCEW(101),runtimeModule,SND_RESOURCE|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP))wht::Log(runtimeFolder,"WHT 1.29: start sound unavailable; skipped.");
 if(resourceFailed)return;
 if(!resourceArchive&&!wht::MountResources129(executableBase,mixPath,resourceArchive)){resourceFailed=true;toolMenu.failed=true;wht::Log(runtimeFolder,"WHT 1.29: embedded MPQ mount failed.");return;}
 if(toolMenu.parent&&toolMenu.parent!=owner){timeBoss129.Reset();regen129.Reset();blackBar129.Reset();mouseTooltip129.Reset();hotkeyLabels129.Reset();wht::ClearUiLayout129();memoryFrame=memoryFont=clockFrame=clockFont=nullptr;resourceBoard={};memoryNext=resourceNext=0;resourceExpanded=true;messageTimers={};allSkills.Reset();smartCast129.Reset();autoClick.Reset();autoSkills129.Reset();pinnedTarget129={};consoleState={};skillsIconFrame=skillsIcon=nullptr;skillsIconShown=skillsIconPressed=false;dropIconFrame=dropIcon=nullptr;dropIconShown=dropIconPressed=false;inventoryActions129.Reset();mth129.Reset();ResetInventoryIcons129();}
 RECT r{};GetClientRect(window,&r);wht::SetViewport129(r.right,r.bottom);auto canvas=wht::Canvas129(r.right,r.bottom);wht::ToolMenuOverlay::SetViewport(canvas.width,canvas.height);
 const bool allowed=runtimeConfig.mode==wht::RunMode::Local;
 ApplyFogEffect129(allowed);ApplyLevels129(allowed&&runtimeConfig.stateTagShowLevel);ApplyStateValues129(allowed);
 ApplyHpDimensions129(levelHooksReady129&&allowed);
 bool nativeTyping=false;if(wht::ChatOpen129(executableBase,nativeTyping))chatTyping=nativeTyping;
 const auto smartSelection=wht::SelectedSkill129(executableBase);
 if(!smartCast129.Valid(allowed&&!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&GetForegroundWindow()==window&&runtimeConfig.modeCast==1,wht::TargetMode129(executableBase),smartSelection.unit,smartSelection.identity))smartCast129.Reset();
 if(inventoryActions129.autoPickGoku&&!(allowed&&(runtimeConfig.mth49Enable||runtimeConfig.g17Enable)))inventoryActions129.ToggleGoku(false);
 const auto inventoryResult=inventoryActions129.TickNative(executableBase,allowed&&!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&!mth129.run.Active()&&mth129.pending<0&&mth129.route<0&&!mth129.returnPending&&GetForegroundWindow()==window,GetTickCount64());
 if(inventoryResult)wht::ShowNotification129(executableBase,inventoryResult,localMessage129);
 const bool mthEnabled=allowed&&(runtimeConfig.mth49Enable||runtimeConfig.g17Enable);
 const auto returnResult=mth129.TickReturn(executableBase,allowed,!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&GetForegroundWindow()==window);
 if(returnResult)wht::ShowNotification129(executableBase,returnResult,localMessage129);
 const auto routeResult=mth129.TickRoute(executableBase,mthEnabled,!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&GetForegroundWindow()==window,GetTickCount());
 if(routeResult&&runtimeConfig.mthNoti)wht::ShowNotification129(executableBase,routeResult,localMessage129);
 const auto mthResult=mth129.Tick(executableBase,clickButton,mthEnabled,!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&GetForegroundWindow()==window,GetTickCount());
 if(mthResult&&runtimeConfig.mthNoti)wht::ShowNotification129(executableBase,std::string(wht::MthName129(mth129.kind))+": "+mthResult,localMessage129);
 allSkills.Tick(executableBase,allowed&&!toolMenu.open&&!chatTyping&&!macroRecorder.playing&&!macroRecorder.recording&&GetForegroundWindow()==window,clickButton,GetTickCount64());
 fpsState.Apply(executableBase,allowed&&runtimeConfig.showFps,fpsMode);
 stats129.Apply(executableBase,allowed&&runtimeConfig.statsCommas,originalLifeText129);
 lifeMana129.Apply(executableBase,LifeFlags129(),originalLifeText129);
 consoleState.Apply(executableBase,allowed&&runtimeConfig.hideConsoleUI,showConsole,hideConsole);
 timeOfDay.Apply(executableBase,allowed&&runtimeConfig.hideTimeOfDay,showFdf,hideFdf);
 fogClick129.Tick(executableBase,allowed&&runtimeConfig.fogClick,GetTickCount64(),[](const std::string& text){wht::ShowNotification129(executableBase,text,localMessage129);});
 messageTimers.Tick(executableBase,runtimeConfig,allowed&&!chatTyping,GetTickCount64(),clearFrame);
 const bool clickAllowed=allowed&&!toolMenu.open&&!chatTyping&&!macroRecorder.recording&&!macroRecorder.playing&&GetForegroundWindow()==window&&!IsIconic(window);
 POINT cursor{};const bool inside=GetCursorPos(&cursor)&&ScreenToClient(window,&cursor)&&cursor.x>=0&&cursor.y>=0&&cursor.x<r.right&&cursor.y<r.bottom;
 noCooldown129.Tick(clickAllowed,GetTickCount64(),[&](){return wht::ResetCooldown129(executableBase,wht::SelectedSkill129(executableBase),wht::ResolveCooldownReset129(executableBase));});
 const auto chatNow129=GetTickCount64();
 const auto sendQueued129=[&](const std::string& text){
  if(chatNow129<queuedChatDue129||!wht::SendQuickChat129(executableBase,text,quickChatApi))return false;
  queuedChatDue129=chatNow129+250;return true;
 };
 repeatMessage129.Tick(clickAllowed,chatNow129,sendQueued129);
 repeatChat129.Tick(clickAllowed,chatNow129,sendQueued129);
 autoSkills129.Tick(executableBase,runtimeConfig,clickAllowed,GetTickCount64());
 autoClick.Tick(runtimeConfig,clickAllowed&&inside,GetTickCount64(),[&](UINT message,WPARAM key){PostMessageW(window,message,key,MAKELPARAM(cursor.x,cursor.y));});
 if(!toolMenu.failed&&!SafeUpdateMenu(owner,allowed)){toolMenu.failed=true;wht::Log(runtimeFolder,"WHT 1.29: native menu exception at "+std::string(toolMenu.initStage));}
 timeBoss129.Update(executableBase,owner,allowed&&runtimeConfig.timeBoss&&(runtimeConfig.mth49Enable||runtimeConfig.g17Enable),toolMenu);
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
 toolMenu.helpMeasure=reinterpret_cast<decltype(toolMenu.helpMeasure)>(executableBase+0x486fa0);
 showLevel129.textWidth=reinterpret_cast<decltype(showLevel129.textWidth)>(executableBase+0x486fa0);
 showLevel129.backgroundColor=reinterpret_cast<decltype(showLevel129.backgroundColor)>(executableBase+0x487ba0);
 wchar_t self[32768]{};GetModuleFileNameW(static_cast<HMODULE>(module),self,32768);mixPath=self;
 auto address=reinterpret_cast<unsigned char*>(executableBase)+0x47e380;
 const unsigned char signature[]={0x55,0x8b,0xec,0x53,0x56,0x57,0x8b,0x7d,0x08,0x8b,0xf1};
 if(memcmp(address,signature,sizeof(signature))||MH_Initialize()!=MH_OK||MH_CreateHook(address,SetText,reinterpret_cast<void**>(&original))!=MH_OK||MH_EnableHook(address)!=MH_OK){wht::Log(folder,"WHT 1.29: version hook installation failed.");return 2;}
 auto clipAddress=reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"user32.dll"),"ClipCursor"));
 if(clipAddress&&MH_CreateHook(clipAddress,ClipCursorHook129,reinterpret_cast<void**>(&originalClipCursor129))==MH_OK){
  if(MH_EnableHook(clipAddress)!=MH_OK){MH_RemoveHook(clipAddress);originalClipCursor129=nullptr;}
 }

 // Native D3D9 parameter builder, called for creation and reset.
 const unsigned char presentSig129[]={0x55,0x8b,0xec,0x83,0xec,0x14,0xa1};
 const auto presentBuilder129=reinterpret_cast<void*>(executableBase+0x3dba60);
 if(!memcmp(presentBuilder129,presentSig129,sizeof(presentSig129))&&
    MH_CreateHook(presentBuilder129,PresentParametersHook129,reinterpret_cast<void**>(&originalPresentParameters129))==MH_OK){
  if(MH_EnableHook(presentBuilder129)!=MH_OK)originalPresentParameters129=nullptr;
 }
 // 1893F0 takes six stack arguments, returns duration in x87 and uses
 // plain RET: cdecl, unlike the legacy fastcall entry. Root +1A0/+1A4
 // select its model frames; native callbacks are always forwarded.
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
 const auto dispatch=reinterpret_cast<void*>(executableBase+0x2cc540);
 const unsigned char dispatchSignature[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
 if(!memcmp(dispatch,dispatchSignature,sizeof(dispatchSignature))&&!memcmp((void*)(executableBase+0x2cc590),"\x8b\x5d\x08\x0f\xb6\x43\x15",7)&&MH_CreateHook(dispatch,SelectionDispatchHook129,reinterpret_cast<void**>(&originalSelectionDispatch129))==MH_OK){if(MH_EnableHook(dispatch)!=MH_OK)originalSelectionDispatch129=nullptr;}
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
 InstallSpeedClocks129();
 wht::Log(folder,speedClocksReady129?"WHT 1.29 speed clock hooks ready.":"WHT 1.29 speed clock hooks unavailable.");
 const unsigned char descriptionSignature[]={0x55,0x8b,0xec,0x83,0xec,0x58};
 auto attackDescription=reinterpret_cast<void*>(executableBase+0x2289b0);
 auto movementDescription=reinterpret_cast<void*>(executableBase+0x229740);
 if(!memcmp(attackDescription,descriptionSignature,sizeof(descriptionSignature))&&
    !memcmp(movementDescription,descriptionSignature,sizeof(descriptionSignature))&&
    MH_CreateHook(attackDescription,AttackDescription129,reinterpret_cast<void**>(&originalAttackDescription129))==MH_OK&&
    MH_CreateHook(movementDescription,MovementDescription129,reinterpret_cast<void**>(&originalMovementDescription129))==MH_OK&&
    MH_EnableHook(attackDescription)==MH_OK&&MH_EnableHook(movementDescription)==MH_OK)detailedStatsHooksReady129=true;
 if(!detailedStatsHooksReady129){MH_DisableHook(attackDescription);MH_DisableHook(movementDescription);MH_RemoveHook(attackDescription);MH_RemoveHook(movementDescription);originalAttackDescription129=nullptr;originalMovementDescription129=nullptr;}
 wht::Log(folder,detailedStatsHooksReady129?"WHT 1.29 detailed damage/armor tooltips ready.":"WHT 1.29 detailed damage/armor tooltip hooks unavailable.");
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
 wht::Log(folder,wht::InstallPing129()?"WHT 1.29: Warcraft echo ping observation installed.":"WHT 1.29: ping observation unavailable.");
 const unsigned char clearSignature[]={0x55,0x8b,0xec,0x51,0x53,0x56,0x8b,0xf1};
 auto clear=reinterpret_cast<unsigned char*>(executableBase)+0x48b7b0;
 if(!memcmp(clear,clearSignature,sizeof(clearSignature)))clearFrame=reinterpret_cast<wht::ClearFrame129>(clear);
 // Exact cdecl scheduler APIs; independent of the FPS display function.
 unlockFps129.Probe(executableBase);
 const unsigned char fpsSetterSig129[]={0x55,0x8b,0xec,0x8b,0x45,0x0c,0x85,0xc0};
 const unsigned char fpsGetterSig129[]={0x55,0x8b,0xec,0x8b,0x45,0x08,0x85,0xc0};
 const unsigned char fpsContextSig129[]={0x55,0x8b,0xec,0xff,0x35};
 if(!memcmp(reinterpret_cast<void*>(executableBase+0x440230),fpsSetterSig129,sizeof(fpsSetterSig129))&&
    !memcmp(reinterpret_cast<void*>(executableBase+0x43faa0),fpsGetterSig129,sizeof(fpsGetterSig129))&&
    !memcmp(reinterpret_cast<void*>(executableBase+0x5df90),fpsContextSig129,sizeof(fpsContextSig129)))
  fpsApi129={reinterpret_cast<decltype(fpsApi129.context)>(executableBase+0x5df90),reinterpret_cast<decltype(fpsApi129.get)>(executableBase+0x43faa0),reinterpret_cast<decltype(fpsApi129.set)>(executableBase+0x440230)};
 const unsigned char messageSig129[]={0x55,0x8b,0xec,0x83,0xec,0x10,0x56,0x8b,0xf1};
 if(!memcmp(reinterpret_cast<void*>(executableBase+0x1f4860),messageSig129,sizeof(messageSig129)))
  localMessage129=reinterpret_cast<wht::LocalMessage129>(executableBase+0x1f4860);
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
