#pragma once
#include "detailed_tooltip131.hpp"
#include <fstream>
inline uintptr_t detailMoveObjectTest131=0;inline unsigned detailNativeCallsTest131=0;inline bool detailExtendedTest131=false;
#ifdef _WIN64
#define DETAIL_EDX131
#else
#define DETAIL_EDX131 ,void*
#endif
inline uintptr_t __fastcall DetailMoveTest131(uintptr_t object DETAIL_EDX131,float* result){check(object==detailMoveObjectTest131,"movement native this preserved");*result=312.5f;return reinterpret_cast<uintptr_t>(result);}
inline void __fastcall DetailNormalTest131(uintptr_t object DETAIL_EDX131,const char* title,const char* body,unsigned a,unsigned b,unsigned c,bool reset,uintptr_t item){
 check(object==77&&std::string(title)=="title"&&std::string(body)=="body"&&!a&&!b&&!c&&reset&&!item,"normal native tooltip ABI and reset/cost arguments");++detailNativeCallsTest131;detailExtendedTest131=false;
}
inline void __fastcall DetailExtendedTest131(uintptr_t object DETAIL_EDX131,const char* title,const char* body,const char* extra,unsigned a,unsigned b,unsigned c,uintptr_t item){
 check(object==77&&std::string(title)=="title"&&std::string(body)=="body"&&std::string(extra)=="extra"&&!a&&!b&&!c&&!item,"extended native tooltip ABI retains extra description");++detailNativeCallsTest131;detailExtendedTest131=true;
}
#undef DETAIL_EDX131
inline void CheckDetailedTooltip131(){
 wht::DetailNativeApi131 api{reinterpret_cast<decltype(api.normal)>(DetailNormalTest131),reinterpret_cast<decltype(api.extended)>(DetailExtendedTest131),reinterpret_cast<decltype(api.movement)>(DetailMoveTest131)};
 wht::DetailView131 abi;abi.frame=77;abi.title="title";abi.extra="extra";
 for(unsigned n=0;n<256;++n){abi.extended=false;check(wht::WriteDetail131(api,abi,"body")&&!detailExtendedTest131,"normal builder forwarded");abi.extended=true;check(wht::WriteDetail131(api,abi,"body")&&detailExtendedTest131,"advanced builder forwarded");}
 check(detailNativeCallsTest131==512,"one native call per requested write");
 wht::DetailView131 v;v.frame=1;v.anchor=2;v.body="native damage";v.current=v.body;wht::DetailText131 state;unsigned writes=0;
 auto write=[&](const std::string& value){v.current=value;++writes;return true;};
 const auto detail=wht::AttackSpeedTooltip(2,2,5,.2f);
 check(state.Apply(v,3,4,detail,true,write)&&v.current.find("1.0/sec")!=std::string::npos&&v.current.find("+100%")!=std::string::npos,"reference detailed attacks retain native body");
 check(state.Apply(v,3,4,detail,true,write)&&writes==1,"stationary unchanged tooltip does not rebuild layout");
 check(state.Apply(v,3,4,"",false,write)&&v.current==v.body&&writes==2,"disable restores native body");
 state.Apply(v,3,4,detail,true,write);v.current="another writer";check(!state.Apply(v,3,4,detail,true,write)&&v.current=="another writer","newer foreign tooltip text preserved");
 v.current=v.body;state.Apply(v,3,4,detail,true,write);check(!state.Apply(v,3,5,detail,true,write),"changed unit identity cannot own previous output");
 v.current=v.body;state.Reset();check(!state.Apply(v,3,4,std::string(1025,'x'),true,write)&&v.current==v.body,"oversize result refused without truncation");
 check(wht::AttackSpeedTooltip(.1f,2,5,.2f).find("0.1/sec")!=std::string::npos,"native map minimum clamps attack rate");
 check(wht::AttackSpeedTooltip(7,2,5,.2f).find("2.5/sec")!=std::string::npos,"native map maximum clamps attack rate");
 for(const auto& p:wht::detailProfiles131){
  // Optional captured-image audit: retain relative calls/code, relocate only
  // the two known x86 absolute clamp operands to this allocated image.
#ifdef _WIN64
  const unsigned capturedPid=p.build==12164?6688:29928;
#else
  const unsigned capturedPid=p.build==12164?14740:23884;
#endif
  std::ifstream capture("../analysis/memory-"+std::to_string(capturedPid)+".bin",std::ios::binary|std::ios::ate);
  if(capture){const auto length=capture.tellg();check(length>0,"capture is nonempty");std::vector<unsigned char> image(static_cast<size_t>(length));capture.seekg(0);capture.read(reinterpret_cast<char*>(image.data()),image.size());check(bool(capture),"complete capture read");
   const auto address=reinterpret_cast<uintptr_t>(image.data());
#ifndef _WIN64
   *reinterpret_cast<uintptr_t*>(image.data()+p.clamp+10)=address+p.minimum;*reinterpret_cast<uintptr_t*>(image.data()+p.clamp+0x33)=address+p.maximum;
#endif
   check(wht::DetailCode131(address,p),"captured real 1.31 code matches all detailed tooltip guards");
   std::cout<<"Captured detailed tooltip profile verified: "<<p.build<<"\n";
  }
  const auto* life=wht::LifeProfileFor131(p.build);check(life!=nullptr,"detail build has matching portrait profile");
  const size_t bytes=(std::max)({p.minimum,p.maximum,p.normalRef,p.extendedRef,p.attackLoad,p.moveLoad,p.clamp,p.moveGet,p.normalSet,p.extendedSet})+256;
  std::vector<unsigned char> memory(bytes);const uintptr_t base=reinterpret_cast<uintptr_t>(memory.data());
  for(auto item:{std::pair<uintptr_t,const wht::DetailSignature131*>{p.normalSet,&p.normalSignature},{p.extendedSet,&p.extendedSignature},{p.moveGet,&p.moveSignature}})memcpy(memory.data()+item.first,item.second->bytes.data(),16);
  for(auto item:{std::pair<uintptr_t,uintptr_t>{p.normalRef,p.normalSet},{p.extendedRef,p.extendedSet}}){memory[item.first]=0xe8;*reinterpret_cast<int32_t*>(memory.data()+item.first+1)=int32_t(item.second-item.first-5);}
#ifdef _WIN64
  const unsigned char a[]={0x48,0x8b,0x9f,0x70,5,0,0},m[]={0x4d,0x8b,0xb4,0x24,0x60,5,0,0};memcpy(memory.data()+p.attackLoad,a,sizeof(a));memcpy(memory.data()+p.moveLoad,m,sizeof(m));
  memcpy(memory.data()+p.clamp+0x10,"\xf3\x0f\x10\x05",4);*reinterpret_cast<int32_t*>(memory.data()+p.clamp+0x14)=int32_t(p.minimum-p.clamp-0x18);
  memcpy(memory.data()+p.clamp+0x32,"\x0f\x2f\x0d",3);*reinterpret_cast<int32_t*>(memory.data()+p.clamp+0x35)=int32_t(p.maximum-p.clamp-0x39);
#else
  memcpy(memory.data()+p.attackLoad,"\x8b\xb6",2);memcpy(memory.data()+p.moveLoad,"\x8b\xb6",2);
  *reinterpret_cast<unsigned*>(memory.data()+p.attackLoad+2)=p.build==12164?0x460:0x458;*reinterpret_cast<unsigned*>(memory.data()+p.moveLoad+2)=p.build==12164?0x454:0x44c;
  memcpy(memory.data()+p.clamp+6,"\xf3\x0f\x10\x05",4);*reinterpret_cast<uintptr_t*>(memory.data()+p.clamp+10)=base+p.minimum;
  memcpy(memory.data()+p.clamp+0x30,"\x0f\x2f\x0d",3);*reinterpret_cast<uintptr_t*>(memory.data()+p.clamp+0x33)=base+p.maximum;
#endif
  check(wht::DetailCode131(base,p),"complete exact native profile guard");memory[p.normalRef]=0x90;check(!wht::DetailCode131(base,p),"foreign tooltip call refused");memory[p.normalRef]=0xe8;
  alignas(uintptr_t) unsigned char unit[0x700]{},attack[0x700]{},move[0x100]{};
  const uintptr_t object=reinterpret_cast<uintptr_t>(unit);*reinterpret_cast<uintptr_t*>(unit)=base+life->unit;*reinterpret_cast<uintptr_t*>(attack)=base+p.attack;*reinterpret_cast<uintptr_t*>(move)=base+p.move;
  *reinterpret_cast<uintptr_t*>(unit+(sizeof(void*)==8?0x570:p.build==12164?0x460:0x458))=reinterpret_cast<uintptr_t>(attack);
  *reinterpret_cast<uintptr_t*>(unit+(sizeof(void*)==8?0x560:p.build==12164?0x454:0x44c))=reinterpret_cast<uintptr_t>(move);
  *reinterpret_cast<uintptr_t*>(move+(sizeof(void*)==8?0x50:0x30))=object;
  const unsigned speed=sizeof(void*)==8?0x268:0x1b0,period=sizeof(void*)==8?0x1d8:0x158,stride=sizeof(void*)==8?16:8;
  *reinterpret_cast<uintptr_t*>(attack+speed)=base+p.miniB;*reinterpret_cast<float*>(attack+speed+sizeof(void*))=2;
  for(unsigned weapon=0;weapon<2;++weapon){*reinterpret_cast<uintptr_t*>(attack+period+weapon*stride)=base+p.mini;*reinterpret_cast<float*>(attack+period+weapon*stride+sizeof(void*))=weapon?3.f:2.f;}
  *reinterpret_cast<float*>(memory.data()+p.minimum)=.2f;*reinterpret_cast<float*>(memory.data()+p.maximum)=5;
  float multiplier=0,cooldown=0,maximum=0,minimum=0;check(wht::DetailAttack131(base,p,*life,object,0,multiplier,cooldown,maximum,minimum)&&multiplier==2&&cooldown==2&&maximum==5&&minimum==.2f,"exact native ability and bounded speed/cooldown fields");
  check(wht::DetailAttack131(base,p,*life,object,1,multiplier,cooldown,maximum,minimum)&&cooldown==3,"second attack uses second base cooldown");
  check(!wht::DetailAttack131(base,p,*life,object,2,multiplier,cooldown,maximum,minimum),"unknown weapon index refused");
  detailMoveObjectTest131=reinterpret_cast<uintptr_t>(move);float movement=0;check(wht::DetailMovement131(base,p,*life,object,api,movement)&&movement==312.5f,"movement getter ABI and exact owner");
  *reinterpret_cast<uintptr_t*>(move+(sizeof(void*)==8?0x50:0x30))=0;check(!wht::DetailMovement131(base,p,*life,object,api,movement),"missing movement owner cannot invoke fallback getter");
  alignas(uintptr_t) unsigned char frame[0x300]{},anchor[0xb00]{},font[0x140]{};
  *reinterpret_cast<uintptr_t*>(frame)=base+p.basic;frame[wht::detailAttached131]=frame[wht::detailAttached131-1]=1;
  *reinterpret_cast<uintptr_t*>(frame+wht::detailSource131)=reinterpret_cast<uintptr_t>(anchor);*reinterpret_cast<uintptr_t*>(frame+wht::detailSource131+2*sizeof(void*))=reinterpret_cast<uintptr_t>(font);
  *reinterpret_cast<uintptr_t*>(font)=base+life->font;const char text[]="native body";*reinterpret_cast<const char**>(font+wht::detailFontText131)=text;
  strcpy_s(reinterpret_cast<char*>(anchor+wht::detailSource131),256,"title");strcpy_s(reinterpret_cast<char*>(anchor+wht::detailBody131),1025,text);
  wht::DetailView131 view;check(wht::ReadDetailView131(base,p,*life,reinterpret_cast<uintptr_t>(frame),false,view)&&view.current==text&&view.body==text,"native tooltip anchor/font/body ownership");
  const wht::StatsPanels131* panels=nullptr;for(const auto& value:wht::statsPanels131)if(value.build==p.build)panels=&value;
  alignas(uintptr_t) unsigned char info[0x680]{},primary[0x220]{},secondary[0x220]{},armor[0x220]{};
  *reinterpret_cast<uintptr_t*>(primary)=*reinterpret_cast<uintptr_t*>(secondary)=base+panels->damage;*reinterpret_cast<uintptr_t*>(armor)=base+panels->armor;
  const unsigned first=sizeof(void*)==8?0x620:0x3e0;
  *reinterpret_cast<uintptr_t*>(info+first)=reinterpret_cast<uintptr_t>(primary);*reinterpret_cast<uintptr_t*>(info+first+sizeof(void*))=reinterpret_cast<uintptr_t>(secondary);*reinterpret_cast<uintptr_t*>(info+first+2*sizeof(void*))=reinterpret_cast<uintptr_t>(armor);
  *reinterpret_cast<uintptr_t*>(primary+wht::detailHelper131)=11;*reinterpret_cast<uintptr_t*>(secondary+wht::detailHelper131)=22;*reinterpret_cast<uintptr_t*>(armor+wht::detailHelper131)=33;
  wht::DetailedTooltip131 discovery;discovery.Discover(base,p.build,reinterpret_cast<uintptr_t>(info),100);check(discovery.panels.size()==3&&discovery.panels[0].weapon==0&&discovery.panels[1].weapon==1,"typed native panel order supplies correct weapon");
  discovery.Discover(base,p.build,reinterpret_cast<uintptr_t>(info),10000);check(discovery.next==200,"complete live slot cache avoids repeated broad scan");
  *reinterpret_cast<uintptr_t*>(info+first)=0;discovery.Discover(base,p.build,reinterpret_cast<uintptr_t>(info),10001);check(discovery.panels.size()==2&&discovery.next==10101,"changed native slot invalidates cached ownership");
  frame[wht::detailAttached131]=0;check(!wht::ReadDetailView131(base,p,*life,reinterpret_cast<uintptr_t>(frame),false,view),"hidden tooltip never becomes visible through feature");
 }
 check(!wht::DetailProfileFor131(129),"no inherited detailed-tooltip native addresses");
}
