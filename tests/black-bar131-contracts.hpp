#pragma once
inline void CheckBlackBar131(){
 check(!wht::BlackBarProfileFor131(129),"unknown black bar profile refused");
 for(const auto& p:wht::blackBarProfiles131){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char ui[0x300]{},viewport[0x300]{},anchors[2][0x30]{};
  auto ptr=[](unsigned char* p,unsigned off)->uintptr_t&{return *reinterpret_cast<uintptr_t*>(p+off);};
  constexpr unsigned offset=sizeof(uintptr_t)==8?0x158:0xbc;auto u=reinterpret_cast<uintptr_t>(ui),v=reinterpret_cast<uintptr_t>(viewport);uintptr_t root=0,layout=0;
  ptr(ui,0)=base+p.uiTable;ptr(viewport,0)=base+p.worldTable;*reinterpret_cast<unsigned*>(ui+offset+sizeof(uintptr_t))=9;*reinterpret_cast<unsigned*>(viewport+offset+sizeof(uintptr_t))=9;
  check(wht::BlackBarLayouts131(base,p,u,v,root,layout)&&root==u+offset&&layout==v+offset,"independently laid out FDF viewport/root");
  unsigned points[]={6,2};
  for(unsigned i=0;i<2;++i){auto a=anchors[i];ptr(a,0)=base+p.anchorTable;ptr(a,sizeof(uintptr_t))=root;*reinterpret_cast<int*>(a+2*sizeof(uintptr_t))=int(points[i]);*reinterpret_cast<float*>(a+2*sizeof(uintptr_t)+4)=0;*reinterpret_cast<float*>(a+2*sizeof(uintptr_t)+8)=i?-.02f:.13f;ptr(viewport,offset+2*sizeof(uintptr_t)+points[i]*sizeof(uintptr_t))=reinterpret_cast<uintptr_t>(a);}
  std::array<wht::LayoutPoint131,2> initial{};
  check(wht::ReadLayoutPoint131(base,p,layout,root,6,initial[0])&&wht::ReadLayoutPoint131(base,p,layout,root,2,initial[1]),"native relative anchors read");
  auto current=initial;wht::BlackBar131 feature;unsigned writes=0;
  auto set=[&](unsigned point,const wht::LayoutPoint131& value){check(point==6||point==2,"only two viewport anchors changed");current[point==6?0:1]=value;++writes;return true;};
  check(feature.Apply(u,layout,root,current,true,set)&&writes==1&&current[0].y==0&&current[1].y==-.02f,"expand viewport preserving top inset");
  feature.Apply(u,layout,root,current,true,set);check(writes==1,"stable layout causes no redundant setter");
  feature.Apply(u,layout,root,current,false,set);check(current==initial&&!feature.Active(),"disable restores original anchors");
  feature.Apply(u,layout,root,current,true,set);current[0].y=.05f;const auto before=current;feature.Apply(u,layout,root,current,false,set);check(current==before,"map layout change not overwritten on disable");
  feature.Reset();current=initial;feature.Apply(u,layout,root,current,true,set);const auto n=writes;feature.Apply(u+8,layout+8,root,current,false,set);check(writes==n&&!feature.Active(),"new UI forgets previous anchors");
  feature.Reset();current=initial;check(!feature.Apply(u,layout,root,current,true,[](unsigned,const wht::LayoutPoint131&){return false;})&&!feature.Active(),"failed setter not recorded as a successful write");
  auto a=anchors[0];ptr(a,sizeof(uintptr_t))=root+8;wht::LayoutPoint131 sampled;check(!wht::ReadLayoutPoint131(base,p,layout,root,6,sampled),"unrelated relative frame refused");ptr(a,sizeof(uintptr_t))=root;
  *reinterpret_cast<float*>(a+2*sizeof(uintptr_t)+4)=NAN;check(!wht::ReadLayoutPoint131(base,p,layout,root,6,sampled),"nonfinite anchor refused");
  ++ptr(viewport,0);check(!wht::BlackBarLayouts131(base,p,u,v,root,layout),"wrong viewport identity refused");
  check(!wht::BlackBarCode131(0,p)&&!wht::ReadLayoutPoint131(base,p,0,root,6,sampled),"missing layout/module refused");
 }
}
