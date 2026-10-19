#pragma once
#include "mth_route131.hpp"
#include <fstream>
inline void CheckMthRoute131(){
 for(const auto& p:wht::pointProfiles131){
#ifdef _WIN64
  const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);if(!input)continue;std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});const auto base=reinterpret_cast<uintptr_t>(memory.data());
#ifndef _WIN64
  *reinterpret_cast<uintptr_t*>(memory.data()+p.entry+0x4d)=base+p.table;
#endif
  check(wht::PointNative131(base,p.build)!=nullptr,"captured point order entry/action11/table/direct native caller verified");
  memory[p.caller]^=1;check(!wht::PointNative131(base,p.build),"changed native caller refuses point order");
 }
 wht::SelfCastSource131 source{1,2,3,{4,5},0,0};
 for(int route=0;route<6;++route){wht::MthRoute131 worker;unsigned sends=0;float tx=0,ty=0;auto read=[&](auto& out){out=source;return true;};
  auto send=[&](const auto&,float x,float y){++sends;tx=x;ty=y;return true;};
  check(worker.Start(route,2,source),"all six reference routes start");worker.Tick(0,true,true,read,send);const auto r=wht::routes131[route];
  check(sends==1&&tx==r.x1&&ty==r.y1,"first leg uses reference coordinates");
  worker.Tick(50,true,true,read,send);check(sends==1,"100ms route cadence");
  worker.Tick(100,true,true,read,send);check(sends==1,"walking does not repeat movement orders");
  source.x=r.x1;source.y=r.y1;worker.Tick(200,true,true,read,send);check(sends==2&&tx==r.x2&&ty==r.y2&&worker.remaining==2,"second destination after arrival");
  source.x=r.x2;source.y=r.y2;worker.Tick(300,true,true,read,send);check(sends==3&&worker.remaining==1,"one full lap counted only at second destination");
  source.x=r.x1;source.y=r.y1;worker.Tick(400,true,true,read,send);source.x=r.x2;source.y=r.y2;check(worker.Tick(500,true,true,read,send)&&worker.route<0&&sends==4,"two laps finish with exactly four orders");source.x=source.y=0;
 }
 wht::MthRoute131 worker;unsigned count=0;auto read=[&](auto& out){out=source;return true;};auto send=[&](const auto&,float,float){++count;return true;};
 check(!worker.Start(6,1,source)&&!worker.Start(0,0,source)&&!worker.Start(0,21,source),"invalid route/count refused");
 worker.Start(0,1,source);worker.Tick(0,true,true,read,send);worker.Tick(100,true,false,read,send);check(count==1,"menu/focus gate pauses without adding orders");
 worker.Tick(30000,true,true,read,send);check(worker.route<0&&count==1,"unreached destination times out after30s");
 worker.Start(0,1,source);source.id.high++;worker.Tick(40000,true,true,read,send);check(worker.route<0&&count==1,"changed identity cancels before any order");
 worker.Start(0,1,source);worker.Tick(41000,false,true,read,send);check(worker.route<0&&count==1,"disabled MTH master cancels");
 worker.Start(0,1,source);check(worker.Start(1,2,source)&&worker.route==1,"valid new route replaces old reference request");
 worker.Tick(42000,true,true,read,[](const auto&,float,float){return false;});check(worker.route<0,"native refusal cancels without retry");
}
