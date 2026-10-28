#pragma once
#include "fog_native_profile131.hpp"
#include "fog_modifier_lifecycle.hpp"
#include "remove_unit131.hpp"
#include "component_lookup131.hpp"
#include <sstream>
namespace wht {
inline const FogNativeProfile131* FogNativeProfileFor131(unsigned build){for(const auto& p:fogNativeProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool FogNativeCode131(uintptr_t base,const FogNativeProfile131& p){
 ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};const auto* rp=ResourceProfileFor131(p.build);if(!rp||!ResourceTextGuard131(base,code))return false;
 __try{return *reinterpret_cast<uintptr_t*>(base+p.fogTable+24*sizeof(uintptr_t))==base+p.configure&&*reinterpret_cast<uintptr_t*>(base+p.fogTable+23*sizeof(uintptr_t))==base+p.fogDelete&&*reinterpret_cast<uintptr_t*>(base+p.rectTable+23*sizeof(uintptr_t))==base+p.rectDelete&&*reinterpret_cast<uintptr_t*>(base+rp->playerTable+25*sizeof(uintptr_t))==base+p.ownerGetter;}__except(1){return false;}
}
using FogZeroNative131=uint32_t(__cdecl*)();
using FogCreateNative131=uint32_t(__cdecl*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
using FogActionNative131=void(__cdecl*)(uint32_t);
using FogResolveNative131=uintptr_t(__cdecl*)(uint32_t);
inline bool InvokeFogZero131(FogZeroNative131 fn,uint32_t& out){if(!fn)return false;__try{out=fn();return out!=0;}__except(1){return false;}}
inline bool InvokeFogCreate131(FogCreateNative131 fn,uint32_t player,uint32_t rectangle,uint32_t& out){if(!fn||!player||!rectangle)return false;__try{out=fn(player,4,rectangle,1,0);return out!=0;}__except(1){return false;}}
inline bool InvokeFogAction131(FogActionNative131 fn,uint32_t handle){if(!fn||!handle)return false;__try{fn(handle);return true;}__except(1){return false;}}
inline uintptr_t InvokeFogResolve131(FogResolveNative131 fn,uint32_t handle){if(!fn||!handle)return 0;__try{return fn(handle);}__except(1){return 0;}}
inline FogReferenceState FogAgentState131(uintptr_t base,unsigned build,uint32_t low,uint32_t high){
 const auto* p=RemoveUnitProfileFor131(build);if(!p||!RemoveUnitCode131(base,*p)||!(low||high))return FogReferenceState::Unknown;
 const auto node=ResolveRemoveUnitAgent131(base,*p,{low,high});if(!node)return FogReferenceState::Gone;
 __try{return *reinterpret_cast<uintptr_t*>(node+(sizeof(uintptr_t)==8?0x30:0x20))?FogReferenceState::Gone:FogReferenceState::Live;}__except(1){return FogReferenceState::Unknown;}
}
inline bool FogCoordinates131(const float* v){for(unsigned i=0;i<4;++i)if(!std::isfinite(v[i])||std::fabs(v[i])>1000000.f)return false;return v[0]<v[2]&&v[1]<v[3];}
struct FogObjectFields131 {uint32_t low=0,high=0,flags=0,mask=0;float coordinates[4]{};};
inline bool ReadFogObjectFields131(uintptr_t base,const FogNativeProfile131& p,uintptr_t object,bool rectangle,FogObjectFields131& out,bool geometry=true){
 __try{if(!ResourceIdentity131(object,base,rectangle?p.rectTable:p.fogTable))return false;const auto id=DropIdentity131(object);if(!id.Valid())return false;out.low=id.low;out.high=id.high;
  if(!rectangle){out.flags=*reinterpret_cast<uint32_t*>(object+(sizeof(uintptr_t)==8?0x38:0x20));out.mask=*reinterpret_cast<uint32_t*>(object+(sizeof(uintptr_t)==8?0x3c:0x24));}
  memcpy(out.coordinates,reinterpret_cast<void*>(object+(rectangle?(sizeof(uintptr_t)==8?0x40:0x24):(sizeof(uintptr_t)==8?0x50:0x30))),sizeof(out.coordinates));return !geometry||FogCoordinates131(out.coordinates);
 }__except(1){return false;}
}
inline bool ReadFogContext131(uintptr_t base,const FogNativeProfile131& p,uintptr_t ui,uint64_t generation,FogModifierContext& out,unsigned& physical){
 const auto* rp=ResourceProfileFor131(p.build);const auto* inv=InventoryProfile(p.build);const auto* np=DropNetworkProfileFor131(p.build);if(!rp||!inv||!np||!generation||!FogNativeCode131(base,p)||!ResourceCode131(base,*rp)||!ComponentIdentity131(ui,base+p.uiTable))return false;
 const auto network=DropNetworkNative131(base,*np,*inv);if(!network)return false;
 __try{FogModifierContext v;v.generation=generation;v.world=ResourceWorld131(base,*rp,v.local);v.session=DropSession131(network);if(!v.world||!v.session)return false;
  v.player=reinterpret_cast<decltype(ResourceNative131::player)>(base+rp->playerFn)(v.world,v.local);if(!ResourceIdentity131(v.player,base,rp->playerTable)||*reinterpret_cast<int*>(v.player+rp->statusOffset)!=1)return false;
  physical=*reinterpret_cast<unsigned char*>(v.player+rp->idOffset);if(physical>=24||!v.Valid())return false;unsigned local=24;if(ResourceWorld131(base,*rp,local)!=v.world||local!=v.local||DropSession131(network)!=v.session)return false;out=v;return true;
 }__except(1){return false;}
}
class FogNativeBackend131 {
 uintptr_t base_,ui_;uint64_t generation_;const FogNativeProfile131* p_;
 bool Context(const FogModifierContext& expected,bool player){FogModifierContext now;return ReadContext(now)&&(player?SameFogContext(expected,now):SameFogWorld(expected,now));}
 FogResolveNative131 Resolver(bool rectangle)const{return reinterpret_cast<FogResolveNative131>(base_+(rectangle?p_->rect:p_->fog));}
 bool Fields(const FogModifierContext& expected,const FogModifierReference& owned,bool rectangle,FogObjectFields131& out){return Context(expected,false)&&Inspect(expected,owned,rectangle)==FogReferenceState::Live&&ReadFogObjectFields131(base_,*p_,owned.object,rectangle,out,false)&&out.low==owned.serialLow&&out.high==owned.serialHigh;}
 bool Fill(const FogModifierContext& expected,bool rectangle,FogModifierReference& out){
  if(!Context(expected,true)||!out.handle)return false;const auto object=InvokeFogResolve131(Resolver(rectangle),out.handle);FogObjectFields131 fields;
  if(!object||!ReadFogObjectFields131(base_,*p_,object,rectangle,fields,false)||FogAgentState131(base_,p_->build,fields.low,fields.high)!=FogReferenceState::Live||InvokeFogResolve131(Resolver(rectangle),out.handle)!=object||!Context(expected,true))return false;
  out.object=object;out.serialLow=fields.low;out.serialHigh=fields.high;out.ownerMask=fields.mask;memcpy(out.coordinates,fields.coordinates,sizeof(out.coordinates));return FogCoordinates131(out.coordinates);
 }
 bool Configuration(const FogModifierContext& expected,const FogModifierReference& owned,bool active){FogObjectFields131 fields;if(!Fields(expected,owned,false,fields)||!FogCoordinates131(fields.coordinates)||!FogCoordinates131(owned.coordinates))return false;return owned.ownerMask&&!(owned.ownerMask&(owned.ownerMask-1))&&owned.ownerMask<0x1000000&&fields.mask==owned.ownerMask&&(fields.flags&7)==4&&(fields.flags&0x80)&&!(fields.flags&0x100)&&bool(fields.flags&0x40)==active&&!memcmp(fields.coordinates,owned.coordinates,sizeof(owned.coordinates));}
 bool PlayerHandle(const FogModifierContext& expected,uint32_t& handle){
  if(!Context(expected,true)||!InvokeFogZero131(reinterpret_cast<FogZeroNative131>(base_+p_->local),handle))return false;
  return InvokeFogResolve131(reinterpret_cast<FogResolveNative131>(base_+p_->player),handle)==expected.player&&Context(expected,true);
 }
public:
 FogNativeBackend131(uintptr_t base,unsigned build,uintptr_t ui,uint64_t generation):base_(base),ui_(ui),generation_(generation),p_(FogNativeProfileFor131(build)){}
 bool ReadContext(FogModifierContext& out){unsigned physical=24;return p_&&ReadFogContext131(base_,*p_,ui_,generation_,out,physical);}
 FogReferenceState Inspect(const FogModifierContext& expected,const FogModifierReference& owned,bool rectangle){
  if(!p_||!owned.Identified()||!Context(expected,false))return FogReferenceState::Unknown;
  const auto state=FogAgentState131(base_,p_->build,owned.serialLow,owned.serialHigh);if(state!=FogReferenceState::Live)return state;
  const auto object=InvokeFogResolve131(Resolver(rectangle),owned.handle);FogObjectFields131 fields;
  if(object!=owned.object||!ReadFogObjectFields131(base_,*p_,object,rectangle,fields,false)||fields.low!=owned.serialLow||fields.high!=owned.serialHigh||!Context(expected,false))return FogReferenceState::Unknown;
  return FogReferenceState::Live;
 }
 bool CreateBounds(const FogModifierContext& expected,FogModifierReference& out){uint32_t player=0;if(!PlayerHandle(expected,player)||!InvokeFogZero131(reinterpret_cast<FogZeroNative131>(base_+p_->bounds),out.handle))return false;return Fill(expected,true,out);}
 bool CreateModifier(const FogModifierContext& expected,const FogModifierReference& rectangle,FogModifierReference& out){
  uint32_t player=0;FogObjectFields131 bounds;FogModifierContext current;unsigned physical=24;
  if(!PlayerHandle(expected,player)||!Fields(expected,rectangle,true,bounds)||!ReadFogContext131(base_,*p_,ui_,generation_,current,physical)||!SameFogContext(expected,current))return false;
  if(!InvokeFogCreate131(reinterpret_cast<FogCreateNative131>(base_+p_->create),player,rectangle.handle,out.handle)||!Fill(expected,false,out))return false;
  const auto mask=uint32_t(1)<<physical;const bool config=out.ownerMask==mask&&!memcmp(out.coordinates,bounds.coordinates,sizeof(out.coordinates));
  out.ownerMask=mask;memcpy(out.coordinates,bounds.coordinates,sizeof(out.coordinates));return config&&Configuration(expected,out,false);
 }
 bool Active(const FogModifierContext& expected,const FogModifierReference& owned){return Context(expected,true)&&Configuration(expected,owned,true);}
 bool Start(const FogModifierContext& expected,const FogModifierReference& owned){return Context(expected,true)&&Configuration(expected,owned,false)&&InvokeFogAction131(reinterpret_cast<FogActionNative131>(base_+p_->start),owned.handle)&&Active(expected,owned);}
 bool Stop(const FogModifierContext& expected,const FogModifierReference& owned){
  FogObjectFields131 fields;if(!Fields(expected,owned,false,fields))return false;if(!(fields.flags&0x40))return true;
  return InvokeFogAction131(reinterpret_cast<FogActionNative131>(base_+p_->stop),owned.handle)&&Fields(expected,owned,false,fields)&&!(fields.flags&0x40);
 }
 bool Destroy(const FogModifierContext& expected,const FogModifierReference& owned,bool rectangle){return Inspect(expected,owned,rectangle)==FogReferenceState::Live&&InvokeFogAction131(reinterpret_cast<FogActionNative131>(base_+(rectangle?p_->removeRect:p_->destroy)),owned.handle);}
};
inline bool ParseFogToggle131(const std::wstring& args,bool active,bool& out){if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;std::wistringstream in(args);in>>std::ws;if(in.eof()){out=!active;return true;}int value=0;std::wstring extra;if(!(in>>value)||(in>>extra))return false;out=value!=0;return true;}
class FogModifierFeature131 {
 FogModifierLifecycle lifecycle_;uintptr_t ui_=0;uint64_t generation_=1;bool wasAllowed_=false;
public:
 void UiChanged(uintptr_t base,unsigned build,uintptr_t ui){if(ui==ui_)return;if(ui_&&lifecycle_.HasResources()){FogNativeBackend131 backend(base,build,ui_,generation_);std::string ignored;lifecycle_.Request(false,backend,ignored);}lifecycle_.OnWorldGone();if(!++generation_)++generation_;ui_=ui;wasAllowed_=false;}
 bool Command(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,const std::wstring& args,std::string& report){
  bool next=false;if(!ParseFogToggle131(args,ui==ui_&&lifecycle_.Phase()==FogModifierPhase::Active,next)){report="Usage: #mh [0|1]";return false;}UiChanged(base,build,ui);
  if(next&&!allowed){report="Maphack: settings or session disabled";return false;}FogNativeBackend131 backend(base,build,ui_,generation_);return lifecycle_.Request(next,backend,report);
 }
 bool Update(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,std::string& report){UiChanged(base,build,ui);const bool cleanup=wasAllowed_&&!allowed&&lifecycle_.HasResources();wasAllowed_=allowed;if(!cleanup)return true;FogNativeBackend131 backend(base,build,ui_,generation_);return lifecycle_.Request(false,backend,report);}
};
}
