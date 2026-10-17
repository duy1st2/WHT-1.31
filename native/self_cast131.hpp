#pragma once
#include "quickcast131.hpp"
#include "quick_drop_network131.hpp"
namespace wht {
struct SelfCastProfile131 {unsigned build;uintptr_t cancel,submit,sender;};
#ifdef _WIN64
inline constexpr SelfCastProfile131 selfCastProfiles131[]={{12164,0xc5c290,0xcbbf70,0xdd8190},{12173,0xc9f5b0,0xcfbf80,0xe14280}};
#else
inline constexpr SelfCastProfile131 selfCastProfiles131[]={{12164,0xb0fbd0,0xb663a0,0xc5fce0},{12173,0xacf750,0xb260d0,0xc1ae40}};
#endif
struct SelfCastApi131 {
 void(__thiscall* cancel)(uintptr_t)=nullptr;
 void(__thiscall* submit)(uintptr_t,unsigned,uintptr_t,float,float,uintptr_t,unsigned,unsigned)=nullptr;
 explicit operator bool()const{return cancel&&submit;}
};
inline SelfCastApi131 SelfCastNative131(uintptr_t base,unsigned build){
 const SelfCastProfile131* p=nullptr;for(const auto& profile:selfCastProfiles131)if(profile.build==build)p=&profile;if(!base||!p)return {};
 __try{
#ifdef _WIN64
  const unsigned char cancel[]={0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x20,0x33,0xf6,0x48,0x8d,0x99,0x20,3,0,0,0x8b,3};
  const unsigned char submit[]={0x40,0x53,0x48,0x83,0xec,0x30,0x44,0x8b,0x4c,0x24,0x78,0x8b,0xda};
  const unsigned char end[]={0x48,0x83,0xc4,0x30,0x5b,0xc3};
  constexpr uintptr_t callOffset=0xc1,endOffset=0xc6;
#else
  const unsigned char cancel[]={0x55,0x8b,0xec,0x51,0x8b,0x91,0xc8,1,0,0,0x33,0xc0,0x56,0x57,0x8d,0xb9,0xc8,1,0,0};
  const unsigned char submit[]={0x55,0x8b,0xec,0x8b,0x55,0x20,0x8b,0xc2,0x56,0xc1,0xe8,4};
  const unsigned char end[]={0x83,0xc4,0x18,0x5e,0x5d,0xc2,0x1c,0};
  constexpr uintptr_t callOffset=0x89,endOffset=0x8e;
#endif
  if(memcmp(reinterpret_cast<void*>(base+p->cancel),cancel,sizeof(cancel))||memcmp(reinterpret_cast<void*>(base+p->submit),submit,sizeof(submit))||memcmp(reinterpret_cast<void*>(base+p->submit+endOffset),end,sizeof(end)))return {};
  const auto call=base+p->submit+callOffset;
  if(*reinterpret_cast<unsigned char*>(call)!=0xe8||call+5+*reinterpret_cast<int32_t*>(call+1)!=base+p->sender)return {};
  return {reinterpret_cast<decltype(SelfCastApi131::cancel)>(base+p->cancel),reinterpret_cast<decltype(SelfCastApi131::submit)>(base+p->submit)};
 }__except(1){return {};}
}
struct SelfCastSource131 {uintptr_t ui=0,unit=0,session=0;DropObjectId id{};float x=0,y=0;};
inline bool SameSelfCastSource131(const SelfCastSource131& a,const SelfCastSource131& b){return a.ui&&a.ui==b.ui&&a.unit&&a.unit==b.unit&&a.session&&a.session==b.session&&a.id.Valid()&&a.id==b.id;}
template<class Selected,class Alive> bool ReadSelfCastSource131(uintptr_t base,unsigned build,uintptr_t ui,Selected selected,Alive alive,SelfCastSource131& out){
 const auto* p=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(!ui||!p||!np)return false;
 const auto api=DropNetworkNative131(base,*np,*p);if(!api)return false;
 const auto unit=selected();if(!InventoryIdentity131(unit,base,p->unitTable)||!alive(unit)||!DropOwned131(base,build,api,unit))return false;
 __try{
  const auto id=DropIdentity131(unit);const auto session=DropSession131(api);if(!id.Valid()||!session)return false;
  float xyz[3]{};api.position(unit+(sizeof(uintptr_t)==8?0x358:0x284),xyz);
  if(!std::isfinite(xyz[0])||!std::isfinite(xyz[1])||selected()!=unit)return false;
  out={ui,unit,session,id,xyz[0],xyz[1]};return true;
 }__except(1){return false;}
}
inline unsigned SelfCastOrder131(uintptr_t mode){__try{return mode?*reinterpret_cast<unsigned*>(mode+(sizeof(uintptr_t)==8?0x1c:0x10)):0;}__except(1){return 0;}}
inline bool FinishSelfCast131(const SelfCastApi131& api,const SelfCastSource131& source,uintptr_t mode,unsigned order){
 if(!api||!source.ui||!source.unit||!mode||!order)return false;
 __try{
  // Preserve mode/order before pop: cancellation may clear mode state.
  api.cancel(source.ui);api.submit(mode,order,0,source.x,source.y,source.unit,1,0);return true;
 }__except(1){return false;}
}
template<class Read,class Target,class Click,class Order,class Finish> bool ActivateSelfCast131(Read read,Target target,Click click,Order order,Finish finish){
 if(target())return false;
 SelfCastSource131 before{},after{};if(!read(before)||!click()||!read(after)||!SameSelfCastSource131(before,after))return false;
 const auto mode=target();if(!mode)return true; // instant/toggle already performed native activation
 const unsigned saved=order(mode);return saved&&finish(after,mode,saved);
}
}
