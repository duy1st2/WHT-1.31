#pragma once
#include "menu_fog_vertex_profile131.hpp"
#include "component_lookup131.hpp"
#include "quick_drop_network131.hpp"
#include "auto_visibility_profile131.hpp"
#include "notification131.hpp"
#include "fast_menu_transition131.hpp"
#include <mutex>
#include <deque>
namespace wht {
// Only exchange the exact owned callback. Never overwrite another module's slot.
struct NativeDataSlot131 {
 uintptr_t address=0,original=0,replacement=0;
 bool Swap(uintptr_t expected,uintptr_t desired){
  if(!address||address%alignof(uintptr_t))return false;
  __try{if(*reinterpret_cast<uintptr_t*>(address)!=expected)return false;DWORD old=0;if(!VirtualProtect(reinterpret_cast<void*>(address),sizeof(uintptr_t),PAGE_READWRITE,&old))return false;
   const bool changed=InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(address),reinterpret_cast<void*>(desired),reinterpret_cast<void*>(expected))==reinterpret_cast<void*>(expected);
   DWORD ignored=0;const bool protectedAgain=VirtualProtect(reinterpret_cast<void*>(address),sizeof(uintptr_t),old,&ignored)!=FALSE;
   if(!protectedAgain&&changed)InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(address),reinterpret_cast<void*>(expected),reinterpret_cast<void*>(desired));return changed&&protectedAgain;
  }__except(1){return false;}
 }
 bool Install(uintptr_t at,uintptr_t from,uintptr_t to){if(address){__try{return address==at&&original==from&&replacement==to&&*reinterpret_cast<uintptr_t*>(address)==to;}__except(1){return false;}}address=at;original=from;replacement=to;if(Swap(from,to))return true;address=original=replacement=0;return false;}
 bool Restore(){if(!address)return true;if(!Swap(replacement,original))return false;address=original=replacement=0;return true;}
};
struct ScopedVertexAlpha131 {
 uintptr_t object=0,table=0,offset=0;unsigned char saved=0;
 ScopedVertexAlpha131(uintptr_t unit,uintptr_t expected,unsigned alpha,bool enabled){
  if(!enabled)return;__try{if(!unit||*reinterpret_cast<uintptr_t*>(unit)!=expected)return;auto& value=*reinterpret_cast<unsigned char*>(unit+alpha);if(value==255)return;object=unit;table=expected;offset=alpha;saved=value;value=255;}__except(1){object=0;}
 }
 void Restore(){__try{if(object&&*reinterpret_cast<uintptr_t*>(object)==table){auto& value=*reinterpret_cast<unsigned char*>(object+offset);if(value==255)value=saved;}}__except(1){}object=0;}
 ~ScopedVertexAlpha131(){Restore();}
};
inline uintptr_t vertexOriginal131=0,vertexTable131=0;inline unsigned vertexAlpha131=0;
inline std::atomic<bool> vertexEnabled131=false;
inline uintptr_t InvokeVertexColor131(uintptr_t unit,ScopedVertexAlpha131* alpha){__try{return reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(vertexOriginal131)(unit);}__finally{alpha->Restore();}}
#ifdef _WIN64
inline uintptr_t VertexColorCall131(uintptr_t unit){
#else
inline uintptr_t __fastcall VertexColorCall131(uintptr_t unit,void*){
#endif
 ScopedVertexAlpha131 alpha(unit,vertexTable131,vertexAlpha131,vertexEnabled131.load());
 return InvokeVertexColor131(unit,&alpha);
}
struct MenuModelSpeed131 {
 uintptr_t model=0;float saved=1;bool active=false;
 void Apply(uintptr_t current,unsigned offset,bool enabled){
  if(current!=model){model=current;active=false;}if(!model)return;
  __try{auto& speed=*reinterpret_cast<float*>(model+offset);if(enabled){if(!std::isfinite(speed)||speed<=0)return;if(!active||speed!=100.f)saved=speed;speed=100.f;active=true;}else if(active){if(speed==100.f)speed=saved;active=false;}}__except(1){model=0;active=false;}
 }
};
inline uintptr_t glueOriginal131=0,glueBase131=0,glueOwner131=0;inline const MenuFogVertexProfile131* glueProfile131=nullptr;
inline std::atomic<bool> glueEnabled131=false;inline DWORD glueThread131=0;
inline std::array<MenuModelSpeed131,3> glueSpeeds131{};
inline uintptr_t GlueModel131(uintptr_t root,unsigned slot){
 if(!glueProfile131||slot>=3)return 0;__try{if(!root||*reinterpret_cast<uintptr_t*>(root)!=glueBase131+glueProfile131->glueTable)return 0;
  const auto frame=*reinterpret_cast<uintptr_t*>(root+glueProfile131->frames+slot*sizeof(uintptr_t));if(!frame)return 0;
  using Get=uintptr_t(__thiscall*)(uintptr_t);return reinterpret_cast<Get>(glueBase131+glueProfile131->modelGet)(frame);
 }__except(1){return 0;}
}
inline void ApplyGlueSpeeds131(uintptr_t root,bool enabled){if(GetCurrentThreadId()!=glueThread131||!glueProfile131)return;glueOwner131=root;for(unsigned i=0;i<3;++i)glueSpeeds131[i].Apply(GlueModel131(root,i),unsigned(glueProfile131->speed),enabled);}
#ifdef _WIN64
inline uintptr_t GlueUpdateCall131(uintptr_t root,float elapsed,float time,uintptr_t context){
#else
inline uintptr_t __fastcall GlueUpdateCall131(uintptr_t root,void*,float elapsed,float time,uintptr_t context){
#endif
 ApplyGlueSpeeds131(root,glueEnabled131.load());const auto result=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,float,float,uintptr_t)>(glueOriginal131)(root,elapsed,time,context);ApplyGlueSpeeds131(root,glueEnabled131.load());return result;
}
struct FogClickEvent131 {unsigned slot=24;DropObjectId id{};uintptr_t world=0;uint64_t time=0;};
struct FogPacket131 {unsigned slot=24,action=0,count=0;std::array<DropObjectId,12> ids{};};
inline bool FogPacketAccept131(unsigned action,unsigned slot,unsigned count){return (action==1||action==3)&&slot<24&&count>0&&count<=12;}
inline bool ReadFogPacket131(uintptr_t packet,FogPacket131& out){
 __try{constexpr bool wide=sizeof(uintptr_t)==8;if(!ReadableComponent131(packet,wide?0x48:0x24))return false;out.slot=*reinterpret_cast<unsigned char*>(packet+(wide?0x29:0x15));out.action=*reinterpret_cast<unsigned char*>(packet+(wide?0x30:0x18));out.count=*reinterpret_cast<unsigned*>(packet+(wide?0x38:0x1c));
  if(!FogPacketAccept131(out.action,out.slot,out.count))return false;const auto list=*reinterpret_cast<uintptr_t*>(packet+(wide?0x40:0x20));if(!ReadableComponent131(list,out.count*sizeof(DropObjectId)))return false;memcpy(out.ids.data(),reinterpret_cast<void*>(list),out.count*sizeof(DropObjectId));return true;
 }__except(1){return false;}
}
inline std::atomic<uintptr_t> fogWorld131=0;inline std::mutex fogMutex131;inline std::deque<FogClickEvent131> fogEvents131;
// Additive observer: never sends an order, mutates a packet, or replaces its native handler.
inline int __cdecl FogClickCapture131(void* packet){
 const auto world=fogWorld131.load();if(!world)return 1;FogPacket131 value;if(!ReadFogPacket131(reinterpret_cast<uintptr_t>(packet),value))return 1;
 const auto now=GetTickCount64();std::lock_guard<std::mutex> lock(fogMutex131);if(fogWorld131.load()!=world)return 1;
 for(unsigned i=0;i<value.count&&fogEvents131.size()<96;++i)if(value.ids[i].Valid())fogEvents131.push_back({value.slot,value.ids[i],world,now});return 1;
}
inline void ClearFogEvents131(){fogWorld131=0;std::lock_guard<std::mutex> lock(fogMutex131);fogEvents131.clear();}
struct FogRecent131 {
 std::vector<FogClickEvent131> items;
 bool Accept(const FogClickEvent131& e,uint64_t now){items.erase(std::remove_if(items.begin(),items.end(),[&](const auto& a){return now-a.time>=5000;}),items.end());for(const auto& a:items)if(a.world==e.world&&a.slot==e.slot&&a.id==e.id&&now-a.time<2000)return false;if(items.size()>=96)items.erase(items.begin());auto copy=e;copy.time=now;items.push_back(copy);return true;}
};
inline bool FogAlertPolicy131(unsigned physical,unsigned owner,unsigned clicker,bool shared,unsigned flags,unsigned unitMask,unsigned visible){
 if(physical>=24||owner>=24||clicker>=24||owner==clicker||shared||(flags&0x30))return false;
 const unsigned bit=1u<<physical;return !(unitMask&bit)||!(visible&bit);
}
using FogPosition131=uintptr_t(__thiscall*)(uintptr_t,float*);
inline bool FogPoint131(FogPosition131 position,uintptr_t unit,float* xyz){if(!position||!unit||!xyz)return false;__try{position(unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);return std::isfinite(xyz[0])&&std::isfinite(xyz[1]);}__except(1){return false;}}
inline bool FogHiddenNative131(uintptr_t base,unsigned build,const MenuFogVertexProfile131& p,const FogClickEvent131& e,unsigned& owner){
 const auto* rp=ResourceProfileFor131(build);const auto* inv=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);const AutoVisibilityProfile131* vp=nullptr;for(const auto& v:autoVisibilityProfiles131)if(v.build==build)vp=&v;
 if(!rp||!inv||!np||!vp||!ResourceCode131(base,*rp))return false;const auto net=DropNetworkNative131(base,*np,*inv);if(!net)return false;
 __try{
  unsigned local=24;const auto world=ResourceWorld131(base,*rp,local);if(!world||world!=e.world||e.slot>=24)return false;
  using Resolve=uintptr_t(__cdecl*)(unsigned,unsigned);const auto agent=reinterpret_cast<Resolve>(base+p.resolve)(e.id.low,e.id.high);if(!agent||*reinterpret_cast<unsigned*>(agent+(sizeof(void*)==8?0x18:0xc))!=0x2b61676c)return false;
  const auto unit=*reinterpret_cast<uintptr_t*>(agent+(sizeof(void*)==8?0x90:0x54));if(!InventoryIdentity131(unit,base,inv->unitTable)||!(DropIdentity131(unit)==e.id))return false;
  const unsigned flags=*reinterpret_cast<unsigned*>(unit+(sizeof(void*)==8?0x38:0x20));if(flags&0x30)return false;
  owner=net.owner(unit);if(owner>=24||owner==e.slot)return false;
  auto player=reinterpret_cast<decltype(ResourceNative131::player)>(base+rp->playerFn);auto integer=reinterpret_cast<decltype(ResourceNative131::integer)>(base+rp->integerFn);
  const auto clicker=player(world,e.slot);if(!ResourceIdentity131(clicker,base,rp->playerTable))return false;const unsigned physical=*reinterpret_cast<unsigned char*>(clicker+rp->idOffset);if(physical>=24)return false;
  unsigned views[25]{};views[0]=24;for(unsigned i=0;i<24;++i)views[i+1]=1u<<i;unsigned vision=1u<<physical;
  // Integer array kind 5 is shared vision; include each grantor's fog layer.
  for(unsigned i=0;i<24;++i){const auto source=player(world,i);if(!ResourceIdentity131(source,base,rp->playerTable))continue;const unsigned id=*reinterpret_cast<unsigned char*>(source+rp->idOffset);if(id>=24)continue;const auto alliances=*reinterpret_cast<uintptr_t*>(source+rp->allianceOffset);if(!ResourceIdentity131(alliances,base,rp->allianceTable))return false;
   const unsigned shared=unsigned(integer(alliances+rp->allianceMask+5*rp->stateStride));if(shared&(1u<<physical)){if(i==owner)return false;vision|=1u<<id;}}
  views[physical+1]=vision;const auto unitMask=*reinterpret_cast<unsigned*>(unit+(sizeof(void*)==8?0x50:0x2c));if(!(unitMask&(1u<<physical)))return FogAlertPolicy131(physical,owner,e.slot,false,flags,unitMask,~0u);
  float xyz[3]={NAN,NAN,0};if(!FogPoint131(net.position,unit,xyz))return false;
  unsigned visible=~0u,explored=~0u;using Query=void(__thiscall*)(uintptr_t,const float*,unsigned*,unsigned*,const unsigned*);reinterpret_cast<Query>(base+vp->query)(world,xyz,&visible,&explored,views);
  return FogAlertPolicy131(physical,owner,e.slot,false,flags,unitMask,visible);
 }__except(1){return false;}
}
struct MenuFogVertex131 {
 FastMenuDurationService131 duration;
 uintptr_t base=0;const MenuFogVertexProfile131* profile=nullptr;bool checked=false,valid=false,visionValid=false,registered=false;uintptr_t session=0;NativeDataSlot131 colorSlot,glueSlot;FogRecent131 recent;ULONGLONG next=0;bool warned=false;unsigned lastReported=~0u;
 bool Register(bool enabled,uintptr_t world){
  if(!profile)return false;
  if(registered&&(!enabled||world!=session)){ClearFogEvents131();__try{using Fn=void(__cdecl*)(unsigned char,void*,void*);reinterpret_cast<Fn>(base+profile->unreg)(0x16,reinterpret_cast<void*>(FogClickCapture131),nullptr);}__except(1){return false;}registered=false;session=0;recent.items.clear();}
  if(enabled&&!registered){__try{using Fn=void(__cdecl*)(unsigned char,void*,void*,unsigned);reinterpret_cast<Fn>(base+profile->reg)(0x16,reinterpret_cast<void*>(FogClickCapture131),nullptr,0);}__except(1){return false;}registered=true;session=world;fogWorld131=world;}
  if(enabled&&registered&&session==world)fogWorld131=world;return true;
 }
 template<class Notify> void Update(uintptr_t image,unsigned build,bool allowed,bool menuAllowed,uintptr_t gameUi,const Config& c,const std::wstring& folder,Notify notify){
  const auto now=GetTickCount64();if(!checked){base=image;profile=MenuFogVertexProfileFor131(build);valid=profile&&MenuFogVertexGuard131(base,*profile);checked=true;
   for(const auto& v:autoVisibilityProfiles131)if(v.build==build){ResourceTextProfile131 guard{};guard.code=v.code;guard.count=v.count;visionValid=ResourceTextGuard131(base,guard);}if(!valid)Log(folder,"WHT: VertexColor/Fast Menu/Fog Click profile refused.");}
  duration.Update(base,build,menuAllowed&&c.fastMenu,folder);
  if(!valid)return;
  vertexEnabled131=allowed&&c.vertexColor;glueEnabled131=menuAllowed&&c.fastMenu;
  bool ok=true;
  if(vertexEnabled131){vertexOriginal131=base+profile->color;vertexTable131=base+profile->unitTable;vertexAlpha131=unsigned(profile->alpha);ok=colorSlot.Install(base+profile->unitTable+43*sizeof(uintptr_t),vertexOriginal131,reinterpret_cast<uintptr_t>(VertexColorCall131))&&ok;}else ok=colorSlot.Restore()&&ok;
  if(glueEnabled131){glueBase131=base;glueProfile131=profile;glueOriginal131=base+profile->glueUpdate;glueThread131=GetCurrentThreadId();ok=glueSlot.Install(base+profile->glueTable+12*sizeof(uintptr_t),glueOriginal131,reinterpret_cast<uintptr_t>(GlueUpdateCall131))&&ok;}
  else{ApplyGlueSpeeds131(glueOwner131,false);ok=glueSlot.Restore()&&ok;}
  unsigned local=24;auto rp=ResourceProfileFor131(build);const auto world=allowed&&c.fogClick&&visionValid&&gameUi&&rp?ResourceWorld131(base,*rp,local):0;
  const auto np=DropNetworkProfileFor131(build);const auto inv=InventoryProfile(build);const auto net=np&&inv?DropNetworkNative131(base,*np,*inv):DropNetworkApi131{};
  const bool active=world&&net&&DropSession131(net);ok=Register(active,world)&&ok;
  const unsigned live=(vertexEnabled131&&colorSlot.address?1u:0u)|(glueEnabled131&&glueSlot.address?2u:0u)|(registered?4u:0u);if(live!=lastReported){lastReported=live;Log(folder,"WHT: VertexColor callback="+std::to_string(live&1?1:0)+", Fast Menu callback="+std::to_string(live&2?1:0)+", Fog Click observer request="+std::to_string(live&4?1:0));}
  if(!ok&&!warned){Log(folder,"WHT: native callback/observer request refused; requested features may be unavailable.");warned=true;}if(ok)warned=false;
  if(now<next)return;next=now+100;std::deque<FogClickEvent131> events;{std::lock_guard<std::mutex> lock(fogMutex131);events.swap(fogEvents131);}
  if(!active){recent.items.clear();return;}
  unsigned budget=8;for(const auto& e:events){if(!budget)break;--budget;if(e.world!=world||now-e.time>1000)continue;unsigned owner=24;if(FogHiddenNative131(base,build,*profile,e,owner)&&recent.Accept(e,now))notify("Fog Click: Player "+std::to_string(e.slot+1)+" selected a hidden unit of Player "+std::to_string(owner+1));}
 }
 void ResetMatch(){vertexEnabled131=false;colorSlot.Restore();Register(false,0);ClearFogEvents131();recent.items.clear();next=0;}
 void Restore(){duration.Restore();ResetMatch();glueEnabled131=false;ApplyGlueSpeeds131(glueOwner131,false);glueSlot.Restore();}
};
}
