#pragma once
#include "auto_skills.hpp"
#include "quick_drop129.hpp"
#include "inventory_actions129.hpp"
namespace wht {
inline uint32_t HoverSkillOrder129(uintptr_t base,POINT p,int width,int height){
 if(!base||width<=0||height<=0)return 0;
 float x=QuickDropMouseX129(p.x,width,height),y=.6f-float(p.y)*.6f/height;
 __try{for(int i=0;i<12;++i){auto button=Button129(base,false,i);if(!button)continue;auto bounds=reinterpret_cast<float*>(button+0x44);
  if(!InventoryBoundsHit129(bounds[1],bounds[0],bounds[3],bounds[2],x,y))continue;
  auto data=*reinterpret_cast<uintptr_t*>(button+0x194);return data?*reinterpret_cast<uint32_t*>(data+8):0;
 }}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}return 0;
}
inline bool AutoSkillTarget129(uintptr_t base,uint32_t order,float range){
 if(!base||!order)return false;
 __try{
  auto api=ResolveInventoryActions129(base);auto drop=ResolveDropItems129(base);auto position=ResolveTeleportApi129(base);auto selected=SelectedSkill129(base);
  uint32_t code=0;TeleportPosition129 pos;if(!api.query||!drop||!LiveDropUnit129(base,selected,drop)||!ReadCopySource129(base,selected,position,code,pos))return false;
  auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(!world)return false;auto context=*reinterpret_cast<uintptr_t*>(world+0x1c);if(!context)return false;
  auto unit=api.handle(context,selected.unit,0),local=api.local();if(!unit||api.owner(unit)!=local||InventoryReal129(api.life,unit)<=.405f)return false;
  NearbyObjects129 objects;auto rect=NearbyRect129(pos.x,pos.y,range);api.query(rect.data(),0,0x14fd,0,0,CollectNearby129,&objects);
  if(memcmp((void*)(base+0x9a3e0),"\x55\x8b\xec",3)||memcmp((void*)(base+0x9a670),"\x55\x8b\xec",3)||memcmp((void*)(base+0x9aca0),"\x55\x8b\xec",3))return false;
  auto visible=reinterpret_cast<int(__cdecl*)(uint32_t,uint32_t)>(base+0x9a3e0);
  uint32_t target=0;float nearest=range*range,targetX=0,targetY=0;
  for(unsigned i=0;i<objects.count;++i){auto raw=objects.objects[i];if(raw==selected.unit||*reinterpret_cast<uintptr_t*>(raw)!=base+0xa9c504)continue;
   auto handle=api.handle(context,raw,0);if(!handle||api.ally(api.owner(handle),local)||!visible(handle,local)||InventoryReal129(api.life,handle)<=.405f)continue;
   float xyz[3]={NAN,NAN,NAN};position.position(raw+0x26c,xyz);float distance=(xyz[0]-pos.x)*(xyz[0]-pos.x)+(xyz[1]-pos.y)*(xyz[1]-pos.y);
   if(std::isfinite(distance)&&distance<nearest){nearest=distance;target=handle;targetX=xyz[0];targetY=xyz[1];}
  }
  if(!target||!LiveDropUnit129(base,selected,drop))return false;
  if(api.order(unit,order,target))return true;
  if(reinterpret_cast<int(__cdecl*)(uint32_t,uint32_t,const float*,const float*)>(base+0x9aca0)(unit,order,&targetX,&targetY))return true;
  return reinterpret_cast<int(__cdecl*)(uint32_t,uint32_t)>(base+0x9a670)(unit,order)!=0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct AutoSkills129 {
 AutoSkillsState state;
 void Reset(){state.Reset();}
 bool Bind(uintptr_t base,unsigned key,POINT point,int width,int height){auto selected=SelectedSkill129(base);auto order=HoverSkillOrder129(base,point,width,height);return state.Bind(selected.identity,AutoSkillsState::Number(key),order);}
 void Tick(uintptr_t base,const Config& config,bool allowed,uint64_t now){
  if(!config.autoSkillsEnabled){state.running=false;return;}if(!allowed||!state.running||now<state.next)return;
  state.next=now+uint64_t(config.autoSkillsDelay*1000.f);auto selected=SelectedSkill129(base);auto order=state.Current(selected.identity);
  if(order){AutoSkillTarget129(base,order,config.autoSkillsRange);state.Advance(selected.identity);}
 }
};
}
