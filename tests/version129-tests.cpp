#include "network_ping.hpp"
#include "unit_scale129.hpp"
#include "ability_button129.hpp"
#include "inventory_info129.hpp"
#include "mouse_position129.hpp"
#include "pan_camera129.hpp"
#include "unit_info129.hpp"
#include "repeat_chat129.hpp"
#include "mute129.hpp"
#include "../native/version129.cpp"
#include <iostream>
#include <stdexcept>
#include <vector>
unsigned vertexCalls129=0;BYTE vertexObservedAlpha129=0;
uintptr_t __fastcall VertexColorMock129(uintptr_t unit,DWORD){++vertexCalls129;vertexObservedAlpha129=*reinterpret_cast<BYTE*>(unit+0x4e3);return 0xbeef;}
void __fastcall KeyboardSpeedMock129(uintptr_t camera,DWORD,float value){*reinterpret_cast<float*>(camera+0x4f0)=value;}
void __fastcall MouseSpeedMock129(uintptr_t camera,DWORD,float value){*reinterpret_cast<float*>(camera+0x4ec)=value;}
uintptr_t quickUi129=0,quickTarget129=0;
void __fastcall ActivateQuick129(uintptr_t,DWORD,int){*reinterpret_cast<uintptr_t*>(quickUi129+0x1c4)=quickTarget129;}
std::map<void*,std::string> hotkeyFontTexts129;
std::vector<void*> hotkeyFontAllocations129;
void* __stdcall AllocateHotkey129(size_t size,const char*,int,int){auto value=calloc(1,size);hotkeyFontAllocations129.push_back(value);return value;}
void* __fastcall ConstructHotkey129(void* memory,DWORD,void*,int layer,int){if(layer!=2)throw std::runtime_error("Hotkey draw layer");return memory;}
void __fastcall TextHotkey129(void* font,DWORD,const char* text){hotkeyFontTexts129[font]=text?text:"";}
void __fastcall AlignHotkey129(void*,DWORD,int){}
void __fastcall PointHotkey129(void*,DWORD,int,void*,int,float,float,int){}
uint32_t __fastcall FontLevel129(void*,DWORD,const char*,float,int){return 1;}
int mockHeroLevel129=1;
int __fastcall QueryHeroLevel129(uintptr_t,DWORD){return mockHeroLevel129;}
std::map<void*,uint32_t> levelBackgroundColors129;
std::map<void*,float> levelBackgroundWidths129,levelBackgroundHeights129;
float __fastcall WidthLevel129(void* font,DWORD){const auto& text=hotkeyFontTexts129[font];return float(text.size()>12?text.size()-12:0)*.005f;}
void __fastcall ColorLevel129(void* texture,DWORD,const uint32_t* color){levelBackgroundColors129[texture]=*color;}
void __fastcall SizeWidthLevel129(void* texture,DWORD,float value){levelBackgroundWidths129[texture]=value;}
void __fastcall SizeHeightLevel129(void* texture,DWORD,float value){levelBackgroundHeights129[texture]=value;}
void __fastcall HpWidth129(void* bar,DWORD,float value){*reinterpret_cast<float*>(static_cast<char*>(bar)+0x58)=value;}
void __fastcall HpHeight129(void* bar,DWORD,float value){*reinterpret_cast<float*>(static_cast<char*>(bar)+0x5c)=value;}
float mockTimerSeconds129=0;
float* __fastcall RemainingTimer129(uintptr_t,DWORD,float* value){*value=mockTimerSeconds129;return value;}
float mockCooldown129=0;
float __fastcall QueryCooldown129(uintptr_t,DWORD){return mockCooldown129;}
std::string seen;
char buffer[512]{};
void __fastcall Capture(uintptr_t object,DWORD,const char* text){seen=text?text:"";strcpy_s(buffer,seen.c_str());*reinterpret_cast<const char**>(object+0x1f8)=buffer;}

std::map<uintptr_t,std::string> statText129;unsigned statWrites129=0;
void __fastcall CaptureStats129(uintptr_t font,DWORD,const char* text){auto& stored=statText129[font];stored=text?text:"";*reinterpret_cast<const char**>(font+0xa0)=stored.c_str();++statWrites129;}
std::string lifeWritten129;uintptr_t lifeFontWritten129=0;unsigned lifeWriteCount129=0;
void __fastcall CaptureLife129(uintptr_t font,DWORD,const char* text){lifeFontWritten129=font;lifeWritten129=text?text:"";++lifeWriteCount129;}
unsigned customPriority=0,customFlags=0;bool customOpenResult=true;
BOOL __cdecl CaptureCustomOpen(const char*,unsigned priority,unsigned flags,HANDLE* archive){customPriority=priority;customFlags=flags;*archive=customOpenResult?reinterpret_cast<HANDLE>(1):nullptr;return TRUE;}
int cameraCalls=0,chatCalls=0,fogCalls=0;
float fogEnd=0;
void __cdecl CaptureFog(uintptr_t,int parameter,float value){if(parameter!=4)throw std::runtime_error("Fog parameter changed");++fogCalls;fogEnd=value;}
float cameraDistance=0,farZ=0;
void __fastcall CaptureCamera(uintptr_t,DWORD,int field,float value,float duration,int flag){
 if(duration!=0||flag!=1)throw std::runtime_error("Camera ABI arguments changed");
 ++cameraCalls;if(field==0)cameraDistance=value;else if(field==1)farZ=value;
}
void __fastcall CaptureChat(uintptr_t,DWORD,uint32_t,const char*,int,float){++chatCalls;}
int frameShows=0,frameHides=0;
void __fastcall ShowConsole(uintptr_t frame,DWORD){if(*reinterpret_cast<unsigned*>(frame+0x94)) *reinterpret_cast<unsigned*>(frame+0x98)=1;}
void __fastcall HideConsole(uintptr_t frame,DWORD){*reinterpret_cast<unsigned*>(frame+0x98)=0;}
int clearedFrames=0;
void __fastcall CaptureClear(uintptr_t,DWORD){++clearedFrames;}
uint32_t resourceCommandPlayer129=77;
int resourceCommandValues129[7]={0,500,100,0,50,0,300};unsigned resourceCommandWrites129=0;
bool resourceCommandReject129=false;
uint32_t __cdecl ResourceCommandLocalMock129(){return resourceCommandPlayer129;}
int __cdecl ResourceCommandGetMock129(uint32_t player,int state){if(player!=resourceCommandPlayer129)throw std::runtime_error("Resource player ABI");return resourceCommandValues129[state];}
void __cdecl ResourceCommandSetMock129(uint32_t player,int state,int value){if(player!=resourceCommandPlayer129)throw std::runtime_error("Resource player ABI");++resourceCommandWrites129;if(!resourceCommandReject129)resourceCommandValues129[state]=value;}
int fpsCalls=0,buttonCalls=0;uintptr_t clicked=0;
void __fastcall CaptureFps(uintptr_t bar,DWORD,int mode){++fpsCalls;*reinterpret_cast<int*>(bar+0x148)=mode;}
void __fastcall CaptureClick(uintptr_t button,DWORD,int mode){if(mode!=1||(*reinterpret_cast<unsigned*>(button+0x13c)&2)==0)throw std::runtime_error("Click ABI");++buttonCalls;clicked=button;}
int heroClicks=0;
void __fastcall CaptureHero(uintptr_t button,DWORD,int mode){if(mode!=1)throw std::runtime_error("Hero click ABI");++heroClicks;clicked=button;}
void __fastcall MinimizeBoard(uintptr_t board,DWORD){*reinterpret_cast<unsigned*>(board+0x1c4)=1;}
void __fastcall MaximizeBoard(uintptr_t board,DWORD){*reinterpret_cast<unsigned*>(board+0x1c4)=0;}
uintptr_t learnButton=0,skillButton=0,backButton=0;unsigned* skillData=nullptr;unsigned* backData=nullptr;int learned=0;
bool consumeSkill=true;
void __fastcall CaptureLearn(uintptr_t button,DWORD,int mode){
 if(mode!=1)throw std::runtime_error("Learn callback ABI");
 if(button==learnButton){backData[2]=0xd0007;skillData[1]=0x41486572;skillData[2]=0xd0100;}
 else if(button==skillButton){++learned;if(consumeSkill)skillData[1]=skillData[2]=0;}
 else if(button==backButton)backData[2]=0;
 else throw std::runtime_error("Unexpected skill button");
}
void __fastcall ShowFdf(uintptr_t frame,DWORD){*reinterpret_cast<unsigned*>(frame+0xb8)&=~1u;}
void __fastcall HideFdf(uintptr_t frame,DWORD){*reinterpret_cast<unsigned*>(frame+0xb8)|=1;}
uint32_t __cdecl ResourcePlayer(unsigned slot){return slot<4?slot+1:0;}
uint32_t __cdecl ResourceLocal(){return 1;}
int __cdecl ResourceAlly(uint32_t,uint32_t player){return player==2;}
int __cdecl ResourceObserver(uint32_t player){return player==3;}
int __cdecl ResourceSlot(uint32_t player){return player==2?2:1;}
int __cdecl ResourceState(uint32_t,int state){return state==4?100:state==6?80:state*10;}
unsigned __cdecl ResourceColor(uint32_t){return 2;}
unsigned char resourceObjects[4][0x34]{};
uintptr_t __cdecl ResourceResolve(uint32_t player){return player&&player<=4?reinterpret_cast<uintptr_t>(resourceObjects[player-1]):0;}
uintptr_t __fastcall ManaOwnerMock129(uintptr_t object,void*){return object;}
const char* __fastcall ResourceName(uintptr_t,DWORD,int flag){if(flag)throw std::runtime_error("Allocating name path");return "|cffff0000Player|r";}
std::string sentChat;int quickChatCalls=0;uintptr_t quickChatBar=0;
void __fastcall OpenChat(uintptr_t bar,DWORD,int mode){quickChatBar=bar;*reinterpret_cast<unsigned*>(bar+0x270)=mode;}
void __fastcall SetChat(uintptr_t,DWORD,const char* text,int mode){if(mode!=1)throw std::runtime_error("Chat text ABI");sentChat=text;}
int __fastcall SubmitChat(uintptr_t,DWORD,unsigned argument){if(argument)throw std::runtime_error("Chat argument");++quickChatCalls;*reinterpret_cast<unsigned*>(quickChatBar+0x270)=0;return 1;}
void __fastcall ShowFrame(void*,DWORD){++frameShows;}
void __fastcall HideFrame(void*,DWORD){++frameHides;}
static uintptr_t regenProperty129=0;
void __fastcall CurrentStateMock129(uintptr_t property,DWORD,float* output){regenProperty129=property;*output=42.f;}
void __fastcall MaximumStateMock129(uintptr_t property,DWORD,float* output){if(property!=regenProperty129)*output=0;else *output=100.f;}
uintptr_t invulAgentMock129=0,invulUnitMock129=0;
unsigned heroSkillModifyCalls129=0;bool heroSkillAbi129=true,heroSkillReject129=false;
int heroLevelMock129=2,heroXpMock129=100;unsigned heroLevelWrites129=0;bool heroLevelReject129=false,heroLevelAbi129=true;
int __fastcall HeroLevelGetMock129(uintptr_t unit,DWORD){heroLevelAbi129&=unit==invulUnitMock129;return heroLevelMock129;}
int __fastcall HeroLevelRequiredMock129(uintptr_t unit,DWORD,int index){heroLevelAbi129&=unit==invulUnitMock129;return index*100;}
int __fastcall HeroLevelXpMock129(uintptr_t unit,DWORD){heroLevelAbi129&=unit==invulUnitMock129;return heroXpMock129;}
uint32_t* __cdecl HeroLevelRealMock129(uint32_t* out,int value){*out=uint32_t(value);return out;}
void __fastcall HeroLevelAddMock129(uintptr_t unit,DWORD,const uint32_t* real,int show,int a,int b,int c,int d){
 heroLevelAbi129&=unit==invulUnitMock129&&show==1&&a==0&&b==0&&c==0&&d==0;++heroLevelWrites129;
 if(!heroLevelReject129){heroXpMock129+=int(*real);heroLevelMock129=heroXpMock129/100+1;}
}
int __fastcall HeroLevelStripMock129(uintptr_t unit,DWORD,int amount){
 heroLevelAbi129&=unit==invulUnitMock129;++heroLevelWrites129;
 if(!heroLevelReject129){heroLevelMock129-=amount;heroXpMock129=(heroLevelMock129-1)*100;}return !heroLevelReject129;
}
int heroAttributeValues129[3]={10,20,30};unsigned heroAttributeWrites129[3]{};bool heroAttributeAbi129=true,heroAttributeReject129=false;
int __fastcall HeroStrGetMock129(uintptr_t unit,DWORD,int bonus){heroAttributeAbi129&=unit==invulUnitMock129&&bonus==1;return heroAttributeValues129[0];}
int __fastcall HeroAgiGetMock129(uintptr_t unit,DWORD,int bonus){heroAttributeAbi129&=unit==invulUnitMock129&&bonus==1;return heroAttributeValues129[1];}
int __fastcall HeroIntGetMock129(uintptr_t unit,DWORD,int bonus){heroAttributeAbi129&=unit==invulUnitMock129&&bonus==1;return heroAttributeValues129[2];}
void __fastcall HeroStrSetMock129(uintptr_t unit,DWORD,int value,int permanent){heroAttributeAbi129&=unit==invulUnitMock129&&permanent==1;++heroAttributeWrites129[0];if(!heroAttributeReject129)heroAttributeValues129[0]=value;}
void __fastcall HeroAgiSetMock129(uintptr_t unit,DWORD,int value,int permanent){heroAttributeAbi129&=unit==invulUnitMock129&&permanent==1;++heroAttributeWrites129[1];if(!heroAttributeReject129)heroAttributeValues129[1]=value;}
void __fastcall HeroIntSetMock129(uintptr_t unit,DWORD,int value,int permanent){heroAttributeAbi129&=unit==invulUnitMock129&&permanent==1;++heroAttributeWrites129[2];if(!heroAttributeReject129)heroAttributeValues129[2]=value;}
int __fastcall HeroSkillGetMock129(uintptr_t unit,DWORD){
 heroSkillAbi129&=unit==invulUnitMock129;
 const auto data=*reinterpret_cast<uintptr_t*>(unit+0x3fc);
 return *reinterpret_cast<int*>(data+0x94);
}
int __fastcall HeroSkillModifyMock129(uintptr_t unit,DWORD,int delta){
 ++heroSkillModifyCalls129;heroSkillAbi129&=unit==invulUnitMock129&&delta>0;
 if(heroSkillReject129)return 0;
 const auto data=*reinterpret_cast<uintptr_t*>(unit+0x3fc);
 *reinterpret_cast<int*>(data+0x94)+=delta;return 1;
}
bool invulPresentMock129=false,invulRejectMock129=false,invulMapMock129=false,invulAbiMock129=true;
unsigned invulWritesMock129=0;
uintptr_t __cdecl InvulAgentMock129(uint32_t first,uint32_t second){
 return first==11&&second==22?invulAgentMock129:0;
}
uintptr_t __fastcall InvulFindMock129(uintptr_t unit,DWORD,uint32_t code,int a,int b,int c,int d){
 invulAbiMock129&=unit==invulUnitMock129&&code==0x4176756c&&a==0&&b==0&&c==1&&d==1;
 return invulPresentMock129?123:0;
}
void __fastcall InvulSetMock129(uintptr_t unit,DWORD,int enabled){
 invulAbiMock129&=unit==invulUnitMock129&&(enabled==0||enabled==1);++invulWritesMock129;
 if(invulRejectMock129)return;
 invulPresentMock129=enabled!=0;
 auto& flags=*reinterpret_cast<uint32_t*>(unit+0x20);
 if(invulPresentMock129||invulMapMock129)flags|=8;else flags&=~8u;
}
unsigned teleportStops129=0,teleportMoves129=0;bool teleportAbi129=true,teleportReplace129=false;
float teleportActual129[3]{};
void __fastcall TeleportStopMock129(uintptr_t unit,DWORD,int flag){
 ++teleportStops129;teleportAbi129&=unit==invulUnitMock129&&flag==1;
 if(teleportReplace129)*reinterpret_cast<uint32_t*>(unit+0x10)=23;
}
void __fastcall TeleportMoveMock129(uintptr_t unit,DWORD,const float* x,const float* y,int a,int b,int c,int d,int e,int f,int g,int h){
 ++teleportMoves129;teleportAbi129&=unit==invulUnitMock129&&a==1&&b==1&&c==-1&&d==0&&e==0&&f==0&&g==-1&&h==0;
 // Model engine pathing adjustments; feedback must report this actual position.
 teleportActual129[0]=*x+16;teleportActual129[1]=*y-16;teleportActual129[2]=128;
}
uintptr_t __fastcall TeleportReadMock129(uintptr_t position,DWORD,float* out){
 teleportAbi129&=position==invulUnitMock129+0x26c;
 memcpy(out,teleportActual129,sizeof(teleportActual129));return reinterpret_cast<uintptr_t>(out);
}
bool removalPresent129=true,removalReject129=false,removalReplace129=false,removalReadd129=false,removalAbi129=true;
unsigned removalCalls129=0,removalRefreshes129=0;
uintptr_t __fastcall RemovalFindMock129(uintptr_t unit,DWORD,uint32_t code,int a,int b,int c,int d){
 removalAbi129&=unit==invulUnitMock129&&a==0&&b==1&&c==1&&d==1;
 return code==0x41486862&&removalPresent129?123:0;
}
void __fastcall RemovalMock129(uintptr_t unit,DWORD,uintptr_t ability){
 ++removalCalls129;removalAbi129&=unit==invulUnitMock129&&ability==123;
 if(!removalReject129)removalPresent129=false;
 if(removalReplace129)*reinterpret_cast<uint32_t*>(unit+0x10)=23;
}
void __cdecl RemovalRefreshMock129(uintptr_t unit){
 ++removalRefreshes129;removalAbi129&=unit==invulUnitMock129;
 if(removalReadd129)removalPresent129=true;
}
uintptr_t addedAbilityMock129=0;bool addPresentMock129=false,addRejectMock129=false,addStepRejectMock129=false,addAbiMock129=true;
unsigned addCallsMock129=0,addBeforeMock129=0,addAfterMock129=0,addRefreshMock129=0,addStepsMock129=0;
uintptr_t __fastcall AddFindMock129(uintptr_t unit,DWORD,uint32_t code,int a,int b,int c,int d){
 addAbiMock129&=unit==invulUnitMock129&&code==0x41486862&&a==0&&b==1&&c==1&&d==1;
 return addPresentMock129?addedAbilityMock129:0;
}
void __cdecl AddBeforeMock129(uintptr_t unit){++addBeforeMock129;addAbiMock129&=unit==invulUnitMock129;}
void __cdecl AddAfterMock129(uintptr_t unit){++addAfterMock129;addAbiMock129&=unit==invulUnitMock129;}
void __cdecl AddRefreshMock129(uintptr_t unit){++addRefreshMock129;addAbiMock129&=unit==invulUnitMock129;}
uintptr_t __cdecl AddCreateMock129(uintptr_t unit,uint32_t code,int a,int b,int c){
 ++addCallsMock129;addAbiMock129&=unit==invulUnitMock129&&code==0x41486862&&a==0&&b==0&&c==0;
 if(addRejectMock129)return 0;
 addPresentMock129=true;*reinterpret_cast<int*>(addedAbilityMock129+0x50)=0;return addedAbilityMock129;
}
int __fastcall AddMaximumMock129(uintptr_t ability,DWORD){addAbiMock129&=ability==addedAbilityMock129;return 3;}
bool AddStepMock129(uintptr_t base,uintptr_t ability,bool up){
 ++addStepsMock129;addAbiMock129&=base==executableBase&&ability==addedAbilityMock129;
 if(addStepRejectMock129)return true; // Engine accepted the call but made no progress.
 *reinterpret_cast<int*>(ability+0x50)+=up?1:-1;return true;
}
unsigned killCallsMock129=0;bool killAbiMock129=true,killReassignMock129=false;
void __fastcall KillUnitMock129(uintptr_t unit,DWORD,const float* life){
 ++killCallsMock129;killAbiMock129&=unit==invulUnitMock129&&life==reinterpret_cast<const float*>(executableBase+0xd2d88c)&&*life==0.f;
 if(killReassignMock129)*reinterpret_cast<uint32_t*>(unit+0x10)=23;
}
int wmain(int argc,wchar_t** argv){try{

 auto checkAt=[](bool ok,int line){if(!ok)throw std::runtime_error("version129 contract failed at line "+std::to_string(line));};
#define check(...) checkAt((__VA_ARGS__),__LINE__)
 {
  using namespace wht;
  ResourceCommandApi129 api{ResourceCommandLocalMock129,ResourceCommandGetMock129,ResourceCommandSetMock129};
  int amount=0;ResourceCommandResult129 result;const char* error=nullptr;
  check(ParseResourceCommand129(ResourceCommand129::Gold,L"",amount)&&amount==100000);
  check(ParseResourceCommand129(ResourceCommand129::Food,L"",amount)&&amount==200);
  check(!ParseResourceCommand129(ResourceCommand129::Gold,L"0",amount)&&!ParseResourceCommand129(ResourceCommand129::Gold,L"1 extra",amount));
  check(!ParseResourceCommand129(ResourceCommand129::Full,L"500",amount));
  check(ApplyResourceCommand129(77,api,ResourceCommand129::Gold,100,result,&error)&&result.gold==600&&result.lumber==100);
  check(ApplyResourceCommand129(77,api,ResourceCommand129::Gold,100,result,&error)&&result.gold==700);
  check(ApplyResourceCommand129(77,api,ResourceCommand129::Lumber,50,result,&error)&&result.lumber==150);
  check(ApplyResourceCommand129(77,api,ResourceCommand129::Food,400,result,&error)&&result.food==400);
  const auto writes=resourceCommandWrites129;
  check(!ApplyResourceCommand129(77,api,ResourceCommand129::Gold,INT_MAX,result,&error)&&resourceCommandWrites129==writes);
  check(!ApplyResourceCommand129(88,api,ResourceCommand129::Gold,1,result,&error)&&resourceCommandWrites129==writes);
  resourceCommandReject129=true;
  check(!ApplyResourceCommand129(77,api,ResourceCommand129::Lumber,1,result,&error)&&error);
  resourceCommandReject129=false;
  check(ApplyResourceCommand129(77,api,ResourceCommand129::Full,1,result,&error)&&result.gold==1000000&&result.lumber==1000000&&result.food==300);
  check(!ResolveResourceCommandApi129(0));
 }
 {
  using namespace wht::network_ping;
  Reset();const unsigned char request[]={0xf7,1,8,0,42,0,0,0};
  const unsigned char response[]={0xf7,0x46,8,0,42,0,0,0};
  ObserveBytes(123,reinterpret_cast<const char*>(request),3,true,100);
  ObserveBytes(123,reinterpret_cast<const char*>(request+3),5,true,100);
  check(connections.size()==1&&connections[0].pending.size()==1&&!connections[0].sampled);
  ObserveBytes(123,reinterpret_cast<const char*>(response),8,false,137);
  check(connections[0].sampled&&connections[0].rtt==37&&connections[0].pending.empty());
  Closed(123);check(connections.empty());Reset();
 }

 window=CreateWindowExW(0,L"STATIC",L"test",WS_POPUP,0,0,800,600,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
 check(window!=nullptr);original=reinterpret_cast<TextFn>(Capture);
 unsigned char object[1024]{};auto ptr=reinterpret_cast<uintptr_t>(object);
 SetText(ptr,0,"1.29.2.9231");check(seen.find("800 x 600")!=std::string::npos);check(seen.find("|cff00ff00(1.29b)|r")!=std::string::npos);
 SetWindowPos(window,nullptr,0,0,1024,768,SWP_NOMOVE|SWP_NOZORDER);
 Refresh(nullptr,0,0,0);check(seen.find("1024 x 768")!=std::string::npos);
 SetText(ptr,0,"Other menu text");Refresh(nullptr,0,0,0);check(seen=="Other menu text"&&label==0);
 if(timer)KillTimer(nullptr,timer);DestroyWindow(window);

 int field=-1;float value=0;
 check(ParseCameraCommand("#cam 3000",field,value)&&field==0&&value==3000);
 check(ParseCameraCommand("#ZOOM 500",field,value)&&field==0);
 check(ParseCameraCommand("#fogz 20000",field,value)&&field==1);
 for(auto bad:{"#cam 499","#cam 10001","#cam 3000 extra","#cam nan","#other 3000"})check(!ParseCameraCommand(bad,field,value));
 std::vector<unsigned char> globals(0xd5ea00),world(64),ui(0x500),scene(0x500),fogParameters(64),fogSlots(24),fogValue(64);
 executableBase=reinterpret_cast<uintptr_t>(globals.data());
 *reinterpret_cast<uintptr_t*>(globals.data()+0xd3b6f4)=reinterpret_cast<uintptr_t>(world.data());
 *reinterpret_cast<uintptr_t*>(globals.data()+0xd3d7f8)=reinterpret_cast<uintptr_t>(ui.data());
 *reinterpret_cast<uintptr_t*>(ui.data()+0x294)=1;
 *reinterpret_cast<uintptr_t*>(ui.data()+0x3fc)=reinterpret_cast<uintptr_t>(scene.data());
 *reinterpret_cast<uintptr_t*>(scene.data()+0x3e4)=reinterpret_cast<uintptr_t>(fogParameters.data());
 *reinterpret_cast<unsigned*>(fogParameters.data()+8)=5;
 *reinterpret_cast<uintptr_t*>(fogParameters.data()+0xc)=reinterpret_cast<uintptr_t>(fogSlots.data());
 *reinterpret_cast<uintptr_t*>(fogSlots.data()+16)=reinterpret_cast<uintptr_t>(fogValue.data());
 fogValue[0xc]=6;setFogReal=CaptureFog;
 *reinterpret_cast<uint16_t*>(world.data()+0x28)=4;
 setCamera=reinterpret_cast<CameraFn>(CaptureCamera);originalChat=reinterpret_cast<ChatFn>(CaptureChat);
 wchar_t temp[32768]{},ini[32768]{};GetTempPathW(32768,temp);GetTempFileNameW(temp,L"wht",0,ini);
 runtimeFolder=temp;runtimeIni=ini;runtimeConfig.mode=wht::RunMode::Local;runtimeConfig.camera=2800;runtimeConfig.fogZ=20000;
 ChatDisplay(1,0,5,"#cam 3300",0,10);check(cameraCalls==0&&chatCalls==1);
 ChatDisplay(1,0,4,"#cam 3300",0,10);check(cameraDistance==3300&&farZ==20000&&cameraCalls==2&&chatCalls==2);
 ChatDisplay(1,0,4,"#fogz 8000",0,10);check(cameraDistance==3300&&farZ==8000&&fogEnd==8000&&fogCalls==2&&cameraCalls==4);
 fogValue[0xd]=2;ApplyCamera();check(fogCalls==2&&cameraCalls==6);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x3fc)=0;ApplyCamera();check(fogCalls==2&&cameraCalls==8);
 runtimeConfig.mode=wht::RunMode::Disabled;ChatDisplay(1,0,4,"#cam 4000",0,10);check(cameraDistance==3300&&cameraCalls==8&&chatCalls==4);
 // The 1.29 UI has larger allocations and different visibility/vtable offsets.
 auto api=wht::FrameApi129(0x400000);
 auto canvas=wht::Canvas129(1920,1080);auto p=canvas.Project({240,0});
 check(canvas.x==0&&canvas.y==0&&canvas.width==1920&&canvas.height==1080&&p.x==240&&p.y==0);
 check(reinterpret_cast<uintptr_t>(api.frameCtor)==0x888120);
 check(api.setAbsPoint==reinterpret_cast<decltype(api.setAbsPoint)>(wht::AbsPoint129));
 wht::SetViewport129(1920,1080);check(std::abs(wht::UiX129(.797f)-.9293333f)<.0001f);
 wht::SetViewport129(1440,1080);check(std::abs(wht::UiX129(.797f)-.797f)<.0001f);
 check(wht::nativeFrameLayout.frameSize==0x128&&wht::nativeFrameLayout.fontSize==0xc8&&wht::nativeFrameLayout.textureSize==0xec);
 std::vector<uintptr_t> vtable(32);std::vector<unsigned char> frame(0x128);
 vtable[0x68/4]=reinterpret_cast<uintptr_t>(HideFrame);vtable[0x6c/4]=reinterpret_cast<uintptr_t>(ShowFrame);
 *reinterpret_cast<uintptr_t*>(frame.data())=reinterpret_cast<uintptr_t>(vtable.data());
 *reinterpret_cast<uint32_t*>(frame.data()+0x88)=3;
 *reinterpret_cast<uint32_t*>(frame.data()+0x94)=1;
 *reinterpret_cast<uint32_t*>(frame.data()+0x90)=0x12345678;
 const auto fakeBase=reinterpret_cast<uintptr_t>(vtable.data())-0xac905c;
 check(wht::NativeBringToolFrameToFront(frame.data(),fakeBase));
 check(frameHides==1&&frameShows==1&&*reinterpret_cast<uint32_t*>(frame.data()+0x88)==8);
 check(*reinterpret_cast<uint32_t*>(frame.data()+0xcc)==1&&*reinterpret_cast<uint32_t*>(frame.data()+0x90)==0x12345678);
 wht::NativeFrameHide(frame.data());check(frameHides==2&&*reinterpret_cast<uint32_t*>(frame.data()+0x94)==0);
 wht::NativeFrameShow(frame.data());check(frameShows==2&&*reinterpret_cast<uint32_t*>(frame.data()+0x94)==1);
 wht::nativeFrameLayout={};
 HANDLE customHandle=nullptr;
 check(wht::MountCustom129(0,L"custom.mpq",customHandle,CaptureCustomOpen)&&customPriority<15&&customFlags==0);
 customOpenResult=false;check(!wht::MountCustom129(0,L"custom.mpq",customHandle,CaptureCustomOpen)&&!customHandle);customOpenResult=true;
 wchar_t systemDir[32768]{};GetSystemDirectoryW(systemDir,32768);
 check(Is32BitDll129(std::wstring(systemDir)+L"\\kernel32.dll"));check(!Is32BitDll129(runtimeIni));
 const auto systemDll=std::wstring(systemDir)+L"\\kernel32.dll";
 check(LoadPlugin129(systemDll,false));const auto loadedCount=extraLibraries129.size();
 check(LoadPlugin129(systemDll,false)&&extraLibraries129.size()==loadedCount);
 check(!LoadPlugin129(runtimeIni,false)&&extraLibraries129.size()==loadedCount);
 check(!LoadPlugin129(runtimeFolder+L"\\wht-missing-plugin.dll",false));

 wht::MatchStart129 start;
 check(!start.Observe(1,false,0,true));check(start.Observe(1,true,0,true));check(!start.Observe(1,true,1,true));
 check(!start.Observe(2,true,0,false));check(!start.Observe(2,true,1,true));
 check(!start.Observe(3,true,30,true));check(!start.Observe(3,true,0,true));
 start.Observe(0,false,0,true);check(start.Observe(3,true,0,true));
 runtimeConfig.mode=wht::RunMode::Local;
 check(ToolCommand(L"#sound 1")&&runtimeConfig.soundStartGame);check(!ToolCommand(L"#sound 2"));check(ToolCommand(L"#sound 0")&&!runtimeConfig.soundStartGame);
 check(PluginPath129(L"  \"C:\\WHT files\\test.mpq\"  ")==L"C:\\WHT files\\test.mpq");
 check(PluginPath129(L"  ").empty());
 runtimeConfig.mode=wht::RunMode::Local;
 check(ToolCommand(L"#tools")&&toolMenu.open);check(!ToolCommand(L"#tools extra"));
 size_t english=0,record=0;for(size_t i=0;i<wht::kToolMenuControls.size();++i){const auto& d=wht::kToolMenuControls[i];if(!wcscmp(d.section,L"Language")&&!wcscmp(d.key,L"English"))english=i;if(!wcscmp(d.section,L"MacroRecorder")&&!wcscmp(d.key,L"Enabled"))record=i;}
 check(ToolCommand(L"#toolui "+std::to_wstring(english)+L" toggle")&&runtimeConfig.english);
 check(ToolCommand(L"#toolui 0 toggle")&&!runtimeConfig.showCooldown); // Cooldown checkbox is now ported.
 check(ToolCommand(L"#toolui "+std::to_wstring(record)+L" toggle")&&!runtimeConfig.macroEnabled);
 check(!ToolCommand(L"#macro record")&&ToolCommand(L"#macro stop"));
 std::vector<unsigned char> clockManager(0x80),scheduler(0x80),textMessages(0x190),chatMessages(0x190);
 *reinterpret_cast<uintptr_t*>(globals.data()+0xd5e9ec)=reinterpret_cast<uintptr_t>(clockManager.data());
 *reinterpret_cast<uintptr_t*>(clockManager.data()+0x44)=reinterpret_cast<uintptr_t>(scheduler.data());
 float seconds=0;*reinterpret_cast<float*>(scheduler.data()+0x40)=31.5f;
 check(wht::ReadClock129(executableBase,seconds)&&seconds==31.5f);
 // Simulation clock can stay paused or jump with speed; no wall-time fallback.
 check(wht::ReadClock129(executableBase,seconds)&&seconds==31.5f);
 *reinterpret_cast<float*>(scheduler.data()+0x40)=65.25f;check(wht::ReadClock129(executableBase,seconds)&&seconds==65.25f);
 *reinterpret_cast<float*>(scheduler.data()+0x40)=-1;check(!wht::ReadClock129(executableBase,seconds));
 *reinterpret_cast<uintptr_t*>(ui.data()+0x428)=reinterpret_cast<uintptr_t>(textMessages.data());
 *reinterpret_cast<uintptr_t*>(ui.data()+0x42c)=reinterpret_cast<uintptr_t>(chatMessages.data());
 *reinterpret_cast<uintptr_t*>(textMessages.data())=*reinterpret_cast<uintptr_t*>(chatMessages.data())=executableBase+0xac92d8;
 clearFrame=reinterpret_cast<wht::ClearFrame129>(CaptureClear);
 check(ToolCommand(L"#clear")&&ToolCommand(L"#clearc")&&clearedFrames==2);
 check(!ToolCommand(L"#clear extra"));chatTyping=true;check(!ToolCommand(L"#clear"));chatTyping=false;
 wht::MessageTimers129 timers;runtimeConfig.clearTextEnabled=runtimeConfig.clearChatEnabled=true;
 timers.Tick(executableBase,runtimeConfig,true,1000,clearFrame);
 timers.Tick(executableBase,runtimeConfig,false,4000,clearFrame);check(clearedFrames==2);
 timers.Tick(executableBase,runtimeConfig,true,5000,clearFrame);
 timers.Tick(executableBase,runtimeConfig,true,8000,clearFrame);check(clearedFrames==4);
 *reinterpret_cast<unsigned*>(textMessages.data()+0x128)=5000;check(!ToolCommand(L"#clear"));
 wht::AutoClick129 clicks;runtimeConfig.autoClickLeftKey='L';runtimeConfig.autoClickRightKey='R';runtimeConfig.autoClickInterval=20;
 std::vector<UINT> sent;auto send=[&](UINT m,WPARAM){sent.push_back(m);};
 check(clicks.Key('L',runtimeConfig));clicks.Tick(runtimeConfig,true,100,send);check(sent.size()==2&&sent[0]==WM_LBUTTONDOWN&&sent[1]==WM_LBUTTONUP);
 clicks.Tick(runtimeConfig,true,119,send);check(sent.size()==2);
 clicks.Tick(runtimeConfig,false,120,send);check(sent.size()==2);
 check(clicks.Key('R',runtimeConfig));clicks.Tick(runtimeConfig,true,200,send);check(sent.size()==6&&sent[4]==WM_RBUTTONDOWN&&sent[5]==WM_RBUTTONUP);
 clicks.Reset();clicks.Tick(runtimeConfig,true,300,send);check(sent.size()==6);
 check(ToolCommand(L"#full 1")&&runtimeConfig.borderless);check(!ToolCommand(L"#full 2"));
 check(ToolCommand(L"#lock")&&runtimeConfig.lockCursor);
 check(ToolCommand(L"#timer 0")&&!runtimeConfig.showTimer);
 std::vector<unsigned char> resource(0x180),fpsFont(0xc8),memFont(0xc8),indicator(0x1d0);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x40c)=reinterpret_cast<uintptr_t>(resource.data());
 *reinterpret_cast<uintptr_t*>(resource.data())=executableBase+0xa99b54;
 *reinterpret_cast<uintptr_t*>(resource.data()+0x14c)=reinterpret_cast<uintptr_t>(fpsFont.data());
 *reinterpret_cast<uintptr_t*>(resource.data()+0x150)=reinterpret_cast<uintptr_t>(memFont.data());
 *reinterpret_cast<uintptr_t*>(fpsFont.data())=*reinterpret_cast<uintptr_t*>(memFont.data())=executableBase+0xac8fa4;
 wht::FpsState129 fps;auto setFps=reinterpret_cast<wht::FpsMode129>(CaptureFps);
 fps.Apply(executableBase,true,setFps);fps.Apply(executableBase,true,setFps);check(fpsCalls==1);
 fps.Apply(executableBase,false,setFps);check(fpsCalls==2&&*reinterpret_cast<int*>(resource.data()+0x148)==0);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x438)=reinterpret_cast<uintptr_t>(indicator.data());
 *reinterpret_cast<uintptr_t*>(indicator.data())=executableBase+0xa9a32c;
 *reinterpret_cast<unsigned*>(indicator.data()+0xb8)=0;
 wht::TimeOfDay129 tod;auto show=reinterpret_cast<wht::ClearFrame129>(ShowFdf),hide=reinterpret_cast<wht::ClearFrame129>(HideFdf);
 tod.Apply(executableBase,true,show,hide);check(*reinterpret_cast<unsigned*>(indicator.data()+0xb8)==1);
 tod.Apply(executableBase,false,show,hide);check(*reinterpret_cast<unsigned*>(indicator.data()+0xb8)==0);
 std::vector<unsigned char> console(0x138);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x468)=reinterpret_cast<uintptr_t>(console.data());
 *reinterpret_cast<uintptr_t*>(console.data())=executableBase+0xa98b40;
 *reinterpret_cast<unsigned*>(console.data()+0x94)=1;*reinterpret_cast<unsigned*>(console.data()+0x98)=1;
 wht::ConsoleState129 consoleVisibility;
 const auto showConsoleMock=reinterpret_cast<wht::ClearFrame129>(ShowConsole),hideConsoleMock=reinterpret_cast<wht::ClearFrame129>(HideConsole);
 consoleVisibility.Apply(executableBase,true,showConsoleMock,hideConsoleMock);check(consoleVisibility.active&&*reinterpret_cast<unsigned*>(console.data()+0x94)==0&&*reinterpret_cast<unsigned*>(console.data()+0x98)==0);
 ShowConsole(reinterpret_cast<uintptr_t>(console.data()),0);check(*reinterpret_cast<unsigned*>(console.data()+0x98)==0); // Engine update must not reattach hidden console.
 consoleVisibility.Apply(executableBase,false,showConsoleMock,hideConsoleMock);check(!consoleVisibility.active&&*reinterpret_cast<unsigned*>(console.data()+0x94)==1&&*reinterpret_cast<unsigned*>(console.data()+0x98)==1);
 *reinterpret_cast<unsigned*>(console.data()+0x94)=0;
 consoleVisibility.Apply(executableBase,true,showConsoleMock,hideConsoleMock);consoleVisibility.Apply(executableBase,false,showConsoleMock,hideConsoleMock);check(*reinterpret_cast<unsigned*>(console.data()+0x94)==0);
 *reinterpret_cast<uintptr_t*>(console.data())=0;consoleVisibility.Apply(executableBase,true,showConsoleMock,hideConsoleMock);check(!consoleVisibility.active);
 check(ToolCommand(L"#console 1")&&runtimeConfig.hideConsoleUI);check(ToolCommand(L"#console 0")&&!runtimeConfig.hideConsoleUI);
 check(ToolCommand(L"#hidemenu 1")&&runtimeConfig.hideOptionMenu);check(!ToolCommand(L"#hidemenu 2"));check(ToolCommand(L"#hidemenu 0")&&!runtimeConfig.hideOptionMenu);
 check(ToolCommand(L"#showskills 0")&&!runtimeConfig.showInsAllSkills);check(ToolCommand(L"#showskills 1")&&runtimeConfig.showInsAllSkills);
 std::vector<unsigned char> inventoryPanel(0x158),inventoryBar(0x14c),commandBar(0x1d0),buttonData(0x700);
 std::vector<std::vector<unsigned char>> buttons(12,std::vector<unsigned char>(0x1bc));std::array<uintptr_t,12> itemList{};std::array<uintptr_t,12> rows{};std::array<std::array<uintptr_t,4>,3> rowButtons{};
 *reinterpret_cast<uintptr_t*>(ui.data()+0x404)=reinterpret_cast<uintptr_t>(inventoryPanel.data());
 *reinterpret_cast<uintptr_t*>(inventoryPanel.data()+0x14c)=reinterpret_cast<uintptr_t>(inventoryBar.data());
 *reinterpret_cast<uintptr_t*>(inventoryBar.data())=executableBase+0xa97474;
 *reinterpret_cast<unsigned*>(inventoryBar.data()+0x12c)=6;*reinterpret_cast<uintptr_t*>(inventoryBar.data()+0x130)=reinterpret_cast<uintptr_t>(itemList.data());
 *reinterpret_cast<uintptr_t*>(ui.data()+0x408)=reinterpret_cast<uintptr_t>(commandBar.data());
 *reinterpret_cast<uintptr_t*>(commandBar.data())=executableBase+0xa95c60;*reinterpret_cast<uintptr_t*>(commandBar.data()+0x154)=reinterpret_cast<uintptr_t>(rows.data());
 for(int i=0;i<12;++i){auto p=reinterpret_cast<uintptr_t>(buttons[i].data());*reinterpret_cast<uintptr_t*>(p)=executableBase+0xa9528c;*reinterpret_cast<uintptr_t*>(p+0x128)=1;*reinterpret_cast<uintptr_t*>(p+0x194)=reinterpret_cast<uintptr_t>(buttonData.data());rowButtons[i/4][i%4]=p;if(i<6)itemList[i*2+1]=p;}
 for(int i=0;i<3;++i)rows[i*4+1]=reinterpret_cast<uintptr_t>(rowButtons[i].data());
 for(int i=0;i<12;++i)check(wht::Button129(executableBase,false,i)==reinterpret_cast<uintptr_t>(buttons[i].data()));
 for(int i=0;i<6;++i)check(wht::Button129(executableBase,true,i)==reinterpret_cast<uintptr_t>(buttons[i].data()));
 wht::ToolMenuOverlay labelMenu129;labelMenu129.api.alloc=AllocateHotkey129;
 labelMenu129.api.fontCtor=reinterpret_cast<decltype(labelMenu129.api.fontCtor)>(ConstructHotkey129);
 labelMenu129.api.setText=reinterpret_cast<decltype(labelMenu129.api.setText)>(TextHotkey129);
 labelMenu129.api.setTextAlign=reinterpret_cast<decltype(labelMenu129.api.setTextAlign)>(AlignHotkey129);
 labelMenu129.api.setRelPoint=reinterpret_cast<decltype(labelMenu129.api.setRelPoint)>(PointHotkey129);
 labelMenu129.api.setFont=reinterpret_cast<decltype(labelMenu129.api.setFont)>(FontLevel129);
 std::vector<unsigned char> overhead129(0x15c),heroUnit129(0x168);
 const auto overheadAddress129=reinterpret_cast<uintptr_t>(overhead129.data());
 *reinterpret_cast<uintptr_t*>(overhead129.data())=executableBase+0xa95104;
 *reinterpret_cast<unsigned*>(overhead129.data()+0x13c)=5;
 *reinterpret_cast<uintptr_t*>(overhead129.data()+0x158)=reinterpret_cast<uintptr_t>(heroUnit129.data());
 heroUnit129[0x37]='H';mockHeroLevel129=42;
 check(wht::OverheadHeroLevel129(executableBase,overheadAddress129,reinterpret_cast<wht::HeroLevelObject129>(QueryHeroLevel129))==42);
 heroUnit129[0x37]='h';check(!wht::OverheadHeroLevel129(executableBase,overheadAddress129,reinterpret_cast<wht::HeroLevelObject129>(QueryHeroLevel129)));heroUnit129[0x37]='H';
 *reinterpret_cast<unsigned*>(overhead129.data()+0x13c)=0;check(!wht::OverheadHeroLevel129(executableBase,overheadAddress129,reinterpret_cast<wht::HeroLevelObject129>(QueryHeroLevel129)));*reinterpret_cast<unsigned*>(overhead129.data()+0x13c)=5;
 labelMenu129.api.textureCtor=reinterpret_cast<decltype(labelMenu129.api.textureCtor)>(ConstructHotkey129);
 labelMenu129.api.setWidth=reinterpret_cast<decltype(labelMenu129.api.setWidth)>(SizeWidthLevel129);
 labelMenu129.api.setHeight=reinterpret_cast<decltype(labelMenu129.api.setHeight)>(SizeHeightLevel129);
 wht::ShowLevel129 levels129;
 levels129.textWidth=reinterpret_cast<decltype(levels129.textWidth)>(WidthLevel129);
 levels129.backgroundColor=reinterpret_cast<decltype(levels129.backgroundColor)>(ColorLevel129);
 levels129.Apply(true,labelMenu129.api);levels129.Write(overheadAddress129,42,labelMenu129.api);
 auto levelFont129=levels129.entries.at(overheadAddress129).fonts[1];const auto levelAllocations129=hotkeyFontAllocations129.size();
 check(hotkeyFontTexts129[levelFont129]=="|cffffffff42|r");
 auto levelBackground129=levels129.entries.at(overheadAddress129).background;
 check(levelBackground129&&levelBackgroundColors129[levelBackground129]==0xc0000000);
 const auto twoDigitsWidth129=levelBackgroundWidths129[levelBackground129];
 check(std::abs(levelBackgroundHeights129[levelBackground129]-.012546875f)<.000001f);
 levels129.Write(overheadAddress129,43,labelMenu129.api);check(hotkeyFontTexts129[levelFont129]=="|cffffffff43|r"&&hotkeyFontAllocations129.size()==levelAllocations129);
 levels129.Apply(false,labelMenu129.api);check(hotkeyFontTexts129[levelFont129].empty());
 check(levelBackgroundColors129[levelBackground129]==0);
 levels129.Apply(true,labelMenu129.api);levels129.Write(overheadAddress129,10000,labelMenu129.api);check(hotkeyFontTexts129[levelFont129]=="|cffffffff9999|r"&&hotkeyFontAllocations129.size()==levelAllocations129);
 check(levelBackgroundWidths129[levelBackground129]>twoDigitsWidth129&&levelBackgroundColors129[levelBackground129]==0xc0000000);
 levels129.Write(overheadAddress129,0,labelMenu129.api);check(hotkeyFontTexts129[levelFont129].empty());
 check(levelBackgroundColors129[levelBackground129]==0);
 levels129.Forget(overheadAddress129);check(levels129.entries.empty());
 auto dimensionApi129=labelMenu129.api;
 dimensionApi129.setWidth=reinterpret_cast<decltype(dimensionApi129.setWidth)>(HpWidth129);
 dimensionApi129.setHeight=reinterpret_cast<decltype(dimensionApi129.setHeight)>(HpHeight129);
 auto& hpWidth129=*reinterpret_cast<float*>(overhead129.data()+0x58);
 auto& hpHeight129=*reinterpret_cast<float*>(overhead129.data()+0x5c);
 hpWidth129=.03f;hpHeight129=.004f;
 wht::HpDimensions129 dimensions129;dimensions129.Apply(executableBase,.06f,.008f,dimensionApi129);
 dimensions129.Update(executableBase,overheadAddress129,dimensionApi129);check(hpWidth129==.06f&&hpHeight129==.008f);
 hpWidth129=.045f;dimensions129.Update(executableBase,overheadAddress129,dimensionApi129);
 dimensions129.Apply(executableBase,0.f,0.f,dimensionApi129);check(hpWidth129==.045f&&hpHeight129==.004f);
 dimensions129.Apply(executableBase,.06f,.008f,dimensionApi129);hpHeight129=.01f;
 dimensions129.Apply(executableBase,0.f,0.f,dimensionApi129);check(hpWidth129==.045f&&hpHeight129==.01f);
 dimensions129.Forget(overheadAddress129);check(dimensions129.entries.empty());
 *reinterpret_cast<unsigned*>(overhead129.data()+0x13c)=0;
 dimensions129.Apply(executableBase,.1f,.1f,dimensionApi129);dimensions129.Update(executableBase,overheadAddress129,dimensionApi129);check(dimensions129.entries.empty()&&hpWidth129==.045f);
 *reinterpret_cast<unsigned*>(overhead129.data()+0x13c)=5;
 wht::Config labelConfig129;labelConfig129.showCooldown=false;labelConfig129.showHotkeyInPanel=true;labelConfig129.commandEnabled=true;
 labelConfig129.command[0].enabled=true;labelConfig129.command[0].vk='Q';
 *reinterpret_cast<unsigned*>(buttonData.data()+8)=0xd000f;
 wht::HotkeyLabels129 labelState129;labelState129.Update(executableBase,labelConfig129,true,labelMenu129);
 auto firstFont129=labelState129.entries[0].fonts[2];const auto allocatedLabels129=hotkeyFontAllocations129.size();
 check(firstFont129&&hotkeyFontTexts129[firstFont129]=="|cffffcc00Q|r");
 *reinterpret_cast<uintptr_t*>(buttons[0].data()+0x194)=0;
 labelState129.Update(executableBase,labelConfig129,true,labelMenu129);
 check(hotkeyFontTexts129[firstFont129].empty());
 *reinterpret_cast<uintptr_t*>(buttons[0].data()+0x194)=reinterpret_cast<uintptr_t>(buttonData.data());
 labelConfig129.command[0].vk='E';labelState129.Update(executableBase,labelConfig129,true,labelMenu129);
 check(labelState129.entries[0].fonts[2]==firstFont129&&hotkeyFontAllocations129.size()==allocatedLabels129&&hotkeyFontTexts129[firstFont129]=="|cffffcc00E|r");
 labelConfig129.showHotkeyInPanel=false;labelState129.Update(executableBase,labelConfig129,true,labelMenu129);
 check(hotkeyFontTexts129[firstFont129].empty());
 labelConfig129.showCooldown=true;mockCooldown129=12.1f;
 const auto cooldownQuery129=reinterpret_cast<wht::ButtonCooldownFn129>(QueryCooldown129);
 labelState129.Update(executableBase,labelConfig129,true,labelMenu129,cooldownQuery129);
 auto cooldownFont129=labelState129.entries[0].cooldownFonts[1];
 check(cooldownFont129&&hotkeyFontTexts129[cooldownFont129]=="|cffffffff13|r");
 mockCooldown129=.5f;labelState129.Update(executableBase,labelConfig129,true,labelMenu129,cooldownQuery129);
 check(hotkeyFontTexts129[cooldownFont129]=="|cffffffff0.5|r");
 mockCooldown129=0;labelState129.Update(executableBase,labelConfig129,true,labelMenu129,cooldownQuery129);
 check(hotkeyFontTexts129[cooldownFont129].empty());
 mockCooldown129=std::numeric_limits<float>::quiet_NaN();check(wht::ButtonCooldown129(executableBase,reinterpret_cast<uintptr_t>(buttons[0].data()),cooldownQuery129)==0);
 check(wht::CooldownText129(61.1f)=="1:02"&&wht::CooldownText129(-1).empty());
 mockCooldown129=5;labelConfig129.showCooldown=false;labelState129.Update(executableBase,labelConfig129,true,labelMenu129,cooldownQuery129);
 check(hotkeyFontTexts129[cooldownFont129].empty());
 std::vector<unsigned char> spellAbility129(0x120);std::vector<uintptr_t> spellVt129(0x300/4);
 spellVt129[0x2e0/4]=executableBase+0x74a270;
 *reinterpret_cast<uintptr_t*>(spellAbility129.data())=reinterpret_cast<uintptr_t>(spellVt129.data());
 *reinterpret_cast<unsigned*>(spellAbility129.data()+0x20)=0x200;
 *reinterpret_cast<uintptr_t*>(spellAbility129.data()+0xd4)=executableBase+0xa6e238;
 *reinterpret_cast<uintptr_t*>(spellAbility129.data()+0xe0)=1;
 *reinterpret_cast<uintptr_t*>(buttonData.data()+0x6d4)=reinterpret_cast<uintptr_t>(spellAbility129.data());
 const auto timerQuery129=reinterpret_cast<wht::TimerRemainingFn129>(RemainingTimer129);
 const auto cooldownButton129=reinterpret_cast<uintptr_t>(buttons[0].data());
 for(float seconds:{10.f,9.f,8.f,.3f,.2f,.1f,0.f}){mockTimerSeconds129=seconds;check(wht::BoundCooldown129(executableBase,cooldownButton129,timerQuery129)==seconds);}
 *reinterpret_cast<unsigned*>(spellAbility129.data()+0x20)=0;mockTimerSeconds129=10;
 check(wht::BoundCooldown129(executableBase,cooldownButton129,timerQuery129)==0);
 *reinterpret_cast<unsigned*>(spellAbility129.data()+0x20)=0x200;*reinterpret_cast<uintptr_t*>(spellAbility129.data()+0xe0)=0;
 check(wht::BoundCooldown129(executableBase,cooldownButton129,timerQuery129)==0);
 std::vector<unsigned char> sellAbility129(0x480);
 *reinterpret_cast<uintptr_t*>(sellAbility129.data())=executableBase+0xb1b5e0;
 for(unsigned offset:{0xccu,0x100u,0x1c4u,0x318u})*reinterpret_cast<unsigned*>(sellAbility129.data()+offset)=12;
 *reinterpret_cast<unsigned*>(sellAbility129.data()+0xd0)=0x48414243;
 *reinterpret_cast<unsigned*>(sellAbility129.data()+0x9c)=1;
 *reinterpret_cast<uintptr_t*>(sellAbility129.data()+0x31c)=executableBase+0xa7baf8;
 *reinterpret_cast<uintptr_t*>(sellAbility129.data()+0x328)=1;
 *reinterpret_cast<uintptr_t*>(sellAbility129.data()+0x1c8)=executableBase+0xa7baf8;
 *reinterpret_cast<uintptr_t*>(sellAbility129.data()+0x1d4)=1;
 *reinterpret_cast<unsigned*>(buttonData.data()+8)=0x48414243;
 *reinterpret_cast<uintptr_t*>(buttonData.data()+0x6d4)=reinterpret_cast<uintptr_t>(sellAbility129.data());
 mockTimerSeconds129=30;check(wht::BoundCooldown129(executableBase,cooldownButton129,timerQuery129)==30);
 *reinterpret_cast<uintptr_t*>(sellAbility129.data()+0x328)=0;mockTimerSeconds129=15;
 check(wht::BoundCooldown129(executableBase,cooldownButton129,timerQuery129)==15);
 *reinterpret_cast<float*>(sellAbility129.data()+0x108)=1;
 check(wht::BoundCooldown129(executableBase,cooldownButton129,timerQuery129)==0);
 *reinterpret_cast<unsigned*>(sellAbility129.data()+0x9c)=3;
 check(wht::BoundCooldown129(executableBase,cooldownButton129,timerQuery129)==0);
 *reinterpret_cast<uintptr_t*>(buttonData.data()+0x6d4)=0;

 for(auto allocation:hotkeyFontAllocations129)free(allocation);hotkeyFontAllocations129.clear();
 *reinterpret_cast<unsigned*>(buttonData.data()+8)=0;

 std::array<uintptr_t,16> targetMode{};targetMode[0]=executableBase+0xa927c0;
 const auto targetPtr=reinterpret_cast<uintptr_t>(targetMode.data());
 *reinterpret_cast<uintptr_t*>(ui.data()+0x250)=targetPtr;
 check(!wht::TargetMode129(executableBase));
 *reinterpret_cast<uintptr_t*>(ui.data()+0x1c4)=targetPtr;
 check(wht::TargetMode129(executableBase)==targetPtr);
 targetMode[0]=0;check(!wht::TargetMode129(executableBase));targetMode[0]=executableBase+0xa927c0;
 *reinterpret_cast<uintptr_t*>(ui.data()+0x1c4)=0;
 check(wht::QuickCastTransition129(true,0,targetPtr));
 check(!wht::QuickCastTransition129(true,targetPtr,targetPtr)&&!wht::QuickCastTransition129(true,0,0)&&!wht::QuickCastTransition129(false,0,targetPtr));
 quickUi129=reinterpret_cast<uintptr_t>(ui.data());quickTarget129=targetPtr;int castCount129=0;
 auto castMock129=[&](uintptr_t target){check(target==targetPtr);++castCount129;};
 check(wht::ActivateSlot129(executableBase,false,0,true,reinterpret_cast<wht::ClickFn129>(ActivateQuick129),castMock129)&&castCount129==1);
 check(wht::ActivateSlot129(executableBase,false,0,true,reinterpret_cast<wht::ClickFn129>(ActivateQuick129),castMock129)&&castCount129==1);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x1c4)=0;quickTarget129=0;
 check(wht::ActivateSlot129(executableBase,false,0,true,reinterpret_cast<wht::ClickFn129>(ActivateQuick129),castMock129)&&castCount129==1);
 wht::SmartCast129 pendingCast129;int releasedCasts129=0;
 auto releaseMock129=[&](uintptr_t target){check(target==targetPtr);++releasedCasts129;return true;};
 pendingCast129.Arm('Q',targetPtr,123,456);
 check(!pendingCast129.Release('W',true,targetPtr,123,456,releaseMock129)&&pendingCast129.target==targetPtr);
 check(pendingCast129.Release('Q',true,targetPtr,123,456,releaseMock129)&&releasedCasts129==1&&!pendingCast129.target);
 check(!pendingCast129.Release('Q',true,targetPtr,123,456,releaseMock129));
 pendingCast129.Arm('Q',targetPtr,123,456);
 check(!pendingCast129.Release('Q',true,targetPtr,124,456,releaseMock129)&&!pendingCast129.target);
 pendingCast129.Arm('Q',targetPtr,123,456);
 check(!pendingCast129.Release('Q',false,targetPtr,123,456,releaseMock129));
 pendingCast129.Arm('Q',targetPtr,123,456);
 check(!pendingCast129.Release('Q',true,targetPtr,123,457,releaseMock129));
 pendingCast129.Arm('Q',targetPtr,123,456);
 check(!pendingCast129.Release('Q',true,0,123,456,releaseMock129)&&releasedCasts129==1);
 check(wht::QuickCastPoint129(500,300,1600,900));
 check(!wht::QuickCastPoint129(500,30,1600,900)&&!wht::QuickCastPoint129(500,800,1600,900)&&!wht::QuickCastPoint129(-1,300,1600,900));
 check(!wht::Button129(executableBase,true,6));check(wht::IsShop129(executableBase));
 *reinterpret_cast<unsigned*>(buttonData.data()+8)=0xd000f;check(!wht::IsShop129(executableBase));
 clickButton=reinterpret_cast<wht::ClickFn129>(CaptureClick);
 check(wht::ClickButton129(executableBase,false,11,clickButton)&&buttonCalls==1&&clicked==reinterpret_cast<uintptr_t>(buttons[11].data()));
 check(*reinterpret_cast<unsigned*>(buttons[11].data()+0x13c)==0);
 runtimeConfig.commandEnabled=true;runtimeConfig.command[5].enabled=true;runtimeConfig.command[5].vk='Q';check(SlotKey129('Q')&&clicked==reinterpret_cast<uintptr_t>(buttons[5].data()));
 runtimeConfig.commandEnabled=false;check(!SlotKey129('Q'));clickButton=nullptr;
 std::vector<unsigned char> heroBar(0x188);
 std::array<uintptr_t,28> heroRows{};std::array<uintptr_t,7> heroPointers{};
 std::vector<std::vector<unsigned char>> heroButtons(7,std::vector<unsigned char>(0x208));
 const auto heroOwner=reinterpret_cast<uintptr_t>(heroBar.data());
 *reinterpret_cast<uintptr_t*>(ui.data()+0x41c)=heroOwner;
 *reinterpret_cast<uintptr_t*>(heroBar.data())=executableBase+0xa96aa0;
 *reinterpret_cast<unsigned*>(heroBar.data()+0x128)=7;*reinterpret_cast<unsigned*>(heroBar.data()+0x12c)=1;
 *reinterpret_cast<unsigned*>(heroBar.data()+0x180)=1;
 *reinterpret_cast<uintptr_t*>(heroBar.data()+0x160)=executableBase+0xa96b18;
 *reinterpret_cast<uintptr_t*>(heroBar.data()+0x154)=reinterpret_cast<uintptr_t>(heroRows.data());
 for(int slot=0;slot<7;++slot){auto button=reinterpret_cast<uintptr_t>(heroButtons[slot].data());heroPointers[slot]=button;heroRows[slot*4+1]=reinterpret_cast<uintptr_t>(&heroPointers[slot]);
  *reinterpret_cast<uintptr_t*>(button)=executableBase+0xa96a08;*reinterpret_cast<unsigned*>(button+0x94)=1;
  *reinterpret_cast<uintptr_t*>(button+0x1c8)=1;*reinterpret_cast<uintptr_t*>(button+0x128)=heroOwner+0x160;
  runtimeConfig.heroes[slot]='1'+slot;
 }
 clickButton=reinterpret_cast<wht::ClickFn129>(CaptureHero);
 for(int slot=0;slot<7;++slot)check(HeroKey129('1'+slot)&&clicked==heroPointers[slot]);
 check(heroClicks==7&&!wht::HeroButton129(executableBase,-1)&&!wht::HeroButton129(executableBase,7));
 *reinterpret_cast<unsigned*>(heroButtons[0].data()+0x94)=0;check(!HeroKey129('1'));
 *reinterpret_cast<unsigned*>(heroButtons[0].data()+0x94)=1;*reinterpret_cast<uintptr_t*>(heroButtons[0].data()+0x1c8)=0;check(!HeroKey129('1'));
 *reinterpret_cast<unsigned*>(heroBar.data()+0x180)=0;check(!HeroKey129('2'));clickButton=nullptr;
 std::vector<unsigned char> board(0x1d0);const auto boardPtr=reinterpret_cast<uintptr_t>(board.data());
 *reinterpret_cast<uintptr_t*>(ui.data()+0x3ec)=boardPtr;*reinterpret_cast<uintptr_t*>(board.data())=executableBase+0xa9af4c;
 *reinterpret_cast<uintptr_t*>(board.data()+0x174)=1;*reinterpret_cast<uintptr_t*>(board.data()+0x1b0)=1;
 auto minBoard=reinterpret_cast<wht::ClearFrame129>(MinimizeBoard),maxBoard=reinterpret_cast<wht::ClearFrame129>(MaximizeBoard);
 check(wht::ToggleMultiboard129(executableBase,minBoard,maxBoard)&&*reinterpret_cast<unsigned*>(board.data()+0x1c4)==1);
 check(wht::ToggleMultiboard129(executableBase,minBoard,maxBoard)&&*reinterpret_cast<unsigned*>(board.data()+0x1c4)==0);
 *reinterpret_cast<unsigned*>(board.data()+0x1c4)=2;check(!wht::ToggleMultiboard129(executableBase,minBoard,maxBoard));
 *reinterpret_cast<unsigned*>(board.data()+0x1c4)=0;*reinterpret_cast<uintptr_t*>(board.data()+0x174)=0;check(!wht::ToggleMultiboard129(executableBase,minBoard,maxBoard));
 *reinterpret_cast<uintptr_t*>(ui.data()+0x3fc)=reinterpret_cast<uintptr_t>(scene.data());fogValue[0xd]=0;
 CameraNative(1,0,0,1650,0,1);CameraNative(1,0,1,5000,0,1);FogNative(reinterpret_cast<uintptr_t>(fogParameters.data()),4,6000);
 runtimeConfig.camera=3500;runtimeConfig.fogZ=9000;ApplyCamera();check(cameraDistance==3500&&fogEnd==9000);
 check(ToolCommand(L"#commandsettings")&&!commandSettingsActive&&cameraDistance==1650&&farZ==5000&&fogEnd==6000);
 const int beforePaused=cameraCalls;ApplyCamera();check(cameraCalls==beforePaused);
 check(ToolCommand(L"#commandsettings")&&commandSettingsActive&&cameraDistance==3500&&fogEnd==9000);
 check(!ToolCommand(L"#commandsettings extra"));
 std::vector<unsigned char> chatBar(0x280),chatEdit(0x280),chatLabel(0x200);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x43c)=reinterpret_cast<uintptr_t>(chatBar.data());
 *reinterpret_cast<uintptr_t*>(chatBar.data())=executableBase+0xa9587c;
 *reinterpret_cast<uintptr_t*>(chatBar.data()+0x274)=reinterpret_cast<uintptr_t>(chatEdit.data());
 *reinterpret_cast<uintptr_t*>(chatBar.data()+0x278)=reinterpret_cast<uintptr_t>(chatLabel.data());
 *reinterpret_cast<uintptr_t*>(chatEdit.data())=executableBase+0xa95730;
 quickChatApi={reinterpret_cast<decltype(quickChatApi.open)>(OpenChat),reinterpret_cast<decltype(quickChatApi.text)>(SetChat),reinterpret_cast<decltype(quickChatApi.submit)>(SubmitChat)};
 const auto vietnamese=wht::Utf8(L"Ch\u00e0o b\u1ea1n");
 check(wht::SendQuickChat129(executableBase,vietnamese,quickChatApi)&&sentChat==vietnamese&&quickChatCalls==1);
 *reinterpret_cast<unsigned*>(chatBar.data()+0x270)=1;bool typing=false;check(wht::ChatOpen129(executableBase,typing)&&typing);
 check(!wht::SendQuickChat129(executableBase,"hello",quickChatApi));*reinterpret_cast<unsigned*>(chatBar.data()+0x270)=0;
 check(!wht::SendQuickChat129(executableBase,"hello\nworld",quickChatApi)&&quickChatCalls==1);
 runtimeConfig.quickChatEnabled=true;runtimeConfig.quickChat[0].vk='C';runtimeConfig.quickChat[0].text=vietnamese;
 check(ChatKey129('C')&&quickChatCalls==2);
 std::vector<unsigned char> portrait(0x274),skillUnit(0x20);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x434)=reinterpret_cast<uintptr_t>(portrait.data());
 *reinterpret_cast<uintptr_t*>(portrait.data())=executableBase+0xa981d4;
 *reinterpret_cast<uintptr_t*>(portrait.data()+0x248)=reinterpret_cast<uintptr_t>(skillUnit.data());
 *reinterpret_cast<unsigned*>(skillUnit.data()+0xc)=123;*reinterpret_cast<unsigned*>(skillUnit.data()+0x10)=456;
 std::vector<unsigned char> hpFont129(0xc8),mpFont129(0xc8);
 const auto hp129=reinterpret_cast<uintptr_t>(hpFont129.data()),mp129=reinterpret_cast<uintptr_t>(mpFont129.data());
 *reinterpret_cast<uintptr_t*>(hpFont129.data())=*reinterpret_cast<uintptr_t*>(mpFont129.data())=executableBase+0xac8fa4;
 *reinterpret_cast<uintptr_t*>(portrait.data()+0x250)=hp129;*reinterpret_cast<uintptr_t*>(portrait.data()+0x254)=mp129;
 wht::LifeMana129 life;const auto lifeSet=reinterpret_cast<wht::FontText129>(CaptureLife129);
 check(life.Handle(executableBase,hp129,"12345 / 67890",0x25a0a3,3,lifeSet)&&lifeWritten129=="12,345 / 67,890");
 const auto beforeWidthClear=lifeWriteCount129;
 check(life.Handle(executableBase,hp129,"",0x25a0d5,3,lifeSet)&&lifeWriteCount129==beforeWidthClear);
 life.Apply(executableBase,0,lifeSet);check(lifeWritten129.empty());
 life.Apply(executableBase,2,lifeSet);check(lifeWritten129=="12345 / 67890");
 check(life.Handle(executableBase,mp129,"4000 / 10000",0x25a299,1,lifeSet)&&lifeWritten129=="4,000 / 10,000"&&lifeFontWritten129==mp129);
 life.Apply(executableBase,0,lifeSet);check(lifeWritten129=="4000 / 10000");
 check(!life.Handle(executableBase,mp129,"unrelated",0x100,3,lifeSet));
 const auto beforeSelection=lifeWriteCount129;
 *reinterpret_cast<unsigned*>(skillUnit.data()+0x10)=457;life.Apply(executableBase,3,lifeSet);check(lifeWriteCount129==beforeSelection);
 *reinterpret_cast<unsigned*>(skillUnit.data()+0x10)=456;
 check(!life.Handle(executableBase,mp129,"",0x25a2cb,3,lifeSet)); // Never revive an old unit's label.
 check(ToolCommand(L"#hpmpcommas 1")&&runtimeConfig.lifeManaCommas);check(ToolCommand(L"#hpmpnohide 1")&&runtimeConfig.lifeManaNoHide);
 check(!ToolCommand(L"#hpmpnohide 2"));check(ToolCommand(L"#hpmpcommas 0")&&!runtimeConfig.lifeManaCommas);check(ToolCommand(L"#hpmpnohide 0")&&!runtimeConfig.lifeManaNoHide);
 std::vector<unsigned char> statFont129(0xc8);const auto statFont=reinterpret_cast<uintptr_t>(statFont129.data());
 *reinterpret_cast<uintptr_t*>(statFont129.data())=executableBase+0xac8fa4;
 wht::Stats129 stats;const auto statSet=reinterpret_cast<wht::FontText129>(CaptureStats129);
 check(wht::StatsKind129(0x25544a)==0&&wht::StatsKind129(0x254d69)==1&&wht::StatsKind129(0x255e48)==4&&wht::StatsKind129(0x254633)==-1);
 std::array<unsigned char,0x21c> wrapperStack{};*reinterpret_cast<uintptr_t*>(wrapperStack.data()+0x218)=executableBase+0x25544a;
 check(wht::StatsOuter129(reinterpret_cast<uintptr_t>(wrapperStack.data()))==executableBase+0x25544a);
 check(stats.Handle(executableBase,statFont,"12345 - 23456 |CFF00FF00+1000|R",0,true,statSet)&&statText129[statFont]=="12,345 - 23,456 |CFF00FF00+1,000|R");
 stats.Apply(executableBase,false,statSet);check(statText129[statFont]=="12345 - 23456 |CFF00FF00+1000|R");
 check(stats.Handle(executableBase,statFont,"12345 |CFFFF0000-1000.5|R",1,true,statSet)&&statText129[statFont]=="12,345 |CFFFF0000-1,000.5|R");
 for(int kind=2;kind<5;++kind)check(stats.Handle(executableBase,statFont,"12000",kind,true,statSet)&&statText129[statFont]=="12,000");
 CaptureStats129(statFont,0,"native changed label");const auto statBefore=statWrites129;stats.Apply(executableBase,false,statSet);check(statWrites129==statBefore);
 check(!stats.Handle(executableBase,statFont,"hero name",2,true,statSet));
 check(stats.Handle(executableBase,statFont,"12000",2,true,statSet));const auto statSelectionCount=statWrites129;
 *reinterpret_cast<unsigned*>(skillUnit.data()+0x10)=457;stats.Apply(executableBase,false,statSet);check(statWrites129==statSelectionCount);
 *reinterpret_cast<unsigned*>(skillUnit.data()+0x10)=456;
 check(ToolCommand(L"#statscommas 1")&&runtimeConfig.statsCommas);check(!ToolCommand(L"#statscommas 3"));check(ToolCommand(L"#statscommas 0")&&!runtimeConfig.statsCommas);
 std::array<std::array<unsigned,8>,12> learnData{};
 for(int slot=0;slot<12;++slot)*reinterpret_cast<uintptr_t*>(buttons[slot].data()+0x194)=reinterpret_cast<uintptr_t>(learnData[slot].data());
 learnButton=reinterpret_cast<uintptr_t>(buttons[0].data());skillButton=reinterpret_cast<uintptr_t>(buttons[1].data());backButton=reinterpret_cast<uintptr_t>(buttons[11].data());
 skillData=learnData[1].data();backData=learnData[11].data();learnData[0][1]=0;learnData[0][2]=0xd0020;
 const auto learnClick=reinterpret_cast<wht::ClickFn129>(CaptureLearn);wht::AllSkills129 learner;
 check(learner.Start(executableBase,learnClick,0)&&learner.active);
 check(learner.opening&&backData[2]==0); // No native click inside chat dispatch.
 learner.Tick(executableBase,true,learnClick,0); // Entire sequence completes before next render.
 check(!learner.active&&learned==1&&backData[2]==0);
 check(learner.Start(executableBase,learnClick,4000));
 *reinterpret_cast<unsigned*>(skillUnit.data()+0x10)=457;learner.Tick(executableBase,true,learnClick,4000);check(!learner.active&&learned==1);
 check(learner.Start(executableBase,learnClick,5000));learner.Tick(executableBase,false,learnClick,5000);check(!learner.active);
 backData[2]=0;consumeSkill=false;learned=0;check(learner.Start(executableBase,learnClick,6000));
 learner.Tick(executableBase,true,learnClick,6000);
 check(!learner.active&&learned==20&&backData[2]==0);consumeSkill=true;
 check(learner.Start(executableBase,reinterpret_cast<wht::ClickFn129>(CaptureHero),50000));
 learner.Tick(executableBase,true,reinterpret_cast<wht::ClickFn129>(CaptureHero),50000);check(!learner.active);
 learned=0;backData[2]=0;
 check(wht::LearnSlot129(executableBase,1,learnClick)&&learned==1&&backData[2]==0);
 check(wht::LearnSlot129(executableBase,2,learnClick)&&learned==1&&backData[2]==0); // Unavailable slot still closes the temporary menu.
 check(!wht::LearnSlot129(executableBase,11,learnClick));
 backData[2]=0xd0007;skillData[1]=0x41486572;skillData[2]=0xd0100;
 check(wht::LearnSlot129(executableBase,1,learnClick)&&learned==2&&backData[2]==0xd0007);
 backData[2]=0;learnData[0][2]=0;
 check(!wht::LearnSlot129(executableBase,1,learnClick));learnData[0][2]=0xd0020;
 wht::ResourceApi129 resApi{ResourcePlayer,ResourceLocal,ResourceAlly,ResourceObserver,ResourceSlot,ResourceState,ResourceColor,ResourceResolve,reinterpret_cast<decltype(wht::ResourceApi129::name)>(ResourceName)};
 wht::ResourceRow resRow;
 check(wht::ResourceRow129(resApi,0,1,resRow)&&resRow.name==L"Player"&&resRow.gold==10&&resRow.cap==80);
 check(wht::ResourceRow129(resApi,1,1,resRow)&&resRow.left);
 check(wht::ResourceRow129(resApi,2,1,resRow));
 resourceObjects[2][0x30]=27;check(!wht::ResourceRow129(resApi,2,1,resRow));resourceObjects[2][0x30]=2;
 check(!wht::ResourceRow129(resApi,3,1,resRow)&&!wht::ResourceRow129(resApi,24,1,resRow));
 check(!wht::ReadResources129(executableBase,false,resApi).ready);
 const auto heading=wht::ResourceBoard129::HeaderRect();
 const auto first=wht::ResourceBoard129::RowRect(0),second=wht::ResourceBoard129::RowRect(1);
 check(std::abs(heading.right-heading.width-.034f)<.00001f);
 check(std::abs(heading.bottom+heading.height-.578f)<.00001f);
 check(std::abs(first.bottom+first.height-heading.bottom)<.00001f);
 check(std::abs(first.bottom-second.bottom-.0165f)<.00001f);
 check(ToolCommand(L"#res 1")&&runtimeConfig.showResource);
 check(!ToolCommand(L"#res 2")&&runtimeConfig.showResource);
 check(ToolCommand(L"#resource")&&!runtimeConfig.showResource);
 wht::Config labelsConfig;
 check(wht::LabelKey129(labelsConfig,true,false,0)==VK_NUMPAD7);
 check(wht::LabelKey129(labelsConfig,false,false,0)==0);
 check(wht::LabelKey129(labelsConfig,false,false,0,'M')=='M');
 labelsConfig.commandEnabled=true;labelsConfig.command[0].enabled=true;labelsConfig.command[0].vk='Q';
 check(wht::LabelKey129(labelsConfig,false,false,0,'M')=='Q');
 check(wht::LabelKey129(labelsConfig,false,true,0)==0);
 check(wht::LabelKey129(labelsConfig,true,false,6)==0);
 check(wht::LabelName129(VK_F12)=="F12"&&wht::LabelName129(VK_NUMPAD7)=="N7");
 check(ToolCommand(L"#showhotkey 1")&&runtimeConfig.showHotkeyInPanel);
 check(!ToolCommand(L"#showhotkey 2")&&runtimeConfig.showHotkeyInPanel);
 check(ToolCommand(L"#showhotkey 0")&&!runtimeConfig.showHotkeyInPanel);
 float tooltipX129=0,tooltipY129=0;
 check(wht::TooltipPosition129({100,100},1600,900,true,1.f,tooltipX129,tooltipY129));
 check(std::abs(tooltipX129-.2675f)<.00001f);
 check(!wht::TooltipPosition129({-1,100},1600,900,true,1.f,tooltipX129,tooltipY129));
 check(!wht::TooltipPosition129({100,100},1600,900,false,1.f,tooltipX129,tooltipY129));
 std::vector<unsigned char> tooltipMock129(0x190);
 struct AnchorMock129{uintptr_t vt;wht::TooltipAnchor129 value;} tooltipAnchor129{executableBase+0xac9a2c,{reinterpret_cast<uintptr_t>(ui.data())+0xbc,8,.1f,-.2f}};
 *reinterpret_cast<uintptr_t*>(tooltipMock129.data())=executableBase+0xa94fe4;
 *reinterpret_cast<uintptr_t*>(ui.data()+0x1dc)=reinterpret_cast<uintptr_t>(tooltipMock129.data());
 auto layoutMock129=reinterpret_cast<uintptr_t*>(tooltipMock129.data());layoutMock129[1]=9;layoutMock129[10]=reinterpret_cast<uintptr_t>(&tooltipAnchor129);
 wht::MouseTooltip129 tooltipState129;int tooltipWrites129=0;
 auto tooltipSetter129=[&](uintptr_t,int,const wht::TooltipAnchor129& value){tooltipAnchor129.value=value;++tooltipWrites129;};
 auto tooltipOriginal129=tooltipAnchor129.value;
 tooltipState129.Apply(executableBase,reinterpret_cast<uintptr_t>(ui.data()),true,{100,100},1600,900,true,1.f,tooltipSetter129);
 check(tooltipState129.active&&tooltipWrites129==1);
 tooltipState129.Apply(executableBase,reinterpret_cast<uintptr_t>(ui.data()),false,{100,100},1600,900,true,1.f,tooltipSetter129);
 check(!tooltipState129.active&&tooltipAnchor129.value==tooltipOriginal129&&tooltipWrites129==2);
 check(ToolCommand(L"#mousetooltip 1")&&runtimeConfig.mouseTooltip);
 check(!ToolCommand(L"#mousetooltip 2"));
 check(ToolCommand(L"#mousetooltip 0")&&!runtimeConfig.mouseTooltip);
 std::vector<unsigned char> viewportMock129(0x100);
 auto viewportLayout129=reinterpret_cast<uintptr_t*>(viewportMock129.data()+0xbc);viewportLayout129[1]=9;
 AnchorMock129 bottomAnchor129{executableBase+0xac9a2c,{reinterpret_cast<uintptr_t>(ui.data())+0xbc,6,0,.13f}};
 AnchorMock129 topAnchor129{executableBase+0xac9a2c,{reinterpret_cast<uintptr_t>(ui.data())+0xbc,2,0,-.02f}};
 viewportLayout129[8]=reinterpret_cast<uintptr_t>(&bottomAnchor129);viewportLayout129[4]=reinterpret_cast<uintptr_t>(&topAnchor129);
 *reinterpret_cast<uintptr_t*>(ui.data()+0x3fc)=reinterpret_cast<uintptr_t>(viewportMock129.data());
 wht::BlackBar129 blackState129;int blackWrites129=0;
 auto blackSetter129=[&](uintptr_t,int point,const wht::TooltipAnchor129& value){(point==6?bottomAnchor129:topAnchor129).value=value;++blackWrites129;};
 blackState129.Apply(executableBase,reinterpret_cast<uintptr_t>(ui.data()),true,blackSetter129);
 check(blackState129.active&&bottomAnchor129.value.y==0&&blackWrites129==1);
 blackState129.Apply(executableBase,reinterpret_cast<uintptr_t>(ui.data()),false,blackSetter129);
 check(!blackState129.active&&bottomAnchor129.value.y==.13f&&blackWrites129==2);
 viewportLayout129[4]=0;
 blackState129.Apply(executableBase,reinterpret_cast<uintptr_t>(ui.data()),true,blackSetter129);
 check(!blackState129.active&&blackWrites129==2);
 check(ToolCommand(L"#blackbar 1")&&runtimeConfig.hideBlackBar);
 check(!ToolCommand(L"#blackbar 2"));
 check(ToolCommand(L"#blackbar 0")&&!runtimeConfig.hideBlackBar);
 std::vector<unsigned char> vertexUnitMock129(0x4e4);
 const auto vertexUnitAddress129=reinterpret_cast<uintptr_t>(vertexUnitMock129.data());
 *reinterpret_cast<uintptr_t*>(vertexUnitAddress129)=executableBase+0xa9c504;
 auto& vertexPackedColor129=*reinterpret_cast<uint32_t*>(vertexUnitAddress129+0x4e0);
 vertexPackedColor129=0x44123456;
 {wht::ScopedVertexAlpha129 alpha(executableBase,vertexUnitAddress129,true);check(vertexPackedColor129==0xff123456);}
 check(vertexPackedColor129==0x44123456);
 {wht::ScopedVertexAlpha129 alpha(executableBase,vertexUnitAddress129,false);check(vertexPackedColor129==0x44123456);}
 {wht::ScopedVertexAlpha129 alpha(executableBase,vertexUnitAddress129,true);vertexPackedColor129=0x22123456;}
 check(vertexPackedColor129==0x22123456); // Preserve a newer native writer.
 *reinterpret_cast<uintptr_t*>(vertexUnitAddress129)=0;
 {wht::ScopedVertexAlpha129 alpha(executableBase,vertexUnitAddress129,true);check(vertexPackedColor129==0x22123456);}
 *reinterpret_cast<uintptr_t*>(vertexUnitAddress129)=executableBase+0xa9c504;
 originalVertexColor129=reinterpret_cast<VertexColorFn129>(VertexColorMock129);
 check(ToolCommand(L"#vertexcolor 1")&&runtimeConfig.vertexColor);
 check(VertexColorNative129(vertexUnitAddress129,0)==0xbeef&&vertexObservedAlpha129==255&&vertexPackedColor129==0x22123456&&vertexCalls129==1);
 check(!ToolCommand(L"#vertexcolor 2")&&!ToolCommand(L"#vertexcolor 1 extra"));
 check(ToolCommand(L"#vertexcolor 0")&&!runtimeConfig.vertexColor);
 check(VertexColorNative129(vertexUnitAddress129,0)==0xbeef&&vertexObservedAlpha129==0x22&&vertexCalls129==2);
 originalVertexColor129=nullptr;
 std::vector<unsigned char> scrollCameraMock129(0x500);
 const auto scrollCameraAddress129=reinterpret_cast<uintptr_t>(scrollCameraMock129.data());
 auto& keyboardSpeed129=*reinterpret_cast<float*>(scrollCameraAddress129+0x4f0);
 auto& mouseSpeed129=*reinterpret_cast<float*>(scrollCameraAddress129+0x4ec);
 keyboardSpeed129=.75f;mouseSpeed129=.5f;
 wht::Scroll129 keyboardLease129,mouseLease129;
 const auto keyboardSetter129=reinterpret_cast<wht::ScrollSetter129>(KeyboardSpeedMock129);
 const auto mouseSetter129=reinterpret_cast<wht::ScrollSetter129>(MouseSpeedMock129);
 check(keyboardLease129.Apply(scrollCameraAddress129,0x4f0,200,true,keyboardSetter129)&&keyboardSpeed129==2.f&&mouseSpeed129==.5f);
 check(mouseLease129.Apply(scrollCameraAddress129,0x4ec,150,true,mouseSetter129)&&mouseSpeed129==1.5f);
 keyboardSpeed129=.8f;
 check(keyboardLease129.Apply(scrollCameraAddress129,0x4f0,200,true,keyboardSetter129));
 check(keyboardLease129.Apply(scrollCameraAddress129,0x4f0,200,false,keyboardSetter129)&&keyboardSpeed129==.8f);
 mouseSpeed129=.9f;
 check(mouseLease129.Apply(scrollCameraAddress129,0x4ec,150,false,mouseSetter129)&&mouseSpeed129==.9f);
 check(mouseLease129.Apply(scrollCameraAddress129,0x4ec,0,true,mouseSetter129)&&mouseSpeed129==1.f);
 check(!mouseLease129.Apply(scrollCameraAddress129,0x4ec,701,true,mouseSetter129));
 check(mouseLease129.Apply(0,0x4ec,100,true,mouseSetter129)&&!mouseLease129.active);
 std::vector<unsigned char> fogSceneMock129(0x3e8),fogParamsMock129(0x10);
 std::array<uintptr_t,5> fogFieldsMock129{1,2,3,4,5};
 *reinterpret_cast<uintptr_t*>(fogSceneMock129.data()+0x3e4)=reinterpret_cast<uintptr_t>(fogParamsMock129.data());
 *reinterpret_cast<unsigned*>(fogParamsMock129.data()+8)=5;
 *reinterpret_cast<uintptr_t*>(fogParamsMock129.data()+0xc)=reinterpret_cast<uintptr_t>(fogFieldsMock129.data());
 wht::FogEffect129 fogState129;uint32_t fogTint129=0xff123456;
 const auto fogSceneAddress129=reinterpret_cast<uintptr_t>(fogSceneMock129.data());
 check(fogState129.Capture(fogSceneAddress129,2,100.f,2000.f,.5f,&fogTint129));
 fogTint129=0;unsigned fogWrites129=0,fogStyleWritten129=0;float fogEndWritten129=0;uint32_t fogColorWritten129=0;
 auto fogWriter129=[&](uintptr_t scene,unsigned mode,float start,float end,float density,const uint32_t* color){check(scene==fogSceneAddress129);++fogWrites129;fogStyleWritten129=mode;fogEndWritten129=end;fogColorWritten129=*color;if(mode==1)check(start==0&&end==0&&density==0);};
 check(fogState129.Apply(fogSceneAddress129,true,fogWriter129)&&fogState129.active);
 check(fogWrites129==1&&fogStyleWritten129==1&&fogEndWritten129==0&&fogColorWritten129==0xff123456);
 check(fogState129.Apply(fogSceneAddress129,true,fogWriter129)&&fogWrites129==1);
 check(fogState129.Apply(fogSceneAddress129,false,fogWriter129)&&!fogState129.active);
 check(fogStyleWritten129==2&&fogEndWritten129==2000.f);
 fogEffect129=fogState129;fogEffect129.active=true;
 FogNative(fogEffect129.parameters,4,5000.f);
 check(fogEnd==0.f&&fogEffect129.end==5000.f);
 fogEffectWriting129=true;FogNative(fogEffect129.parameters,4,5000.f);fogEffectWriting129=false;
 check(fogEnd==5000.f); // Restoration must bypass the active mask.
 fogEffect129.Reset();
 std::array<std::array<unsigned char,0x28>,5> currentFogFields129{};
 const unsigned currentFogTypes129[]={1,5,6,6,6};
 for(unsigned n=0;n<5;++n){currentFogFields129[n][0xc]=static_cast<unsigned char>(currentFogTypes129[n]);fogFieldsMock129[n]=reinterpret_cast<uintptr_t>(currentFogFields129[n].data());}
 auto fogReadMock129=+[](uintptr_t parameters,unsigned index,unsigned,void* output){
  auto table=*reinterpret_cast<uintptr_t*>(parameters+0xc);auto field=*reinterpret_cast<uintptr_t*>(table+index*4);std::memcpy(output,reinterpret_cast<void*>(field+0x1c),12);
 };
 *reinterpret_cast<uint32_t*>(currentFogFields129[0].data()+0x1c)=0xffaabbcc;
 *reinterpret_cast<unsigned*>(currentFogFields129[1].data()+0x1c)=2;
 *reinterpret_cast<float*>(currentFogFields129[2].data()+0x1c)=.75f;
 *reinterpret_cast<float*>(currentFogFields129[3].data()+0x1c)=150.f;
 *reinterpret_cast<float*>(currentFogFields129[4].data()+0x1c)=3500.f;
 wht::FogEffect129 initialFog129;
 check(initialFog129.ReadCurrent(fogSceneAddress129,fogReadMock129));
 check(initialFog129.style==2&&initialFog129.start==150.f&&initialFog129.end==3500.f&&initialFog129.density==.75f&&initialFog129.color==0xffaabbcc);
 check(initialFog129.Apply(fogSceneAddress129,true,fogWriter129));
 check(initialFog129.Apply(fogSceneAddress129,false,fogWriter129)&&fogEndWritten129==3500.f);
 currentFogFields129[3][0xc]=5;
 check(!initialFog129.ReadCurrent(fogSceneAddress129,fogReadMock129)&&!initialFog129.observed);
 fogFieldsMock129[3]=99;
 check(!fogState129.Apply(fogSceneAddress129,true,fogWriter129)&&!fogState129.observed);
 check(!fogState129.Capture(fogSceneAddress129,1,std::numeric_limits<float>::quiet_NaN(),1,1,&fogTint129));
 uint32_t playerHpColor129=0;
 check(wht::PlayerHpColor129(0,playerHpColor129)&&playerHpColor129==0xffff0303);
 check(wht::PlayerHpColor129(11,playerHpColor129)&&playerHpColor129==0xff4e2a04);
 check(wht::PlayerHpColor129(12,playerHpColor129)&&playerHpColor129==0xff9b0000);
 check(wht::PlayerHpColor129(15,playerHpColor129)&&playerHpColor129==0xffbe00fe);
 constexpr uint32_t extendedColors129[]={0xffebcd87,0xfff8a48b,0xffbfff80,0xffdcb9eb,0xff282828,0xffebf0ff,0xff00781e,0xffa46f33};
 for(unsigned i=0;i<8;++i){
  check(wht::PlayerHpColor129(16+i,playerHpColor129)&&playerHpColor129==extendedColors129[i]);
  check(wht::PlayerColorTag129(16+i,true)=="|cff7d7d7d");
 }
 check(wht::PlayerColorTag129(23,false)=="|cffa46f33");
 playerHpColor129=0xff123456;
 check(!wht::PlayerHpColor129(24,playerHpColor129)&&playerHpColor129==0xff123456);
 check(wht::PlayerColorTag129(14,false)=="|cff00eaff");
 check(wht::PlayerColorTag129(15,true)=="|cff7d7d7d");
 check(wht::PlayerColorTag129(99,false)=="|cffffffff");
 check(!wht::PlayerHpColor129(24,playerHpColor129));
 uint32_t configuredHp129=0;
 check(wht::HpConfiguredColor129(false,0xff112233,0xff445566,false,false,0,configuredHp129)&&configuredHp129==0xff112233);
 check(wht::HpConfiguredColor129(false,0xff112233,0xff445566,true,false,0,configuredHp129)&&configuredHp129==0xff445566);
 check(wht::HpConfiguredColor129(true,0xff112233,0xff445566,true,true,0xffabcdef,configuredHp129)&&configuredHp129==0xffabcdef);
 check(!wht::HpConfiguredColor129(true,0,0,false,false,0,configuredHp129)&&configuredHp129==0xffabcdef);
 wht::HpColor129::Entry colorEntry129;uint32_t actualHpColor129=0xff00ff00;unsigned colorWrites129=0;
 auto setHpColor129=[&](uintptr_t texture,uint32_t value){check(texture==123||texture==456);actualHpColor129=value;++colorWrites129;};
 wht::HpColor129::Tint(colorEntry129,123,actualHpColor129,true,0xffff0303,setHpColor129);
 check(colorEntry129.tinted&&actualHpColor129==0xffff0303&&colorEntry129.original==0xff00ff00);
 wht::HpColor129::Tint(colorEntry129,123,actualHpColor129,true,0xffff0303,setHpColor129);
 check(colorWrites129==1);
 wht::HpColor129::Tint(colorEntry129,123,actualHpColor129,false,0,setHpColor129);
 check(!colorEntry129.tinted&&actualHpColor129==0xff00ff00);
 wht::HpColor129::Tint(colorEntry129,123,actualHpColor129,true,0xff0042ff,setHpColor129);
 actualHpColor129=0xff102030;
 wht::HpColor129::Tint(colorEntry129,123,actualHpColor129,false,0,setHpColor129);
 check(actualHpColor129==0xff102030&&!colorEntry129.tinted);
 wht::HpColor129::Tint(colorEntry129,456,0xffabcdef,true,0xffff0303,setHpColor129);
 check(colorEntry129.texture==456&&colorEntry129.original==0xffabcdef);
 wht::HpColor129::Tint(colorEntry129,456,actualHpColor129,false,0,setHpColor129);
 check(actualHpColor129==0xffabcdef);
 check(ToolCommand(L"#colorplayer 1")&&runtimeConfig.colorPlayer);
 check(!ToolCommand(L"#colorp 2"));
 check(ToolCommand(L"#colorp 0")&&!runtimeConfig.colorPlayer);
 for(unsigned i=0;i<4;++i)resourceObjects[i][0x30]=static_cast<unsigned char>(i);
 wht::Config manaPolicy129;
 check(wht::ManaColor129(manaPolicy129,false)==manaPolicy129.manaBarColor);
 check(wht::ManaColor129(manaPolicy129,true)==manaPolicy129.manaBarColorEnemy);
 check(!wht::ManaEnemy129(0,nullptr,resApi));
 const auto manaOwnerMock=reinterpret_cast<wht::ManaOwner129>(ManaOwnerMock129);
 check(!wht::ManaEnemy129(reinterpret_cast<uintptr_t>(resourceObjects[0]),manaOwnerMock,resApi));
 check(!wht::ManaEnemy129(reinterpret_cast<uintptr_t>(resourceObjects[1]),manaOwnerMock,resApi));
 check(wht::ManaEnemy129(reinterpret_cast<uintptr_t>(resourceObjects[3]),manaOwnerMock,resApi));
 resourceObjects[3][0x30]=24;
 check(!wht::ManaEnemy129(reinterpret_cast<uintptr_t>(resourceObjects[3]),manaOwnerMock,resApi));
 resourceObjects[3][0x30]=2;
 check(!wht::ManaEnemy129(reinterpret_cast<uintptr_t>(resourceObjects[3]),manaOwnerMock,resApi));
 resourceObjects[3][0x30]=3;
 manaPolicy129.manaBarColorEnemy=0xff123456;
 check(wht::ManaColor129(manaPolicy129,true)==0xff123456);


 check(wht::ManaEligible129(manaPolicy129,true,true,true,100.f));
 check(!wht::ManaEligible129(manaPolicy129,false,true,true,100.f));
 check(!wht::ManaEligible129(manaPolicy129,true,false,true,100.f));
 check(!wht::ManaEligible129(manaPolicy129,true,true,true,0.f));
 check(!wht::ManaEligible129(manaPolicy129,true,true,true,std::numeric_limits<float>::quiet_NaN()));
 manaPolicy129.manaBarHero=false;
 check(!wht::ManaEligible129(manaPolicy129,true,true,true,100.f));
 check(wht::ManaEligible129(manaPolicy129,true,true,false,100.f));
 manaPolicy129.manaBarNonHero=false;
 check(!wht::ManaEligible129(manaPolicy129,true,true,false,100.f));
 check(wht::StateValueText129(12.9f,100.f,false,false)=="|cff00ff0012|r");
 check(wht::StateValueText129(-1.f,100.f,true,true)=="|cffc3dbff0/100|r");
 check(wht::StateValueText129(120.f,100.f,true,false)=="|cff00ff00100/100|r");
 check(wht::StateValueText129(1.f,0.f,true,false).empty());
 // Native stat-tag ABI and regeneration rate contract.
 std::vector<unsigned char> regenUnit129(0x1d0),regenState129(0x88);
 *reinterpret_cast<uintptr_t*>(regenUnit129.data())=executableBase+0xa9c504;
 *reinterpret_cast<uint32_t*>(regenUnit129.data()+0x1a8)=123;
 *reinterpret_cast<uint32_t*>(regenUnit129.data()+0x1ac)=456;
 *reinterpret_cast<uint32_t*>(regenUnit129.data()+0x1c8)=789;
 *reinterpret_cast<uint32_t*>(regenUnit129.data()+0x1cc)=456;
 static uintptr_t regenObject129=0;regenObject129=reinterpret_cast<uintptr_t>(regenState129.data());
 auto resolveRegen129=+[](uint32_t low,uint32_t high)->uintptr_t{return ((low==123||low==789)&&high==456)?regenObject129:0;};
 *reinterpret_cast<float*>(regenState129.data()+0x7c)=4.3f;float regenValue129=0;
 check(wht::ReadRegen129(executableBase,reinterpret_cast<uintptr_t>(regenUnit129.data()),false,resolveRegen129,regenValue129)&&regenValue129==4.3f);
 check(wht::ReadRegen129(executableBase,reinterpret_cast<uintptr_t>(regenUnit129.data()),true,resolveRegen129,regenValue129));


 float stateValue129=0,stateMaximum129=0;
 check(wht::ReadStateValues129(executableBase,reinterpret_cast<uintptr_t>(regenUnit129.data()),true,resolveRegen129,reinterpret_cast<wht::StatRead129>(CurrentStateMock129),reinterpret_cast<wht::StatRead129>(MaximumStateMock129),stateValue129,stateMaximum129));
 check(regenProperty129==reinterpret_cast<uintptr_t>(regenUnit129.data())+0x1c0&&stateValue129==42.f&&stateMaximum129==100.f);
 std::vector<unsigned char> stateBar129(0x160);
 *reinterpret_cast<uintptr_t*>(stateBar129.data())=executableBase+0xa95104;
 *reinterpret_cast<unsigned*>(stateBar129.data()+0x13c)=5;
 *reinterpret_cast<unsigned*>(stateBar129.data()+0x94)=1;
 *reinterpret_cast<uintptr_t*>(stateBar129.data()+0x158)=reinterpret_cast<uintptr_t>(regenUnit129.data());
 regenUnit129[0x37]='H';uintptr_t sampledStateUnit129=0;bool sampledVisible129=false,sampledHero129=false;
 check(wht::SampleStateBar129(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),sampledStateUnit129,sampledVisible129,sampledHero129)&&sampledVisible129&&sampledHero129);
 *reinterpret_cast<unsigned*>(stateBar129.data()+0x94)=0;
 check(wht::SampleStateBar129(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),sampledStateUnit129,sampledVisible129,sampledHero129)&&!sampledVisible129);
 *reinterpret_cast<unsigned*>(regenUnit129.data()+0x20)=8;
 *reinterpret_cast<unsigned*>(stateBar129.data()+0x94)=1;
 bool invulnerableSample129=false;
 check(wht::InvulnerableHp129(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),invulnerableSample129)&&invulnerableSample129);
 wht::InvulnerableBar129 invulState129;bool actualBarVisible129=true;unsigned nativeBarShows129=0;
 auto invulShow129=[&](uintptr_t){actualBarVisible129=*reinterpret_cast<unsigned*>(stateBar129.data()+0x94)!=0;++nativeBarShows129;};
 auto invulHide129=[&](uintptr_t){actualBarVisible129=false;};
 invulState129.Apply(executableBase,true,invulShow129,invulHide129);
 check(invulState129.Update(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),invulShow129,invulHide129)&&!actualBarVisible129);
 check(*reinterpret_cast<unsigned*>(stateBar129.data()+0x94)==1);
 invulState129.Apply(executableBase,false,invulShow129,invulHide129);
 check(actualBarVisible129&&invulState129.suppressed.empty());
 invulState129.Apply(executableBase,true,invulShow129,invulHide129);
 invulState129.Update(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),invulShow129,invulHide129);
 *reinterpret_cast<unsigned*>(stateBar129.data()+0x94)=0;
 invulState129.Apply(executableBase,false,invulShow129,invulHide129);
 check(!actualBarVisible129); // The game hid it in fog while the tool was active.
 invulState129.Apply(executableBase,true,invulShow129,invulHide129);
 invulState129.Update(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),invulShow129,invulHide129);
 *reinterpret_cast<unsigned*>(regenUnit129.data()+0x20)=0;
 *reinterpret_cast<unsigned*>(stateBar129.data()+0x94)=1;
 check(!invulState129.Update(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),invulShow129,invulHide129)&&actualBarVisible129);
 invulState129.Forget(reinterpret_cast<uintptr_t>(stateBar129.data()));
 check(invulState129.suppressed.empty());
 invulnerableHookReady129=true;
 check(ToolCommand(L"#barinvul 1")&&runtimeConfig.hideInvulnerableBars);
 check(!ToolCommand(L"#barinvul 2"));
 check(ToolCommand(L"#barinvul 0")&&!runtimeConfig.hideInvulnerableBars);
 invulnerableHookReady129=false;
 check(!ToolCommand(L"#barinvul 1"));
 *reinterpret_cast<unsigned*>(stateBar129.data()+0x13c)=0;
 check(!wht::SampleStateBar129(executableBase,reinterpret_cast<uintptr_t>(stateBar129.data()),sampledStateUnit129,sampledVisible129,sampledHero129));
 *reinterpret_cast<float*>(regenState129.data()+0x7c)=-2.f;
 check(wht::ReadRegen129(executableBase,reinterpret_cast<uintptr_t>(regenUnit129.data()),true,resolveRegen129,regenValue129)&&regenValue129==-2.f);
 check(wht::RegenText129(true,-2.f).find("-2.00")!=std::string::npos);
 *reinterpret_cast<float*>(regenState129.data()+0x7c)=std::numeric_limits<float>::quiet_NaN();
 check(!wht::ReadRegen129(executableBase,reinterpret_cast<uintptr_t>(regenUnit129.data()),true,resolveRegen129,regenValue129));
 *reinterpret_cast<uintptr_t*>(regenUnit129.data())=0;
 check(!wht::ReadRegen129(executableBase,reinterpret_cast<uintptr_t>(regenUnit129.data()),false,resolveRegen129,regenValue129));
 check(ToolCommand(L"#regenhp 1")&&runtimeConfig.showRegenHP);
 check(ToolCommand(L"#regenmp 0")&&!runtimeConfig.showRegenMana);
 check(!ToolCommand(L"#regenmp 2"));
 check(ToolCommand(L"#regenhp 0")&&!runtimeConfig.showRegenHP);
 check(ToolCommand(L"#cooldown 1")&&runtimeConfig.showCooldown);
 check(!ToolCommand(L"#cooldown 2"));
 check(ToolCommand(L"#cooldown 0")&&!runtimeConfig.showCooldown);
 levelHooksReady129=true;
 check(ToolCommand(L"#wmp 0.04")&&std::abs(runtimeConfig.barWidthMp-.04f)<.000001f);
 check(ToolCommand(L"#hmp 0.003")&&std::abs(runtimeConfig.barHeightMp-.003f)<.000001f);
 check(!ToolCommand(L"#wmp -1")&&!ToolCommand(L"#hmp 101"));
 check(ToolCommand(L"#wmp 0")&&runtimeConfig.barWidthMp==0.f);
 check(ToolCommand(L"#hmp 0")&&runtimeConfig.barHeightMp==0.f);
 check(ToolCommand(L"#whp 0.06")&&std::abs(runtimeConfig.barWidthHp-.06f)<.000001f);
 check(ToolCommand(L"#hhp 0.008")&&std::abs(runtimeConfig.barHeightHp-.008f)<.000001f);
 check(!ToolCommand(L"#whp -1")&&!ToolCommand(L"#hhp 101")&&!ToolCommand(L"#whp 1 extra"));
 check(ToolCommand(L"#whp 0")&&runtimeConfig.barWidthHp==0.f);
 check(ToolCommand(L"#hhp 0")&&runtimeConfig.barHeightHp==0.f);
 check(ToolCommand(L"#showlevel 1")&&runtimeConfig.stateTagShowLevel);
 check(!ToolCommand(L"#showlevel 2"));
 check(ToolCommand(L"#showlevel 0")&&!runtimeConfig.stateTagShowLevel);
 levelHooksReady129=false;
 check(!ToolCommand(L"#showlevel 1"));
 levelHooksReady129=true;
 check(ToolCommand(L"#chp 0xFF112233")&&runtimeConfig.hpBarColor==0xff112233);
 check(ToolCommand(L"#chpe ff445566")&&runtimeConfig.hpBarColorEnemy==0xff445566);
 check(ToolCommand(L"#cmp 0XFF123456")&&runtimeConfig.manaBarColor==0xff123456);
 check(ToolCommand(L"#cmpe FFABCDEF")&&runtimeConfig.manaBarColorEnemy==0xffabcdef);
 check(!ToolCommand(L"#chp FF1122")&&!ToolCommand(L"#chpe 0xFFZZ1122")&&!ToolCommand(L"#cmp FF112233 extra"));
 check(runtimeConfig.hpBarColor==0xff112233);
 levelHooksReady129=false;check(!ToolCommand(L"#chp FF000000"));
 localFilesWriter129=+[](bool){return true;};
 check(ToolCommand(L"#allowlocal 1")&&runtimeConfig.allowLocalFiles);
 check(ToolCommand(L"#alf")&&!runtimeConfig.allowLocalFiles);
 check(ToolCommand(L"#localfiles 1")&&runtimeConfig.allowLocalFiles);
 check(!ToolCommand(L"#alf 2")&&!ToolCommand(L"#allowlocal 0 extra"));
 localFilesWriter129=+[](bool){return false;};
 check(!ToolCommand(L"#alf 0")&&runtimeConfig.allowLocalFiles&&wht::LoadConfig(runtimeIni).allowLocalFiles);
 localFilesWriter129=wht::WriteLocalFiles129;
 for(size_t index=wht::kToolMenuControls.size()-5;index<wht::kToolMenuControls.size()-2;++index){
  const auto& control=wht::kToolMenuControls[index];check(ControlAvailable129(control));
  check(ToolCommand(L"#toolui "+std::to_wstring(index)+L" toggle"));
  check(wht::ToolMenuOverlay::ReadValue(runtimeConfig,index)==L"1");
  check(ToolCommand(L"#toolui "+std::to_wstring(index)+L" toggle"));
  check(wht::ToolMenuOverlay::ReadValue(runtimeConfig,index)==L"0");
 }
 wht::TimeBoss129 bossTracker129;
 bossTracker129.units[100]=42;
 bossTracker129.Observe(20,[](uintptr_t unit,wht::BossUnit129& out){out={unit,42,0x4870616c,100};return true;});
 check(bossTracker129.state.Remaining(20)[2]==0);
 bossTracker129.Observe(25,[](uintptr_t unit,wht::BossUnit129& out){out={unit,42,0x4870616c,0};return true;});
 check(bossTracker129.state.Remaining(25)[2]==31);
 bossTracker129.Observe(26,[](uintptr_t unit,wht::BossUnit129& out){out={unit,42,0x4870616c,0};return true;});
 check(bossTracker129.state.Remaining(26)[2]==30);
 bossTracker129.Observe(27,[](uintptr_t unit,wht::BossUnit129& out){out={unit,43,0x4870616c,100};return true;});
 check(bossTracker129.units.empty());
 bossTracker129.Reset();check(bossTracker129.state.Remaining(0)[0]==493);
 check(bossTracker129.state.Remaining(493)[0]==1007);
 for(size_t index=0;index<wht::kToolMenuControls.size();++index){
  const auto& control=wht::kToolMenuControls[index];
  if(!wcscmp(control.section,L"MTHKEY")&&!wcscmp(control.key,L"TimeBoss")){
   levelHooksReady129=false;check(!ControlAvailable129(control));
   levelHooksReady129=true;check(ControlAvailable129(control));
   check(ToolCommand(L"#toolui "+std::to_wstring(index)+L" toggle")&&runtimeConfig.timeBoss);
   check(ToolCommand(L"#toolui "+std::to_wstring(index)+L" toggle")&&!runtimeConfig.timeBoss);
  }
 }
 const auto castModeIndex129=wht::kToolMenuControls.size()-9;
 const auto& castModeControl129=wht::kToolMenuControls[castModeIndex129];
 check(!ControlAvailable129(castModeControl129));
 clickButton=reinterpret_cast<wht::ClickFn129>(CaptureClick);
 check(ControlAvailable129(castModeControl129));
 check(wht::ToolMenuOverlay::Display(castModeControl129,L"1")=="Smart Cast  v");
 check(wht::ToolMenuOverlay::ChoiceName(castModeControl129,2)=="Quick Cast Old");
 check(wht::EnglishControlLabel(castModeControl129)=="Casting mode");
 check(ToolCommand(L"#toolui "+std::to_wstring(castModeIndex129)+L" set 1")&&runtimeConfig.modeCast==1);
 check(!ToolCommand(L"#toolui "+std::to_wstring(castModeIndex129)+L" set 3"));
 levelHooksReady129=true;
 const auto fontSizeIndex129=wht::kToolMenuControls.size()-8;
 check(ToolCommand(L"#toolui "+std::to_wstring(fontSizeIndex129)+L" set 8")&&runtimeConfig.stateTagFontSize==8);
 check(!ToolCommand(L"#toolui "+std::to_wstring(fontSizeIndex129)+L" set 21"));
 check(ToolCommand(L"#toolui "+std::to_wstring(fontSizeIndex129)+L" set 4")&&runtimeConfig.stateTagFontSize==4);
 check(std::abs(wht::StateFontSize129(4)-.006f)<1.e-6f);
 check(std::abs(wht::StateFontSize129(8)-.012f)<1.e-6f);
 check(std::abs(wht::StateFontSize129(std::numeric_limits<float>::quiet_NaN())-.006f)<1.e-6f);
 check(wht::StateFontOffset129(.012f,true)<wht::StateFontOffset129(.006f,true));
 struct FontReflowMock129 {
  unsigned fonts=0,anchors=0;float appliedSize=0,offset=0;
  void setFont(void*,const char*,float size,int){++fonts;appliedSize=size;}
  void setRelPoint(void*,int,void*,int,float,float y,int){++anchors;offset=y;}
 } fontReflow129;
 float appliedFont129=0;
 wht::ReflowStateFont129(reinterpret_cast<void*>(1),2,true,.006f,appliedFont129,fontReflow129);
 wht::ReflowStateFont129(reinterpret_cast<void*>(1),2,true,.006f,appliedFont129,fontReflow129);
 check(fontReflow129.fonts==1&&fontReflow129.anchors==1);
 wht::ReflowStateFont129(reinterpret_cast<void*>(1),2,true,.012f,appliedFont129,fontReflow129);
 check(fontReflow129.fonts==2&&fontReflow129.appliedSize==.012f&&fontReflow129.offset==wht::StateFontOffset129(.012f,true));
 levelHooksReady129=false;
 check(wht::StateValueText129(12345.9f,98765.f,true,false,true)=="|cff00ff0012,345/98,765|r");
 check(wht::StateValueText129(12345.f,20000.f,false,true,true)=="|cffc3dbff12,345|r");
 check(wht::StateValueText129(999.f,1000.f,true,false,true)=="|cff00ff00999/1,000|r");
 check(wht::StateValueText129(-1.f,1000.f,false,false,true)=="|cff00ff000|r");
 check(wht::StateValueText129(2000.f,1000.f,false,false,true)=="|cff00ff001,000|r");
 check(wht::StateValueText129(std::numeric_limits<float>::infinity(),1000.f,false,false,true).empty());
 levelHooksReady129=true;
 check(ToolCommand(L"#hpvalue 0")&&!runtimeConfig.stateTagHpValue);
 check(ToolCommand(L"#manavalue 1")&&runtimeConfig.stateTagManaValue&&!runtimeConfig.stateTagHpValue);
 check(ToolCommand(L"#hpvalue")&&runtimeConfig.stateTagHpValue);
 check(!ToolCommand(L"#manavalue 2")&&!ToolCommand(L"#hpvalue 0 extra"));
 levelHooksReady129=false;check(!ToolCommand(L"#hpvalue 1"));
 check(wht::NotificationText("Fast Menu: ON").find("|cff00ff00ON")!=std::string::npos);
 check(wht::NotificationText("Fast Menu: OFF").find("|cffff0000OFF")!=std::string::npos);
 check(wht::NotificationText("Set FPS Limit: 120").find("|cff00ff00120")!=std::string::npos);
 check(!wht::ShowNotification129(0,"test",nullptr));
 std::array<uint32_t,14> presentationMock129{};presentationMock129[0]=1920;presentationMock129[1]=1080;
 check(!wht::UnlockPresentation129(presentationMock129.data(),false)&&presentationMock129[13]==0);
 check(wht::UnlockPresentation129(presentationMock129.data(),true)&&presentationMock129[13]==0x80000000u);
 check(presentationMock129[0]==1920&&presentationMock129[1]==1080);
 check(!wht::UnlockPresentation129(nullptr,true));
 check(wht::UnlockFps129::Owns(0xfb,false)&&wht::UnlockFps129::Owns(0xff,true));
 check(!wht::UnlockFps129::Owns(0xff,false)&&!wht::UnlockFps129::Owns(0x90,true));
 struct FpsMock129 {
  uintptr_t id=100;unsigned value=16,calls=0;
  uintptr_t context(unsigned){return id;}unsigned get(uintptr_t){return value;}
  void set(unsigned ms,uintptr_t){value=ms;++calls;}
 } fpsMock129;
 const unsigned char hookedScale129[]={0xe9,0,0,0,0,0x90,0x8b,0x4d,0x0c,0x56};
 auto originalScale129=+[](uintptr_t,int,float){};
 check(wht::ResolveScaleSetter129(hookedScale129,nullptr)==nullptr);
 check(wht::ResolveScaleSetter129(hookedScale129,originalScale129)==originalScale129);
 check(wht::ResolveScaleSetter129(nullptr,nullptr)==nullptr);
 {
  std::vector<unsigned char> invulUnit(0x410),invulAgent(0x24),invulPortrait(0x250),heroSkillData(0xa0);
  auto& portraitAddress=*reinterpret_cast<uintptr_t*>(ui.data()+0x434);const auto oldPortrait=portraitAddress;
  portraitAddress=reinterpret_cast<uintptr_t>(invulPortrait.data());
  invulUnitMock129=reinterpret_cast<uintptr_t>(invulUnit.data());invulAgentMock129=reinterpret_cast<uintptr_t>(invulAgent.data());
  *reinterpret_cast<uintptr_t*>(invulPortrait.data())=executableBase+0xa981d4;
  *reinterpret_cast<uintptr_t*>(invulPortrait.data()+0x248)=invulUnitMock129;
  *reinterpret_cast<uintptr_t*>(invulUnit.data())=executableBase+0xa9c504;
  *reinterpret_cast<uint32_t*>(invulUnit.data()+0xc)=11;*reinterpret_cast<uint32_t*>(invulUnit.data()+0x10)=22;
  *reinterpret_cast<uint32_t*>(invulAgent.data()+0xc)=0x2b61676c;
  *reinterpret_cast<unsigned char*>(invulUnit.data()+0x37)='H';
  *reinterpret_cast<uintptr_t*>(invulUnit.data()+0x3fc)=reinterpret_cast<uintptr_t>(heroSkillData.data());
  *reinterpret_cast<int*>(heroSkillData.data()+0x94)=3;
  wht::HeroSkillPointsApi129 heroSkillApi{InvulAgentMock129,
   reinterpret_cast<decltype(wht::HeroSkillPointsApi129::get)>(HeroSkillGetMock129),
   reinterpret_cast<decltype(wht::HeroSkillPointsApi129::modify)>(HeroSkillModifyMock129)};
  int parsedPoints=0,actualAdded=0,totalPoints=0;const char* skillReason=nullptr;
  check(wht::ParseHeroSkillPoints129(L"",parsedPoints)&&parsedPoints==500);
  check(wht::ParseHeroSkillPoints129(L" 12 ",parsedPoints)&&parsedPoints==12);
  check(!wht::ParseHeroSkillPoints129(L"0",parsedPoints)&&!wht::ParseHeroSkillPoints129(L"10001",parsedPoints));
  check(!wht::ParseHeroSkillPoints129(L"12 extra",parsedPoints)&&!wht::ParseHeroSkillPoints129(L"abc",parsedPoints));
  const auto heroSelection=wht::SelectedSkill129(executableBase);
  wht::HeroLevelApi129 levelApi{InvulAgentMock129,
   reinterpret_cast<decltype(wht::HeroLevelApi129::level)>(HeroLevelGetMock129),
   reinterpret_cast<decltype(wht::HeroLevelApi129::required)>(HeroLevelRequiredMock129),
   reinterpret_cast<decltype(wht::HeroLevelApi129::experience)>(HeroLevelXpMock129),HeroLevelRealMock129,
   reinterpret_cast<decltype(wht::HeroLevelApi129::add)>(HeroLevelAddMock129),
   reinterpret_cast<decltype(wht::HeroLevelApi129::strip)>(HeroLevelStripMock129)};
  int parsedLevel=0,actualLevel=0;const char* levelReason=nullptr;
  check(wht::ParseHeroLevel129(L"",parsedLevel)&&parsedLevel==10);
  check(wht::ParseHeroLevel129(L"100",parsedLevel)&&parsedLevel==100);
  check(!wht::ParseHeroLevel129(L"0",parsedLevel)&&!wht::ParseHeroLevel129(L"10001",parsedLevel)&&!wht::ParseHeroLevel129(L"5 extra",parsedLevel));
  check(wht::ParseHeroLevel129(L"10000",parsedLevel)&&parsedLevel==10000);
  check(wht::SetHeroLevel129(executableBase,heroSelection,levelApi,10,actualLevel,&levelReason)&&actualLevel==10&&heroXpMock129==900&&heroLevelAbi129);
  check(wht::SetHeroLevel129(executableBase,heroSelection,levelApi,3,actualLevel,&levelReason)&&actualLevel==3&&heroLevelWrites129==2);
  check(wht::SetHeroLevel129(executableBase,heroSelection,levelApi,3,actualLevel,&levelReason)&&heroLevelWrites129==2);
  heroLevelReject129=true;
  check(!wht::SetHeroLevel129(executableBase,heroSelection,levelApi,5,actualLevel,&levelReason)&&levelReason&&actualLevel==3);
  heroLevelReject129=false;
  *reinterpret_cast<unsigned char*>(invulUnit.data()+0x37)='h';
  const auto levelWrites=heroLevelWrites129;
  check(!wht::SetHeroLevel129(executableBase,heroSelection,levelApi,5,actualLevel,&levelReason)&&heroLevelWrites129==levelWrites);
  *reinterpret_cast<unsigned char*>(invulUnit.data()+0x37)='H';
  auto staleHero=heroSelection;++staleHero.identity;
  check(!wht::SetHeroLevel129(executableBase,staleHero,levelApi,5,actualLevel,&levelReason)&&heroLevelWrites129==levelWrites);
  check(wht::SetHeroLevel129(executableBase,heroSelection,levelApi,400,actualLevel,&levelReason)&&actualLevel==400);
  check(!wht::ResolveHeroLevelApi129(executableBase));
  check(wht::AddHeroSkillPoints129(executableBase,heroSelection,heroSkillApi,12,actualAdded,totalPoints,&skillReason));
  check(actualAdded==12&&totalPoints==15&&heroSkillModifyCalls129==1&&heroSkillAbi129&&!skillReason);
  heroSkillReject129=true;
  check(!wht::AddHeroSkillPoints129(executableBase,heroSelection,heroSkillApi,1,actualAdded,totalPoints,&skillReason)&&skillReason);
  heroSkillReject129=false;
  *reinterpret_cast<unsigned char*>(invulUnit.data()+0x37)='h';
  check(!wht::AddHeroSkillPoints129(executableBase,heroSelection,heroSkillApi,1,actualAdded,totalPoints,&skillReason));
  *reinterpret_cast<unsigned char*>(invulUnit.data()+0x37)='H';
  *reinterpret_cast<uint32_t*>(invulAgent.data()+0x20)=1;
  check(!wht::AddHeroSkillPoints129(executableBase,heroSelection,heroSkillApi,1,actualAdded,totalPoints,&skillReason));
  *reinterpret_cast<uint32_t*>(invulAgent.data()+0x20)=0;
  check(!wht::ResolveHeroSkillPointsApi129(executableBase));
  wht::HeroAttributesApi129 attributesApi;
  attributesApi.agent=InvulAgentMock129;
  attributesApi.get[0]=reinterpret_cast<wht::HeroAttributesApi129::GetFn>(HeroStrGetMock129);
  attributesApi.get[1]=reinterpret_cast<wht::HeroAttributesApi129::GetFn>(HeroAgiGetMock129);
  attributesApi.get[2]=reinterpret_cast<wht::HeroAttributesApi129::GetFn>(HeroIntGetMock129);
  attributesApi.set[0]=reinterpret_cast<wht::HeroAttributesApi129::SetFn>(HeroStrSetMock129);
  attributesApi.set[1]=reinterpret_cast<wht::HeroAttributesApi129::SetFn>(HeroAgiSetMock129);
  attributesApi.set[2]=reinterpret_cast<wht::HeroAttributesApi129::SetFn>(HeroIntSetMock129);
  int attributeValue=0,attributeActual[3]{};const char* attributeReason=nullptr;
  check(wht::ParseHeroAttributeValue129(L"",attributeValue)&&attributeValue==500);
  check(wht::ParseHeroAttributeValue129(L" 99 ",attributeValue)&&attributeValue==99);
  check(!wht::ParseHeroAttributeValue129(L"0",attributeValue)&&!wht::ParseHeroAttributeValue129(L"10001",attributeValue));
  check(!wht::ParseHeroAttributeValue129(L"99 extra",attributeValue));
  check(wht::SetHeroAttributes129(executableBase,heroSelection,attributesApi,wht::HeroAttribute129::Strength,99,attributeActual,&attributeReason));
  check(heroAttributeValues129[0]==99&&heroAttributeValues129[1]==20&&heroAttributeWrites129[0]==1&&heroAttributeAbi129&&!attributeReason);
  check(wht::SetHeroAttributes129(executableBase,heroSelection,attributesApi,wht::HeroAttribute129::Agility,88,attributeActual,&attributeReason));
  check(wht::SetHeroAttributes129(executableBase,heroSelection,attributesApi,wht::HeroAttribute129::Intelligence,77,attributeActual,&attributeReason));
  check(wht::SetHeroAttributes129(executableBase,heroSelection,attributesApi,wht::HeroAttribute129::All,123,attributeActual,&attributeReason));
  check(heroAttributeValues129[0]==123&&heroAttributeValues129[1]==123&&heroAttributeValues129[2]==123&&heroAttributeWrites129[0]==2&&heroAttributeWrites129[1]==2&&heroAttributeWrites129[2]==2&&heroAttributeAbi129);
  heroAttributeReject129=true;
  check(!wht::SetHeroAttributes129(executableBase,heroSelection,attributesApi,wht::HeroAttribute129::Strength,456,attributeActual,&attributeReason)&&attributeReason);
  heroAttributeReject129=false;
  *reinterpret_cast<unsigned char*>(invulUnit.data()+0x37)='h';
  check(!wht::SetHeroAttributes129(executableBase,heroSelection,attributesApi,wht::HeroAttribute129::Strength,456,attributeActual,&attributeReason));
  *reinterpret_cast<unsigned char*>(invulUnit.data()+0x37)='H';
  check(!wht::ResolveHeroAttributesApi129(executableBase));
  wht::InvulnerabilityApi129 invulApi{InvulAgentMock129,reinterpret_cast<decltype(wht::InvulnerabilityApi129::find)>(InvulFindMock129),reinterpret_cast<decltype(wht::InvulnerabilityApi129::set)>(InvulSetMock129)};
  const auto selection=wht::SelectedSkill129(executableBase);wht::InvulnerabilityResult129 result;const char* reason=nullptr;
  check(wht::ChangeInvulnerability129(executableBase,selection,invulApi,true,result,&reason)&&result.ability&&result.effective&&!reason);
  check(wht::ChangeInvulnerability129(executableBase,selection,invulApi,true,result,&reason)&&invulWritesMock129==1);
  check(wht::ChangeInvulnerability129(executableBase,selection,invulApi,false,result,&reason)&&!result.ability&&!result.effective&&invulWritesMock129==2);
  check(wht::ChangeInvulnerability129(executableBase,selection,invulApi,false,result,&reason)&&invulWritesMock129==2);
  invulRejectMock129=true;
  check(!wht::ChangeInvulnerability129(executableBase,selection,invulApi,true,result,&reason)&&reason&&invulWritesMock129==3);
  invulRejectMock129=false;invulMapMock129=true;
  check(wht::ChangeInvulnerability129(executableBase,selection,invulApi,true,result,&reason));
  check(wht::ChangeInvulnerability129(executableBase,selection,invulApi,false,result,&reason)&&!result.ability&&result.effective);
  std::vector<unsigned char> addedAbilityStorage(0x60);addedAbilityMock129=reinterpret_cast<uintptr_t>(addedAbilityStorage.data());
  wht::AddAbilityApi129 addApi;
  addApi.common={InvulAgentMock129,reinterpret_cast<decltype(wht::RemoveAbilityApi129::find)>(AddFindMock129),reinterpret_cast<decltype(wht::RemoveAbilityApi129::remove)>(RemovalMock129),AddRefreshMock129};
  addApi.before=AddBeforeMock129;addApi.add=AddCreateMock129;addApi.after=AddAfterMock129;
  addApi.maximum=reinterpret_cast<decltype(wht::AddAbilityApi129::maximum)>(AddMaximumMock129);addApi.step=AddStepMock129;
  wht::AddAbilityResult129 addResult;
  check(wht::AddAbilityToUnit129(executableBase,selection,addApi,0x41486862,9,addResult,&reason));
  check(addResult.added&&addResult.level==3&&addCallsMock129==1&&addBeforeMock129==1&&addAfterMock129==1&&addStepsMock129==2);
  check(wht::AddAbilityToUnit129(executableBase,selection,addApi,0x41486862,1,addResult,&reason)&&!addResult.added&&addResult.level==3&&addStepsMock129==2);
  check(wht::AddAbilityToUnit129(executableBase,selection,addApi,0x41486862,2,addResult,&reason)&&!addResult.added&&addResult.level==2&&addStepsMock129==3);
  addStepRejectMock129=true;
  check(!wht::AddAbilityToUnit129(executableBase,selection,addApi,0x41486862,3,addResult,&reason)&&reason&&addStepsMock129==4);
  addStepRejectMock129=false;addPresentMock129=false;addRejectMock129=true;
  check(!wht::AddAbilityToUnit129(executableBase,selection,addApi,0x41486862,1,addResult,&reason)&&reason&&!addResult.added);
  check(addCallsMock129==2&&addBeforeMock129==2&&addAfterMock129==2&&addAbiMock129);
  addRejectMock129=false;const auto creations=addCallsMock129;
  check(!wht::AddAbilityToUnit129(executableBase,{},addApi,0x41486862,1,addResult,&reason)&&addCallsMock129==creations);
  check(!wht::ResolveAddAbilityApi129(executableBase));
  check(!wht::AddSelectedAbility129(0,0x41486862,1,addResult,&reason)&&reason);
  wht::KillUnitApi129 killApi{InvulAgentMock129,reinterpret_cast<decltype(wht::KillUnitApi129::kill)>(KillUnitMock129)};
  check(wht::KillUnit129(executableBase,selection,killApi,&reason)&&killCallsMock129==1&&killAbiMock129);
  *reinterpret_cast<float*>(globals.data()+0xd2d88c)=1.f;
  check(!wht::KillUnit129(executableBase,selection,killApi,&reason)&&reason&&killCallsMock129==1);
  *reinterpret_cast<float*>(globals.data()+0xd2d88c)=0.f;
  killReassignMock129=true;
  check(wht::KillUnit129(executableBase,selection,killApi,&reason)&&killCallsMock129==2);
  // The death callback can invalidate the selected pointer; avoid reading it again.
  check(!wht::KillUnit129(executableBase,selection,killApi,&reason)&&killCallsMock129==2);
  killReassignMock129=false;*reinterpret_cast<uint32_t*>(invulUnit.data()+0x10)=22;
  *reinterpret_cast<uint32_t*>(invulAgent.data()+0x20)=1;
  check(!wht::KillUnit129(executableBase,selection,killApi,&reason)&&killCallsMock129==2);
  *reinterpret_cast<uint32_t*>(invulAgent.data()+0x20)=0;
  check(!wht::ResolveKillUnitApi129(executableBase));
  check(!wht::KillSelectedUnit129(0,&reason)&&reason);
  wht::RemoveAbilityApi129 removalApi{InvulAgentMock129,reinterpret_cast<decltype(wht::RemoveAbilityApi129::find)>(RemovalFindMock129),reinterpret_cast<decltype(wht::RemoveAbilityApi129::remove)>(RemovalMock129),RemovalRefreshMock129};
  bool removedAbility=false;
  check(wht::RemoveAbilityFromUnit129(executableBase,selection,removalApi,0x41486862,removedAbility,&reason)&&removedAbility);
  check(removalCalls129==1&&removalRefreshes129==1&&!removalPresent129&&removalAbi129);
  check(wht::RemoveAbilityFromUnit129(executableBase,selection,removalApi,0x41486862,removedAbility,&reason)&&!removedAbility&&removalCalls129==1);
  check(wht::RemoveAbilityFromUnit129(executableBase,selection,removalApi,0xdeadbeef,removedAbility,&reason)&&!removedAbility&&removalCalls129==1);
  removalPresent129=true;removalReject129=true;
  check(!wht::RemoveAbilityFromUnit129(executableBase,selection,removalApi,0x41486862,removedAbility,&reason)&&reason&&!removedAbility);
  removalReject129=false;removalReadd129=true;
  check(!wht::RemoveAbilityFromUnit129(executableBase,selection,removalApi,0x41486862,removedAbility,&reason)&&reason&&!removedAbility);
  removalReadd129=false;removalReplace129=true;const auto refreshBeforeReplacement=removalRefreshes129;
  check(!wht::RemoveAbilityFromUnit129(executableBase,selection,removalApi,0x41486862,removedAbility,&reason)&&reason);
  check(removalRefreshes129==refreshBeforeReplacement);
  removalReplace129=false;*reinterpret_cast<uint32_t*>(invulUnit.data()+0x10)=22;
  const auto removeCalls=removalCalls129;
  check(!wht::RemoveAbilityFromUnit129(executableBase,{},removalApi,0x41486862,removedAbility,&reason));
  check(removalCalls129==removeCalls&&removalAbi129);
  check(!wht::ResolveRemoveAbilityApi129(executableBase));
  check(!wht::RemoveSelectedAbility129(0,0x41486862,removedAbility,&reason)&&reason);
  wht::TeleportApi129 teleportApi{InvulAgentMock129,reinterpret_cast<decltype(wht::TeleportApi129::stop)>(TeleportStopMock129),reinterpret_cast<decltype(wht::TeleportApi129::move)>(TeleportMoveMock129),reinterpret_cast<decltype(wht::TeleportApi129::position)>(TeleportReadMock129)};
  wht::TeleportPosition129 destination;
  check(wht::TeleportUnit129(executableBase,selection,teleportApi,100,-200,destination,&reason));
  check(destination.x==116&&destination.y==-216&&teleportStops129==1&&teleportMoves129==1&&teleportAbi129);
  check(!wht::TeleportUnit129(executableBase,selection,teleportApi,NAN,0,destination,&reason));
  check(!wht::TeleportUnit129(executableBase,selection,teleportApi,100001,0,destination,&reason));
  check(!wht::TeleportUnit129(executableBase,{},teleportApi,0,0,destination,&reason));
  check(teleportStops129==1&&teleportMoves129==1);
  teleportReplace129=true;
  check(!wht::TeleportUnit129(executableBase,selection,teleportApi,0,0,destination,&reason));
  check(teleportStops129==2&&teleportMoves129==1&&reason);
  teleportReplace129=false;*reinterpret_cast<uint32_t*>(invulUnit.data()+0x10)=22;
  check(!wht::ResolveTeleportApi129(executableBase));
  check(!wht::TeleportSelected129(0,0,0,destination,&reason)&&reason);
  const auto writes=invulWritesMock129;
  *reinterpret_cast<uint32_t*>(invulAgent.data()+0x20)=1;
  check(!wht::ChangeInvulnerability129(executableBase,selection,invulApi,true,result,&reason)&&invulWritesMock129==writes);
  *reinterpret_cast<uint32_t*>(invulAgent.data()+0x20)=0;
  *reinterpret_cast<uint32_t*>(invulUnit.data()+0x10)=23;
  check(!wht::ChangeInvulnerability129(executableBase,selection,invulApi,true,result,&reason)&&invulWritesMock129==writes);
  *reinterpret_cast<uint32_t*>(invulUnit.data()+0x10)=22;
  *reinterpret_cast<uintptr_t*>(invulUnit.data())=0;
  check(!wht::ChangeInvulnerability129(executableBase,selection,invulApi,true,result,&reason)&&invulWritesMock129==writes);
  check(!wht::ResolveInvulnerabilityApi129(executableBase));
  check(!wht::SetUnitInvulnerability129(0,true,result,&reason)&&reason);
  check(invulAbiMock129);portraitAddress=oldPortrait;
 }
 float unitScale129=0;
 check(wht::ParseUnitScale129(L"",unitScale129)&&unitScale129==1);
 check(wht::ParseUnitScale129(L"200",unitScale129)&&unitScale129==2);
 check(wht::ParseUnitScale129(L"0.5",unitScale129)&&unitScale129==.5f);
 check(wht::ParseUnitScale129(L"0",unitScale129)&&unitScale129==.05f);
 check(wht::ParseUnitScale129(L"99999",unitScale129)&&unitScale129==20);
 check(!wht::ParseUnitScale129(L"2 extra",unitScale129)&&!wht::ParseUnitScale129(L"abc",unitScale129));
 check(!wht::SetUnitScale129(0,1));
 uint32_t addCode129=0;int addLevel129=0;
 check(wht::ParseAddAbility129(L"AHhb",addCode129,addLevel129)&&addCode129==0x41486862&&addLevel129==1);
 check(wht::ParseAddAbility129(L"0x41486862 3",addCode129,addLevel129)&&addCode129==0x41486862&&addLevel129==3);
 for(auto invalid:{L"",L"AHhb 0",L"AHhb -1",L"AHhb 10001",L"AHhb abc",L"AHhb 2.5",L"AHhb 3 extra"})check(!wht::ParseAddAbility129(invalid,addCode129,addLevel129));
 uint32_t removeCode129=0;
 check(wht::ParseRemoveAbility129(L"AHhb",removeCode129)&&removeCode129==0x41486862);
 check(wht::ParseRemoveAbility129(L" 0x41486862 ",removeCode129)&&removeCode129==0x41486862);
 check(wht::ParseRemoveAbility129(L"0X41486a62",removeCode129)&&removeCode129==0x41486a62);
 for(auto invalid:{L"",L"AHh",L"AHhbb",L"AHhb extra",L"0x00000000",L"0x123",L"0x4148686z"})check(!wht::ParseRemoveAbility129(invalid,removeCode129));
 uint32_t abilityCode129=0;
 check(wht::AbilityCode129(L"AHhb",abilityCode129)&&abilityCode129==0x41486862);
 check(!wht::AbilityCode129(L"AHh",abilityCode129)&&!wht::AbilityCode129(L"AHhbb",abilityCode129));
 check(!wht::AbilityCode129(L"A Hh",abilityCode129));check(!wht::SetAbilityButton129(0,0,false));
 wht::InventoryInfo129 inventory129;
 check(wht::InventoryInfoText129(inventory129)=="Item Slots: [Empty]");
 inventory129.capacity=6;inventory129.codes[0]=0x49747331;inventory129.codes[5]=0x49747332;
 check(wht::InventoryInfoText129(inventory129)=="Item Slots: [0: Its1] [5: Its2] ");
 check(!wht::ReadInventoryInfo129(0,inventory129));
 wht::MousePosition129 mousePos129{12.5f,-24.f,128.f};
 check(wht::MousePositionText129(mousePos129,1600,900)=="X: 12.50000, Y: -24.00000, Z: 128.00000 | Res: 1600, 900");
 check(!wht::ReadMousePosition129(0,mousePos129));
 float panX129=0,panY129=0;
 check(wht::ParsePan129(L"2928 304",panX129,panY129)&&panX129==2928&&panY129==304);
 check(wht::ParsePan129(L"-3008 -5312",panX129,panY129)&&panX129==-3008&&panY129==-5312);
 check(!wht::ParsePan129(L"",panX129,panY129)&&!wht::ParsePan129(L"1",panX129,panY129));
 check(!wht::ParsePan129(L"1 2 extra",panX129,panY129)&&!wht::ParsePan129(L"70000 0",panX129,panY129));
 check(!wht::PanCamera129(0,0,0,0));
 check(wht::CursorFlagsText129(POINT{100,200},POINT{10,20},false,0)=="MouseXY: [100, 200], ClientXY: [10, 20]");
 check(wht::CursorFlagsText129(POINT{-100,200},POINT{10,20},true,0x800)=="MouseXY: [-100, 200], ClientXY: [10, 20] | Unit Flags: 0x00000800");
 uint32_t flags129=0;check(!wht::ReadUnitFlags129(0,flags129));
 check(wht::UnitCodeText129(0x4870616c)=="Unit ID: [Hpal] (0x4870616C)");
 check(wht::UnitPositionText129(12.5f,-24.f)=="X: 12.50000, Y: -24.00000");
 wht::UnitInfo129 missingUnit129;check(!wht::ReadUnitInfo129(0,true,missingUnit129));
 wht::RepeatChat129 repeat129;
 check(!repeat129.Set(1,0)&&!repeat129.Set(25,0));check(repeat129.Set(24,0));
 repeat129.Observe(0,0,"self");repeat129.Observe(23,0,"#cam 3000");check(repeat129.pending.empty());
 repeat129.Observe(23,0,"hello");check(repeat129.pending.size()==1);
 unsigned repeated129=0;auto sendRepeat129=[&](const std::string& text){check(text=="hello");++repeated129;return true;};
 repeat129.Tick(false,100,sendRepeat129);check(repeated129==0);
 repeat129.Tick(true,100,sendRepeat129);check(repeated129==1&&repeat129.pending.empty());
 repeat129.Observe(23,0,"hello");repeat129.Tick(true,349,sendRepeat129);check(repeated129==1);
 repeat129.Tick(true,350,sendRepeat129);check(repeated129==2);
 repeat129.ToggleAll();repeat129.Observe(2,0,"hello");check(repeat129.pending.size()==1);
 repeat129.Reset();check(repeat129.pending.empty()&&!repeat129.all&&repeat129.slot==-1);
 wht::Mute129 mute129;
 check(!mute129.Set(0,0,true)&&!mute129.Set(25,0,true)&&!mute129.Set(1,0,true));
 check(mute129.Set(24,0,true)&&mute129.Hidden(23,0)&&mute129.Hidden(23,2));
 check(!mute129.Hidden(24,0)&&!mute129.Hidden(23,3)&&!mute129.Hidden(23,-1));
 check(mute129.Set(24,0,false)&&!mute129.Hidden(23,0));
 check(mute129.Set(2,0,true));mute129.Reset();check(!mute129.Hidden(1,0));
 wht::FpsLimit129 limiter129;
 check(limiter129.Apply(0,true,fpsMock129)&&fpsMock129.calls==0);
 check(!limiter129.Apply(59,true,fpsMock129));
 check(limiter129.Apply(100,true,fpsMock129)&&fpsMock129.value==10);
 check(limiter129.Apply(100,true,fpsMock129)&&fpsMock129.calls==1);
 check(limiter129.Apply(0,true,fpsMock129)&&fpsMock129.value==16);
 limiter129.Apply(120,true,fpsMock129);fpsMock129.value=12;
 check(limiter129.Apply(0,true,fpsMock129)&&fpsMock129.value==12);
 fpsMock129.id=200;fpsMock129.value=20;
 limiter129.Apply(200,true,fpsMock129);
 check(limiter129.Apply(0,false,fpsMock129)&&fpsMock129.value==20);
 check(limiter129.Apply(0,true,fpsMock129,true)&&fpsMock129.value==0);
 check(limiter129.Apply(0,false,fpsMock129,true)&&fpsMock129.value==20);
 check(limiter129.Apply(144,true,fpsMock129,true)&&fpsMock129.value==7);
 check(limiter129.Apply(0,true,fpsMock129,true)&&fpsMock129.value==0);
 wht::FpsTimerState timerMock129;unsigned beginCount129=0,endCount129=0;
 auto timerBegin129=[&](unsigned ms){check(ms==1);++beginCount129;return 0u;};
 auto timerEnd129=[&](unsigned ms){check(ms==1);++endCount129;return 0u;};
 check(timerMock129.Apply(true,timerBegin129,timerEnd129));
 check(timerMock129.Apply(true,timerBegin129,timerEnd129)&&beginCount129==1);
 check(timerMock129.Apply(false,timerBegin129,timerEnd129)&&endCount129==1);

 std::array<unsigned char,0x190> menuRenderer{};
 auto& animationSpeed=*reinterpret_cast<float*>(menuRenderer.data()+0x188);
 animationSpeed=1.5f;wht::MenuSpeedLease129 menuSpeed;
 menuSpeed.Apply(reinterpret_cast<uintptr_t>(menuRenderer.data()),true);check(animationSpeed==100.f);
 menuSpeed.Apply(reinterpret_cast<uintptr_t>(menuRenderer.data()),true);
 menuSpeed.Apply(reinterpret_cast<uintptr_t>(menuRenderer.data()),false);check(animationSpeed==1.5f);
 menuSpeed.Apply(reinterpret_cast<uintptr_t>(menuRenderer.data()),true);animationSpeed=2.f;
 menuSpeed.Apply(reinterpret_cast<uintptr_t>(menuRenderer.data()),false);check(animationSpeed==2.f);
 check(wht::MenuDuration129(2.f,true)==.001f&&wht::MenuDuration129(2.f,false)==2.f);
 check(wht::MenuDuration129(0.f,true)==0.f&&wht::MenuDuration129(-1.f,true)==-1.f);
 check(std::isnan(wht::MenuDuration129(std::numeric_limits<float>::quiet_NaN(),true)));
 unsigned transitionCalls129=0;
 auto transitionMock129=[&](unsigned kind,unsigned sequence,unsigned a,unsigned b,uintptr_t callback,uintptr_t context){
  check(kind==4&&sequence==2&&a==3&&b==7&&callback==11&&context==12);++transitionCalls129;return 2.f;
 };
 check(wht::FastMenuAnimation129{}.speeds.size()==3);
 check(wht::RunMenuTransition129(true,transitionMock129,4,2,3,7,11,12)==.001f&&transitionCalls129==1);
 check(wht::RunMenuTransition129(false,transitionMock129,4,2,3,7,11,12)==2.f&&transitionCalls129==2);
 originalMenuTransition129=+[](unsigned,unsigned,unsigned,unsigned,uintptr_t,uintptr_t)->float{return 2.f;};
 check(ToolCommand(L"#fastmenu 1")&&runtimeConfig.fastMenu);
 check(!ToolCommand(L"#fastmenu 2")&&!ToolCommand(L"#fastmenu 0 extra"));
 check(ToolCommand(L"#fastmenu")&&!runtimeConfig.fastMenu);
 originalMenuTransition129=nullptr;check(!ToolCommand(L"#fastmenu 1"));
 check(ToolCommand(L"#mc 1")&&runtimeConfig.modeCast==1);
 check(!ToolCommand(L"#mc 3")&&!ToolCommand(L"#mc 1 extra"));
 check(ToolCommand(L"#modecast")&&runtimeConfig.modeCast==2);
 check(ToolCommand(L"#mc")&&runtimeConfig.modeCast==0);
 runtimeConfig.mode=wht::RunMode::Disabled;check(!ToolCommand(L"#tools"));
 DeleteFileW(ini);setCamera=nullptr;setFogReal=nullptr;executableBase=0;
 if(timer)KillTimer(nullptr,timer);
 if(argc==3){
  auto proxy=LoadLibraryW(argv[1]);auto real=LoadLibraryW(argv[2]);check(proxy&&real);
  using Count=unsigned(__stdcall*)();
  auto a=reinterpret_cast<Count>(GetProcAddress(proxy,"_AIL_ms_count@0"));
  auto b=reinterpret_cast<Count>(GetProcAddress(real,"_AIL_ms_count@0"));check(a&&b);
  auto before=b();auto forwarded=a();auto after=b();check(forwarded>=before&&forwarded<=after);
  for(unsigned i=1;i<=353;++i)check(GetProcAddress(proxy,MAKEINTRESOURCEA(i))!=nullptr);
 }
 if(inputHook)UnhookWindowsHookEx(inputHook);
 std::cout<<"PASS version129 label, camera/chat ABI, frame layout, menu settings and macro controls\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
