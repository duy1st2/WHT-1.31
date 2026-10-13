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
int main(){try{
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
 check(wht::ResourceEligible131(1,1,0,2,false,true)&&wht::ResourceEligible131(2,2,0,1,true,false),"allied leavers and observers eligible");
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
  auto state=[&](unsigned index,int value){*reinterpret_cast<int*>(resourcePlayer+resourceProfile.stateBase+index*resourceProfile.stateStride)=value;};
  state(1,123450);state(2,120);state(4,10);state(5,7);state(6,25);
  wht::ResourceNative131 resourceApi{reinterpret_cast<decltype(resourceApi.player)>(ResourceTestPlayer),reinterpret_cast<decltype(resourceApi.integer)>(ResourceTestInteger),reinterpret_cast<decltype(resourceApi.string)>(ResourceTestString)};wht::ResourceRow resourceRow;char resourceName[257]{};
  check(wht::ResourceRaw131(resourceBase,reinterpret_cast<uintptr_t>(resourceWorld),resourceProfile,resourceApi,0,0,resourceRow,resourceName)&&resourceRow.gold==12345&&resourceRow.lumber==12&&resourceRow.food==7&&resourceRow.cap==10&&resourceRow.color==9&&std::string(resourceName)=="Test Player","each build decodes resource units and food cap independently");
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
 *reinterpret_cast<unsigned*>(keyData+8)=1;keyButton[sizeof(void*)==8?0xf1:0x9d]=1;
 *reinterpret_cast<int*>(keyData+(sizeof(void*)==8?0x2c50:0x2c4c))='Q';
 check(wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),0)=='Q',"native panel key uses architecture-specific data layout");
 check(wht::PanelKeyName131(wht::PanelButtonKey131(reinterpret_cast<uintptr_t>(keyButton),12))=="N7", "inventory panel label uses native numpad binding");
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
  if(std::wstring(control.section)==L"MTHKEY")check(!wht::ControlAvailable131(control),"unmapped gameplay controls disabled");
  if(std::wstring(control.section)==L"MacroRecorder")check(wht::ControlAvailable131(control),"macro controls available");
 }
 std::vector<wht::MacroStep> steps;std::string error;
 check(wht::ParseMacro("WAIT 10\nMOVE 5000 5000\nKEYDOWN 81\nKEYUP 81\n",steps,error)&&steps.size()==4,"shared macro parser");
 check(!wht::ParseMacro("WAIT -1\n",steps,error),"negative macro waits rejected");
 check(!wht::ParseMacro("MOVE 10001 0\n",steps,error),"invalid macro coordinates rejected");
 check(wht::profiles131[0].cameraSet!=wht::profiles131[1].cameraSet,"profiles must not share guessed offsets");
 std::cout<<"WHT131 contracts passed\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<"\n";return 1;}}
