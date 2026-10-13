#include "profile64.hpp"
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
wht::MessageTimers131 messageTimers64;
wht::ResourceBoard131 resources64;
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
bool Identity64(uintptr_t object,uintptr_t table){__try{return object&&*reinterpret_cast<uintptr_t*>(object)==base64+table;}__except(1){return false;}}
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
 if(section==L"DelayClear"||section==L"Language"||section==L"MacroRecorder"||section==L"StateTag")return true;
 if(section==L"ManaBar")return key==L"Enable"||key==L"EnableForHero"||key==L"EnableForNonHero";
 if(section==L"Event")return key==L"Camera"||key==L"FogZ"||key==L"Show Mem"||key==L"ShowCooldown"||key==L"ShowRegenHP"||key==L"ShowRegenMana"||key==L"Show Hotkey In Panel"||key==L"Show Resource";
 if(section==L"HotKeyOther")return key==L"hko_menu"||key==L"hko_command";return false;}
void Reload64(){config64=wht::LoadConfig(ini64);wht::toolEnglish=config64.english;menu64.nextRefresh=0;}
bool Command64(const std::wstring& text){std::wistringstream in(text);std::wstring cmd;if(!(in>>cmd))return false;wht::FoldCommandName(cmd);
 if(cmd==L"#menu"||cmd==L"#tool"){menu64.Toggle();return true;}
 if(cmd==L"#toolui"){size_t index;std::wstring op,value;if(!(in>>index>>op)||index>=wht::kToolMenuControls.size())return false;const auto& c=wht::kToolMenuControls[index];if(!Control64(c))return false;
  if(op==L"toggle")value=wht::ToolMenuOverlay::ReadValue(config64,index)==L"1"?L"0":L"1";else if(op==L"set"){std::getline(in,value);if(!value.empty()&&value[0]==L' ')value.erase(0,1);}else return false;
  if(!wht::ValidMenuValue(c,value))return false;
  if(std::wstring(c.section)==L"Language"){const bool english=std::wstring(c.key)==L"English"?value==L"1":value!=L"1";wht::SaveSetting(ini64,L"Language",L"English",english?L"1":L"0");wht::SaveSetting(ini64,L"Language",L"Vietnamese",english?L"0":L"1");}
  else if(!wht::SaveSetting(ini64,c.section,c.key,value))return false;Reload64();return true;}
 if(!authorized64||config64.mode!=wht::RunMode::Local||!ui64)return false;
 if(cmd==L"#cam"||cmd==L"#zoom"||cmd==L"#fogz"){float value=0;std::wstring extra;if(!(in>>value)||(in>>extra)||!std::isfinite(value))return false;bool fog=cmd==L"#fogz";if(fog?(value<1||value>50000):(value<500||value>10000))return false;wht::SaveSetting(ini64,L"Event",fog?L"FogZ":L"Camera",std::to_wstring(value));Reload64();Camera64();return true;}
 if(cmd==L"#cooldown"||cmd==L"#regenhp"||cmd==L"#regenmp"||cmd==L"#showmem"||cmd==L"#commandsettings"){bool current=cmd==L"#cooldown"?config64.showCooldown:cmd==L"#regenhp"?config64.showRegenHP:cmd==L"#regenmp"?config64.showRegenMana:cmd==L"#showmem"?config64.showMem:settings64;std::wstring arg,extra;in>>arg;if(in>>extra)return false;wht::FoldCommandName(arg);bool next=!current;if(arg==L"on"||arg==L"1")next=true;else if(arg==L"off"||arg==L"0")next=false;else if(!arg.empty()&&arg!=L"toggle")return false;
  if(cmd==L"#commandsettings"){settings64=next;Camera64();return true;}auto key=cmd==L"#cooldown"?L"ShowCooldown":cmd==L"#regenhp"?L"ShowRegenHP":cmd==L"#regenmp"?L"ShowRegenMana":L"Show Mem";wht::SaveSetting(ini64,L"Event",key,next?L"1":L"0");Reload64();return true;}
 if(cmd==L"#macro"){std::wstring op;if(!(in>>op))return false;wht::FoldCommandName(op);cmd=op==L"play"?L"#macroplay":op==L"stop"?L"#macrostop":op==L"record"?L"#macrorecord":op==L"edit"?L"#macroscript":L"";}
 if(config64.macroEnabled){if(cmd==L"#macroplay"){macro64.Play();return true;}if(cmd==L"#macrostop"){macro64.Stop();return true;}if(cmd==L"#macrorecord"){macro64.Record();return true;}if(cmd==L"#macroscript"){macro64.Edit();return true;}}
 return false;
}
void Destroy64(){if(menu64.independentHost&&Identity64(reinterpret_cast<uintptr_t>(menu64.independentHost),profile64->simpleVtable)){
 auto table=*reinterpret_cast<uintptr_t*>(menu64.independentHost);reinterpret_cast<void(*)(void*,unsigned)>(*reinterpret_cast<uintptr_t*>(table+16))(menu64.independentHost,1);}
 menu64.Reset();placements64.clear();status64=nullptr;menuLogged64=false;gameplay64.Reset();hotkeys64.Reset();messageTimers64.Reset();resources64.Reset();camera64=0;cameraMask64=0;cameraLogged64=false;typed64=false;chat64.clear();macro64.Stop();}
void Update64(){const auto ui=CurrentUi64();if(ui!=ui64){Destroy64();ui64=ui;}if(!ui)return;Camera64();if(!Assets64())return;
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
  auto observed=gameplay64.Update(base64,ui,*profile64,config64,Allowed64(),menu64,raw);unsigned fresh=observed&~gameplay64.reported;
  if(resources64.Update(base64,profile64->build,Allowed64()&&config64.showResource,GetTickCount64(),menu64)&&!resources64.reported){resources64.reported=true;wht::Log(folder64,"WHT x64: native resource table displayed.");}
  if(hotkeys64.Update(Allowed64()&&config64.showHotkeyInPanel,menu64,[&](unsigned slot){return wht::Button64(ui,base64,*profile64,slot);})&&!hotkeys64.reported){hotkeys64.reported=true;wht::Log(folder64,"WHT x64: native panel hotkey labels displayed.");}
  for(auto entry:{std::pair<unsigned,const char*>{1,"selected-unit HP/s MP/s"},{2,"cooldown seconds"},{4,"native mana bar"},{8,"HP/Mana values"},{16,"hero level"}})if(fresh&entry.first)wht::Log(folder64,std::string("WHT x64: live ")+entry.second+" displayed.");gameplay64.reported|=observed;
 }
 if(status64){std::string text="|cffffcc00WHT " WHT_VERSION_STRING " | 1.31.1."+std::to_string(profile64->build)+" x64|r";if(!authorized64)text+=" |cffff4444Login LoadFileMapWar3|r";else if(config64.showMem){float mb=0,percent=0;if(wht::GetCurrentProcessMemoryUsage(mb,percent))text+=" | "+std::to_string(int(mb))+" MB";}api64.setText(status64,text.c_str());}
 menu64.PlaybackVisual(macro64.playing);macro64.Tick(Allowed64()&&!typed64&&!menu64.open);
}
void Safe64(){__try{Update64();}__except(1){menu64.failed=true;}}
LRESULT CALLBACK Window64(HWND hwnd,UINT message,WPARAM key,LPARAM value){if(message==update64){Safe64();return 0;}
 if(message==WM_KEYDOWN){bool repeat=(value&(1LL<<30))!=0;if(key==config64.menuKey&&!repeat){menu64.Toggle();return 0;}if(menu64.Key(unsigned(key),unsigned((value>>16)&255),Command64))return 0;
  if(!repeat&&key==VK_RETURN){if(typed64){Command64(chat64);typed64=false;chat64.clear();}else{typed64=true;chat64.clear();}}
  if(key==VK_ESCAPE){typed64=false;chat64.clear();}
  if(Allowed64()&&!typed64&&!repeat&&!menu64.open&&key==VK_TAB&&config64.showResource&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){resources64.Toggle();return 0;}
  if(Allowed64()&&!typed64&&!repeat){if(key==config64.commandKey){settings64=!settings64;Camera64();return 0;}
   bool chord=(GetKeyState(VK_CONTROL)&0x8000)&&(GetKeyState(VK_SHIFT)&0x8000);if(config64.macroEnabled){if(key==config64.macroKeys[0]||(chord&&key==VK_F10)){macro64.Play();return 0;}if(key==config64.macroKeys[1]||(chord&&key==VK_F12)){macro64.Stop();return 0;}if(key==config64.macroKeys[2]||(chord&&key==VK_F9)){macro64.Record();return 0;}if(key==config64.macroKeys[3]||(chord&&key==VK_F11)){macro64.Edit();return 0;}}}}
 if(message==WM_CHAR){wchar_t ch=wchar_t(key);if(ch==L'#'){typed64=true;chat64=L"#";}else if(typed64){if(ch==8){if(!chat64.empty())chat64.pop_back();}else if(ch>=32&&chat64.size()<1024)chat64+=ch;}}
 if(message==WM_LBUTTONDOWN||message==WM_LBUTTONUP||message==WM_LBUTTONDBLCLK){POINT p{short(LOWORD(value)),short(HIWORD(value))};RECT r{};GetClientRect(hwnd,&r);if(menu64.Mouse(message,p,r.right,r.bottom,Command64,&config64))return 0;}
 MSG msg{hwnd,message,key,value};macro64.Capture(msg,!menu64.open&&!typed64);return CallWindowProcW(previous64,hwnd,message,key,value);}
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
