#include "native-ping-contracts.hpp"
#include <winsock2.h>
#include "resource_board131.hpp"
#include "tool_help131.hpp"
#include "component_lookup131.hpp"
#include "match_start131.hpp"
#include "stats_labels131.hpp"
#include "life_labels131.hpp"
#include "hp_player_color131.hpp"
#include "invulnerable_bar131.hpp"
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
#include <windows.h>
#include "speed_clock129.hpp"
#include "resource131.hpp"
#include "message_clear131.hpp"
#include "hotkey_labels131.hpp"
#include "runtime131.hpp"
#include "macro_recorder.hpp"
#include "cooldown131.hpp"
#include "regen131.hpp"
#include "state_bars131.hpp"
#include <iostream>
#include <stdexcept>
void check(bool value,const char* text){if(!value)throw std::runtime_error(text);}
#ifdef _WIN64
#define RESOURCE_TEST_EDX
#else
#define RESOURCE_TEST_EDX ,void*
#endif
alignas(uintptr_t) unsigned char selfAbiMode[0x40]{};
unsigned selfAbiCancelCount=0,selfAbiSubmitCount=0;bool selfAbiOk=true;
void __fastcall SelfAbiCancel131(uintptr_t ui RESOURCE_TEST_EDX){selfAbiOk&=ui==11&&selfAbiCancelCount==selfAbiSubmitCount;++selfAbiCancelCount;*reinterpret_cast<unsigned*>(selfAbiMode+(sizeof(void*)==8?0x1c:0x10))=0;}
void __fastcall SelfAbiSubmit131(uintptr_t mode RESOURCE_TEST_EDX,unsigned order,uintptr_t auxiliary,float x,float y,uintptr_t unit,unsigned flags,unsigned extra){
 selfAbiOk&=selfAbiCancelCount==selfAbiSubmitCount+1&&mode==reinterpret_cast<uintptr_t>(selfAbiMode)&&order==0xd0012&&auxiliary==0&&x==100&&y==200&&unit==22&&flags==1&&extra==0;++selfAbiSubmitCount;
}
uintptr_t __fastcall ResourceTestPlayer(uintptr_t world RESOURCE_TEST_EDX,unsigned slot){return slot<24?reinterpret_cast<uintptr_t*>(world)[slot]:0;}
int __fastcall ResourceTestInteger(uintptr_t field RESOURCE_TEST_EDX){return *reinterpret_cast<int*>(field);}
const char* __fastcall ResourceTestString(uintptr_t RESOURCE_TEST_EDX){return "Test Player";}
unsigned priorityHideLevel=0,priorityShowLevel=0,priorityHideCount=0,priorityShowCount=0;
uintptr_t priorityVtable[64]{};alignas(uintptr_t) unsigned char priorityAllocatedFrame[0x300]{};
void __fastcall PriorityTestHide(void* frame RESOURCE_TEST_EDX){++priorityHideCount;priorityHideLevel=*reinterpret_cast<unsigned*>(static_cast<unsigned char*>(frame)+wht::nativeFrameLayout.level);}
void __fastcall PriorityTestShow(void* frame RESOURCE_TEST_EDX){++priorityShowCount;priorityShowLevel=*reinterpret_cast<unsigned*>(static_cast<unsigned char*>(frame)+wht::nativeFrameLayout.level);}
void* __stdcall PriorityTestAlloc(size_t,const char*,int,int){memset(priorityAllocatedFrame,0,sizeof(priorityAllocatedFrame));return priorityAllocatedFrame;}
void* __fastcall PriorityTestCtor(void* frame RESOURCE_TEST_EDX,void* parent){*reinterpret_cast<uintptr_t*>(frame)=reinterpret_cast<uintptr_t>(priorityVtable);*reinterpret_cast<unsigned*>(static_cast<unsigned char*>(frame)+wht::nativeFrameLayout.level)=parent?*reinterpret_cast<unsigned*>(static_cast<unsigned char*>(parent)+wht::nativeFrameLayout.level)+1:0;wht::NativeWriteVisible(frame,1);return frame;}
void __fastcall PriorityTestSize(void* RESOURCE_TEST_EDX,float){}
void __fastcall PriorityTestPoint(void* RESOURCE_TEST_EDX,int,float,float,int){}
#include "clock-fps131-contracts.hpp"
#include "quick-drop131-contracts.hpp"
#include "all-skills131-contracts.hpp"
#include "auto-pick131-contracts.hpp"
#include "goku-pick131-contracts.hpp"
#include "sell-all131-contracts.hpp"
#include "reveal131-contracts.hpp"
#include "hero-keys131-contracts.hpp"
#include "multiboard131-contracts.hpp"
#include "time-of-day131-contracts.hpp"
#include "console-ui131-contracts.hpp"
#include "black-bar131-contracts.hpp"
#include "mouse-tooltip131-contracts.hpp"
#include "invulnerable-bar131-contracts.hpp"
#include "hp-player-color131-contracts.hpp"
#include "life-labels131-contracts.hpp"
#include "stats-labels131-contracts.hpp"
#include "graphics-repair131-contracts.hpp"
#include "quick-chat131-contracts.hpp"
#include "repeat-message131-contracts.hpp"
#include "time-boss131-contracts.hpp"
#include "manual-clear131-contracts.hpp"
#include "detailed-tooltip131-contracts.hpp"
#include "view-commands131-contracts.hpp"
#include "resource-mouse131-contracts.hpp"
#include "mth-notice131-contracts.hpp"
#include "mth-route-buttons131-contracts.hpp"
#include "mth-route131-contracts.hpp"
#include "courier-route131-contracts.hpp"
#include "inventory-info131-contracts.hpp"
#include "notification131-contracts.hpp"
#include "scroll131-contracts.hpp"
#include "window131-contracts.hpp"
#include "presentation131-contracts.hpp"
#include "option-menu131-contracts.hpp"
#include "drop-commands131-contracts.hpp"
#include "auto-click-commands131-contracts.hpp"
#include "mth-craft131-contracts.hpp"
#include "courier-delivery131-contracts.hpp"
#include "hp-dimensions131-contracts.hpp"
#include "auto-skills131-contracts.hpp"
#include "menu-fog-vertex131-contracts.hpp"
#include "unit-info131-contracts.hpp"
#include "button-animation131-contracts.hpp"
#include "host-moderation131-contracts.hpp"
int main(){try{
 TestNativePingReading(check);
 CheckHostModeration131();
 CheckButtonAnimation131();
 CheckUnitInfo131();
 for(bool english:{false,true}){wht::toolEnglish.store(english);for(unsigned kind=0;kind<8;++kind){auto help=wht::HudHelp131(kind);if(help.empty()||help.find('\n')==std::string::npos||help.find('?')!=std::string::npos)throw std::runtime_error("HUD tooltip translation");}}wht::toolEnglish.store(false);

 CheckAutoSkills131();
 CheckMenuFogVertex131();
 CheckHpDimensions131();
 CheckMthCraft131();
 CheckCourierDelivery131();
 CheckGraphicsRepair131();
 CheckQuickChat131();
 CheckRepeatMessage131();
 CheckTimeBoss131();
 CheckManualClear131();
 CheckDetailedTooltip131();
 CheckViewCommands131();
 CheckResourceMouse131();
 CheckMthNotice131();
 CheckRouteButtons131();
 CheckMthRoute131();
 CheckCourierRoute131();
 CheckInventoryInfo131();
 CheckNotifications131();
 CheckScroll131();
 CheckWindow131();
 CheckPresentation131();
 CheckOptionMenu131();
 CheckDropCommands131();
 CheckAutoClickCommands131();
 CheckStatsLabels131();
 {wht::ComponentLookup131 lookup;alignas(uintptr_t) uintptr_t ui[32]{},a[2]{},b[2]{};const uintptr_t base=0x100000,table=0x1234;a[0]=b[0]=base+table;
 ui[1]=1;ui[2]=0x12345;ui[3]=~uintptr_t(0);ui[9]=reinterpret_cast<uintptr_t>(a);
 auto root=reinterpret_cast<uintptr_t>(ui);
 check(lookup.Find(root,base,table,sizeof(ui),0)==reinterpret_cast<uintptr_t>(a),"component discovery skips scalar fields without faults");
 ui[9]=reinterpret_cast<uintptr_t>(b);check(lookup.Find(root,base,table,sizeof(ui),1)==reinterpret_cast<uintptr_t>(b),"cached slot reads current object");
 ui[9]=0;check(!lookup.Find(root,base,table,sizeof(ui),2),"destroyed component rejected");ui[10]=reinterpret_cast<uintptr_t>(a);
 check(!lookup.Find(root,base,table,sizeof(ui),3)&&lookup.Find(root,base,table,sizeof(ui),252)==reinterpret_cast<uintptr_t>(a),"missing component discovery throttled then retried");
 check(!wht::ReadableComponent131(1,8)&&!wht::ReadableComponent131(~uintptr_t(0),8),"invalid memory ranges rejected");}

 {wht::MatchStart131 sound;
 check(!sound.Observe(1,false,0,true),"sound waits for verified clock");
 check(sound.Observe(1,true,0,true),"sound at match start");
 check(!sound.Observe(1,true,1,true),"sound only once");
 sound.Reset();check(!sound.Observe(1,true,0,false)&&!sound.Observe(1,true,1,true),"enable mid-match remains silent");
 check(!sound.Observe(2,true,5.01,true),"late attachment silent");
 check(sound.Observe(3,true,5,true),"new match and boundary");
 sound.Reset();check(!sound.Observe(0,true,0,true),"no UI no sound");}

 CheckLifeLabels131();
 CheckHpPlayerColor131();
 CheckInvulnerableBars131();
 CheckMouseTooltip131();
 CheckBlackBar131();
 CheckConsoleUi131();
 CheckTimeOfDay131();
 CheckMultiboard131();
 CheckHeroKeys131();
 {wht::AutoClick131 click;wht::Config c;c.autoClickLeftKey='L';c.autoClickRightKey='R';c.autoClickInterval=20;
 check(!click.Key(0,c)&&!click.Key('X',c),"only configured click toggles");
 check(click.Key('L',c)&&click.left&&!click.right,"left toggle");
 std::vector<std::pair<UINT,WPARAM>> events;auto send=[&](UINT m,WPARAM k){events.emplace_back(m,k);};
 click.Tick(c,true,100,send);check(events.size()==2&&events[0]==std::make_pair(UINT(WM_LBUTTONDOWN),WPARAM(MK_LBUTTON))&&events[1]==std::make_pair(UINT(WM_LBUTTONUP),WPARAM(0)),"paired left click");
 click.Tick(c,true,119,send);check(events.size()==2,"cadence prevents hot-loop bursts");
 click.Key('R',c);click.Tick(c,false,120,send);check(events.size()==2&&click.left&&click.right,"pause preserves toggles without input");
 click.Tick(c,true,121,send);check(events.size()==6&&events[4].first==WM_RBUTTONDOWN&&events[5].first==WM_RBUTTONUP,"resume both buttons");
 click.Key('L',c);click.Key('R',c);click.Tick(c,true,200,send);check(events.size()==6,"both off stops messages");
 c.autoClickInterval=0;click.Key('R',c);click.Tick(c,true,300,send);check(click.due==305,"lower cadence bound");
 c.autoClickInterval=2000;click.Tick(c,true,305,send);check(click.due==1305,"upper cadence bound");
 click.Reset();check(!click.left&&!click.right&&!click.due,"new match resets toggles");}


 CheckReveal131();
 CheckSellAll131();
 CheckAutoPick131();
 CheckGokuPick131();
 CheckAllSkills131();
 CheckQuickDrop131();
 CheckClockFps131();
 {
  auto savedLayout=wht::nativeFrameLayout;wht::nativeFrameLayout.level=sizeof(void*)==8?0xe4:0x90;wht::nativeFrameLayout.visible=sizeof(void*)==8?0xf0:0x9c;wht::nativeFrameLayout.layers=sizeof(void*)==8?0x114:0xc0;wht::nativeFrameLayout.hide=sizeof(void*)==8?0xd0:0x68;wht::nativeFrameLayout.show=sizeof(void*)==8?0xd8:0x6c;wht::nativeFrameLayout.byteFlags=true;
  const uintptr_t priorityBase=0x10000000;wht::nativeFrameLayout.simpleVtable=reinterpret_cast<uintptr_t>(priorityVtable)-priorityBase;
  priorityVtable[wht::nativeFrameLayout.hide/sizeof(uintptr_t)]=reinterpret_cast<uintptr_t>(PriorityTestHide);priorityVtable[wht::nativeFrameLayout.show/sizeof(uintptr_t)]=reinterpret_cast<uintptr_t>(PriorityTestShow);
  alignas(uintptr_t) unsigned char priorityFrame[0x300]{},priorityOwner[0x300]{};
  *reinterpret_cast<uintptr_t*>(priorityFrame)=reinterpret_cast<uintptr_t>(priorityVtable);*reinterpret_cast<unsigned*>(priorityFrame+wht::nativeFrameLayout.level)=8;wht::NativeWriteVisible(priorityFrame,1);
  check(wht::NativeSetOwnedFrameLevel(priorityFrame,priorityBase,wht::nativeFrameLayout.simpleVtable,wht::kToolFeatureFrameLevel)&&priorityHideLevel==8&&priorityShowLevel==6&&wht::NativeVisibleValue(priorityFrame)==1,"priority change removes old renderer bucket before registering new bucket");
  unsigned priorityCalls=priorityHideCount+priorityShowCount;check(!wht::NativeSetOwnedFrameLevel(priorityFrame,priorityBase,wht::nativeFrameLayout.simpleVtable+8,6)&&priorityHideCount+priorityShowCount==priorityCalls,"priority helper refuses unrelated native objects");
  *reinterpret_cast<unsigned*>(priorityOwner+wht::nativeFrameLayout.level)=6;
  wht::ToolMenuOverlay priorityMenu;priorityMenu.api.alloc=PriorityTestAlloc;priorityMenu.api.frameCtor=reinterpret_cast<decltype(priorityMenu.api.frameCtor)>(PriorityTestCtor);priorityMenu.api.setWidth=reinterpret_cast<decltype(priorityMenu.api.setWidth)>(PriorityTestSize);priorityMenu.api.setHeight=reinterpret_cast<decltype(priorityMenu.api.setHeight)>(PriorityTestSize);priorityMenu.api.setAbsPoint=reinterpret_cast<decltype(priorityMenu.api.setAbsPoint)>(PriorityTestPoint);
  auto lateFeature=priorityMenu.Frame(priorityBase,priorityOwner,{.2f,.2f,.1f,.1f},9);
  check(lateFeature&&*reinterpret_cast<unsigned*>(static_cast<unsigned char*>(lateFeature)+wht::nativeFrameLayout.level)<wht::kToolMenuPanelLevel,"late-created feature requesting top level stays beneath menu background");
  wht::nativeFrameLayout=savedLayout;
 }

 check(wht::ResourceEligible131(0,0,0,1,false,false),"resource board includes local player");
 check(wht::ResourceEligible131(1,1,0,2,false,true)&&wht::ResourceEligible131(2,2,0,1,true,false),"reference allied leavers and observers eligible");
 check(!wht::ResourceEligible131(1,1,0,1,false,false)&&!wht::ResourceEligible131(1,24,0,1,true,true)&&!wht::ResourceEligible131(1,1,0,0,true,true),"enemy neutral and empty slots excluded");
 for(const auto& resourceProfile:wht::resourceProfiles131){
  alignas(uintptr_t) unsigned char resourcePlayer[0x700]{};uintptr_t resourceWorld[24]{};const uintptr_t resourceBase=0x10000000;
  resourceWorld[0]=reinterpret_cast<uintptr_t>(resourcePlayer);*reinterpret_cast<uintptr_t*>(resourcePlayer)=resourceBase+resourceProfile.playerTable;
  const bool native64=sizeof(void*)==8,release131=resourceProfile.build==12164;
  // Native slot fields observed in CPlayerWar3 lifecycle/color checks, not
  // inferred by shifting the state array. Poison the adjacent race field.
  const size_t nativeStatus=native64?(release131?0x40c:0x3f4):(release131?0x2e4:0x2e8);
  const size_t nativeColor=native64?(release131?0x400:0x3e8):(release131?0x2d8:0x2dc);
  const size_t nativeRace=nativeStatus-8;
  *reinterpret_cast<unsigned*>(resourcePlayer+nativeRace)=4;
  *reinterpret_cast<unsigned*>(resourcePlayer+nativeStatus)=1;*reinterpret_cast<unsigned*>(resourcePlayer+nativeColor)=9;
  // Native CPlayer vtable slot25 reads the effective physical ID.
  const size_t nativePhysical=native64?(release131?0x51:0x45):(release131?0x2a:0x31);
  const size_t oldLogical=native64?(release131?0x50:0x44):(release131?0x28:0x30);
  resourcePlayer[oldLogical]=27;resourcePlayer[nativePhysical]=0;
  check(resourceProfile.idOffset==nativePhysical,"physical slot follows verified CPlayer getter");
  auto state=[&](unsigned index,int value){*reinterpret_cast<int*>(resourcePlayer+resourceProfile.stateBase+index*resourceProfile.stateStride)=value;};
  state(1,123450);state(2,120);state(4,10);state(5,7);state(6,25);
  wht::ResourceNative131 resourceApi{reinterpret_cast<decltype(resourceApi.player)>(ResourceTestPlayer),reinterpret_cast<decltype(resourceApi.integer)>(ResourceTestInteger),reinterpret_cast<decltype(resourceApi.string)>(ResourceTestString)};wht::ResourceRow resourceRow;char resourceName[257]{};
  check(wht::ResourceRaw131(resourceBase,reinterpret_cast<uintptr_t>(resourceWorld),resourceProfile,resourceApi,0,0,resourceRow,resourceName)&&resourceRow.gold==12345&&resourceRow.lumber==12&&resourceRow.food==7&&resourceRow.cap==10&&resourceRow.color==9&&std::string(resourceName)=="Test Player","each build decodes resource units and food cap independently");
  resourceWorld[11]=reinterpret_cast<uintptr_t>(resourcePlayer);resourcePlayer[oldLogical]=11;resourcePlayer[nativePhysical]=11;
  check(wht::ResourceRaw131(resourceBase,reinterpret_cast<uintptr_t>(resourceWorld),resourceProfile,resourceApi,11,11,resourceRow,resourceName),"real participant slot12 remains eligible");
  resourcePlayer[nativePhysical]=27;
  check(!wht::ResourceRaw131(resourceBase,reinterpret_cast<uintptr_t>(resourceWorld),resourceProfile,resourceApi,11,11,resourceRow,resourceName),"legacy slot12 remapped to neutral27 is excluded even when self");

 }

 wht::MessageTimers131 clearTimers;wht::Config clearConfig;clearConfig.clearTextEnabled=true;clearConfig.clearChatEnabled=true;clearConfig.clearTextSeconds=1;clearConfig.clearChatSeconds=3;
 unsigned textCalls=0,chatCalls=0;auto clearMock=[&](uintptr_t owner){if(owner==11)++textCalls;else if(owner==22)++chatCalls;return true;};
 check(!clearTimers.Tick(11,22,clearConfig,true,0,clearMock),"message timers arm without deleting text immediately");
 check(clearTimers.Tick(11,22,clearConfig,true,1000,clearMock)==1&&textCalls==1&&!chatCalls,"text and chat clear intervals are independent");
 check(clearTimers.Tick(11,22,clearConfig,true,3000,clearMock)==3&&textCalls==2&&chatCalls==1,"late tick clears once without replaying missed periods");
 check(!clearTimers.Tick(11,22,clearConfig,false,4000,clearMock)&&!clearTimers.Tick(11,22,clearConfig,true,9000,clearMock),"typing or disable suspends and re-arms clear timers");
 clearConfig.clearTextSeconds=17;check(!clearTimers.Tick(11,22,clearConfig,true,9500,clearMock),"period change re-arms text timer");
 clearTimers.Reset();check(!clearTimers.Tick(33,44,clearConfig,true,20000,clearMock),"new match never inherits pending clear");
 alignas(uintptr_t) unsigned char messageUi[0x7e8]{},textFrame[0x280]{},chatFrame[0x280]{};
 const uintptr_t messageBase=0x10000000;const auto& messageProfile=wht::messageProfiles131[0];
 *reinterpret_cast<uintptr_t*>(messageUi+messageProfile.textOffset)=reinterpret_cast<uintptr_t>(textFrame);*reinterpret_cast<uintptr_t*>(messageUi+messageProfile.chatOffset)=reinterpret_cast<uintptr_t>(chatFrame);
 *reinterpret_cast<uintptr_t*>(textFrame)=messageBase+messageProfile.table;*reinterpret_cast<uintptr_t*>(chatFrame)=messageBase+messageProfile.table;
 check(wht::MessageFrame131(messageBase,reinterpret_cast<uintptr_t>(messageUi),messageProfile,false)==reinterpret_cast<uintptr_t>(textFrame)&&wht::MessageFrame131(messageBase,reinterpret_cast<uintptr_t>(messageUi),messageProfile,true)==reinterpret_cast<uintptr_t>(chatFrame),"message profiles separate chat from screen text");
 *reinterpret_cast<unsigned*>(textFrame+(sizeof(void*)==8?0x1d0:0x120))=4097;check(!wht::MessageFrame131(messageBase,reinterpret_cast<uintptr_t>(messageUi),messageProfile,false),"malformed message slot count rejected");

 alignas(uintptr_t) unsigned char keyButton[0x290]{},keyData[0x2d80]{};
 *reinterpret_cast<uintptr_t*>(keyButton+(sizeof(void*)==8?0x270:0x190))=reinterpret_cast<uintptr_t>(keyData);
 *reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?12:8))=1;keyButton[sizeof(void*)==8?0xf1:0x9d]=1;
 *reinterpret_cast<int*>(keyData+(sizeof(void*)==8?0x2c50:0x2c4c))='Q';
 check(wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),0)=='Q',"native panel key uses architecture-specific data layout");
 check(wht::PanelKeyName131(wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),12,true))=="N7", "inventory panel label uses native numpad binding");
 *reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?8:4))=0;
 for(const auto key:{'M','S','H','A','P'}){
  *reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?12:8))=0xd0012;
  *reinterpret_cast<int*>(keyData+(sizeof(void*)==8?0x2c50:0x2c4c))=key;
  check(wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),0)==unsigned(key),"built-in command without ability ID retains native key");
 }
 *reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?12:8))=0;
 check(!wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),0),"empty command order has no label");
 *reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?12:8))=1;
 for(unsigned slot=12;slot<18;++slot){
  check(!wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),slot,false),"empty inventory hides hotkey despite stale visible button data");
  check(wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),slot,true)!=0,"occupied inventory retains hotkey");
  check(!wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),slot,false),"drop or sale clears previous inventory label");
 }
 keyButton[sizeof(void*)==8?0xf1:0x9d]=0;check(!wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),0),"hidden command button has no label");
 check(!wht::PanelButtonKey131(0,0)&&!wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),18),"missing and invalid button slots rejected");

 check(wht::CooldownText131(0).empty()&&wht::CooldownText131(-1).empty()&&wht::CooldownText131(86400).empty(),"inactive or invalid cooldown hidden");
 check(wht::CooldownText131(.45f)=="0.4"&&wht::CooldownText131(2.1f)=="3"&&wht::CooldownText131(60)=="1:00"&&wht::CooldownText131(65)=="1:05","cooldown display units and rounding");
 check(wht::RegenText131(false,1.25f).find("HP/s:")!=std::string::npos&&wht::RegenText131(false,1.25f).find("+1.25")!=std::string::npos,"HP rate display");
 check(wht::RegenText131(true,-2).find("MP/s:")!=std::string::npos&&wht::RegenText131(true,-2).find("-2.00")!=std::string::npos,"negative mana rate display");
 check(wht::StateText131(12345.8f,20000,true,false,true)=="|cff00ff0012,345/20,000|r","overhead state values round down and group both numbers");
 check(wht::StateText131(-5,100,false,true,false)=="|cffc3dbff0|r"&&wht::StateText131(200,100,false,false,false)=="|cff00ff00100|r","overhead values clamp to native limits");
 check(wht::StateText131(1,0,true,false,false).empty(),"units without a state pool have no label");
 check(wht::HeroLevelText131(1)=="1"&&wht::HeroLevelText131(10000)=="10000"&&wht::HeroLevelText131(0).empty()&&wht::HeroLevelText131(-1).empty()&&wht::HeroLevelText131(10001).empty(),"hero level display rejects absent or invalid levels");
 alignas(uintptr_t) unsigned char host[0x120]{},manager[0x200]{},node[12]{},bar[0x158]{},unit[0x200]{};
 *reinterpret_cast<uintptr_t*>(host+0x74)=reinterpret_cast<uintptr_t>(manager);
 *reinterpret_cast<uintptr_t*>(manager+0x18c)=reinterpret_cast<uintptr_t>(node);
 *reinterpret_cast<uintptr_t*>(node+4)=1;*reinterpret_cast<uintptr_t*>(node+8)=reinterpret_cast<uintptr_t>(bar);
 std::unordered_set<uintptr_t> registered;
 check(wht::RegisteredFrames131(host,registered)&&registered.count(reinterpret_cast<uintptr_t>(bar)),"hidden native frames remain in lifecycle snapshot");
 const auto& s=wht::stateBarProfiles131[1];const auto& regen=wht::regenProfiles131[1];const uintptr_t base=0x400000;
 *reinterpret_cast<uintptr_t*>(bar)=base+s.bar;*reinterpret_cast<unsigned*>(bar+0x134)=5;
 *reinterpret_cast<uintptr_t*>(bar+0x154)=reinterpret_cast<uintptr_t>(unit);*reinterpret_cast<uintptr_t*>(unit)=base+regen.unit;
 bar[0x9c]=1;bar[0x9d]=0;unit[0x37]='H';uintptr_t sampled=0;bool visible=false,hero=false;
 check(wht::SampleStateBar131(reinterpret_cast<uintptr_t>(bar),base,s,regen,sampled,visible,hero)&&sampled==reinterpret_cast<uintptr_t>(unit)&&!visible&&hero,"HP parent uses safe unit reference and effective visibility");
 *reinterpret_cast<unsigned*>(bar+0x134)=0;
 check(!wht::SampleStateBar131(reinterpret_cast<uintptr_t>(bar),base,s,regen,sampled,visible,hero),"mana children must not recursively create more bars");
 unsigned char frame[0x120]{};const auto savedLayout=wht::nativeFrameLayout;
 wht::nativeFrameLayout.visible=0x9c;wht::nativeFrameLayout.byteFlags=true;
 frame[0x9d]=1;frame[0x9e]=0xab;frame[0x9f]=0xcd;
 wht::NativeWriteVisible(frame,0);
 check(frame[0x9c]==0&&frame[0x9d]==1&&frame[0x9e]==0xab&&frame[0x9f]==0xcd,"hide must preserve effective visibility and adjacent flags");
 wht::NativeWriteVisible(frame,1);
 check(wht::NativeVisibleValue(frame)==1&&frame[0x9d]==1,"show must preserve native registration state");
 alignas(float) unsigned char backdrop[0x50]{};
 *reinterpret_cast<float*>(backdrop+0x38)=.125f;
 wht::NativeBackdropGeometry(backdrop,.01f,.002f);
 check(*reinterpret_cast<float*>(backdrop+0x34)==.01f&&*reinterpret_cast<float*>(backdrop+0x38)==.125f,"1.31 edge size must not overwrite background tile size");
 for(unsigned off:{0x3c,0x40,0x44,0x48})check(*reinterpret_cast<float*>(backdrop+off)==.002f,"1.31 backdrop insets ABI");
 wht::nativeFrameLayout=savedLayout;
 check(wht::FindProfile131(wht::profiles131[0].hash)->build==12164,"12164 hash dispatch");
 check(wht::FindProfile131(wht::profiles131[1].hash)->build==12173,"12173 hash dispatch");
 check(!wht::FindProfile131("A1950F17905B9CD7D5461D45E6723AF36DDA5304E12DBA27A1FEA85593B15F3F"),"1.29 rejected");
 int field=99;float value=123;
 check(wht::CameraCommand131(L"#ZOOM 3500",field,value)&&field==0&&value==3500,"zoom command");
 check(wht::CameraCommand131(L"#fogz 25000",field,value)&&field==1&&value==25000,"fog command");
 for(const auto* cmd:{L"#cam 0",L"#cam nan",L"#cam 500 extra",L"#fogz 50001",L"#cam -1000",L"#fogz inf",L"#zoom"})check(!wht::CameraCommand131(cmd,field,value),"invalid commands rejected");
 wht::Config config;config.camera=3500;config.fogZ=25000;wht::CameraOverride131 override;
 check(override.Observe(100,0,1650,true,config)==3500,"distance override");
 check(override.Observe(100,1,6000,true,config)==25000,"far clip override");
 check(override.Observe(100,4,45,true,config)==45,"other camera fields preserved");
 unsigned calls=0;override.Restore([&](uintptr_t owner,int f,float v,float duration,int flag){check(owner==100&&duration==0&&flag==1,"restore ABI");check((f==0&&v==1650)||(f==1&&v==6000),"restore latest native request");++calls;});
 check(calls==2&&!override.active,"restoration on suspend");
 check(override.Observe(200,0,1700,false,config)==1700&&override.seen==1,"new identity removes stale values");
 for(const auto& control:wht::kToolMenuControls){
  if(std::wstring(control.section)==L"MTHKEY"){const std::wstring key=control.key;check(wht::ControlAvailable131(control)==(key==L"Shield"||key==L"Ice"||key==L"Wild"||key==L"Frozen"||key==L"Phoenix"||key==L"ShieldDraw"||key==L"IceDraw"||key==L"WildDraw"||key==L"FrozenDraw"||key==L"PhoenixDraw"||key==L"Noti"||key==L"AutoGaDraw1"||key==L"AutoGaDraw2"||key==L"AutoGaDraw3"||key==L"AutoSoiDraw1"||key==L"AutoSoiDraw2"||key==L"AutoSoiDraw3"||key==L"ChayGa1"||key==L"ChayGa2"||key==L"ChayGa3"||key==L"ChaySoi1"||key==L"ChaySoi2"||key==L"ChaySoi3"||key==L"ChayGa1_count"||key==L"ChayGa2_count"||key==L"ChayGa3_count"||key==L"ChaySoi1_count"||key==L"ChaySoi2_count"||key==L"ChaySoi3_count"||key==L"TimeBoss"||key==L"MTH49Enable"||key==L"G17Enable"||key==L"PickGoku"||key==L"PickGokuDraw"),"only ported MTH controls enabled");}
  if(std::wstring(control.section)==L"MacroRecorder")check(wht::ControlAvailable131(control),"macro controls available");
 }
 std::vector<wht::MacroStep> steps;std::string error;
 check(wht::ParseMacro("WAIT 10\nMOVE 5000 5000\nKEYDOWN 81\nKEYUP 81\n",steps,error)&&steps.size()==4,"shared macro parser");
 check(!wht::ParseMacro("WAIT -1\n",steps,error),"negative macro waits rejected");
 check(!wht::ParseMacro("MOVE 10001 0\n",steps,error),"invalid macro coordinates rejected");
 check(wht::profiles131[0].cameraSet!=wht::profiles131[1].cameraSet,"profiles must not share guessed offsets");
 float multiplier=0;check(wht::ParseSpeedMultiplier129(L"",multiplier)&&multiplier==1,"speed default reset");
 check(wht::ParseSpeedMultiplier129(L"2.5",multiplier)&&multiplier==2.5f,"fractional speed");
 for(const auto* arg:{L"0",L"71",L"nan",L"2 extra"})check(!wht::ParseSpeedMultiplier129(arg,multiplier),"invalid speed rejected");
 wht::ScaledClock129 speedClock;speedClock.Set(100,2);check(speedClock.Read(150)==200,"double clock");speedClock.Set(150,1);check(speedClock.Read(200)==250,"clock remains continuous on reset");
 wht::ScaledTick129 ticks;ticks.Set(0xfffffff0u,2);check(ticks.Read(0x10u)==0x30u,"scaled DWORD wrap");
 wht::SlotRemap131 remapTest;wht::Config remapConfig;remapConfig.commandEnabled=true;remapConfig.inventoryEnabled=true;
 remapConfig.command[3]={unsigned('Q'),true};remapConfig.inventory[2]={unsigned('Q'),true};unsigned remapSlot=99,remapCalls=0;
 auto remapClick=[&](unsigned slot){remapSlot=slot;++remapCalls;return true;};
 check(remapTest.Input(WM_KEYDOWN,'Q',false,true,false,remapConfig,remapClick)&&remapSlot==14,"inventory mapping wins duplicate keys");
 check(remapTest.Input(WM_KEYDOWN,'Q',true,true,false,remapConfig,remapClick)&&remapCalls==1,"held key does not issue repeated orders");
 check(remapTest.Input(WM_KEYUP,'Q',false,false,false,remapConfig,remapClick),"release consumed after disable");
 check(!remapTest.Input(WM_KEYDOWN,'Q',false,false,false,remapConfig,remapClick)&&!remapTest.Input(WM_KEYDOWN,'Q',false,true,true,remapConfig,remapClick),"typing/menu and modifier gates");
 remapConfig.inventoryEnabled=false;check(remapTest.Input(WM_KEYDOWN,'Q',false,true,false,remapConfig,remapClick)&&remapSlot==3,"command grid maps row-major slot");remapTest.Reset();
 check(!remapTest.Input(WM_KEYDOWN,'Q',false,true,false,remapConfig,[](unsigned){return false;}),"unavailable button preserves native key");
 check(wht::RemapControl131(L"SlotHotkey",L"SlotCommand3Enabled")&&wht::RemapControl131(L"SlotHotkey",L"QuickCastCommand3")&&!wht::RemapControl131(L"SlotHotkeyShop",L"AutoTargetCommand3"),"only implemented remap controls unlocked");
 uintptr_t castMode=0;unsigned castClicks=0,castTargets=0;
 auto readCastMode=[&](){return castMode;};auto activateCast=[&](){++castClicks;castMode=123;return true;};auto confirmCast=[&](uintptr_t mode){check(mode==123,"confirm exact newly active mode");++castTargets;return true;};
 check(wht::ActivateQuickCast131(true,readCastMode,activateCast,confirmCast)&&castClicks==1&&castTargets==1,"new target mode quick casts once");
 check(wht::ActivateQuickCast131(true,readCastMode,activateCast,confirmCast)&&castTargets==1,"pre-existing target mode never auto confirms");
 castMode=0;check(wht::ActivateQuickCast131(false,readCastMode,activateCast,confirmCast)&&castTargets==1,"disabled quickcast retains normal targeting");
 castMode=0;check(wht::ActivateQuickCast131(true,readCastMode,[](){return true;},confirmCast)&&castTargets==1,"instant or passive command does not click");
 check(!wht::ActivateQuickCast131(true,readCastMode,[](){return false;},confirmCast)&&castTargets==1,"failed activation does not click");
 check(wht::QuickCastPoint131(500,300,1000,800)&&!wht::QuickCastPoint131(500,750,1000,800)&&!wht::QuickCastPoint131(-1,300,1000,800),"target clicks restricted to world viewport");
 wht::SelfCastSource131 selfSource{11,22,33,{44,55},100,200};uintptr_t selfMode=0;unsigned selfRead=0,selfClick=0,selfSend=0;
 auto selfReader=[&](wht::SelfCastSource131& out){++selfRead;out=selfSource;return true;};auto selfTarget=[&](){return selfMode;};
 auto selfActivate=[&](){++selfClick;selfMode=77;return true;};auto selfOrder=[](uintptr_t mode){return mode==77?0xd0012u:0u;};
 auto selfFinish=[&](const wht::SelfCastSource131& source,uintptr_t mode,unsigned order){check(source.unit==22&&mode==77&&order==0xd0012,"self uses saved raw unit/order/mode");++selfSend;return true;};
 check(wht::ActivateSelfCast131(selfReader,selfTarget,selfActivate,selfOrder,selfFinish)&&selfSend==1&&selfRead==2,"self native activation revalidates selection");
 check(!wht::ActivateSelfCast131(selfReader,selfTarget,selfActivate,selfOrder,selfFinish)&&selfClick==1,"self refuses an existing target cursor");
 selfMode=0;unsigned readChanged=0;auto selfChanged=[&](wht::SelfCastSource131& out){out=selfSource;if(++readChanged==2)out.id.high++;return true;};
 check(!wht::ActivateSelfCast131(selfChanged,selfTarget,selfActivate,selfOrder,selfFinish)&&selfSend==1,"self rejects reused unit identity");
 selfMode=0;check(wht::ActivateSelfCast131(selfReader,selfTarget,[](){return true;},selfOrder,selfFinish)&&selfSend==1,"instant self spells do not submit another order");
 check(wht::RemapControl131(L"SlotHotkey",L"AutoTargetCommandMe0")&&wht::RemapControl131(L"SlotHotkeyInv",L"AutoTargetCommandMe5")&&wht::RemapControl131(L"SlotHotkey",L"AutoTargetCommand0"),"self controls enabled without pinned target controls");
 wht::SelfCastApi131 selfAbiApi{reinterpret_cast<decltype(wht::SelfCastApi131::cancel)>(SelfAbiCancel131),reinterpret_cast<decltype(wht::SelfCastApi131::submit)>(SelfAbiSubmit131)};
 for(unsigned i=0;i<256;++i){*reinterpret_cast<unsigned*>(selfAbiMode+(sizeof(void*)==8?0x1c:0x10))=0xd0012;const auto savedOrder=wht::SelfCastOrder131(reinterpret_cast<uintptr_t>(selfAbiMode));check(wht::FinishSelfCast131(selfAbiApi,selfSource,reinterpret_cast<uintptr_t>(selfAbiMode),savedOrder),"native self cancel/submit ABI repeated");}
 check(selfAbiOk&&selfAbiCancelCount==256&&selfAbiSubmitCount==256,"cancel precedes saved-order submission; raw unit and float ABI preserved");
 wht::ToolMenuOverlay escapeMenu;escapeMenu.visible=true;escapeMenu.open=true;
 check(escapeMenu.Escape()&&!escapeMenu.open,"Escape closes tool menu without an active editor");
 escapeMenu.open=true;escapeMenu.editing=1;escapeMenu.editBuffer=L"unsaved";escapeMenu.pressed=1;
 check(escapeMenu.Escape()&&!escapeMenu.open&&escapeMenu.editing==-1&&escapeMenu.editBuffer.empty()&&escapeMenu.pressed==-3,"Escape closes tool menu and discards active edit");
 escapeMenu.open=true;escapeMenu.dropdown=1;check(escapeMenu.Escape()&&!escapeMenu.open&&escapeMenu.dropdown==-1,"Escape closes dropdown and menu in one press");
 check(!escapeMenu.Escape(),"Escape passes through to game when tool UI is closed");
 escapeMenu.macroOpen=true;check(escapeMenu.Escape()&&!escapeMenu.macroOpen,"Escape also closes standalone macro panel");
 wht::SelfCastSource131 pinnedSource{11,66,33,{77,88},300,400};unsigned pinnedCalls=0,pinnedReads=0;uintptr_t pinnedMode=0;
 auto pinSource=[&](wht::SelfCastSource131& out){out=selfSource;return true;};
 auto pinRead=[&](wht::SelfCastSource131& out){out=pinnedSource;out.x+=float(++pinnedReads);return true;};
 auto pinMode=[&](){return pinnedMode;};auto pinClick=[&](){pinnedMode=99;return true;};
 auto pinFinish=[&](const wht::SelfCastSource131& out,uintptr_t mode,unsigned order){check(out.unit==66&&out.x==302&&mode==99&&order==123,"pinned target uses raw destination and refreshed coordinates");++pinnedCalls;return true;};
 check(wht::ActivateTargetCast131(pinSource,pinRead,pinMode,pinClick,[](uintptr_t){return 123u;},pinFinish)&&pinnedCalls==1,"pinned cast activates caster but submits destination");
 pinnedMode=0;unsigned pinChanges=0;auto changedPin=[&](wht::SelfCastSource131& out){out=pinnedSource;if(++pinChanges==2)++out.id.high;return true;};
 check(!wht::ActivateTargetCast131(pinSource,changedPin,pinMode,pinClick,[](uintptr_t){return 123u;},pinFinish)&&pinnedCalls==1,"pinned target reused identity refused");
 pinnedMode=0;check(!wht::ActivateTargetCast131(pinSource,[](wht::SelfCastSource131&){return false;},pinMode,pinClick,[](uintptr_t){return 123u;},pinFinish),"missing/dead pinned target refused before activation");
 auto wrongSession=[&](wht::SelfCastSource131& out){out=pinnedSource;++out.session;return true;};
 check(!wht::ActivateTargetCast131(pinSource,wrongSession,pinMode,pinClick,[](uintptr_t){return 123u;},pinFinish),"pinned target cannot cross sessions");
 wht::ToolMenuOverlay toggleMenu;
 check(!toggleMenu.ToggleKey(VK_F8,VK_F9,false,false)&&!toggleMenu.open,"F8 has no fixed menu shortcut");
 check(toggleMenu.ToggleKey(VK_F9,VK_F9,false,false)&&toggleMenu.open,"configured shortcut opens menu");
 check(!toggleMenu.ToggleKey(VK_F9,VK_F9,true,false)&&toggleMenu.open,"held shortcut does not toggle repeatedly");
 check(toggleMenu.ToggleKey(VK_F9,VK_F9,false,false)&&!toggleMenu.open,"same configured shortcut closes menu");
 check(!toggleMenu.ToggleKey(VK_F8,0,false,false),"disabled shortcut has no F8 fallback");
 check(!toggleMenu.ToggleKey(VK_F9,VK_F9,false,true),"typing does not toggle menu");
 toggleMenu.open=true;toggleMenu.editing=1;check(!toggleMenu.ToggleKey(VK_F9,VK_F9,false,false)&&toggleMenu.open,"key editor can capture currently bound shortcut");
 toggleMenu.CancelEdit();check(toggleMenu.ToggleKey(VK_F8,VK_F8,false,false)&&!toggleMenu.open,"F8 works when explicitly configured");
 check(!wht::AlternatePanel131([](unsigned){return 0u;}),"empty command panel never becomes shop");
 for(const unsigned order:{0xd000fu,0xd0016u,0xd0012u,0xd0004u,0xd0019u})check(!wht::AlternatePanel131([&](unsigned slot){return slot==5?order:0xd0007u;}),"basic unit orders retain hero command bindings");
 check(wht::AlternatePanel131([](unsigned slot){return slot==11?0xd0007u:0x41303030u+slot;}),"learn/book/shop panel selects alternate bindings");
 wht::Config shopConfig;shopConfig.commandEnabled=shopConfig.shopEnabled=true;shopConfig.command[2]={unsigned('Q'),true};shopConfig.shop[2]={unsigned('W'),true};
 wht::SlotRemap131 shopInput;unsigned shopSlot=99;auto shopClick=[&](unsigned slot){shopSlot=slot;return true;};
 check(!shopInput.Input(WM_KEYDOWN,'Q',false,true,false,shopConfig,shopClick,true),"hero key does not activate shop");
 check(shopInput.Input(WM_KEYDOWN,'W',false,true,false,shopConfig,shopClick,true)&&shopSlot==2,"shop key activates exact native grid slot");shopInput.Reset();
 check(!shopInput.Input(WM_KEYDOWN,'W',false,true,false,shopConfig,shopClick,false),"shop key does not replace hero key");
 check(wht::RemappedKey131(shopConfig,2,true)=='W'&&wht::RemappedKey131(shopConfig,2,false)=='Q',"labels and input select matching binding group");
 check(wht::RemapControl131(L"SlotHotkeyShop",L"SlotCommandShop2Enabled")&&!wht::RemapControl131(L"SlotHotkeyShop",L"QuickCastCommandShop2"),"base shop controls only");
 keyButton[sizeof(void*)==8?0xf1:0x9d]=1;
 *reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?12:8))=0;*reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?8:4))=0x49303031;
 check(!wht::PanelButtonPopulated131(reinterpret_cast<uintptr_t>(keyButton),2,false,true)&&!wht::PanelButtonPopulated131(reinterpret_cast<uintptr_t>(keyButton),2,false,false),"passive records with no executable order never show remapped labels");
 *reinterpret_cast<unsigned*>(keyData+(sizeof(void*)==8?8:4))=0;check(!wht::PanelButtonPopulated131(reinterpret_cast<uintptr_t>(keyButton),2,false,true),"empty shop slot has no label");
 wht::SlotRemap131 lazyRemap;unsigned panelScans=0;auto classifyLazy=[&](){++panelScans;return false;};
 for(unsigned i=0;i<1000;++i){lazyRemap.Input(WM_MOUSEMOVE,0,false,true,false,shopConfig,shopClick,classifyLazy);lazyRemap.Input(WM_KEYDOWN,'Z',false,true,false,shopConfig,shopClick,classifyLazy);}
 check(panelScans==0,"mouse and unrelated keys never scan native command panel");
 lazyRemap.Input(WM_KEYDOWN,'Q',false,false,false,shopConfig,shopClick,classifyLazy);check(panelScans==0,"disabled input never scans native panel");
 lazyRemap.Input(WM_KEYDOWN,'Q',false,true,false,shopConfig,shopClick,classifyLazy);check(panelScans==1,"matching fresh key scans native panel once");
 wht::HotkeyLabels131 cadence;check(cadence.Due(0,false)&&!cadence.Due(1000,false)&&cadence.Due(1001,true)&&!cadence.Due(1100,true)&&cadence.Due(1151,true)&&cadence.Due(1152,false),"hotkey labels sample at150ms and clear immediately on disable");
 std::array<wht::SkillButton131,12> learnButtons{};learnButtons[3].order=wht::AllSkills131::plus;int learnPoints=2;unsigned learnedSlot=99;std::vector<unsigned> learnClicks;
 auto learnData=[&](unsigned slot){return learnButtons[slot];};auto learnRead=[&](wht::SelfCastSource131& out){out=selfSource;return true;};
 auto learnClick=[&](unsigned slot){learnClicks.push_back(slot);if(slot==3&&learnButtons[3].order==wht::AllSkills131::plus){learnButtons={};learnButtons[2]={wht::AllSkills131::heroAbility,0x41303031};learnButtons[11].order=wht::AllSkills131::back;}else if(slot==11){learnButtons={};learnButtons[3].order=wht::AllSkills131::plus;}else if(slot==2){--learnPoints;learnedSlot=slot;}return true;};
 check(wht::LearnSlot131(2,learnRead,[&](uintptr_t){return learnPoints;},learnData,learnClick)&&learnedSlot==2&&learnPoints==1&&learnClicks==std::vector<unsigned>({3,2,11}),"single learn shortcut opens plus learns exact slot and returns before render");
 learnClicks.clear();learnButtons={};learnButtons[2]={wht::AllSkills131::heroAbility,0x41303031};learnButtons[11].order=wht::AllSkills131::back;
 check(wht::LearnSlot131(2,learnRead,[&](uintptr_t){return learnPoints;},learnData,learnClick)&&learnClicks==std::vector<unsigned>({2})&&learnButtons[11].order==wht::AllSkills131::back,"manually opened learn panel stays open");
 learnClicks.clear();check(!wht::LearnSlot131(2,learnRead,[](uintptr_t){return 0;},learnData,learnClick)&&learnClicks.empty(),"zero hero points never opens learn panel");
 for(const auto& p:wht::inventoryProfiles131){alignas(uintptr_t) unsigned char passiveItem[0x100]{};*reinterpret_cast<uintptr_t*>(passiveItem)=0x10000000+p.itemTable;const auto object=reinterpret_cast<uintptr_t>(passiveItem);auto& flags=*reinterpret_cast<unsigned*>(passiveItem+(sizeof(void*)==8?0x38:0x20));
 flags=0x2000;check(!wht::InventoryItemUsable131(0x10000000,p,object),"pawnable passive item has no use hotkey");
 flags|=0x200;check(wht::InventoryItemUsable131(0x10000000,p,object),"active usable item retains use hotkey regardless current cooldown/mana");
 flags&=~0x200u;check(!wht::InventoryItemUsable131(0x10000000,p,object),"passive replacement immediately removes use capability");}
 std::cout<<"WHT131 contracts passed\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<"\n";return 1;}}
