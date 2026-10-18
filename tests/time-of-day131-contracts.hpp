#pragma once
inline void CheckTimeOfDay131(){
 check(!wht::TimeOfDayProfileFor131(129),"unknown time-of-day build refused");
 for(const auto& p:wht::timeOfDayProfiles131){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char frame[0x180]{};*reinterpret_cast<uintptr_t*>(frame)=base+p.table;
  const auto owner=reinterpret_cast<uintptr_t>(frame);auto& flags=*reinterpret_cast<unsigned*>(frame+(sizeof(uintptr_t)==8?0x150:0xb8));flags=0x100;
  unsigned sampled=0;check(wht::ReadTimeOfDayFlags131(base,p,owner,sampled)&&sampled==0x100,"independently laid-out FDF flags");
  ++*reinterpret_cast<uintptr_t*>(frame);check(!wht::ReadTimeOfDayFlags131(base,p,owner,sampled),"foreign indicator rejected");--*reinterpret_cast<uintptr_t*>(frame);
  wht::TimeOfDay131 display;unsigned shown=0,hidden=0;
  auto show=[&](uintptr_t f){check(f==owner,"current indicator shown");++shown;flags&=~1u;};auto hide=[&](uintptr_t f){check(f==owner,"current indicator hidden");++hidden;flags|=1u;};
  auto apply=[&](bool enabled){return display.Apply(owner,true,flags,enabled,show,hide);};
  apply(true);check(hidden==1&&flags==0x101,"hide only through native callback");apply(true);check(hidden==1,"already hidden causes no repeat callback");apply(false);check(shown==1&&flags==0x100,"disable restores own visibility and preserves flags");apply(false);check(shown==1,"inactive does not force show");
  flags=0x101;apply(true);apply(false);check(hidden==1&&shown==1&&flags==0x101,"originally hidden remains hidden");
  flags=2;apply(true);check(flags==3,"hide while parent hidden");apply(false);check(flags==2,"restore local bit without changing parent visibility");
  flags=0;apply(true);const unsigned before=shown;display.Apply(owner+8,true,1,false,[&](uintptr_t){++shown;},hide);check(shown==before,"changed frame never restores old pointer");
  display.Apply(owner,false,0,true,show,hide);check(!display.owner&&!display.active,"invalid current frame forgets state");
  check(!wht::TimeOfDayCode131(0,p)&&!wht::ReadTimeOfDayFlags131(base,p,0,sampled),"missing module/frame rejected");
 }
}
