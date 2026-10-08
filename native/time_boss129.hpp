#pragma once
#include "state_values129.hpp"
#include "time_boss.hpp"
namespace wht {
// Port of WHT's TimeBossState loop. Addresses below belong only to 1.29.
struct BossUnit129 {uintptr_t unit=0;uint64_t identity=0;uint32_t code=0;float hp=0;};
inline bool ReadBossUnit129(uintptr_t base,uintptr_t unit,BossUnit129& out){
 __try {
  if(!unit||*reinterpret_cast<uintptr_t*>(unit)!=base+0xa9c504)return false;
  out.unit=unit;out.code=*reinterpret_cast<uint32_t*>(unit+0x34);
  out.identity=(uint64_t(*reinterpret_cast<uint32_t*>(unit+12))<<32)|*reinterpret_cast<uint32_t*>(unit+16);
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 if(!out.identity||TimeBossState::Group(out.code)<0)return false;
 float maximum=0;
 return ReadStateValues129(base,unit,false,reinterpret_cast<RegenResolve129>(base+0x707f0),reinterpret_cast<StatRead129>(base+0x400d80),reinterpret_cast<StatRead129>(base+0x400e20),out.hp,maximum);
}
struct TimeBoss129 {
 TimeBossState state;std::unordered_map<uintptr_t,uint64_t> units;
 void Reset(){state.Reset();units.clear();}
 void Remember(uintptr_t base,uintptr_t unit){
  BossUnit129 sample;if(ReadBossUnit129(base,unit,sample)&&units.size()<8192)units[unit]=sample.identity;
 }
 template<class Reader> void Observe(uint32_t seconds,Reader read){
  for(auto it=units.begin();it!=units.end();){
   BossUnit129 sample;
   if(!read(it->first,sample)||sample.identity!=it->second){it=units.erase(it);continue;}
   state.Observe(sample.identity,sample.code,sample.hp<=.405f,seconds);++it;
  }
 }
 template<class Menu> void Update(uintptr_t base,uintptr_t owner,bool enabled,Menu& menu){
  if(state.root!=owner){Reset();state.root=owner;}
  float clock=0;
  if(!ReadClock129(base,clock)){menu.UpdateBoss({},false);return;}
  const auto seconds=state.Seconds(uint32_t(clock),GetTickCount64(),true);
  if(seconds!=state.lastSecond){state.lastSecond=seconds;
   if(enabled)Observe(seconds,[&](uintptr_t unit,BossUnit129& sample){return ReadBossUnit129(base,unit,sample);});
  }
  menu.UpdateBoss(state.Remaining(seconds),enabled);
 }
};
}
