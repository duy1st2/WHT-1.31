#pragma once
#include <cstdint>
#include <unordered_map>
#include <cmath>
namespace wht {
// Only throttle the tool's tooltip placement, never the game's mouse messages.
struct MouseFollowCadence {
 uintptr_t root=0;uint64_t last=0;bool known=false,enabled=false;
 bool MayPoll(uint64_t now,bool follow,bool active)const{
  return (follow||active)&&(!known||enabled!=follow||now<last||now-last>=16);
 }
 bool Due(uint64_t now,uintptr_t currentRoot,bool follow,bool active){
  if(!follow&&!active){known=false;return false;}
  if(known&&root==currentRoot&&enabled==follow&&now>=last&&now-last<16)return false;
  root=currentRoot;last=now;known=true;enabled=follow;return true;
 }
};
inline uint64_t DenseBarSampleInterval(unsigned count){return count>=768?800u:count>=256?400u:200u;}
// Sampling LOD only: preserve every label/bar and keep selected units/heroes fast.
struct FrameLoadCadence {
 unsigned level=0;bool low=false,recovering=false;uint64_t lowAt=0,recoveryAt=0;
 void Suspend(){low=recovering=false;}
 void Update(float fps,uint64_t now){
  if(!std::isfinite(fps)||fps<=0){Suspend();return;}
  if(fps<50){
   recovering=false;if(!low){low=true;lowAt=now;}
   if(now-lowAt>=1000)level=fps<30?2u:1u;
  }else{low=false;
   if(fps>=58&&level){if(!recovering){recovering=true;recoveryAt=now;}if(now-recoveryAt>=5000){--level;recoveryAt=now;}}
   else recovering=false;
  }
 }
 uint64_t Interval(uint64_t crowd)const{return level==2?(crowd<1200?1200:crowd):level==1?(crowd<600?600:crowd):crowd;}
 uint64_t SweepInterval()const{return level==2?600u:level==1?400u:300u;}
 int BudgetDivisor()const{return level==2?3000:level==1?2000:1000;}
};
// Adjust stat sampling only; native frame anchors continue following units normally.
struct CrowdBarCadence {
 unsigned level=0;
 uint64_t Update(unsigned visible){
  if(visible>=120||(level==2&&visible>=100))level=2;
  else if(visible>=50||(level&&visible>=40))level=1;
  else level=0;
  return level==2?800u:level==1?400u:200u;
 }
};
struct OverheadCadence {
 struct Entry {uint64_t identity=0,time=0;uintptr_t frame=0;};
 uintptr_t session=0;std::unordered_map<uintptr_t,Entry> units;
 bool fullInitialized=false,fullAllowed=false;uint64_t fullTime=0;
 void Observe(uintptr_t current){if(session!=current){session=current;units.clear();fullInitialized=false;}}
 bool Unit(uintptr_t current,uintptr_t object,uint64_t identity,uint64_t now,uintptr_t frame=0,uint64_t interval=200){
  Observe(current);auto it=units.find(object);
  if(it!=units.end()&&it->second.identity==identity&&it->second.frame==frame&&now-it->second.time<interval)return false;
  if(it==units.end()&&units.size()>=4096)units.clear();units[object]={identity,now,frame};return true;
 }
 bool Full(uintptr_t current,bool allowed,uint64_t now){Observe(current);if(fullInitialized&&fullAllowed==allowed&&now-fullTime<300)return false;fullInitialized=true;fullAllowed=allowed;fullTime=now;return true;}
};
struct HoverUpdateCadence {
 uintptr_t root=0;int x=0,y=0,w=0,h=0;unsigned flags=0;const void* profile=nullptr;uint64_t last=0;bool known=false;
 bool Due(uint64_t now,uintptr_t owner,int px,int py,int width,int height,unsigned state,const void* config){
  if(known&&root==owner&&x==px&&y==py&&w==width&&h==height&&flags==state&&profile==config&&now-last<100)return false;
  root=owner;x=px;y=py;w=width;h=height;flags=state;profile=config;last=now;known=true;return true;
 }
};
inline bool BossScanEnabled(bool graphics,bool timer,bool mth49,bool g17){return graphics&&timer&&(mth49||g17);}
struct ButtonUpdateCadence {
 uintptr_t root=0,unit=0;const void* profile=nullptr;unsigned flags=0;uint64_t last=0;bool initialized=false;
 bool Due(uint64_t now,uintptr_t currentRoot,uintptr_t currentUnit,const void* currentProfile,unsigned currentFlags){
  if(initialized&&root==currentRoot&&unit==currentUnit&&profile==currentProfile&&flags==currentFlags&&now-last<100)return false;
  initialized=true;root=currentRoot;unit=currentUnit;profile=currentProfile;flags=currentFlags;last=now;return true;
 }
};
}
