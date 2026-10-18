#pragma once
inline void CheckHeroKeys131(){
 wht::Config c;c.heroes.fill(0);check(wht::HeroKeySlot131(0,c)==-1,"disabled hero binding");
 for(unsigned i=0;i<7;++i)c.heroes[i]='A'+i;
 for(unsigned i=0;i<7;++i)check(wht::HeroKeySlot131('A'+i,c)==int(i),"all seven configured hero keys");
 c.heroes[6]='A';check(wht::HeroKeySlot131('A',c)==0,"first hero wins duplicate key");
 check(wht::HeroKeySlot131('Z',c)==-1&&!wht::HeroProfileFor131(129),"unbound key and unknown build refused");
 for(const auto& p:wht::heroProfiles131){
  constexpr bool wide=sizeof(uintptr_t)==8;const uintptr_t base=0x10000000;
  alignas(uintptr_t) unsigned char bar[0x280]{},rows[7*0x20]{},buttons[7][0x350]{};uintptr_t columns[7]{};
  auto ptr=[](unsigned char* b,unsigned off)->uintptr_t&{return *reinterpret_cast<uintptr_t*>(b+off);};
  auto num=[](unsigned char* b,unsigned off)->unsigned&{return *reinterpret_cast<unsigned*>(b+off);};
  const unsigned dims=wide?0x1d0:0x120,grid=wide?0x1f8:0x148,listener=wide?0x218:0x158,link=wide?0x1d0:0x120,unit=wide?0x2d8:0x1c4,visible=wide?0xf1:0x9d;
  ptr(bar,0)=base+p.bar;bar[visible]=1;num(bar,dims)=7;num(bar,dims+4)=1;num(bar,grid)=7;ptr(bar,listener)=base+p.listener;ptr(bar,grid+sizeof(uintptr_t))=reinterpret_cast<uintptr_t>(rows);
  for(unsigned i=0;i<7;++i){auto row=rows+i*(wide?0x20:0x10);num(row,0)=1;ptr(row,sizeof(uintptr_t))=reinterpret_cast<uintptr_t>(&columns[i]);columns[i]=reinterpret_cast<uintptr_t>(buttons[i]);ptr(buttons[i],0)=base+p.button;buttons[i][visible]=1;ptr(buttons[i],unit)=0x1234+i;ptr(buttons[i],link)=reinterpret_cast<uintptr_t>(bar)+listener;}
  auto get=[&](unsigned i){return wht::HeroBarButton131(base,p,reinterpret_cast<uintptr_t>(bar),i);};
  for(unsigned i=0;i<7;++i)check(get(i)==reinterpret_cast<uintptr_t>(buttons[i]),"native hero grid resolves all seven rows");
  check(!get(7)&&!get(unsigned(-1)),"hero index bounds");
  buttons[2][visible]=0;check(!get(2),"hidden hero refused");buttons[2][visible]=1;
  ptr(buttons[2],unit)=0;check(!get(2),"empty hero refused");ptr(buttons[2],unit)=0x1234;
  ++ptr(buttons[2],link);check(!get(2),"foreign listener refused");--ptr(buttons[2],link);
  ++ptr(buttons[2],0);check(!get(2),"wrong button class refused");--ptr(buttons[2],0);
  num(bar,dims+4)=2;check(!get(0),"wrong grid shape refused");num(bar,dims+4)=1;
  ptr(bar,listener)=0;check(!get(0),"wrong hero callback class refused");ptr(bar,listener)=base+p.listener;
  num(rows,0)=0;check(!get(0),"missing row refused");num(rows,0)=1;
  bar[visible]=0;check(!get(0),"hidden hero bar refused");
 }
}
