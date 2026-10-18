#pragma once
inline void CheckStatsLabels131(){
 check(!wht::StatsProfileFor131(129),"foreign stats profile refused");
 for(const auto& p:wht::statsProfiles131){
  for(unsigned n=0;n<5;++n)check(wht::StatsKind131(p.callers[n],p)==int(n),"five exact native stats callers");
  check(wht::StatsKind131(p.callers[0]+1,p)==-1,"unknown stats caller refused");
  check(!wht::StatsCode131(0,p,*wht::LifeProfileFor131(p.build)),"missing stats code refused");
 }
 alignas(uintptr_t) unsigned char stack[0x260]{};*reinterpret_cast<uintptr_t*>(stack+wht::statsOuterOffset131)=12345;
 check(wht::StatsOuter131(reinterpret_cast<uintptr_t>(stack))==12345,"architecture specific outer return slot");
 wht::StatsLabels131 labels;wht::LifeSource131 owner;owner.ui=1;owner.unit=2;owner.identity=3;labels.Sync(owner);
 std::string actual;unsigned writes=0;auto set=[&](uintptr_t,const char* value){actual=value;++writes;};
 auto read=[&](uintptr_t,std::string& value){value=actual;return true;};
 check(labels.Handle(10,"12345 - 67890",0,true,true,set)&&actual=="12,345 - 67,890","damage range grouped");
 labels.Apply(false,read,set);check(actual=="12345 - 67890","disable restores native text");
 labels.Apply(true,read,set);check(actual=="12,345 - 67,890","enable restores grouping");
 actual="new map text";const auto before=writes;labels.Apply(false,read,set);check(writes==before&&actual=="new map text","newer text preserved");
 check(labels.Handle(10,"12345 |CFF00FF00+1000|R",2,true,true,set)&&actual=="12,345 |CFF00FF00+1,000|R","attribute bonus markup preserved");
 check(!labels.Handle(10,"12345",-1,true,true,set)&&!labels.labels[0].font,"unknown caller invalidates cache");
 check(!labels.Handle(10,"12345",1,false,true,set),"foreign font refused");
 check(!labels.Handle(10,"Invulnerable",1,true,true,set),"nonnumeric armor untouched");
 labels.Handle(10,"12345",3,true,true,set);++owner.identity;labels.Sync(owner);check(!labels.labels[0].font,"unit identity change clears cache");
 std::string grouped;check(wht::GroupStatsText("-12345 |CFFFF0000-1000.5|R",grouped)&&grouped=="-12,345 |CFFFF0000-1,000.5|R","signed fractional bonus preserved");
}
