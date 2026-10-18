#pragma once
#include "stats_labels131.hpp"
#include "component_lookup131.hpp"
namespace wht {
struct StatsPanels131 {unsigned build;uintptr_t damage,armor,hero;};
#ifdef _WIN64
inline constexpr StatsPanels131 statsPanels131[]={{12164,0x1c0eea8,0x1c0ef98,0x1c0f358},{12173,0x1c3dd68,0x1c3de58,0x1c3e218}};
#else
inline constexpr StatsPanels131 statsPanels131[]={{12164,0x182b22c,0x182b2a4,0x182b484},{12173,0x17e010c,0x17e0184,0x17e0364}};
#endif
inline uintptr_t StatsField131(uintptr_t panel,unsigned offset){return ComponentLookup131::Slot(panel,offset);}
template<class Set> inline void PollStatsLabels131(StatsLabels131& labels,uintptr_t base,const LifeProfile131& life,unsigned build,uintptr_t info,bool enabled,Set set){
 const StatsPanels131* p=nullptr;for(const auto& item:statsPanels131)if(item.build==build)p=&item;
 if(!p||!info||!labels.owner.unit||!ReadableComponent131(info,sizeof(uintptr_t)==8?0x680:0x400))return;
 for(unsigned offset=sizeof(uintptr_t);offset<(sizeof(uintptr_t)==8?0x680u:0x400u);offset+=sizeof(uintptr_t)){
  auto panel=StatsField131(info,offset);int kind=-1;
  if(ComponentIdentity131(panel,base+p->damage))kind=0;
  else if(ComponentIdentity131(panel,base+p->armor))kind=1;
  else if(ComponentIdentity131(panel,base+p->hero))kind=2;
  if(kind<0)continue;
  for(unsigned n=0;n<(kind==2?3u:1u);++n){
   const unsigned field=sizeof(uintptr_t)==8?(kind<2?0x1e8:0x1e0+n*8):(kind<2?0x12c:0x128+n*4);
   const auto font=StatsField131(panel,field);if(!StatsFont131(base,life,font))continue;
   std::array<char,129> current{};if(!ReadLifeText131(font,current))continue;
   bool owned=false;for(const auto& label:labels.labels)if(label.font==font&&label.written==current.data()){owned=true;break;}
   if(!owned)labels.Handle(font,current.data(),kind+n,true,enabled,set);
  }
 }
}
}
