#pragma once
inline void CheckInvulnerableBars131(){
 check(!wht::InvulnerableProfileFor131(129),"unverified invulnerable profile refused");
 for(const auto& p:wht::invulnerableProfiles131){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char bar[0x230]{},unit[0x70]{};
  const auto hp=reinterpret_cast<uintptr_t>(bar),u=reinterpret_cast<uintptr_t>(unit);const unsigned visible=sizeof(uintptr_t)==8?0xf0:0x9c,unitOffset=sizeof(uintptr_t)==8?0x210:0x154,kind=sizeof(uintptr_t)==8?0x1e8:0x134,flagsOffset=sizeof(uintptr_t)==8?0x38:0x20;
  *reinterpret_cast<uintptr_t*>(bar)=base+p.bar;*reinterpret_cast<uintptr_t*>(unit)=base+p.unit;*reinterpret_cast<unsigned*>(bar+kind)=5;*reinterpret_cast<uintptr_t*>(bar+unitOffset)=u;bar[visible]=bar[visible+1]=1;bar[visible+2]=0xab;
  auto& flags=*reinterpret_cast<unsigned*>(unit+flagsOffset);flags=0x408;
  bool invul=false,attached=false;check(wht::ReadInvulnerableBar131(base,p,hp,u,invul,attached)&&invul&&attached,"native invulnerability bit sampled");
  wht::InvulnerableBars131 feature;unsigned shows=0,hides=0;bool parent=true;
  auto show=[&](uintptr_t f){check(f==hp,"restore correct bar");++shows;bar[visible+1]=bar[visible]&&parent;};
  auto hide=[&](uintptr_t f){check(f==hp,"hide correct bar");++hides;bar[visible+1]=0;};
  auto apply=[&](bool enabled){return feature.Apply(hp,(flags&8)!=0,enabled,bar[visible+1]!=0,show,hide);};
  check(apply(true)&&hides==1&&bar[visible]==1&&bar[visible+1]==0&&flags==0x408,"hide changes attachment only, not requested visibility or unit flags");
  apply(true);check(hides==1,"no redundant hide");bar[visible+1]=1;apply(true);check(hides==2,"native re-show suppressed");
  flags&=~8u;check(!apply(true)&&shows==1&&bar[visible+1]==1,"losing invulnerability restores");
  flags|=8;apply(true);apply(false);check(shows==2&&bar[visible+1]==1,"disabling restores through native show");
  apply(true);bar[visible]=0;apply(false);check(shows==3&&bar[visible+1]==0&&bar[visible]==0,"engine-requested hidden bar not forced visible");
  bar[visible]=bar[visible+1]=1;apply(true);parent=false;apply(false);check(bar[visible+1]==0,"parent fog-hidden remains hidden");parent=true;
  apply(true);feature.Prune({});check(feature.suppressed.empty(),"destroyed bars pruned before restore");const auto count=shows;apply(false);check(shows==count,"forgotten frame not restored");
  flags=0x400;check(wht::ReadInvulnerableBar131(base,p,hp,u,invul,attached)&&!invul,"other flags do not imply invulnerability");
  check(bar[visible+2]==0xab,"adjacent visibility flags preserved");*reinterpret_cast<uintptr_t*>(bar+unitOffset)=0;check(!wht::ReadInvulnerableBar131(base,p,hp,u,invul,attached),"bar bound to another unit refused");
  check(!wht::InvulnerableCode131(0,p),"missing code module refused");feature.Reset();check(feature.suppressed.empty()&&!feature.ready,"match reset clears tracking");
 }
}
