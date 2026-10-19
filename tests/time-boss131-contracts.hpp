#pragma once
#include "time_boss131.hpp"
inline void CheckTimeBoss131(){
 constexpr uint32_t human=0x4870616c,undead=0x55646561;
 check(wht::TimeBossState::Group(human)==0&&wht::TimeBossState::Group(undead)==1&&wht::TimeBossState::Group(0x68666f6f)<0,"only reference boss rawcodes accepted");
 for(const auto& p:wht::bossProfiles131){
  alignas(uintptr_t) unsigned char unit[0x100]{};const uintptr_t image=0x10000000,object=reinterpret_cast<uintptr_t>(unit);
  *reinterpret_cast<uintptr_t*>(unit)=image+p.unit;wht::DropObjectId id{17,29};memcpy(unit+(sizeof(void*)==8?0x18:0xc),&id,sizeof(id));
  // Independently observed native offsets, not the production offset constant.
  *reinterpret_cast<uint32_t*>(unit+(sizeof(void*)==8?0x58:0x34))=human;
  wht::BossSample131 sample;check(wht::ReadBoss131(image,p,object,sample)&&sample.code==human&&sample.identity==(uint64_t(17)<<32|29),"exact unit class/rawcode/object ID on both build profiles");
  wht::TimeBoss131 boss;boss.active=true;boss.base=image;boss.profile=&p;boss.Remember(object);check(boss.units.size()==1,"remember native observed boss");
  float hp=100;auto read=[&](uintptr_t unit,wht::BossSample131& out){return wht::ReadBoss131(image,p,unit,out);};auto life=[&](uintptr_t,float& out){out=hp;return true;};
  boss.Observe(280,read,life);check(boss.state.Remaining(280)[2]==0,"living observed boss clears spawn countdown");
  hp=0;boss.Observe(300,read,life);check(boss.state.Remaining(300)[2]==31,"observed death schedules 31 seconds");
  boss.Observe(305,read,life);check(boss.state.Remaining(305)[2]==26,"dead sample does not restart timer");
  hp=100;boss.Observe(310,read,life);check(!boss.state.Remaining(310)[2],"same identity revival clears respawn timer");
  hp=0;boss.Observe(320,read,life);check(boss.state.Remaining(320)[2]==31,"revived boss can die again");
  ++reinterpret_cast<wht::DropObjectId*>(unit+(sizeof(void*)==8?0x18:0xc))->low;
  boss.Observe(321,read,life);check(boss.units.empty()&&boss.state.Remaining(321)[2]==30,"reused unit pointer never invents another death");
  *reinterpret_cast<uintptr_t*>(unit)=0;check(!wht::ReadBoss131(image,p,object,sample),"foreign/destroyed class rejected");
  boss.Reset();check(!boss.active&&boss.units.empty()&&boss.state.Remaining(0)==std::array<uint32_t,3>{493,300,300},"match reset restores reference initial schedule");
 }
 wht::TimeBossState schedule;check(schedule.Remaining(493)[0]==1007&&schedule.Remaining(1500)[0]==1007,"reference periodic boss schedule");
 check(!wht::BossProfileFor131(129),"no inherited native 1.29 profile");
}
