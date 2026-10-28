#pragma once
#include "mth_route131.hpp"
#include <fstream>
inline void CheckMthRoute131(){
 int parsed=-1;
 for(const auto* name:{L"#chayga",L"#chaysoi",L"#caysoi"}){
  const int offset=std::wstring(name)==L"#chayga"?0:3;
  check(wht::IsRouteCommand131(name)&&wht::ParseRouteCommand131(name,L"",parsed)&&parsed==offset,"legacy courier command default route1");
  for(int i=1;i<=3;++i)check(wht::ParseRouteCommand131(name,std::to_wstring(i),parsed)&&parsed==offset+i-1,"legacy aliases map each native route exactly");
  for(const auto* value:{L"0",L"-1",L"4"})check(wht::ParseRouteCommand131(name,value,parsed)&&parsed==offset,"reference out of range index defaults1");
  for(const auto* bad:{L"abc",L"1 extra",L"1x",L"2\n",L"99999999999999999999"})check(!wht::ParseRouteCommand131(name,bad,parsed)&&parsed==-1,"invalid route alias cannot start a worker");
 }
 check(!wht::ParseRouteCommand131(L"#chayga",std::wstring(L"1\0x",3),parsed),"embedded NUL refused");
 const wchar_t* canonical[]={L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};
 for(int i=0;i<6;++i)check(wht::ParseRouteCommand131(canonical[i],L"",parsed)&&parsed==i&&!wht::ParseRouteCommand131(canonical[i],L"1",parsed),"existing button/hotkey route grammar retained");
 check(!wht::IsRouteCommand131(L"#soi")&&!wht::ParseRouteCommand131(L"#other",L"",parsed),"unrelated existing command not intercepted");

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
