#pragma once
#include <cmath>
#include <array>
#include <cstdint>
namespace wht {
// 1.29 CModel::GetAnimationSpeed (563BC0) follows +20 -> +98,
// then the renderer getter 40E360 reads +48. Glue frames moved to +1A0/+1A4.
struct MenuSpeedLease129 {
 uintptr_t renderer=0;float saved=1;bool active=false;
 void Apply(uintptr_t current,bool enabled){
  if(renderer!=current){renderer=current;active=false;}
  if(!renderer)return;
  __try{
   auto& speed=*reinterpret_cast<float*>(renderer+0x48);
   if(enabled){
    if(!std::isfinite(speed)||speed<=0)return;
    if(!active||speed!=100.f)saved=speed;
    speed=100.f;active=true;
   }else if(active){if(speed==100.f)speed=saved;active=false;}
  }__except(EXCEPTION_EXECUTE_HANDLER){renderer=0;active=false;}
 }
};
struct FastMenuAnimation129 {
 std::array<MenuSpeedLease129,2> speeds{};
 void Apply(uintptr_t base,bool enabled){
  for(unsigned i=0;i<2;++i){
   uintptr_t renderer=0;
   __try{
    if(base){
     const auto root=*reinterpret_cast<uintptr_t*>(base+0xd3c8f4);
     if(root){
      const auto frame=*reinterpret_cast<uintptr_t*>(root+0x1a0+i*4);
      if(frame&&*reinterpret_cast<uintptr_t*>(frame+0x17c)){
       const auto holder=*reinterpret_cast<uintptr_t*>(frame+0x180);
       const auto model=holder?*reinterpret_cast<uintptr_t*>(holder):0;
       const auto handle=model?*reinterpret_cast<uintptr_t*>(model+0x20):0;
       renderer=handle?*reinterpret_cast<uintptr_t*>(handle+0x98):0;
      }
     }
    }
   }__except(EXCEPTION_EXECUTE_HANDLER){renderer=0;}
   speeds[i].Apply(renderer,enabled);
  }
 }
};
using MenuTransition129=float(__cdecl*)(unsigned,unsigned,unsigned,unsigned,uintptr_t,uintptr_t);
inline float MenuDuration129(float native,bool enabled){
 return enabled&&std::isfinite(native)&&native>.001f?.001f:native;
}
template<class Original> float RunMenuTransition129(bool enabled,Original original,unsigned kind,unsigned sequence,unsigned a,unsigned b,uintptr_t callback,uintptr_t context){
 return MenuDuration129(original(kind,sequence,a,b,callback,context),enabled);
}
}
