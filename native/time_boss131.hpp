#pragma once
#include "time_boss.hpp"
#include "quick_drop_network131.hpp"
#include "stat_paint131.hpp"
#include "clock_fps131.hpp"
#include <unordered_map>
namespace wht {
struct BossProfile131 {unsigned build;uintptr_t unit,level,check,typeMethod;};
#ifdef _WIN64
inline constexpr BossProfile131 bossProfiles131[]={{12164,0x1c16140,0xd096e0,0x7b,0xd23ad0},{12173,0x1c45000,0xd481a0,0x7b,0xd62350}};
inline constexpr unsigned bossCodeOffset131=0x58;
#else
inline constexpr BossProfile131 bossProfiles131[]={{12164,0x182eeb0,0xbabfa0,0x53,0xbc18f0},{12173,0x17e3d90,0xb6b410,0x53,0xb80bc0}};
inline constexpr unsigned bossCodeOffset131=0x34;
#endif
inline const BossProfile131* BossProfileFor131(unsigned build){for(const auto& p:bossProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool BossNative131(uintptr_t base,const BossProfile131& p){
 __try{
#ifdef _WIN64
  const unsigned char type[]={0x44,0x8b,0x57,0x58};constexpr unsigned typeOffset=0x3f;
  const unsigned char check[]={0x0f,0xb6,0x43,0x5b,0x48,0x8b,0x74,0x24,0x50,0x83,0xe8,0x41,0x83,0xf8,0x19};
#else
  const unsigned char type[]={0x8b,0x4f,0x34};constexpr unsigned typeOffset=0x19;
  const unsigned char check[]={0x0f,0xb6,0x46,0x37,0x83,0xf8,0x41,0x72,0x16,0x83,0xf8,0x5a};
#endif
  return base&&*reinterpret_cast<uintptr_t*>(base+p.unit+55*sizeof(uintptr_t))==base+p.typeMethod&&!memcmp(reinterpret_cast<void*>(base+p.typeMethod+typeOffset),type,sizeof(type))&&!memcmp(reinterpret_cast<void*>(base+p.level+p.check),check,sizeof(check));
 }__except(1){return false;}
}
struct BossSample131 {uint64_t identity=0;uint32_t code=0;};
inline bool ReadBoss131(uintptr_t base,const BossProfile131& p,uintptr_t unit,BossSample131& out){
 if(!InventoryIdentity131(unit,base,p.unit))return false;
 __try{const auto id=DropIdentity131(unit);if(!id.Valid())return false;
  const auto code=*reinterpret_cast<uint32_t*>(unit+bossCodeOffset131);if(TimeBossState::Group(code)<0)return false;
  out={uint64_t(id.low)<<32|id.high,code};return out.identity!=0;
 }__except(1){return false;}
}
struct TimeBoss131 {
 TimeBossState state;std::unordered_map<uintptr_t,BossSample131> units;
 uintptr_t base=0;const BossProfile131* profile=nullptr;bool active=false,checked=false;
 uint64_t next=0;
 void Reset(){state.Reset();units.clear();base=0;profile=nullptr;active=checked=false;next=0;}
 void Remember(uintptr_t unit){if(!active||!profile)return;BossSample131 sample;
  if(ReadBoss131(base,*profile,unit,sample)&&(units.count(unit)||units.size()<8192))units[unit]=sample;
 }
 void RememberBar(uintptr_t bar){if(!active)return;
  __try{Remember(*reinterpret_cast<uintptr_t*>(bar+(sizeof(void*)==8?0x210:0x154)));}__except(1){}
 }
 template<class Read,class Life> void Observe(uint32_t seconds,Read read,Life life){
  for(auto it=units.begin();it!=units.end();){BossSample131 sample;float hp=0;
   if(!read(it->first,sample)||sample.identity!=it->second.identity||sample.code!=it->second.code||!life(it->first,hp)){it=units.erase(it);continue;}
   state.Observe(sample.identity,sample.code,hp<=.405f,seconds);++it;
  }
 }
 template<class Menu,class Life> void Update(uintptr_t image,unsigned build,uintptr_t owner,bool enabled,uint64_t now,Menu& menu,Life life){
  if(state.root!=owner||base!=image){Reset();state.root=owner;base=image;profile=BossProfileFor131(build);}
  if(!enabled){active=false;menu.UpdateBoss({},false);return;}
  if(!checked){checked=true;if(!profile||!BossNative131(image,*profile))profile=nullptr;}
  active=profile&&owner;if(!active){menu.UpdateBoss({},false);return;}
  if(now<next)return;next=now+250;
  const auto* clock=ClockFpsProfile(build);uint32_t seconds=0;
  if(!clock||!SimulationSeconds131(image,*clock,seconds)){menu.UpdateBoss({},false);return;}
  if(seconds!=state.lastSecond){state.lastSecond=seconds;
   // Reuse the existing native HP snapshot; no whole-world enumeration.
   for(const auto& entry:statPaintRecords131)RememberBar(entry.first);
   Observe(seconds,[&](uintptr_t unit,BossSample131& out){return ReadBoss131(base,*profile,unit,out);},life);
  }
  menu.UpdateBoss(state.Remaining(seconds),true);
 }
};
}
