#pragma once
#include "copy_unit129.hpp"
#include "drop_items129.hpp"
namespace wht {
struct InventoryActionsApi129 {
 using Getter=uint32_t(__cdecl*)(uint32_t);
 void(__cdecl* query)(const float*,unsigned,unsigned,unsigned,unsigned,int(__cdecl*)(uintptr_t,void*),void*)=nullptr;
 uint32_t(__thiscall* handle)(uintptr_t,uintptr_t,int)=nullptr;
 uint32_t(__cdecl* local)()=nullptr;
 int(__cdecl* order)(uint32_t,uint32_t,uint32_t)=nullptr;
 int(__cdecl* sell)(uint32_t,uint32_t,uint32_t)=nullptr;
 uintptr_t(__thiscall* ability)(uintptr_t,uint32_t,int,int,int,int)=nullptr;
 int(__cdecl* ally)(uint32_t,uint32_t)=nullptr;
 Getter type=nullptr,life=nullptr,x=nullptr,y=nullptr,owned=nullptr,visible=nullptr,pawnable=nullptr,owner=nullptr,playerId=nullptr;
};
inline InventoryActionsApi129 ResolveInventoryActions129(uintptr_t b){
 InventoryActionsApi129 a;if(!b)return a;
 __try {
  const unsigned char query[]={0x55,0x8b,0xec,0x83,0xec,0x10};
  const unsigned char handle[]={0x55,0x8b,0xec,0x51,0x53,0x56,0x57};
  if(memcmp((void*)(b+0x3f4d30),query,sizeof(query))||memcmp((void*)(b+0xe72c0),handle,sizeof(handle)))return {};
  for(unsigned rva:{0x9b010u,0xaf130u,0x2d6570u,0x99510u,0x92fc0u,0x97df0u,0x93000u,0x93040u,0x990f0u,0x991a0u,0x99130u,0x94410u,0x945f0u})if(memcmp((void*)(b+rva),"\x55\x8b\xec",3))return {};
  auto create=ResolveCreateUnit129(b);if(!create)return {};
  a.query=reinterpret_cast<decltype(a.query)>(b+0x3f4d30);a.handle=reinterpret_cast<decltype(a.handle)>(b+0xe72c0);a.local=create.local;
  a.order=reinterpret_cast<decltype(a.order)>(b+0x9b010);a.sell=reinterpret_cast<decltype(a.sell)>(b+0xaf130);a.ability=reinterpret_cast<decltype(a.ability)>(b+0x2d6570);a.ally=reinterpret_cast<decltype(a.ally)>(b+0x99510);
  a.type=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x92fc0);a.life=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x97df0);a.x=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x93000);a.y=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x93040);a.owned=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x990f0);a.visible=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x991a0);a.pawnable=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x99130);a.owner=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x94410);a.playerId=reinterpret_cast<InventoryActionsApi129::Getter>(b+0x945f0);
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
 return a;
}
// GetRectMinY/MinX/MaxY/MaxX read rect+24/+28/+2C/+30 respectively.
inline std::array<float,4> NearbyRect129(float x,float y,float radius){return {y-radius,x-radius,y+radius,x+radius};}
struct NearbyObjects129 {uintptr_t objects[256]{};unsigned count=0;};
inline int __cdecl CollectNearby129(uintptr_t object,void* context){auto& list=*static_cast<NearbyObjects129*>(context);if(object&&list.count<256)list.objects[list.count++]=object;return 1;}
inline float InventoryReal129(InventoryActionsApi129::Getter getter,uint32_t handle){const auto bits=getter(handle);float value;memcpy(&value,&bits,4);return value;}
struct InventoryActions129 {
 bool autoPick=false,selling=false;uint64_t due=0,deadline=0;SkillSelection129 source{};uint32_t pending=0;unsigned sold=0;uint32_t pickupTarget=0;uint64_t pickupSent=0,sellRetry=0;uintptr_t pickupUnit=0;uint64_t pickupIdentity=0;
 void Reset(){*this={};}
 bool StartSell(uintptr_t b,const SkillSelection129& selected,const DropItemsApi129& drop){
  __try {if(!LiveDropUnit129(b,selected,drop))return false;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
  if(!ResolveInventoryActions129(b).query)return false;
  selling=true;source=selected;pending=0;sold=0;sellRetry=0;due=0;deadline=GetTickCount64()+30000;return true;
 }
 // Returns a completion message; callers display it outside the guarded native body.
 const char* TickNative(uintptr_t b,bool allowed,uint64_t now){
  if(!allowed||now<due||(!autoPick&&!selling))return nullptr;
  __try {
   const auto api=ResolveInventoryActions129(b);const auto drop=ResolveDropItems129(b);const auto posApi=ResolveTeleportApi129(b);
   const auto selected=SelectedSkill129(b);uint32_t code=0;TeleportPosition129 pos;
   if(selling&&(selected.unit!=source.unit||selected.identity!=source.identity||now>=deadline)){selling=false;return "Sell All stopped: selection changed or timed out";}
   due=now+(selling?25:250);
   if(!api.query||!drop||!ReadCopySource129(b,selected,posApi,code,pos))return nullptr;
   const auto world=*reinterpret_cast<uintptr_t*>(b+0xd3b6f4);if(!world)return nullptr;
   const auto context=*reinterpret_cast<uintptr_t*>(world+0x1c);if(!context)return nullptr;
   const auto hero=api.handle(context,selected.unit,0),local=api.local();
   if(!hero||!local||api.owner(hero)!=local||InventoryReal129(api.life,hero)<=.405f)return nullptr;
   const auto capacity=drop.size(selected.unit);if(!capacity||capacity>6)return nullptr;
   unsigned occupied=0;uint32_t item=0;bool pendingHeld=false;
   for(unsigned i=0;i<capacity;++i){const auto raw=drop.item(selected.unit,i);if(!raw)continue;++occupied;const auto h=api.handle(context,raw,0);if(h==pending)pendingHeld=true;if(!item&&h&&api.pawnable(h))item=h;}
   if(selling&&pending){if(pendingHeld&&now<sellRetry){due=now+25;return nullptr;}if(!pendingHeld)++sold;pending=0;}
   if(selling&&!item){selling=false;return occupied?"Sell All finished: remaining items cannot be sold":"Sell All completed";}
   if(!selling&&occupied>=capacity){due=now+500;return nullptr;}
   NearbyObjects129 nearby;const float radius=selling?1200.f:777.f;const auto rect=NearbyRect129(pos.x,pos.y,radius);
   api.query(rect.data(),0,selling?0x14fdu:0x4f7u,0,0,CollectNearby129,&nearby);
   uint32_t target=0;float best=radius*radius;bool pickupPresent=false;
   if(pickupUnit!=selected.unit||pickupIdentity!=selected.identity){pickupTarget=0;pickupUnit=selected.unit;pickupIdentity=selected.identity;}
   for(unsigned i=0;i<nearby.count;++i){
    const auto raw=nearby.objects[i];const auto h=api.handle(context,raw,0);if(!h)continue;
    float x=0,y=0;
    if(selling){
     if(*reinterpret_cast<uintptr_t*>(raw)!=b+0xa9c504||!api.ability(raw,0x41706974,0,1,1,1))continue;
     if(InventoryReal129(api.life,h)<=.405f||(api.playerId(api.owner(h))<24&&!api.ally(api.owner(h),local)))continue;
     float xyz[3]={NAN,NAN,NAN};posApi.position(raw+0x26c,xyz);x=xyz[0];y=xyz[1];
    }else{if(!api.type(h)||api.owned(h)||!api.visible(h)||InventoryReal129(api.life,h)<=0)continue;x=InventoryReal129(api.x,h);y=InventoryReal129(api.y,h);}
    if(!selling&&h==pickupTarget)pickupPresent=true;
    const float distance=(x-pos.x)*(x-pos.x)+(y-pos.y)*(y-pos.y);if(std::isfinite(distance)&&distance<best){best=distance;target=h;}
   }
   if(!LiveDropUnit129(b,selected,drop))return nullptr;
   if(!selling&&pickupTarget){if(pickupPresent&&now-pickupSent<5000)return nullptr;pickupTarget=0;}
   if(!target){due=now+500;if(selling){selling=false;return "Sell All: no allied shop nearby";}return nullptr;}
   if(selling){if(api.sell(hero,item,target)){pending=item;sellRetry=now+(best>600.f*600.f?4000:350);due=now+25;}else{selling=false;return "Sell All: shop refused item order";}}
   else{if(api.order(hero,0xd0003,target)){pickupTarget=target;pickupSent=now;}due=now+250;}
  }__except(EXCEPTION_EXECUTE_HANDLER){selling=false;due=now+1000;return "Inventory action: native access failed";}
  return nullptr;
 }
};
}
