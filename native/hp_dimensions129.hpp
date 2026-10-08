#pragma once
#include "native_ui129.hpp"
namespace wht {
struct HpDimensions129 {
 struct Dimension{float original=0,written=0;bool active=false;};
 struct Entry{std::array<Dimension,2> dimensions{};};
 std::unordered_map<uintptr_t,Entry> entries;
 float width=0,height=0;
 void Forget(uintptr_t bar){entries.erase(bar);}
 static bool WorldHp(uintptr_t base,uintptr_t bar){
  __try{return bar&&*reinterpret_cast<uintptr_t*>(bar)==base+0xa95104&&*reinterpret_cast<unsigned*>(bar+0x13c)==5;}
  __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 template<class Api> void Update(uintptr_t base,uintptr_t bar,Api& api){
  if(!WorldHp(base,bar))return;
  auto found=entries.find(bar);
  if(found==entries.end()){
   if((width<=0&&height<=0)||entries.size()>=4096)return;
   found=entries.emplace(bar,Entry{}).first;
  }
  const float values[]={width,height};
  for(unsigned n=0;n<2;++n){
   auto& d=found->second.dimensions[n];
   const float current=*reinterpret_cast<float*>(bar+0x58+n*4);
   if(!std::isfinite(current)||current<0||current>100)continue;
   const float desired=values[n];
   auto setter=n?api.setHeight:api.setWidth;
   if(std::isfinite(desired)&&desired>0&&desired<=100){
    if(!d.active||current!=d.written)d.original=current;
    d.active=true;d.written=desired;
    if(current!=desired)setter(reinterpret_cast<void*>(bar),desired);
   }else if(d.active){
    // Preserve changes made by the game/map after the last tool write.
    if(current==d.written)setter(reinterpret_cast<void*>(bar),d.original);
    d.active=false;
   }
  }
 }
 template<class Api> void Apply(uintptr_t base,float requestedWidth,float requestedHeight,Api& api){
  width=requestedWidth;height=requestedHeight;
  for(auto& pair:entries)Update(base,pair.first,api);
 }
};
}
