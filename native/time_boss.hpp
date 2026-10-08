#pragma once
#include <array>
#include <unordered_set>
#include <cstdint>
namespace wht {
struct TimeBossState {
 uint64_t clockTick=0;uint32_t clockSecond=0;bool clockStarted=false;
 uint32_t Seconds(uint32_t nativeSeconds,uint64_t now,bool nativeAvailable=false){
  if(nativeAvailable){clockStarted=true;clockTick=now;clockSecond=nativeSeconds;return clockSecond;}
  if(!clockStarted){clockStarted=true;clockTick=now;clockSecond=nativeSeconds;return clockSecond;}
  if(nativeSeconds){if(nativeSeconds>clockSecond)clockSecond=nativeSeconds;clockTick=now;}
  else if(now>=clockTick){auto elapsed=(now-clockTick)/1000;clockSecond+=uint32_t(elapsed);clockTick+=elapsed*1000;}
  return clockSecond;
 }
 uintptr_t root=0;uint32_t lastSecond=~0u;std::array<uint32_t,2> due{300,300};std::unordered_set<uint64_t> deaths;std::array<std::unordered_set<uint64_t>,2> tracked;
 std::unordered_set<uint64_t> observedDead;
 void Reset(){*this={};}
 static int Group(uint32_t code){
  static constexpr const char* groups[]={"HpalHamgHmkgHblmHlgrHpb2Hpb1HmgdHmbrHvwdHartHarfHantHgamHdgoHuthHjaiHkalHapmHhkl","UdeaUlicUdreUcrlNmanUwarNmagNpldNkljUvngUdthUmalUbalUvarUsylUanbUearUktlUticUclc"};
  for(int g=0;g<2;++g)for(int n=0;n<20;++n){auto s=groups[g]+n*4;uint32_t id=(uint32_t(uint8_t(s[0]))<<24)|(uint32_t(uint8_t(s[1]))<<16)|(uint32_t(uint8_t(s[2]))<<8)|uint8_t(s[3]);if(code==id)return g;}return -1;
 }
 void Observe(uint64_t identity,uint32_t code,bool dead,uint32_t seconds){
  int g=Group(code);if(g<0||!identity)return;
  if(!dead){
   if(deaths.count(identity)&&!observedDead.erase(identity))return;
   // A revived identity is also a new spawn, even if the engine reuses its handle.
   if(deaths.erase(identity)||tracked[g].insert(identity).second){due[g]=0;tracked[g].insert(identity);}
   return;
  }
  observedDead.insert(identity);if(deaths.count(identity))return;
  due[g]=seconds+31;deaths.insert(identity);for(auto id:tracked[g])deaths.insert(id);tracked[g].clear();
 }
 std::array<uint32_t,3> Remaining(uint32_t seconds)const{return {seconds<493?493-seconds:1007-(seconds-493)%1007,due[1]>seconds?due[1]-seconds:0,due[0]>seconds?due[0]-seconds:0};}
};
}
