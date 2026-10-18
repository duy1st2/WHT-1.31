#pragma once
inline void CheckLifeLabels131(){
 check(!wht::LifeProfileFor131(129),"unprofiled life labels refused");
 for(const auto& p:wht::lifeProfiles131){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char ui[0x20]{},portrait[0x400]{},unit[0x40]{},fonts[2][0x140]{};
  auto ptr=[](unsigned char* b,unsigned off)->uintptr_t&{return *reinterpret_cast<uintptr_t*>(b+off);};
  const auto u=reinterpret_cast<uintptr_t>(ui),port=reinterpret_cast<uintptr_t>(portrait),selected=reinterpret_cast<uintptr_t>(unit);
  ptr(ui,0)=base+p.ui;ptr(portrait,0)=base+p.portrait;ptr(unit,0)=base+p.unit;ptr(portrait,sizeof(uintptr_t)==8?0x3b8:0x244)=selected;
  auto ids=reinterpret_cast<uint32_t*>(unit+(sizeof(uintptr_t)==8?0x18:0xc));ids[0]=123;ids[1]=456;
  for(unsigned n=0;n<2;++n){ptr(fonts[n],0)=base+p.font;ptr(portrait,(sizeof(uintptr_t)==8?0x3c8:0x24c)+n*sizeof(uintptr_t))=reinterpret_cast<uintptr_t>(fonts[n]);}
  auto source=wht::ReadLifeSource131(base,p,u,port);check(source.ui==u&&source.unit==selected&&source.identity==((uint64_t(456)<<32)|123)&&source.fonts[0]==reinterpret_cast<uintptr_t>(fonts[0]),"native portrait label layout and unit identity");
  wht::LifeLabels131 labels;labels.Sync(source);std::array<std::string,2> actual{};unsigned writes=0;
  auto set=[&](uintptr_t font,const char* value){for(unsigned n=0;n<2;++n)if(font==source.fonts[n]){actual[n]=value;++writes;}};
  auto read=[&](uintptr_t font,std::string& value){for(unsigned n=0;n<2;++n)if(font==source.fonts[n]){value=actual[n];return true;}return false;};
  actual[0]="5000 / 9000";labels.Seed(0,actual[0].c_str());labels.Apply(1,read,set);check(actual[0]=="5,000 / 9,000","already visible native numbers format immediately after enabling");
  labels.Handle(source.fonts[0],"9000 / 10000",p.hpNumeric,p,0,set);labels.Handle(source.fonts[0],"",p.hpWidth,p,0,set);check(actual[0].empty(),"disabled policy follows native width clear");labels.Apply(2,read,set);check(actual[0]=="9000 / 10000","enabling No Hide restores pre-existing native width-clear cache");
  check(labels.Handle(source.fonts[0],"12345 / 67890",p.hpNumeric,p,1,set)&&actual[0]=="12,345 / 67,890","group native HP numeric write");
  check(labels.Handle(source.fonts[1],"1000 / 2000",p.mpNumeric,p,3,set)&&actual[1]=="1,000 / 2,000","group native mana numeric write");
  const auto count=writes;check(labels.Handle(source.fonts[1],"",p.mpWidth,p,3,set)&&writes==count&&actual[1]=="1,000 / 2,000","suppress only known mana width clear");
  labels.Apply(1,read,set);check(actual[1].empty(),"turning off No Hide restores native width clear");
  labels.Apply(3,read,set);check(actual[1]=="1,000 / 2,000","enabling No Hide recovers cached numeric label");
  labels.Apply(2,read,set);check(actual[0]=="12345 / 67890"&&actual[1]=="1000 / 2000","turning off commas restores native numbers");
  check(!labels.Handle(source.fonts[0],"",p.hpWidth+1,p,3,set)&&!labels.labels[0].valid,"unrelated caller clears cache and remains native");
  check(!labels.Handle(source.fonts[1],"9999 / 9999",p.hpNumeric,p,3,set),"HP caller cannot rewrite mana label");
  check(!labels.Handle(source.fonts[0],"|cff00ff001 / 2|r",p.hpNumeric,p,3,set),"color escape labels remain untouched");
  labels.Handle(source.fonts[0],"1000 / 2000",p.hpNumeric,p,1,set);actual[0]="map text";const unsigned before=writes;labels.Apply(0,read,set);check(actual[0]=="map text"&&writes==before,"newer native text never overwritten by cached values");
  labels.Handle(source.fonts[0],"1000 / 2000",p.hpNumeric,p,3,set);auto next=source;++next.identity;labels.Sync(next);check(!labels.labels[0].valid,"reused unit identity invalidates cached text");
  ++ptr(fonts[0],0);check(!wht::ReadLifeSource131(base,p,u,port).unit,"foreign font class refused");
  check(!wht::LifeCode131(0,p),"missing font code refused");
 }
}
