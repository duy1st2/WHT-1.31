#pragma once
inline void CheckHpPlayerColor131(){
 bool enemy=true;
 check(wht::HpEnemyPolicy131(0,0,0,false,false,0,enemy)&&!enemy,"own units use configured normal HP color");
 check(wht::HpEnemyPolicy131(1,3,0,false,true,1u<<3,enemy)&&!enemy,"alliance uses physical player id rather than slot");
 check(wht::HpEnemyPolicy131(1,3,0,false,true,0,enemy)&&enemy,"known non-ally uses enemy HP color");
 check(wht::HpEnemyPolicy131(1,3,0,true,false,0,enemy)&&!enemy,"observer keeps normal HP color as 1.29");
 for(unsigned owner=24;owner<28;++owner)check(wht::HpEnemyPolicy131(owner,owner,0,false,false,0,enemy)&&!enemy,"neutral slots keep reference normal HP color");
 check(!wht::HpEnemyPolicy131(1,3,0,false,false,0,enemy)&&!wht::HpEnemyPolicy131(1,24,0,false,true,0,enemy)&&!wht::HpEnemyPolicy131(28,0,0,false,true,0,enemy)&&!wht::HpEnemyPolicy131(1,1,24,false,true,0,enemy),"unknown alliance or invalid player never guesses enemy");

 check(!wht::HpColorProfileFor131(129),"unprofiled HP color build refused");uint32_t palette=0;
 check(wht::PlayerColor129(0,palette)&&palette==0xffff0303&&wht::PlayerColor129(23,palette)&&palette==0xffa46f33&&!wht::PlayerColor129(24,palette),"reference 24-color palette and bounds");
 for(const auto& p:wht::hpColorProfiles131){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char bar[0x230]{},unit[0x40]{},texture[0x100]{};
  const auto hp=reinterpret_cast<uintptr_t>(bar),u=reinterpret_cast<uintptr_t>(unit),t=reinterpret_cast<uintptr_t>(texture);
  *reinterpret_cast<uintptr_t*>(bar)=base+p.bar;*reinterpret_cast<uintptr_t*>(unit)=base+p.unit;*reinterpret_cast<uintptr_t*>(texture)=base+p.texture;
  *reinterpret_cast<unsigned*>(bar+(sizeof(uintptr_t)==8?0x1e8:0x134))=5;*reinterpret_cast<uintptr_t*>(bar+(sizeof(uintptr_t)==8?0x210:0x154))=u;*reinterpret_cast<uintptr_t*>(bar+(sizeof(uintptr_t)==8?0x1e0:0x130))=t;
  auto bytes=texture+(sizeof(uintptr_t)==8?0xb8:0x74);bytes[0]=0xab;bytes[1]=0x33;bytes[2]=0x22;bytes[3]=0x11;
  uintptr_t sampled=0;uint32_t color=0;check(wht::ReadHpTexture131(base,p,hp,u,sampled,color)&&sampled==t&&color==0xab112233,"native texture byte order ARGB");
  wht::HpPlayerColor131::TintState state;unsigned writes=0;uint32_t native=color;
  auto set=[&](uintptr_t tex,uint32_t next){check(tex==t,"current texture receiver");native=next;++writes;return true;};
  check(wht::HpPlayerColor131::Tint(state,t,native,true,0xffff0303,set)&&native==0xffff0303&&writes==1,"apply player color");
  wht::HpPlayerColor131::Tint(state,t,native,true,0xffff0303,set);check(writes==1,"unchanged color avoids native call");
  wht::HpPlayerColor131::Tint(state,t,native,false,0,set);check(native==color&&!state.active,"disable restores original color");
  wht::HpPlayerColor131::Tint(state,t,native,true,0xffff0303,set);native=0xffabcdef;const auto before=writes;wht::HpPlayerColor131::Tint(state,t,native,false,0,set);check(writes==before&&native==0xffabcdef,"engine paint not overwritten on disable");
  wht::HpPlayerColor131::Tint(state,t,native,true,0xffff0303,set);native=0xff123456;wht::HpPlayerColor131::Tint(state,t,native,true,0xff0042ff,set);wht::HpPlayerColor131::Tint(state,t,native,false,0,set);check(native==0xff123456,"latest engine color captured before reapplying");
  state={};check(!wht::HpPlayerColor131::Tint(state,t,native,true,0xffff0303,[](uintptr_t,uint32_t){return false;})&&!state.active,"failed setter not marked as success");
  ++*reinterpret_cast<uintptr_t*>(texture);check(!wht::ReadHpTexture131(base,p,hp,u,sampled,color),"foreign texture refused");--*reinterpret_cast<uintptr_t*>(texture);
  *reinterpret_cast<uintptr_t*>(bar+(sizeof(uintptr_t)==8?0x210:0x154))=0;check(!wht::ReadHpTexture131(base,p,hp,u,sampled,color),"rebound HP bar refused");
  wht::HpPlayerColor131 colors;colors.entries[hp]={};colors.Prune({});check(colors.entries.empty(),"destroyed bar tracking pruned");check(!wht::HpColorCode131(0,p),"missing module rejected");
 }
}
