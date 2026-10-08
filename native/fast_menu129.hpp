#pragma once
#include <cmath>
#include <array>
#include <cstdint>
namespace wht {
// 1.29 CModel animation update 562B80 forwards +188 to 51CF10.
// The renderer +48 value is overwritten by that update.
struct MenuSpeedLease129 {
 uintptr_t renderer=0;float saved=1;bool active=false;
 void Apply(uintptr_t current,bool enabled){
  if(renderer!=current){renderer=current;active=false;}
  if(!renderer)return;
  __try{
   auto& speed=*reinterpret_cast<float*>(renderer+0x188);
   if(enabled){
    if(!std::isfinite(speed)||speed<=0)return;
    if(!active||speed!=100.f)saved=speed;
    speed=100.f;active=true;
   }else if(active){if(speed==100.f)speed=saved;active=false;}
  }__except(EXCEPTION_EXECUTE_HANDLER){renderer=0;active=false;}
 }
};
struct FastMenuAnimation129 {
 std::array<MenuSpeedLease129,3> speeds{};
 void Apply(uintptr_t base,bool enabled){
  for(unsigned i=0;i<speeds.size();++i){
   uintptr_t renderer=0;
   __try{
    if(base){
     const auto root=*reinterpret_cast<uintptr_t*>(base+0xd3c8f4);
     if(root){
      const auto frame=*reinterpret_cast<uintptr_t*>(root+0x19c+i*4);
      if(frame&&*reinterpret_cast<uintptr_t*>(frame+0x17c)){
       const auto holder=*reinterpret_cast<uintptr_t*>(frame+0x180);
       const auto model=holder?*reinterpret_cast<uintptr_t*>(holder):0;
       renderer=model;
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
