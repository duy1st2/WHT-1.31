#include "presentation131.hpp"
#include "auto_heal131.hpp"
#include "casting_mode.hpp"
#include "option_menu131.hpp"
#include "window_services131.hpp"
#include "scroll131.hpp"
#include "inventory_info131.hpp"
#include "courier_base131.hpp"
#include "mth_craft_buttons131.hpp"
#include "auto_skills_native131.hpp"
#include "hp_dimensions131.hpp"
#include "mth_text131.hpp"
#include "ui_perf131.hpp"
#include "courier_delivery131.hpp"
#include "mth_queue131.hpp"
#include "courier_route131.hpp"
#include "mth_route_buttons131.hpp"
#include "mth_route131.hpp"
#include "mth_notice131.hpp"
#include "drop_commands131.hpp"
#include "auto_click_commands131.hpp"
#include "view_commands131.hpp"
#include "detailed_tooltip131.hpp"
#include "manual_clear131.hpp"
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
#include "tool_help131.hpp"
#include "macro_recorder.hpp"
#include "memory_overlay.hpp"
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include "command_feedback131.hpp"
#include "menu_fog_vertex131.hpp"
namespace {
wht::OptionMenu131 optionMenu64;

wht::WindowServices131 windowServices64;
wht::PresentationService131 presentation64;
wht::MenuFogVertex131 menuFogVertex64;

wht::ScrollPair131 scroll64;

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
wht::ManualClear131 manualClear64;
wht::ResourceBoard131 resources64;
wht::GraphicsDiagnostic131 graphicsDiagnostic64;
wht::MatchStart131 matchStart64;
wht::TimeBoss131 timeBoss64;
wht::DetailedTooltip131 detailTooltip64;
wht::ClockOverlay131 clock64;wht::FpsState131 fps64;
wht::QuickDropInput131 quickDrop64;wht::QuickDropFlow131 quickDropFlow64;
wht::Reveal131 reveal64;
wht::SellIcon131 sellIcon64;wht::SellAll131 sellAll64;
wht::AutoHeal131 autoHeal64,autoMana64;wht::AutoSkills131 autoSkills64;wht::AutoVision131 autoVision64;
wht::CourierDelivery131 delivery64;
wht::MthText131 mthText64;wht::UiPerf131 perf64;
wht::MthCraftRun131 craft64;wht::SelectionRoster131 craftRoster64;wht::CraftButtons131 craftButtons64;
wht::LocalInfo131 localInfo64;wht::CourierBase131 courierBase64;wht::SelfCastSource131 courier64;bool routePinned64=false;wht::RouteButtons131 routeButtons64;wht::MthRoute131 mthRoute64;wht::MthNotice131 mthNotice64;wht::GokuIcon131 gokuIcon64;bool gokuEnabled64=false;wht::AutoPickIcon131 pickIcon64;wht::AutoPick131 autoPick64;bool autoPickEnabled64=false;
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

wht::MthCraftApi131 CraftApi64(){const auto* p=wht::PickProfileFor131(profile64->build);return {base64,ui64,p?wht::Component64(ui64,base64,p->worldTable):0,profile64->build,craftRoster64,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float v=0,m=0,r=0;return wht::State64(unit,base64,*profile64,false,v,m,r)&&v>.405f;},SkillData64,ClickSkill64};}
wht::AutoSkillsApi131 AutoApi64(){return {CraftApi64(),autoVision64,profile64->uiVtable,[&](unsigned slot){return wht::Button64(ui64,base64,*profile64,slot);},&folder64};}
void CraftReport64(wht::MthResult r){if(const auto* text=wht::CraftNotice131(r)){wht::Log(folder64,text);mthNotice64.EventText(wht::MthNoticeKind131::OrderRefused,text,config64.mthNoti,GetTickCount64());}}
bool LearnSlot64(unsigned slot){return wht::LearnSlot131(slot,[&](wht::SelfCastSource131& out){return wht::ReadSelfCastSource131(base64,profile64->build,ui64,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},out);},SkillPoints64,SkillData64,ClickSkill64);}

uintptr_t camera64=0;std::array<float,2> cameraRequested64{},cameraWritten64{};unsigned cameraMask64=0;bool cameraLogged64=false;
bool Allowed64(){return authorized64&&settings64&&config64.mode==wht::RunMode::Local&&ui64;}
bool ReadCamera64(unsigned field,float& value){if(!wht::Object64(camera64,base64,profile64->cameraVtable))return false;__try{reinterpret_cast<void*(*)(uintptr_t,float*)>(base64+profile64->floatCurrent)(camera64+(field?0x1e8:0x1a8),&value);return std::isfinite(value)&&value>0&&value<1000000;}__except(1){return false;}}
void SetCamera64(unsigned field,float value){reinterpret_cast<void(*)(uintptr_t,unsigned,float,float,int)>(base64+profile64->cameraSet)(camera64,field,value,0,1);}
void Camera64(){if(!wht::Object64(camera64,base64,profile64->cameraVtable)){camera64=wht::Component64(ui64,base64,profile64->cameraVtable);cameraMask64=0;}if(!camera64)return;
 scroll64.Update(base64,profile64->build,camera64,Allowed64()&&settings64,config64);
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
 if(wht::MenuFogVertexControl131(section,key)||wht::PresentationControl131(section,key))return true;
 if(section==L"MTHKEY"&&(key==L"Shield"||key==L"Ice"||key==L"Wild"||key==L"Frozen"||key==L"Phoenix"||key==L"ShieldDraw"||key==L"IceDraw"||key==L"WildDraw"||key==L"FrozenDraw"||key==L"PhoenixDraw"||key==L"Noti"||key==L"AutoGaDraw1"||key==L"AutoGaDraw2"||key==L"AutoGaDraw3"||key==L"AutoSoiDraw1"||key==L"AutoSoiDraw2"||key==L"AutoSoiDraw3"||key==L"ChayGa1"||key==L"ChayGa2"||key==L"ChayGa3"||key==L"ChaySoi1"||key==L"ChaySoi2"||key==L"ChaySoi3"||key==L"ChayGa1_count"||key==L"ChayGa2_count"||key==L"ChayGa3_count"||key==L"ChaySoi1_count"||key==L"ChaySoi2_count"||key==L"ChaySoi3_count"||key==L"PickGoku"||key==L"PickGokuDraw"||key==L"TimeBoss"||key==L"MTH49Enable"||key==L"G17Enable"))return true;
 if(section==L"Graphics"&&(key==L"timeOfDayUI"||key==L"ConsoleUI"||key==L"BlackBarUI"||key==L"MouseTooltipUI"||key==L"BarInvul"||key==L"Color Player"||key==L"Text Life/Mana Commas"||key==L"Text Life/Mana No Hide"||key==L"Text Stats Commas"||key==L"Detailed Stats Tooltip"))return true;
 if(section==L"AutoHeal"||section==L"AutoMana")return key==L"Enabled"||key==L"Percent";
 if(section==L"AutoSkills")return key==L"AutoSkills_Enable"||key==L"AutoSkills_Delay"||key==L"AutoSkills_Range"||key==L"AutoSkills_Active";
 if(section==L"Window")return key==L"Borderless"||key==L"Lock Cursor";
 if(section==L"Quick Chat")return true;
 if(section==L"DelayClear"||section==L"Language"||section==L"MacroRecorder"||section==L"StateTag")return true;
 if(section==L"Event")return key==L"Hide Option Menu"||key==L"Keyboard Scroll Speed"||key==L"Mouse Scroll Speed"||key==L"Camera"||key==L"FogZ"||key==L"Show Mem"||key==L"ShowCooldown"||key==L"ShowRegenHP"||key==L"ShowRegenMana"||key==L"Show Hotkey In Panel"||key==L"Show Resource"||key==L"Show Timer"||key==L"Show FPS"||key==L"Sound Start Game"||key==L"Quick Drop Item"||key==L"Reveal Invisibles"||key==L"Show Sell All"||key==L"Show Auto Pick"||key==L"Show Drop All"||key==L"Show Ins All Skills";
 if(section==L"HotKeyHero"&&(key==L"SetIdTarget"||key==L"SellFast"||key==L"AutoClickLeft"||key==L"AutoClickRight"||key.rfind(L"HeroF",0)==0))return true;
 if(wht::RemapControl131(section,key))return true;
 if(section==L"HotKeyOther")return key==L"hko_menu"||key==L"hko_command"||key==L"hko_MinimizeMultiboard";return false;}
void Reload64(){config64=wht::LoadConfig(ini64);wht::toolEnglish=config64.english;menu64.nextRefresh=0;}
bool ReadSelectedRoute64(wht::SelfCastSource131& out){return wht::ReadSelfCastSource131(base64,profile64->build,ui64,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float value=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,value,maximum,rate)&&value>.405f;},out)&&wht::RouteMovable131(base64,profile64->build,out.unit);}
bool ReadCourier64(uintptr_t unit,wht::SelfCastSource131& out){return wht::ReadSelfCastSource131(base64,profile64->build,ui64,[&](){return unit;},[&](uintptr_t unit){float v=0,m=0,r=0;return wht::State64(unit,base64,*profile64,false,v,m,r)&&v>.405f;},out)&&wht::RouteMovable131(base64,profile64->build,unit);}
bool ResolveRoute64(wht::SelfCastSource131& out){
 if(ReadSelectedRoute64(out)){courier64=out;routePinned64=true;return true;}
 if(courier64.unit&&ReadCourier64(courier64.unit,out)&&wht::SameSelfCastSource131(out,courier64)){routePinned64=true;return true;}
 courier64={};
 const auto* sp=wht::SellProfileFor131(profile64->build);const auto* pp=wht::PickProfileFor131(profile64->build);const auto* inv=wht::InventoryProfile(profile64->build);if(!sp||!pp||!inv)return false;
 auto api=wht::SellNative131(base64,*sp);auto inventory=wht::InventoryNative131(base64,*inv);if(!api||!inventory)return false;
 __try{const auto worldFrame=wht::Component64(ui64,base64,pp->worldTable);if(!worldFrame)return false;const auto list=api.list(worldFrame,true);if(list!=worldFrame+sp->listOffset)return false;const auto count=*reinterpret_cast<unsigned*>(list);const auto data=*reinterpret_cast<uintptr_t*>(list+sizeof(uintptr_t));if(count>8192||(count&&!data))return false;
  wht::SelfCastSource131 fallback;bool fallbackItems=false;
  for(unsigned pass=0;pass<2&&!fallback.unit;++pass)for(unsigned i=0;i<count;++i){const auto unit=*reinterpret_cast<uintptr_t*>(data+i*sizeof(uintptr_t));wht::SelfCastSource131 candidate;if(!ReadCourier64(unit,candidate)||(pass==0&&!wht::RouteNonHero131(base64,profile64->build,unit)))continue;
   bool hasItem=false;if(wht::InventoryIdentity131(wht::ComponentLookup131::Slot(unit,inv->inventoryOffset),base64,inv->inventoryTable)){const auto capacity=inventory.size(unit);if(capacity<=6)for(unsigned k=0;k<capacity;++k)hasItem|=inventory.item(unit,k)!=0;}
   const uint64_t id=(uint64_t(candidate.id.low)<<32)|candidate.id.high,oldId=(uint64_t(fallback.id.low)<<32)|fallback.id.high;
   if(!fallback.unit||(hasItem&&!fallbackItems)||(hasItem==fallbackItems&&id<oldId)){fallback=candidate;fallbackItems=hasItem;}
  }
  if(!fallback.unit)return false;out=courier64=fallback;routePinned64=true;return true;
 }__except(1){return false;}
}
bool ReadRoute64(wht::SelfCastSource131& out){return routePinned64?ReadCourier64(mthRoute64.owner.unit,out):ReadSelectedRoute64(out);}

bool BeginQuickDrop64(unsigned slot){
 if(!Allowed64()||!settings64||slot>=6)return false;
 return quickDropFlow64.Run(GetTickCount64(),[&](){sellAll64.Reset();},[&](){
  const auto* p=wht::InventoryProfile(profile64->build);if(!p)return false;
  const auto* networkProfile=wht::DropNetworkProfileFor131(profile64->build);if(!networkProfile)return false;
  const auto network=wht::DropNetworkNative131(base64,*networkProfile,*p);
  const auto ok=wht::QuickDropNetwork131(base64,*p,wht::InventoryNative131(base64,*p),network,slot,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},[&](uintptr_t unit){return wht::DropOwned131(base64,profile64->build,network,unit);});
  return ok;
 });
}
bool CaptureDrop64(wht::DropSnapshot& snapshot){return wht::DropSnapshot131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},snapshot);}
bool BeginSell64(int selectedSlot=-1){
 if(!(Allowed64()&&!craft64.active&&!delivery64.active&&!menu64.open&&!macro64.playing&&!macro64.recording&&!dropAllNetwork64.active&&GetForegroundWindow()==window64))return false;
  autoPick64.Reset();wht::SellView131 view;
  const bool started=wht::ReadSellView131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},view)&&sellAll64.Start(view,GetTickCount64(),selectedSlot);
 wht::ShowNotification131(base64,profile64->build,ui64,started?"Sell All: finding a nearby shop":"Sell All: select a living unit with sellable items");wht::Log(folder64,started?"WHT: Sell All queued.":"WHT: Sell All unavailable (selection/pawnable items/active worker).");
 return started;
}
bool SellHovered64(){POINT point{};RECT client{};if(!GetCursorPos(&point)||!ScreenToClient(window64,&point)||!GetClientRect(window64,&client))return false;
 const int slot=wht::InventorySlotHit131(point,client.right,client.bottom,[&](unsigned slot){return wht::Button64(ui64,base64,*profile64,slot+12);});
 if(slot<0)return false;BeginSell64(slot);return true;
}
bool CommandImpl64(const std::wstring& text){std::wistringstream in(text);std::wstring cmd;if(!(in>>cmd))return false;wht::FoldCommandName(cmd);
 if(cmd==L"#menu"||cmd==L"#tool"){menu64.Toggle();return true;}
 if(cmd==L"#toolui"){size_t index;std::wstring op,value;if(!(in>>index>>op)||index>=wht::kToolMenuControls.size())return false;const auto& c=wht::kToolMenuControls[index];if(!Control64(c))return false;
  if(op==L"toggle")value=wht::ToolMenuOverlay::ReadValue(config64,index)==L"1"?L"0":L"1";else if(op==L"set"){std::getline(in,value);if(!value.empty()&&value[0]==L' ')value.erase(0,1);}else return false;
  if(!wht::ValidMenuValue(c,value))return false;
  if(const auto* mode=wht::MthModeControl131(c.section,c.key))return wht::ApplyMthMode131(mode,value,config64,[&](const wchar_t* section,const wchar_t* key,const wchar_t* saved){return wht::SaveSetting(ini64,section,key,saved);},[&](){Reload64();});
  if(std::wstring(c.section)==L"Language"){const bool english=std::wstring(c.key)==L"English"?value==L"1":value!=L"1";wht::SaveSetting(ini64,L"Language",L"English",english?L"1":L"0");wht::SaveSetting(ini64,L"Language",L"Vietnamese",english?L"0":L"1");}
  else if(!wht::SaveSetting(ini64,c.section,c.key,value))return false;Reload64();return true;}
 if(!authorized64||config64.mode!=wht::RunMode::Local||!ui64)return false;
 if(cmd==L"#mth49"||cmd==L"#g17"){std::wstring args;std::getline(in,args);return wht::ApplyMthMode131(cmd,args,config64,[&](const wchar_t* section,const wchar_t* key,const wchar_t* value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();});}
 if(cmd==L"#mc"||cmd==L"#modecast"){std::wstring args;std::getline(in,args);return wht::ApplyCastMode131(args,config64,[&](const wchar_t* section,const wchar_t* key,const wchar_t* saved){return wht::SaveSetting(ini64,section,key,saved);},[&](){Reload64();});}
 if(cmd==L"#reloadconfig"){std::wstring args;std::getline(in,args);return wht::ReloadConfigCommand131(args,[&](){Reload64();Camera64();});}
 if(cmd==L"#pcam"||cmd==L"#pzoom"){std::wstring args;std::getline(in,args);return wht::ApplyPresetCamera131(args,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();Camera64();});}
 if(cmd==L"#full"||cmd==L"#lock"){std::wstring args;std::getline(in,args);return wht::ApplyWindowToggle131(cmd,args,config64,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();windowServices64.Apply(window64,Allowed64()&&config64.borderless,Allowed64()&&config64.lockCursor,menu64.open);});}
 if(cmd==L"#whp"||cmd==L"#hhp"){std::wstring args;std::getline(in,args);const auto* p=wht::HpDimensionProfileFor131(profile64->build);if(!p||!wht::HpDimensionCode131(base64,*p))return false;const bool ok=wht::ApplyHpDimension131(cmd,args,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();});if(ok)wht::ShowNotification131(base64,profile64->build,ui64,std::string(cmd==L"#whp"?"HP bar width: ":"HP bar height: ")+std::to_string(cmd==L"#whp"?config64.barWidthHp:config64.barHeightHp)+" (0 = native)");return ok;}
 if(cmd==L"#wsize"){std::wstring args;std::getline(in,args);int width=0,height=0;return wht::ParseWindowSize131(args,width,height)&&wht::ResizeWindow131(window64,width,height);}
 if(cmd==L"#kscroll"||cmd==L"#mscroll"){std::wstring args;std::getline(in,args);if(!wht::ScrollCode131(base64,profile64->build))return false;return wht::ApplyScrollCommand131(cmd,args,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();Camera64();});}
 if(cmd==L"#islot"){std::wstring extra;if(in>>extra)return false;wht::InventoryInfo131 info;const bool ok=wht::ReadInventoryInfo131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},info);auto text=ok?wht::InventoryInfoText131(info,true):std::string("Item Slots: select a unit; inventory could not be read");localInfo64.Event(text,GetTickCount64());wht::Log(folder64,text);return ok;}
 if(wht::DeliveryCommand131(cmd)){std::wstring extra;if(in>>extra)return false;auto api=CraftApi64();wht::DeliveryBag131 c,h;const bool ok=Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable)&&!craft64.active&&!delivery64.active&&mthRoute64.route<0&&!courierBase64.pending&&!gokuEnabled64&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64&&wht::DeliveryRead131(api,courier64,false,c)&&wht::DeliveryFindHero131(api,c.source,h)&&delivery64.Start(c,h,GetTickCount64());if(ok&&(courierBase64.ui!=c.source.ui||courierBase64.session!=c.source.session))courierBase64.Save(c.source);const char* text=ok?"Courier Delivery: queued":"Courier Delivery: use #setga, give courier items, select your hero and stop other MTH actions";wht::Log(folder64,text);wht::ShowNotification131(base64,profile64->build,ui64,text);return ok;}
 if(const int action=wht::CraftCommand131(cmd);action>=0){std::wstring extra;if(in>>extra)return false;auto api=CraftApi64();if(mthRoute64.route>=0||courierBase64.pending||gokuEnabled64||!(!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64)){CraftReport64(wht::MthResult::Busy);return false;}const auto r=craft64.Queue(action,Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable),api);CraftReport64(r);return r==wht::MthResult::Running;}
 if(const auto* draw=wht::CraftDrawCommand131(cmd)){std::wstring args;std::getline(in,args);return wht::ApplyCraftDraw131(*draw,args,config64,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();});}
 if(wht::IsAutoHealCommand(cmd)){std::wstring args;std::getline(in,args);if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;std::wistringstream arguments(args);const wchar_t* key=nullptr;std::wstring value;auto slot=[&](unsigned index){auto api=AutoApi64();wht::SelfCastSource131 source;wht::InventoryInfo131 info;return api.ReadSource(source)&&wht::ReadInventoryInfo131(base64,profile64->build,[&](){return api.core.Selected();},info)?info.codes[index]:0u;};if(!wht::AutoHealSetting(cmd,arguments,config64,slot,key,value)||!wht::SaveSetting(ini64,L"AutoHeal",key,value))return false;Reload64();wht::ShowNotification131(base64,profile64->build,ui64,cmd==L"#autoheal"?(config64.autoHealEnabled?"Auto Heal: ON":"Auto Heal: OFF"):cmd==L"#healhp"?"Auto Heal HP: "+wht::Utf8(value)+"%":"Auto Heal: registered healing item");return true;}
 if(wht::IsAutoManaCommand(cmd)){std::wstring args;std::getline(in,args);if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;std::wistringstream arguments(args);const wchar_t* key=nullptr;std::wstring value;auto slot=[&](unsigned index){auto api=AutoApi64();wht::SelfCastSource131 source;wht::InventoryInfo131 info;return api.ReadSource(source)&&wht::ReadInventoryInfo131(base64,profile64->build,[&](){return api.core.Selected();},info)?info.codes[index]:0u;};if(!wht::AutoManaSetting(cmd,arguments,config64,slot,key,value)||!wht::SaveSetting(ini64,L"AutoMana",key,value))return false;Reload64();wht::ShowNotification131(base64,profile64->build,ui64,cmd==L"#automana"?(config64.autoManaEnabled?"Auto Mana: ON":"Auto Mana: OFF"):cmd==L"#healmp"?"Auto Mana: "+wht::Utf8(value)+"%":"Auto Mana: registered mana item");return true;}
 if(cmd==L"#autoskilldelay"||cmd==L"#autoskillrange"){std::wstring args;std::getline(in,args);const bool ok=wht::AutoSkillSetting131(cmd,args,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();autoSkills64.CancelScan();autoSkills64.due=0;});if(ok)wht::ShowNotification131(base64,profile64->build,ui64,std::string(cmd==L"#autoskilldelay"?"Auto Skills delay (seconds): ":"Auto Skills range: ")+std::to_string(cmd==L"#autoskilldelay"?config64.autoSkillsDelay:config64.autoSkillsRange));return ok;}
 if(cmd==L"#autoskillswap"){std::wstring args;std::getline(in,args);int first=0,second=0;if(!wht::AutoSkillSwapArgs131(args,first,second))return false;auto api=AutoApi64();wht::SelfCastSource131 source;if(!api.ReadSource(source)||!autoSkills64.Swap(source,first,second))return false;wht::ShowNotification131(base64,profile64->build,ui64,autoSkills64.Summary(source));return true;}
 if(cmd==L"#autoskilllist"||cmd==L"#autoskillclear"){std::wstring args;std::getline(in,args);int index=0;if(cmd==L"#autoskillclear"){if(!AutoSkillClearIndex131(args,index))return false;}else if(args.find_first_not_of(L" \t")!=std::wstring::npos)return false;auto api=AutoApi64();wht::SelfCastSource131 source;if(!api.ReadSource(source))return false;if(cmd==L"#autoskillclear"&&!autoSkills64.Clear(source,index))return false;wht::ShowNotification131(base64,profile64->build,ui64,autoSkills64.Summary(source));return true;}
 if(cmd==L"#autoskills"){std::wstring args;std::getline(in,args);bool next=false;if(!wht::ViewToggle131(args,autoSkills64.running,next))return false;auto api=AutoApi64();wht::SelfCastSource131 source;uintptr_t context=0;unsigned local=24,mask=0;const unsigned* views=nullptr;const bool ok=(!next||(config64.autoSkillsEnabled&&api.ReadSource(source)&&api.Context(context,local,mask,views)))&&autoSkills64.Toggle(source,next);wht::ShowNotification131(base64,profile64->build,ui64,ok?(next?"Auto Skills: ON":"Auto Skills: OFF"):"Auto Skills: enable in menu, select your unit, bind skills with Ctrl+Alt+1..9; try again in an active match");return ok;}
 if(cmd==L"#stopga"||cmd==L"#stopmth"){std::wstring extra;if(in>>extra)return false;
  delivery64.Reset();{auto api=CraftApi64();if(craft64.active)craft64.Finish(wht::MthResult::Disabled,api);}mthRoute64.Reset();courierBase64.Cancel();if(gokuEnabled64){gokuEnabled64=false;autoPick64.Reset();}
  bool stopped=false;wht::SelfCastSource131 source;
  if(courier64.unit&&ReadCourier64(courier64.unit,source)&&wht::SameSelfCastSource131(source,courier64)){const auto api=wht::MthQueueNative131(base64,profile64->build);const auto select=wht::CourierSelectNative131(base64,profile64->build);wht::SelectionRoster131 roster;if(api.stop&&select&&wht::ReadSelectionRoster131(base64,profile64->build,roster))stopped=wht::StopCourierNative131(roster,source.unit,select,api,ui64);}
  wht::ShowNotification131(base64,profile64->build,ui64,stopped?"Courier/MTH: stopped":"Courier/MTH: pending actions cancelled; no courier Stop submitted");return true;
 }
 if(cmd==L"#gabase"||cmd==L"#backga"||cmd==L"#backsoi"){std::wstring extra;if(in>>extra)return false;
  wht::SelfCastSource131 source;if(cmd==L"#gabase"){const bool ok=wht::ReadSelfCastSource131(base64,profile64->build,ui64,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float v=0,m=0,rate=0;return wht::State64(unit,base64,*profile64,false,v,m,rate)&&v>.405f;},source)&&courierBase64.Save(source);mthNotice64.Event(ok?wht::MthNoticeKind131::BaseSaved:wht::MthNoticeKind131::BaseRefused,config64.mthNoti,GetTickCount64());wht::Log(folder64,ok?"WHT: courier base position saved.":"WHT: base not saved; select a living owned unit.");return ok;}
  if(craft64.active||!settings64||!(config64.mth49Enable||config64.g17Enable)||!courier64.unit||!ReadCourier64(courier64.unit,source)||!wht::SameSelfCastSource131(source,courier64)||!courierBase64.Queue(source)){mthNotice64.Event(wht::MthNoticeKind131::CourierRequired,config64.mthNoti,GetTickCount64());wht::Log(folder64,"WHT: courier return refused; use #setga and #gabase in this match first.");return false;}
  delivery64.Reset();mthRoute64.Reset();mthNotice64.Event(wht::MthNoticeKind131::ReturnQueued,config64.mthNoti,GetTickCount64());wht::Log(folder64,"WHT: courier return queued.");return true;
 }
 if(cmd==L"#setga"){std::wstring extra;if(in>>extra||craft64.active||delivery64.active||mthRoute64.route>=0||courierBase64.pending)return false;wht::SelfCastSource131 source;if(!ReadSelectedRoute64(source)){mthNotice64.Event(wht::MthNoticeKind131::CourierRequired,config64.mthNoti,GetTickCount64());return false;}courier64=source;mthNotice64.Event(wht::MthNoticeKind131::CourierAssigned,config64.mthNoti,GetTickCount64());wht::Log(folder64,"WHT: selected courier assigned.");return true;}
 if(const int route=wht::RouteCommand131(cmd);route>=0){std::wstring extra;if(in>>extra)return false;if(craft64.active||delivery64.active||courierBase64.pending||!settings64||!(config64.mth49Enable||config64.g17Enable)||!wht::PointNative131(base64,profile64->build))return false;
  const int counts[]={config64.chayGa1Count,config64.chayGa2Count,config64.chayGa3Count,config64.chaySoi1Count,config64.chaySoi2Count,config64.chaySoi3Count};wht::SelfCastSource131 source;const bool ok=ResolveRoute64(source)&&mthRoute64.Start(route,counts[route],source);mthNotice64.Event(ok?wht::MthNoticeKind131::RouteQueued:wht::MthNoticeKind131::CourierRequired,config64.mthNoti,GetTickCount64());wht::Log(folder64,ok?"WHT: MTH native queued route requested.":"WHT: no living owned moving courier found; select courier and use #setga.");return ok;}
 if(const auto* draw=wht::RouteDrawCommand131(cmd)){std::wstring args;std::getline(in,args);return wht::ApplyRouteDraw131(*draw,args,config64,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();});}
 if(cmd==L"#gokudraw"){std::wstring args;std::getline(in,args);return wht::ApplyGokuDraw131(args,config64.pickGokuDraw,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();});}
 if(cmd==L"#goku"||cmd==L"#autopickgoku"){std::wstring args;std::getline(in,args);bool next=false;if(craft64.active||delivery64.active||!settings64||!wht::ViewToggle131(args,gokuEnabled64,next)||(next&&!(config64.mth49Enable||config64.g17Enable)))return false;gokuEnabled64=next;autoPick64.Reset();mthNotice64.Event(next?wht::MthNoticeKind131::On:wht::MthNoticeKind131::Off,config64.mthNoti,GetTickCount64());wht::Log(folder64,next?"WHT: Auto Pick Goku ON.":"WHT: Auto Pick Goku OFF.");return true;}
 if(wht::DropCommand131(cmd)){std::wstring args;std::getline(in,args);int slot=-1;
  if(!settings64||menu64.open||macro64.playing||macro64.recording||allSkills64.active||craft64.active||delivery64.active||dropAllNetwork64.active||GetForegroundWindow()!=window64||!wht::DropCommandSlot131(cmd,args,slot))return false;
  if(slot>=0){const bool ok=BeginQuickDrop64(unsigned(slot));wht::Log(folder64,ok?"WHT: single item drop submitted once.":"WHT: item drop refused.");return ok;}
  sellAll64.Reset();quickDropFlow64.pickupAfter=GetTickCount64()+2000;
  wht::DropSnapshot snapshot;const bool ok=CaptureDrop64(snapshot)&&dropAllNetwork64.Start(snapshot,GetTickCount64());
  wht::Log(folder64,ok?"WHT: drop all worker started.":"WHT: drop all command refused.");return ok;
 }
 if(wht::AutoClickCommand131(cmd)){std::wstring args;std::getline(in,args);if(!settings64)return false;const bool ok=wht::ApplyAutoClickCommand131(cmd,args,autoClick64,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();});wht::Log(folder64,ok?"WHT: Auto Click Left="+std::to_string(autoClick64.left)+" Right="+std::to_string(autoClick64.right)+" interval="+std::to_string(config64.autoClickInterval)+"ms":"WHT: Usage #autol/#autor [0|1]; #clickdelay <5..1000>");return ok;}
 if(const auto* option=wht::ViewCommandFor131(cmd)){std::wstring args;std::getline(in,args);return wht::ApplyViewCommand131(*option,args,config64,[&](const wchar_t* section,const wchar_t* key,const wchar_t* value){return wht::SaveSetting(ini64,section,key,value);},[&](){Reload64();});}
 if(const auto kind=wht::ManualClearKind131(cmd)){std::wstring extra;if(!settings64||(in>>extra))return false;return manualClear64.Queue(ui64,kind,GetTickCount64());}
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
bool Command64(const std::wstring& text){
 const bool ok=CommandImpl64(text);std::wistringstream in(text);std::wstring name,args;if(!(in>>name))return ok;wht::FoldCommandName(name);std::getline(in,args);
 if(Allowed64()){auto feedback=wht::CommandFeedback131(name,args,ok,config64,menu64.open,settings64,config64.borderless,config64.lockCursor,autoPickEnabled64,autoClick64.left,autoClick64.right);if(ok&&name==L"#wsize")feedback=wht::WindowSizeText131(window64);if(!feedback.empty())wht::ShowNotification131(base64,profile64->build,ui64,feedback);}return ok;
}

void Destroy64(){
 optionMenu64.Reset();

 menuFogVertex64.ResetMatch();presentation64.Restore();windowServices64.Restore();

 if(profile64)scroll64.Update(base64,profile64->build,camera64,false,config64);scroll64.Reset();
wht::ForgetUiRefresh131();wht::ResetSpeed131();wht::ResetLifeMana131();if(menu64.independentHost&&Identity64(reinterpret_cast<uintptr_t>(menu64.independentHost),profile64->simpleVtable)){
 auto table=*reinterpret_cast<uintptr_t*>(menu64.independentHost);reinterpret_cast<void(*)(void*,unsigned)>(*reinterpret_cast<uintptr_t*>(table+16))(menu64.independentHost,1);}
 menu64.Reset();placements64.clear();status64=nullptr;menuLogged64=false;gameplay64.Reset();hotkeys64.Reset();remap64.Reset();quickChatEcho64.Reset();repeatMessage64.Reset();timeOfDay64.Reset();consoleUi64.Reset();blackBar64.Reset();mouseTooltip64.Reset();autoClick64.Reset();pinned64={};messageTimers64.Reset();manualClear64.Reset();resources64.Reset();clock64.Reset();matchStart64.Reset();timeBoss64.Reset();detailTooltip64.Reset();fps64.Reset();quickDrop64={};quickDropFlow64.Reset();dropIcon64.Reset();pickIcon64.Reset();gokuIcon64.Reset();craft64.Reset();delivery64.Reset();autoSkills64.Reset();autoHeal64.Reset();autoMana64.Reset();craftRoster64={};craftButtons64.Reset();mthNotice64.Reset();localInfo64.Reset();mthRoute64.Reset();courierBase64.Reset();routeButtons64.Reset();courier64={};routePinned64=false;gokuEnabled64=false;sellIcon64.Reset();reveal64.Reset();sellAll64.Reset();autoPick64.Reset();autoPickEnabled64=false;dropAllNetwork64.Reset();skillsIcon64.Reset();allSkills64.Reset();camera64=0;cameraMask64=0;cameraLogged64=false;typed64=false;chat64.clear();macro64.Stop();}
void PresentGraphics64(){
 wht::UiPerfScope131 graphicsTiming(perf64,1);
 if(wht::uiRefreshOwner131!=ui64||!ui64)return;
if(config64.lifeManaCommas||config64.lifeManaNoHide||config64.statsCommas||wht::lifeFlags131||!wht::lifeUi131)if(const auto* p=wht::LifeProfileFor131(profile64->build))wht::UpdateLifeMana131(base64,profile64->build,ui64,wht::Component64(ui64,base64,p->portrait),wht::Component64(ui64,base64,profile64->infoVtable),(Allowed64()&&settings64)?((config64.lifeManaCommas?1u:0u)|(config64.lifeManaNoHide?2u:0u)|(config64.statsCommas?4u:0u)):0u,[](uintptr_t unit,bool mana,float& value,float& maximum){float rate=0;return wht::State64(unit,base64,*profile64,mana,value,maximum,rate);},true);
 if(config64.statsTooltip||detailTooltip64.Active())if(const auto* life=wht::LifeProfileFor131(profile64->build)){const auto source=wht::ReadLifeSource131(base64,*life,ui64,wht::Component64(ui64,base64,life->portrait));
  detailTooltip64.Update(base64,profile64->build,ui64,source,wht::Component64(ui64,base64,profile64->infoVtable),Allowed64()&&settings64&&config64.statsTooltip&&!menu64.open,GetTickCount64(),[&](uintptr_t table){return wht::Component64(ui64,base64,table);});}
}
void PresentBar64(uintptr_t bar){
 timeBoss64.RememberBar(bar);
 if(!ui64||!menu64.initialized)return;
 auto raw=api64;raw.setWidth=reinterpret_cast<decltype(raw.setWidth)>(base64+profile64->width);raw.setAbsPoint=reinterpret_cast<decltype(raw.setAbsPoint)>(base64+profile64->absolute);raw.setRelPoint=reinterpret_cast<decltype(raw.setRelPoint)>(base64+profile64->relative);gameplay64.WriteBar(bar,base64,*profile64,config64,Allowed64()&&settings64,raw);
}
void Update64(){
 perf64.Report(folder64,Allowed64());wht::UiPerfScope131 updateTiming(perf64,0);
const auto ui=CurrentUi64();if(ui!=ui64){Destroy64();ui64=ui;}if(!Allowed64()||!settings64)repeatMessage64.Reset();if(!Allowed64()||!settings64||!config64.timeBoss||!(config64.mth49Enable||config64.g17Enable))timeBoss64.active=false;if(!Allowed64())wht::ResetSpeed131(); manualClear64.Update(base64,profile64->build,ui64,Allowed64()&&settings64,!typed64&&!menu64.open&&GetForegroundWindow()==window64,GetTickCount64());

menuFogVertex64.Update(base64,profile64->build,Allowed64(),authorized64.load()&&settings64&&config64.mode==wht::RunMode::Local,ui,config64,folder64,[&](const std::string& text){wht::ShowNotification131(base64,profile64->build,ui,text);});
presentation64.Update(base64,profile64->build,Allowed64()&&settings64&&ui!=0,config64.vsync,config64.unlockFps,folder64);
windowServices64.Apply(window64,Allowed64()&&config64.borderless,Allowed64()&&config64.lockCursor,menu64.open);
if(!ui)return; wht::InstallUiRefresh131(base64,profile64->build,ui,PresentGraphics64);graphicsDiagnostic64.Update(base64,profile64->build,ui,config64,Allowed64(),folder64,consoleUi64,mouseTooltip64,[&](uintptr_t table){return wht::Component64(ui,base64,table);}); wht::UpdateMatchStart131(matchStart64,base64,profile64->build,ui,Allowed64()&&settings64&&config64.soundStartGame,module64,folder64); if(auto p=wht::RevealProfileFor131(profile64->build)){
  if(reveal64.Update(base64,profile64->build,wht::Component64(ui64,base64,p->worldTable),Allowed64()&&settings64&&config64.revealInvisible)&&!reveal64.reported){reveal64.reported=true;wht::Log(folder64,"WHT: native invisible-unit display enabled.");}
 }
if(config64.lifeManaCommas||config64.lifeManaNoHide||config64.statsCommas||wht::lifeFlags131||!wht::lifeUi131)if(const auto* p=wht::LifeProfileFor131(profile64->build))wht::UpdateLifeMana131(base64,profile64->build,ui64,wht::Component64(ui64,base64,p->portrait),wht::Component64(ui64,base64,profile64->infoVtable),(Allowed64()&&settings64)?((config64.lifeManaCommas?1u:0u)|(config64.lifeManaNoHide?2u:0u)|(config64.statsCommas?4u:0u)):0u,[](uintptr_t unit,bool mana,float& value,float& maximum){float rate=0;return wht::State64(unit,base64,*profile64,mana,value,maximum,rate);});
 if(config64.mouseTooltip||mouseTooltip64.active){const auto* tip=wht::MouseTooltipProfileFor131(profile64->build);const auto* console=wht::ConsoleProfileFor131(profile64->build);
  if(tip&&console)wht::UpdateMouseTooltip131(mouseTooltip64,base64,profile64->build,ui64,wht::Component64(ui64,base64,tip->table),wht::ConsoleComponent131(ui64,base64,console->table),window64,Allowed64()&&settings64&&!menu64.open&&config64.mouseTooltip);
 }
 if(config64.hideBlackBar||blackBar64.Active())if(const auto* p=wht::BlackBarProfileFor131(profile64->build))blackBar64.Update(base64,profile64->build,ui64,wht::Component64(ui64,base64,p->worldTable),Allowed64()&&settings64&&config64.hideBlackBar);
 if(config64.hideConsoleUI||consoleUi64.active)if(const auto* p=wht::ConsoleProfileFor131(profile64->build))consoleUi64.Update(base64,profile64->build,wht::ConsoleComponent131(ui64,base64,p->table),Allowed64()&&settings64&&config64.hideConsoleUI);
 if(config64.hideTimeOfDay||timeOfDay64.active)if(const auto* p=wht::TimeOfDayProfileFor131(profile64->build))timeOfDay64.Update(base64,profile64->build,wht::Component64(ui64,base64,p->table),Allowed64()&&settings64&&config64.hideTimeOfDay);
 wht::UpdateAutoClick131(autoClick64,config64,window64,Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&!sellAll64.active&&!dropAllNetwork64.active);
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
  if(!sellAll64.active)quickDropFlow64.pickupAfter=(std::max)(quickDropFlow64.pickupAfter,GetTickCount64()+1000);
  if(result==wht::SellResult131::Submitted)wht::Log(folder64,"WHT: Sell All synchronized shop order submitted.");
  else if(result!=wht::SellResult131::Working&&result!=wht::SellResult131::Idle)wht::Log(folder64,std::string("WHT: Sell All ")+(result==wht::SellResult131::Complete?"completed":result==wht::SellResult131::NoShop?"no accessible purchase shop":result==wht::SellResult131::Cancelled?"cancelled":"remaining")+"; sold="+std::to_string(sellAll64.sold)+" skipped="+std::to_string(sellAll64.skipped)+" remaining="+std::to_string(sellAll64.remaining)+" gold delta="+std::to_string(sellAll64.goldDelta));
  if(result!=wht::SellResult131::Submitted&&result!=wht::SellResult131::Working&&result!=wht::SellResult131::Idle)wht::ShowNotification131(base64,profile64->build,ui64,std::string("Sell All: ")+(result==wht::SellResult131::Complete?"completed":result==wht::SellResult131::NoShop?"no nearby shop":result==wht::SellResult131::Cancelled?"cancelled":"remaining items")+"; sold="+std::to_string(sellAll64.sold));
 }
 sellIcon64.latched=sellAll64.active;sellIcon64.Update(base64,Allowed64()&&config64.showSellAll,menu64);
 if(gokuEnabled64&&!(Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable))){gokuEnabled64=false;autoPick64.Reset();}
 if((autoPickEnabled64||gokuEnabled64)&&!craft64.active&&!delivery64.active&&!sellAll64.active&&(Allowed64()&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64)&&!dropAllNetwork64.active){const auto now=GetTickCount64();
  if(now>=autoPick64.next&&now>=quickDropFlow64.pickupAfter){auto p=wht::PickProfileFor131(profile64->build);wht::PickSnapshot131 snapshot;bool acquiredGoku=false;
   const bool valid=p&&wht::ReadPickSnapshot131(base64,profile64->build,wht::Component64(ui64,base64,p->worldTable),[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},snapshot,gokuEnabled64?wht::gokuType131:0,&acquiredGoku);
   if(valid&&gokuEnabled64&&acquiredGoku){gokuEnabled64=false;autoPick64.Reset();autoPick64.next=now+250;mthNotice64.Event(wht::MthNoticeKind131::Acquired,config64.mthNoti,now);wht::Log(folder64,"WHT: Auto Pick Goku OFF (confirmed in inventory).");}else{
   const auto result=autoPick64.Tick(now,valid?&snapshot:nullptr,[&](const wht::DropSnapshot& source,const wht::PickCandidate131& item){const auto* inventory=wht::InventoryProfile(profile64->build);if(gokuEnabled64&&(!inventory||!wht::ItemCodeGuard131(base64,*inventory)||wht::ItemCodeValue131(base64,*inventory,item.object)!=wht::gokuType131))return false;return wht::SendPick131(base64,profile64->build,source,item,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;});});
   if(result==wht::PickResult131::Submitted)wht::Log(folder64,"WHT: Auto Pick native target order submitted.");
   }
   // Invalid selection/session clears only the target; the manual toggle stays on.

  }
 }else autoPick64.Reset();
 if(delivery64.active){auto api=CraftApi64();const auto result=delivery64.Tick(GetTickCount64(),Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable),!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!craft64.active&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&GetForegroundWindow()==window64,[&](const wht::SelfCastSource131& source,bool hero,wht::DeliveryBag131& bag){return wht::DeliveryRead131(api,source,hero,bag);},[&](const wht::SelfCastSource131& c,const wht::SelfCastSource131& h){wht::SelectionRoster131 roster;const auto select=wht::CourierSelectNative131(base64,profile64->build);const auto point=wht::PointNative131(base64,profile64->build);return select&&point&&wht::ReadSelectionRoster131(base64,profile64->build,roster)&&wht::SendCourierPointNative131(roster,c.unit,select,point,h.x,h.y);},[&](const wht::SelfCastSource131& c,const wht::SelfCastSource131& h,const wht::DropObjectId& item){return wht::DeliverySend131(api,c,h,item);},[&](const wht::SelfCastSource131& c){courierBase64.Queue(c);});if(const auto* text=wht::DeliveryNotice131(result)){wht::Log(folder64,text);mthNotice64.EventText(wht::MthNoticeKind131::OrderRefused,text,config64.mthNoti,GetTickCount64());}}
 if(courierBase64.pending)courierBase64.Tick(Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable),!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64,ReadCourier64,[&](const wht::SelfCastSource131& source,float x,float y){const auto select=wht::CourierSelectNative131(base64,profile64->build);const auto point=wht::PointNative131(base64,profile64->build);wht::SelectionRoster131 roster;const bool ok=select&&point&&wht::ReadSelectionRoster131(base64,profile64->build,roster)&&wht::SendCourierPointNative131(roster,source.unit,select,point,x,y);mthNotice64.Event(ok?wht::MthNoticeKind131::ReturnSent:wht::MthNoticeKind131::OrderRefused,config64.mthNoti,GetTickCount64());wht::Log(folder64,ok?"WHT: courier returning to saved base.":"WHT: courier return native order refused; no retry.");return ok;});
 if(mthRoute64.route>=0)mthRoute64.TickQueued(Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable),!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64,ReadRoute64,[&](const wht::SelfCastSource131& before,const wht::Route131& route,int count){wht::SelfCastSource131 current;if(!ReadCourier64(before.unit,current)||!wht::SameSelfCastSource131(before,current)){mthNotice64.Event(wht::MthNoticeKind131::OrderRefused,config64.mthNoti,GetTickCount64());return false;}const auto api=wht::MthQueueNative131(base64,profile64->build);const auto select=wht::CourierSelectNative131(base64,profile64->build);wht::SelectionRoster131 roster;if(!api||!select||!wht::ReadSelectionRoster131(base64,profile64->build,roster)){mthNotice64.Event(wht::MthNoticeKind131::OrderRefused,config64.mthNoti,GetTickCount64());return false;}const bool submitted=wht::SubmitCourierRoute131(roster,current.unit,select,api,ui64,route,count);if(submitted){int index=-1;for(int i=0;i<6;++i)if(wht::routes131[i].x1==route.x1&&wht::routes131[i].y1==route.y1&&wht::routes131[i].x2==route.x2&&wht::routes131[i].y2==route.y2)index=i;mthNotice64.EventText(wht::MthNoticeKind131::RouteSent,wht::MthRouteStarted131(index,count),config64.mthNoti,GetTickCount64());}else mthNotice64.Event(wht::MthNoticeKind131::OrderRefused,config64.mthNoti,GetTickCount64());wht::Log(folder64,submitted?"WHT: MTH native route queue submitted.":"WHT: MTH native route queue failed; no replay.");return submitted;});
 if(craft64.active){auto api=CraftApi64();CraftReport64(craft64.Tick(GetTickCount64(),Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable),!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&GetForegroundWindow()==window64,api));}
 if(config64.autoHealEnabled&&GetTickCount64()>=autoHeal64.state.due&&GetTickCount64()>=autoHeal64.state.retry&&GetTickCount64()>=autoMana64.state.retry&&(Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!craft64.active&&!delivery64.active&&!autoSkills64.running&&!autoPickEnabled64&&!gokuEnabled64&&mthRoute64.route<0&&!courierBase64.pending&&!autoClick64.left&&!autoClick64.right&&GetForegroundWindow()==window64)){auto api=AutoApi64();if(!api.Target())autoHeal64.Tick(GetTickCount64(),config64,true,api,[&](uintptr_t unit,float& hp,float& maximum){float rate=0;return wht::State64(unit,base64,*profile64,false,hp,maximum,rate);});}
 if(config64.autoManaEnabled&&GetTickCount64()>=autoMana64.state.due&&GetTickCount64()>=autoMana64.state.retry&&GetTickCount64()>=autoHeal64.state.retry&&(Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!craft64.active&&!delivery64.active&&!autoSkills64.running&&!autoPickEnabled64&&!gokuEnabled64&&mthRoute64.route<0&&!courierBase64.pending&&!autoClick64.left&&!autoClick64.right&&GetForegroundWindow()==window64)){auto api=AutoApi64();if(!api.Target())autoMana64.Tick(GetTickCount64(),config64,true,api,[&](uintptr_t unit,float& hp,float& maximum){float rate=0;return wht::State64(unit,base64,*profile64,true,hp,maximum,rate);},true);}
 if(!config64.autoSkillsEnabled){autoSkills64.running=false;autoSkills64.CancelScan();}else if(autoSkills64.running&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!craft64.active&&!delivery64.active&&!autoPickEnabled64&&!gokuEnabled64&&mthRoute64.route<0&&!courierBase64.pending&&!autoClick64.left&&!autoClick64.right&&GetForegroundWindow()==window64&&(autoSkills64.scanning||GetTickCount64()>=autoSkills64.due)){auto api=AutoApi64();if(!api.Target())autoSkills64.Tick(GetTickCount64(),true,true,config64.autoSkillsDelay,config64.autoSkillsRange,api);}
 mthNotice64.Update(base64,profile64->build,ui64,Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable),config64,menu64,GetTickCount64());
 localInfo64.Update(base64,profile64->build,ui64,Allowed64()&&settings64,menu64,GetTickCount64());
 wht::NotificationDiagnostic131(folder64);
 {wht::UiPerfScope131 toolbarTiming(perf64,2);
 auto mthRaw64=menu64.api;mthRaw64.setWidth=reinterpret_cast<decltype(mthRaw64.setWidth)>(base64+profile64->width);mthRaw64.setAbsPoint=reinterpret_cast<decltype(mthRaw64.setAbsPoint)>(base64+profile64->absolute);
 craftButtons64.Update(base64,Allowed64()&&settings64,config64,craft64.active?craft64.kind:-1,menu64,&mthRaw64);
 routeButtons64.Update(base64,Allowed64()&&settings64,config64,mthRoute64.route,menu64,&mthRaw64);
 gokuIcon64.latched=gokuEnabled64;gokuIcon64.Update(base64,Allowed64()&&settings64&&!menu64.open&&(config64.mth49Enable||config64.g17Enable)&&config64.pickGokuDraw,menu64,&mthRaw64);
 mthText64.Update(base64,profile64->build,folder64,{&craftButtons64.buttons[0],&craftButtons64.buttons[1],&craftButtons64.buttons[2],&craftButtons64.buttons[3],&craftButtons64.buttons[4],&routeButtons64.buttons[0],&routeButtons64.buttons[1],&routeButtons64.buttons[2],&routeButtons64.buttons[3],&routeButtons64.buttons[4],&routeButtons64.buttons[5],&gokuIcon64});
 }
 pickIcon64.latched=autoPickEnabled64;pickIcon64.Update(base64,Allowed64()&&config64.showAutoPick,menu64);
 dropIcon64.Update(base64,Allowed64()&&config64.showDropAll,menu64);
 if(dropAllNetwork64.active){wht::DropSnapshot snapshot;const bool valid=Allowed64()&&!sellAll64.active&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64&&wht::DropSnapshot131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},snapshot);
  const auto result=dropAllNetwork64.Step(valid?&snapshot:nullptr,GetTickCount64(),[&](const wht::DropSnapshot& source,const wht::DropItem& item){return wht::SendDropSnapshot131(base64,profile64->build,source,item);});
  if(result==wht::DropResult::Complete||result==wht::DropResult::Remaining)quickDropFlow64.pickupAfter=GetTickCount64()+2000;
  if(result==wht::DropResult::Complete)wht::Log(folder64,"WHT: synchronized drop all completed; original items absent from inventory.");
  else if(result==wht::DropResult::Cancelled)wht::Log(folder64,"WHT: synchronized drop all cancelled.");
  else if(result==wht::DropResult::Remaining)wht::Log(folder64,"WHT: synchronized drop all stopped; remaining "+std::to_string(dropAllNetwork64.remaining));
  if(result==wht::DropResult::Complete||result==wht::DropResult::Cancelled||result==wht::DropResult::Remaining)wht::ShowNotification131(base64,profile64->build,ui64,std::string("Drop All: ")+(result==wht::DropResult::Complete?"completed":result==wht::DropResult::Cancelled?"cancelled":"remaining items"));
 }
  skillsIcon64.Update(base64,Allowed64()&&config64.showInsAllSkills,menu64);
  if(allSkills64.Tick(GetTickCount64(),Allowed64()&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64,[&](){return wht::Selected64(ui64,base64,*profile64);},SkillPoints64,SkillData64,ClickSkill64)){wht::Log(folder64,"WHT x64: all skills native sequence finished; submitted "+std::to_string(allSkills64.submitted));wht::ShowNotification131(base64,profile64->build,ui64,"All Skills: submitted "+std::to_string(allSkills64.submitted));}
  if(fps64.Update(base64,ui,profile64->build,Allowed64(),config64.showFps)&&!fps64.reported){fps64.reported=true;wht::Log(folder64,"WHT x64: native FPS toggle installed.");}
  if(hotkeys64.Due(GetTickCount64(),Allowed64()&&config64.showHotkeyInPanel)&&hotkeys64.Update(Allowed64()&&config64.showHotkeyInPanel,menu64,[&](unsigned slot){return wht::Button64(ui,base64,*profile64,slot);},&config64,(Allowed64()&&config64.showHotkeyInPanel)?InventoryMask64(true):0,(Allowed64()&&config64.showHotkeyInPanel)?AlternatePanel64():false)&&!hotkeys64.reported){hotkeys64.reported=true;wht::Log(folder64,"WHT x64: native panel hotkey labels displayed.");}
  for(auto entry:{std::pair<unsigned,const char*>{1,"selected-unit HP/s MP/s"},{2,"cooldown seconds"},{8,"HP/Mana values"},{16,"hero level"}})if(fresh&entry.first)wht::Log(folder64,std::string("WHT x64: live ")+entry.second+" displayed.");gameplay64.reported|=observed;
 }
 if(status64){std::string text="|cffffcc00WHT " WHT_VERSION_STRING " | 1.31.1."+std::to_string(profile64->build)+" x64|r";if(!authorized64)text+=" |cffff4444Login LoadFileMapWar3|r";else if(config64.showMem){float mb=0,percent=0;if(wht::GetCurrentProcessMemoryUsage(mb,percent))text+=" | "+std::to_string(int(mb))+" MB";}api64.setText(status64,text.c_str());}
 timeBoss64.Update(base64,profile64->build,ui64,Allowed64()&&settings64&&config64.timeBoss&&(config64.mth49Enable||config64.g17Enable),GetTickCount64(),menu64,[&](uintptr_t unit,float& hp){float maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,hp,maximum,rate);});
 repeatMessage64.Tick(Allowed64()&&settings64,!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64,GetTickCount64(),SendChat64);
 menu64.PlaybackVisual(macro64.playing);macro64.Tick(Allowed64()&&!typed64&&!menu64.open);
 wht::UpdateToolHelp131(window64,menu64,config64,dropIcon64,skillsIcon64,pickIcon64,sellIcon64,gokuIcon64,craftButtons64,routeButtons64,resources64);
}
void Safe64(){__try{Update64();}__except(1){menu64.failed=true;}}
bool SendChat64(const std::string& text){
 __try{return wht::SendQuickChat131(text,wht::QuickChatNative131(base64,profile64->build),[](wht::QuickChatView131& out){const auto ui=CurrentUi64();return ui&&ui==ui64&&wht::ReadQuickChat131(base64,profile64->build,ui,out);});}__except(1){return false;}
}
LRESULT CALLBACK Window64(HWND hwnd,UINT message,WPARAM key,LPARAM value){if(message==update64){Safe64();return 0;}
 if(optionMenu64.Held(message,unsigned(key)))return 0;
 if(message==WM_KILLFOCUS)quickChatEcho64.Reset();
 if(message==WM_KEYDOWN&&!(value&(1LL<<30)))quickChatEcho64.Reset();
 if(message==WM_CHAR&&quickChatEcho64.Character(value,GetTickCount64()))return 0;
 if((message==WM_KEYUP||message==WM_SYSKEYUP)&&remap64.Input(message,unsigned(key),false,false,false,config64,[](unsigned){return false;}))return 0;
 {RECT rect{};GetClientRect(hwnd,&rect);
 if(resources64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&settings64&&config64.showResource&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd))return 0;
 if(skillsIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&config64.showInsAllSkills&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd,[&](){
  const auto started=allSkills64.Start(GetTickCount64(),[&](){return wht::Selected64(ui64,base64,*profile64);},SkillPoints64,SkillData64);
  wht::Log(folder64,started?"WHT x64: all skills sequence queued.":"WHT x64: all skills unavailable (hero/points/menu).");
 }))return 0;}
 {RECT rect{};GetClientRect(hwnd,&rect);
 if(craftButtons64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64,[&](unsigned action){Command64(wht::craftCommands131[action]);}))return 0;
 if(routeButtons64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64,[&](unsigned route){const wchar_t* commands[]={L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};Command64(commands[route]);}))return 0;
 if(gokuIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==window64,[&](){Command64(L"#goku");}))return 0;
 if(pickIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,!craft64.active&&!delivery64.active&&Allowed64()&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd&&!dropAllNetwork64.active&&!sellAll64.active,[&](){autoPickEnabled64=!autoPickEnabled64;autoPick64.Reset();wht::ShowNotification131(base64,profile64->build,ui64,autoPickEnabled64?"Auto Pick: ON":"Auto Pick: OFF");wht::Log(folder64,autoPickEnabled64?"WHT: Auto Pick enabled.":"WHT: Auto Pick disabled.");}))return 0;
 if(sellIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,!craft64.active&&!delivery64.active&&Allowed64()&&config64.showSellAll&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd&&!dropAllNetwork64.active,[&](){
  BeginSell64();
 }))return 0;
 if(dropIcon64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,!craft64.active&&!delivery64.active&&Allowed64()&&config64.showDropAll&&!sellAll64.active&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd,[&](){
  autoPick64.Reset();quickDropFlow64.pickupAfter=GetTickCount64()+2000;
  const auto* p=wht::InventoryProfile(profile64->build);if(!p)return;
  wht::DropSnapshot snapshot;
  const bool started=wht::DropSnapshot131(base64,profile64->build,[&](){return wht::Selected64(ui64,base64,*profile64);},[&](uintptr_t unit){float current=0,maximum=0,rate=0;return wht::State64(unit,base64,*profile64,false,current,maximum,rate)&&current>.405f;},snapshot)&&dropAllNetwork64.Start(snapshot,GetTickCount64());
  wht::Log(folder64,started?"WHT: synchronized drop all queued.":"WHT: drop all unavailable or already active.");
 }))return 0;}
 if(quickDrop64.Mouse(message,!craft64.active&&!delivery64.active&&Allowed64()&&config64.quickDropItem&&!consoleUi64.active&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&GetForegroundWindow()==hwnd&&(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)&&!([&](){const auto* p=wht::QuickCastProfile(profile64->build);return p&&wht::QuickCastTarget131(base64,ui64,profile64->uiVtable,*p);})(),[&](){
  RECT rect{};GetClientRect(hwnd,&rect);POINT point{short(LOWORD(value)),short(HIWORD(value))};
  return wht::InventorySlotHit131(point,rect.right,rect.bottom,[&](unsigned slot){return wht::Button64(ui64,base64,*profile64,slot+12);});
 },[&](unsigned slot){
  const bool ok=BeginQuickDrop64(slot);
  wht::Log(folder64,ok?"WHT x64: quick drop synchronized action submitted.":"WHT x64: quick drop refused or slot empty.");
 }))return 0;
 if(message==WM_SYSKEYDOWN&&GetForegroundWindow()==window64&&menu64.ToggleKey(unsigned(key),config64.menuKey,bool(value&(1LL<<30)),typed64)){optionMenu64.ToolToggle(unsigned(key));return 0;}
 if(message==WM_KEYDOWN){bool repeat=(value&(1LL<<30))!=0;if(menu64.ToggleKey(unsigned(key),config64.menuKey,repeat,typed64)){optionMenu64.ToolToggle(unsigned(key));return 0;}if(!repeat&&key==VK_ESCAPE&&menu64.Escape())return 0;if(menu64.Key(unsigned(key),unsigned((value>>16)&255),Command64)){optionMenu64.ToolToggle(unsigned(key));return 0;}
  if(!repeat&&key==VK_RETURN){if(typed64){Command64(chat64);typed64=false;chat64.clear();}else{typed64=true;chat64.clear();}}
  if(key==VK_ESCAPE){typed64=false;chat64.clear();}
  if(Allowed64()&&!typed64&&!repeat&&!menu64.open&&key==VK_TAB&&config64.showResource&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){resources64.Toggle();return 0;}
  if(Allowed64()&&!typed64&&!repeat){if(key==config64.commandKey){settings64=!settings64;Camera64();wht::ShowNotification131(base64,profile64->build,ui64,settings64?"Command Settings: ON":"Command Settings: OFF");return 0;}
   bool chord=(GetKeyState(VK_CONTROL)&0x8000)&&(GetKeyState(VK_SHIFT)&0x8000);if(config64.macroEnabled){if(key==config64.macroKeys[0]||(chord&&key==VK_F10)){macro64.Play();return 0;}if(key==config64.macroKeys[1]||(chord&&key==VK_F12)){macro64.Stop();return 0;}if(key==config64.macroKeys[2]||(chord&&key==VK_F9)){macro64.Record();return 0;}if(key==config64.macroKeys[3]||(chord&&key==VK_F11)){macro64.Edit();return 0;}}}}

 if(message==WM_SYSKEYDOWN&&menu64.open&&menu64.Key(unsigned(key),unsigned((value>>16)&255),Command64)){optionMenu64.ToolToggle(unsigned(key));return 0;}
 if(optionMenu64.Key(message,unsigned(key),Allowed64()&&settings64&&config64.hideOptionMenu,typed64,GetForegroundWindow()==hwnd,(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000)||(GetKeyState(VK_MENU)&0x8000)))return 0;
 if((message==WM_KEYDOWN||message==WM_SYSKEYDOWN)&&!(value&(1LL<<30))&&config64.autoSkillsEnabled&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64){const bool ctrl=(GetKeyState(VK_CONTROL)&0x8000)!=0,alt=(GetKeyState(VK_MENU)&0x8000)!=0,shift=(GetKeyState(VK_SHIFT)&0x8000)!=0;const int index=wht::AutoSkillNumber131(unsigned(key));if(ctrl&&alt&&!shift&&index>=0){auto api=AutoApi64();wht::SelfCastSource131 source;POINT point{};RECT client{};const bool position=GetCursorPos(&point)&&ScreenToClient(window64,&point)&&GetClientRect(window64,&client);const int slot=position?api.Hover(point,client.right,client.bottom):-1;const bool bound=slot>=0&&api.ReadSource(source)&&autoSkills64.Bind(source,index,api.Data(unsigned(slot)));wht::ShowNotification131(base64,profile64->build,ui64,bound?"Auto Skills: skill bound to sequence slot "+std::to_string(index+1):"Auto Skills: select your living unit and hover a skill button");if(key<256)remap64.pressed[key]=true;return 0;}if(!ctrl&&!alt&&!shift&&key&&key==config64.autoSkillsKey){Command64(L"#autoskills");if(key<256)remap64.pressed[key]=true;return 0;}}
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&Allowed64()&&settings64&&(config64.mth49Enable||config64.g17Enable)&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!sellAll64.active&&!dropAllNetwork64.active&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){const unsigned keys[]={config64.shieldHotkey,config64.iceHotkey,config64.wildHotkey,config64.frozenHotkey,config64.phoenixHotkey};for(unsigned i=0;i<5;++i)if(key&&key==keys[i]){Command64(wht::craftCommands131[i]);if(key<256)remap64.pressed[key]=true;return 0;}}
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&(config64.mth49Enable||config64.g17Enable)&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){const unsigned keys[]={config64.chayGa1Hotkey,config64.chayGa2Hotkey,config64.chayGa3Hotkey,config64.chaySoi1Hotkey,config64.chaySoi2Hotkey,config64.chaySoi3Hotkey};const wchar_t* commands[]={L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};for(unsigned i=0;i<6;++i)if(key&&key==keys[i]){Command64(commands[i]);if(key<256)remap64.pressed[key]=true;return 0;}}
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&key&&key==config64.pickGokuHotkey&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&(config64.mth49Enable||config64.g17Enable)&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){Command64(L"#goku");if(key<256)remap64.pressed[key]=true;return 0;}
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&key&&key==config64.multiboardKey&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const auto* p=wht::MultiboardProfileFor131(profile64->build);
  if(p&&wht::ToggleMultiboard131(base64,profile64->build,wht::Component64(ui64,base64,p->table))){if(key<256)remap64.pressed[key]=true;return 0;}
 }
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const int slot=wht::HeroKeySlot131(unsigned(key),config64);const auto* p=wht::HeroProfileFor131(profile64->build);
  if(slot>=0&&p&&wht::ClickHero131(base64,profile64->build,wht::Component64(ui64,base64,p->bar),unsigned(slot))){if(key<256)remap64.pressed[key]=true;return 0;}
 }
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==window64&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)&&autoClick64.Key(unsigned(key),config64)){if(key<256)remap64.pressed[key]=true;wht::ShowNotification131(base64,profile64->build,ui64,std::string("Auto Click ")+(key==config64.autoClickLeftKey?"Left: ":"Right: ")+((key==config64.autoClickLeftKey?autoClick64.left:autoClick64.right)?"ON":"OFF"));wht::Log(folder64,std::string("WHT: Auto Click ")+(key==config64.autoClickLeftKey?"Left: ":"Right: ")+((key==config64.autoClickLeftKey?autoClick64.left:autoClick64.right)?"ON":"OFF"));return 0;}
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&key&&key==config64.sellFastKey&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==hwnd&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  if(SellHovered64()){if(key<256)remap64.pressed[key]=true;return 0;}
 }
 if(message==WM_KEYDOWN&&!(value&(1LL<<30))&&key&&key==config64.setIdTargetKey&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==hwnd&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const bool ok=PinTarget64();if(key<256)remap64.pressed[key]=true;wht::Log(folder64,ok?"WHT: selected native unit pinned for target cast.":"WHT: no live target selected; pin cleared.");return 0;
 }
 if(remap64.Input(message,unsigned(key),bool(value&(1LL<<30)),Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==hwnd,(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_MENU)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000),config64,[&](unsigned slot){
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
 if(message==WM_KEYDOWN&&!bool(value&(1LL<<30))&&key&&key<256&&Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==hwnd&&!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)){
  const auto target=[&](){const auto* p=wht::QuickCastProfile(profile64->build);return p?wht::QuickCastTarget131(base64,ui64,profile64->uiVtable,*p):uintptr_t(0);};
  if(config64.modeCast==1&&!target()){const bool shop=AlternatePanel64();bool learning=false;if(!shop&&config64.shopEnabled)for(const auto& b:config64.shop)if(b.enabled&&b.vk==key)learning=true;
   if(!learning){const unsigned mask=InventoryMask64(true);const auto found=wht::FindNativeCastSlot(config64,shop,unsigned(key),[&](bool inventory,int index){const unsigned slot=unsigned(index)+(inventory?12:0);const auto button=wht::Button64(ui64,base64,*profile64,slot);return wht::PanelButtonKey131(button,slot,inventory&&(mask&(1u<<index)));});
    if(found.slot>=0&&wht::ActivateQuickCast131(true,target,[&](){return ClickSkill64(unsigned(found.slot)+(found.inventory?12:0));},[&](uintptr_t mode){return wht::QuickCastCursor131(hwnd,mode,target);})){remap64.pressed[key]=true;return 0;}
   }
  }
 }
 if(message==WM_KEYDOWN&&wht::QuickChatKey131(unsigned(key),bool(value&(1LL<<30)),Allowed64()&&settings64&&!typed64&&!menu64.open&&!macro64.playing&&!macro64.recording&&!allSkills64.active&&!craft64.active&&!delivery64.active&&GetForegroundWindow()==hwnd,(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000)||(GetKeyState(VK_MENU)&0x8000),config64,SendChat64)){remap64.pressed[key]=true;quickChatEcho64.Sent(value,GetTickCount64());return 0;}
 if(message==WM_CHAR){wchar_t ch=wchar_t(key);if(ch==L'#'){typed64=true;chat64=L"#";}else if(typed64){if(ch==8){if(!chat64.empty())chat64.pop_back();}else if(ch>=32&&chat64.size()<1024)chat64+=ch;}}
 if(message==WM_LBUTTONDOWN||message==WM_LBUTTONUP||message==WM_LBUTTONDBLCLK){POINT p{short(LOWORD(value)),short(HIWORD(value))};RECT r{};GetClientRect(hwnd,&r);if(menu64.Mouse(message,p,r.right,r.bottom,Command64,&config64))return 0;}
 {RECT rect{};GetClientRect(hwnd,&rect);if(optionMenu64.Mouse(message,{short(LOWORD(value)),short(HIWORD(value))},rect.right,rect.bottom,Allowed64()&&settings64&&config64.hideOptionMenu,typed64,GetForegroundWindow()==hwnd))return 0;}
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
