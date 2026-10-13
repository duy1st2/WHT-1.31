#include "resource131.hpp"
#include "message_clear131.hpp"
#include "hotkey_labels131.hpp"
#include "gameplay64.hpp"
#include <iostream>
#include <stdexcept>
void check(bool value,const char* why){if(!value)throw std::runtime_error(why);}
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
static_assert(sizeof(void*)==8);
 check(wht::FindProfile64(wht::profiles64[0].hash)->build==12164&&wht::FindProfile64(wht::profiles64[1].hash)->build==12173&&!wht::FindProfile64("invalid"),"exact x64 dispatch");
 check(wht::SecondsText64(2.1f)=="3"&&wht::SecondsText64(65)=="1:05"&&wht::SecondsText64(0).empty()&&wht::SecondsText64(INFINITY).empty(),"cooldown units and inactive values");
 check(wht::StateText64(12345.8f,20000,true,false,true)=="|cff00ff0012,345/20,000|r"&&wht::StateText64(5,0,true,true,false).empty(),"state values and empty mana pools");
 alignas(uintptr_t) unsigned char host[0x1d0]{},manager[0x300]{},node[24]{},bar[0x218]{},unit[0x600]{};
 auto ptr=[](void* object,size_t offset)->uintptr_t&{return *reinterpret_cast<uintptr_t*>(static_cast<unsigned char*>(object)+offset);};
 ptr(host,0xb8)=reinterpret_cast<uintptr_t>(manager);ptr(manager,0x2b0)=reinterpret_cast<uintptr_t>(node);ptr(node,8)=1;ptr(node,16)=reinterpret_cast<uintptr_t>(bar);
 std::unordered_set<uintptr_t> frames;check(wht::Frames64(host,frames)&&frames.count(reinterpret_cast<uintptr_t>(bar)),"64-bit manager node and frame pointers");
 const auto& p=wht::profiles64[1];const uintptr_t base=0x140000000;ptr(bar,0)=base+p.statVtable;ptr(unit,0)=base+p.unitVtable;ptr(bar,0x210)=reinterpret_cast<uintptr_t>(unit);*reinterpret_cast<unsigned*>(bar+0x1e8)=5;bar[0xf0]=1;bar[0xf1]=0;
 alignas(uintptr_t) unsigned char ui[0x7e8]{},command[0x300]{},rows[3*0x20]{},buttons[12][0x290]{};
 uintptr_t columns[3][4]{};ptr(ui,0x100)=reinterpret_cast<uintptr_t>(command);ptr(command,0)=base+p.commandVtable;
 *reinterpret_cast<unsigned*>(command+0x1f8)=3;ptr(command,0x200)=reinterpret_cast<uintptr_t>(rows);
 for(unsigned row=0;row<3;++row){ptr(rows,row*0x20+8)=reinterpret_cast<uintptr_t>(columns[row]);for(unsigned col=0;col<4;++col){unsigned slot=row*4+col;ptr(buttons[slot],0)=base+p.buttonVtable;columns[row][col]=reinterpret_cast<uintptr_t>(buttons[slot]);check(wht::Button64(reinterpret_cast<uintptr_t>(ui),base,p,slot)==columns[row][col],"native x64 command grid resolves all twelve icons");}}
 *reinterpret_cast<unsigned*>(command+0x1f8)=0;check(!wht::Button64(reinterpret_cast<uintptr_t>(ui),base,p,0),"uninitialized command grid rejected");
 uintptr_t sampled=0;bool visible=true;check(wht::Hp64(reinterpret_cast<uintptr_t>(bar),base,p,sampled,visible)&&sampled==reinterpret_cast<uintptr_t>(unit)&&!visible,"x64 unit safe ref and effective visibility");
 *reinterpret_cast<unsigned*>(bar+0x1e8)=0;check(!wht::Hp64(reinterpret_cast<uintptr_t>(bar),base,p,sampled,visible),"mana child excluded from HP enumeration");
 ptr(node,8)=reinterpret_cast<uintptr_t>(node);frames.clear();check(!wht::Frames64(host,frames),"cyclic frame list rejected");
 alignas(float) unsigned char style[0x78]{};*reinterpret_cast<float*>(style+0x64)=.125f;wht::NativeBackdropGeometry(style,.01f,.002f);check(*reinterpret_cast<float*>(style+0x60)==.01f&&*reinterpret_cast<float*>(style+0x64)==.125f,"64-bit backdrop preserves background tile size");for(unsigned offset:{0x68,0x6c,0x70,0x74})check(*reinterpret_cast<float*>(style+offset)==.002f,"64-bit backdrop insets");
 std::cout<<"WHT x64 contracts passed\n";return 0;
 }catch(const std::exception& error){std::cerr<<error.what()<<"\n";return 1;}}
