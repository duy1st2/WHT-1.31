#pragma once
#include "detail_tooltip_profile131.hpp"
#include "stats_tooltip.hpp"
#include "stats_poll131.hpp"
#include "drop_all.hpp"
#include <vector>
namespace wht {
inline constexpr unsigned detailSource131=sizeof(void*)==8?0x1d0:0x120;
inline constexpr unsigned detailBody131=sizeof(void*)==8?0x2d0:0x220;
inline constexpr unsigned detailExtra131=sizeof(void*)==8?0x6d1:0x621;
inline constexpr unsigned detailFontText131=sizeof(void*)==8?0x108:0xa8;
inline constexpr unsigned detailHelper131=sizeof(void*)==8?0x1d8:0x124;
inline constexpr unsigned detailAttached131=sizeof(void*)==8?0xf1:0x9d;
inline bool DetailString131(uintptr_t address,size_t maximum,std::string& text){
 text.clear();if(!address||maximum>2048)return false;
 __try{for(size_t n=0;n<=maximum;++n){const char ch=*reinterpret_cast<const char*>(address+n);if(!ch)return true;text+=ch;}}__except(1){}text.clear();return false;
}
struct DetailView131 {uintptr_t frame=0,anchor=0,font=0;bool extended=false;std::string title,body,extra,current;};
inline bool ReadDetailView131(uintptr_t base,const DetailProfile131& p,const LifeProfile131& life,uintptr_t frame,bool extended,DetailView131& out,bool strings=true){
 if(!ComponentIdentity131(frame,base+(extended?p.uber:p.basic)))return false;
 __try{
  if(*reinterpret_cast<unsigned char*>(frame+detailAttached131)!=1||*reinterpret_cast<unsigned char*>(frame+detailAttached131-1)!=1)return false;
  const auto anchor=ComponentLookup131::Slot(frame,detailSource131),font=ComponentLookup131::Slot(frame,detailSource131+2*sizeof(void*));
  if(!anchor||!StatsFont131(base,life,font))return false;
  out.title.clear();out.body.clear();out.extra.clear();out.current.clear();out.frame=frame;out.anchor=anchor;out.font=font;out.extended=extended;
  if(!strings)return true;
  return DetailString131(anchor+detailSource131,255,out.title)&&!out.title.empty()&&DetailString131(anchor+detailBody131,1024,out.body)&&DetailString131(anchor+detailExtra131,1023,out.extra)&&DetailString131(ComponentLookup131::Slot(font,detailFontText131),2048,out.current);
 }__except(1){return false;}
}
struct DetailNativeApi131 {
 void(__thiscall* normal)(uintptr_t,const char*,const char*,unsigned,unsigned,unsigned,bool,uintptr_t)=nullptr;
 void(__thiscall* extended)(uintptr_t,const char*,const char*,const char*,unsigned,unsigned,unsigned,uintptr_t)=nullptr;
 uintptr_t(__thiscall* movement)(uintptr_t,float*)=nullptr;
};
inline DetailNativeApi131 DetailNative131(uintptr_t base,const DetailProfile131& p){return {reinterpret_cast<decltype(DetailNativeApi131::normal)>(base+p.normalSet),reinterpret_cast<decltype(DetailNativeApi131::extended)>(base+p.extendedSet),reinterpret_cast<decltype(DetailNativeApi131::movement)>(base+p.moveGet)};}
inline bool WriteDetail131(const DetailNativeApi131& api,const DetailView131& view,const char* body){
 if(!body||!view.frame||view.title.empty())return false;
 __try{
  if(view.extended){if(!api.extended)return false;api.extended(view.frame,view.title.c_str(),*body?body:nullptr,view.extra.empty()?nullptr:view.extra.c_str(),0,0,0,0);}
  else{if(!api.normal)return false;api.normal(view.frame,view.title.c_str(),*body?body:nullptr,0,0,0,true,0);}
  return true;
 }__except(1){return false;}
}
inline bool DetailAttack131(uintptr_t base,const DetailProfile131& p,const LifeProfile131& life,uintptr_t unit,unsigned weapon,float& multiplier,float& cooldown,float& maximum,float& minimum){
 if(weapon>1||!ComponentIdentity131(unit,base+life.unit))return false;
 __try{
  const unsigned offset=sizeof(void*)==8?0x570:p.build==12164?0x460:0x458;
  const auto ability=ComponentLookup131::Slot(unit,offset);
  if(!ComponentIdentity131(ability,base+p.attack))return false;
  const auto speed=ability+(sizeof(void*)==8?0x268:0x1b0),period=ability+(sizeof(void*)==8?0x1d8+weapon*16:0x158+weapon*8);
  if(!ComponentIdentity131(speed,base+p.miniB)||!ComponentIdentity131(period,base+p.mini))return false;
  multiplier=*reinterpret_cast<float*>(speed+sizeof(void*));cooldown=*reinterpret_cast<float*>(period+sizeof(void*));
  minimum=*reinterpret_cast<float*>(base+p.minimum);maximum=*reinterpret_cast<float*>(base+p.maximum);
  return std::isfinite(multiplier)&&multiplier>0&&multiplier<1e6f&&std::isfinite(cooldown)&&cooldown>0&&cooldown<1e6f&&std::isfinite(minimum)&&minimum>0&&std::isfinite(maximum)&&maximum>=minimum&&maximum<1e6f;
 }__except(1){return false;}
}
inline bool DetailMovement131(uintptr_t base,const DetailProfile131& p,const LifeProfile131& life,uintptr_t unit,const DetailNativeApi131& api,float& speed){
 if(!api.movement||!ComponentIdentity131(unit,base+life.unit))return false;
 __try{
  const auto ability=ComponentLookup131::Slot(unit,sizeof(void*)==8?0x560:p.build==12164?0x454:0x44c);
  if(!ComponentIdentity131(ability,base+p.move)||ComponentLookup131::Slot(ability,sizeof(void*)==8?0x50:0x30)!=unit)return false;
  float result=NAN;const auto returned=api.movement(ability,&result);
  if(returned!=reinterpret_cast<uintptr_t>(&result)||!ComponentIdentity131(ability,base+p.move)||ComponentLookup131::Slot(ability,sizeof(void*)==8?0x50:0x30)!=unit)return false;
  speed=result;return std::isfinite(speed)&&speed>=0&&speed<1e6f;
 }__except(1){return false;}
}
struct DetailText131 {
 uintptr_t frame=0,anchor=0,unit=0;uint64_t identity=0;std::string written;bool active=false;
 void Reset(){*this={};}
 template<class Write> bool Apply(const DetailView131& view,uintptr_t selected,uint64_t id,const std::string& detail,bool enabled,Write write){
  if(frame!=view.frame||anchor!=view.anchor||unit!=selected||identity!=id){Reset();frame=view.frame;anchor=view.anchor;unit=selected;identity=id;}
  if(!frame||!anchor||!unit||!id)return false;
  const bool owned=active&&view.current==written;
  if(!enabled||detail.empty()){
   if(owned&&view.current!=view.body&&!write(view.body))return false;
   active=false;written.clear();return true;
  }
  if(view.current!=view.body&&!owned){Reset();return false;}
  std::string desired=view.body;if(!desired.empty())desired+="|n|n";desired+=detail;
  if(desired.size()>1024){if(owned&&view.current!=view.body&&!write(view.body))return false;active=false;written.clear();return false;}
  if(view.current!=desired&&!write(desired))return false;
  written=desired;active=true;return true;
 }
};
struct DetailedTooltip131 {
 struct Panel {uintptr_t object=0,anchor=0,table=0;unsigned kind=0,weapon=0,slot=0;};
 uintptr_t owner=0,image=0,info=0;unsigned version=0;uint64_t next=0;bool ready=false;
 std::vector<Panel> panels;std::array<DetailText131,2> texts;
 bool Active()const{return texts[0].active||texts[1].active;}
 void Reset(){owner=image=info=0;version=0;next=0;ready=false;panels.clear();texts={};}
 void Discover(uintptr_t base,unsigned build,uintptr_t current,uint64_t now){
  if(info==current&&panels.size()==3){bool valid=true;
   for(const auto& panel:panels)if(StatsField131(current,panel.slot)!=panel.object||!ComponentIdentity131(panel.object,panel.table)||StatsField131(panel.object,detailHelper131)!=panel.anchor){valid=false;break;}
   if(valid)return;
  }
  if(info==current&&now<next)return;info=current;next=now+100;panels.clear();
  const StatsPanels131* p=nullptr;for(const auto& v:statsPanels131)if(v.build==build)p=&v;
  if(!p||!current||!ReadableComponent131(current,sizeof(void*)==8?0x680:0x400))return;
  unsigned weapon=0;for(unsigned offset=sizeof(void*);offset<(sizeof(void*)==8?0x680u:0x400u);offset+=sizeof(void*)){
   const auto panel=StatsField131(current,offset);unsigned kind=0;
   if(ComponentIdentity131(panel,base+p->damage))kind=1;
   else if(ComponentIdentity131(panel,base+p->armor))kind=2;
   if(!kind)continue;
   bool duplicate=false;for(const auto& old:panels)if(old.object==panel)duplicate=true;if(duplicate)continue;
   const unsigned index=kind==1?weapon++:0;if(kind==1&&index>=2)continue;
   const auto anchor=StatsField131(panel,detailHelper131);if(!anchor)continue;
   panels.push_back({panel,anchor,base+(kind==1?p->damage:p->armor),kind,index,offset});
   if(panels.size()>=3)break;
  }
 }
 template<class Component> void Update(uintptr_t base,unsigned build,uintptr_t ui,const LifeSource131& source,uintptr_t currentInfo,bool enabled,uint64_t now,Component component){
  if(owner!=ui||image!=base||version!=build){Reset();owner=ui;image=base;version=build;const auto* p=DetailProfileFor131(build);ready=p&&DetailCode131(base,*p);}
  if(!ready||!ui)return;
  if(!enabled&&!texts[0].active&&!texts[1].active)return;
  const auto* p=DetailProfileFor131(build);const auto* life=LifeProfileFor131(build);if(!p||!life)return;
  const auto api=DetailNative131(base,*p);
  for(unsigned n=0;n<2;++n){DetailView131 view;
   if(!ReadDetailView131(base,*p,*life,component(n?p->uber:p->basic),n!=0,view,false)){texts[n].Reset();continue;}
   Discover(base,build,currentInfo,now);
   const Panel* found=nullptr;for(const auto& panel:panels)if(panel.anchor==view.anchor&&StatsField131(currentInfo,panel.slot)==panel.object&&ComponentIdentity131(panel.object,panel.table)&&StatsField131(panel.object,detailHelper131)==view.anchor){found=&panel;break;}
   if(!found||!source.unit||source.ui!=ui||!source.identity){texts[n].Reset();continue;}
   if(!ReadDetailView131(base,*p,*life,view.frame,n!=0,view)){texts[n].Reset();continue;}
   std::string detail;
   if(enabled){float a=0,b=0,c=0,d=0;
    if(found->kind==1&&DetailAttack131(base,*p,*life,source.unit,found->weapon,a,b,c,d))detail=AttackSpeedTooltip(a,b,c,d);
    else if(found->kind==2&&DetailMovement131(base,*p,*life,source.unit,api,a))detail=MovementSpeedTooltip(a);
   }
   texts[n].Apply(view,source.unit,source.identity,detail,enabled,[&](const std::string& value){return WriteDetail131(api,view,value.c_str());});
  }
 }
};
}
