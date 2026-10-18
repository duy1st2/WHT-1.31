#pragma once
inline void CheckMouseTooltip131(){
 check(!wht::MouseTooltipProfileFor131(129),"unprofiled tooltip refused");
 float x=0,y=0;check(wht::TooltipPosition131({0,0},800,600,true,1,x,y)&&std::abs(x-.2175f)<.00001f&&std::abs(y+.0575f)<.00001f,"upper left tooltip placement");
 check(wht::TooltipPosition131({600,450},800,600,true,1,x,y)&&std::abs(x-.6f)<.00001f&&std::abs(y+.4425f)<.00001f,"lower right tooltip placement");
 check(wht::TooltipPosition131({600,450},800,600,true,1.5f,x,y)&&std::abs(x-.9f)<.00001f,"horizontal widescreen scale");
 for(POINT p:std::array<POINT,4>{{{-1,0},{0,-1},{800,0},{0,600}}})check(!wht::TooltipPosition131(p,800,600,true,1,x,y),"outside client pauses follow");
 check(!wht::TooltipPosition131({1,1},0,600,true,1,x,y)&&!wht::TooltipPosition131({1,1},800,600,false,1,x,y)&&!wht::TooltipPosition131({1,1},800,600,true,NAN,x,y),"invalid size/focus/scale refused");
 for(const auto& tip:wht::mouseTooltipProfiles131){
  const auto* p=wht::BlackBarProfileFor131(tip.build);const auto* cp=wht::ConsoleProfileFor131(tip.build);const uintptr_t base=0x10000000;
  alignas(uintptr_t) unsigned char ui[0x300]{},frame[0x100]{},anchor[0x30]{},console[0x200]{};
  auto ptr=[](unsigned char* p,unsigned off)->uintptr_t&{return *reinterpret_cast<uintptr_t*>(p+off);};
  auto u=reinterpret_cast<uintptr_t>(ui),f=reinterpret_cast<uintptr_t>(frame),c=reinterpret_cast<uintptr_t>(console);const unsigned offset=sizeof(uintptr_t)==8?0x158:0xbc;uintptr_t root=0;
  ptr(ui,0)=base+p->uiTable;*reinterpret_cast<unsigned*>(ui+offset+sizeof(uintptr_t))=9;ptr(frame,0)=base+tip.table;*reinterpret_cast<unsigned*>(frame+sizeof(uintptr_t))=9;
  check(wht::TooltipRoot131(base,*p,tip,u,f,root)&&root==u+offset,"native tooltip primary layout and root validated");
  ptr(console,0)=base+cp->table;check(wht::TooltipConsole131(base,tip.build,c)==c,"native console relative target accepted");
  ptr(frame,2*sizeof(uintptr_t)+8*sizeof(uintptr_t))=reinterpret_cast<uintptr_t>(anchor);ptr(anchor,0)=base+p->anchorTable;ptr(anchor,sizeof(uintptr_t))=c;*reinterpret_cast<int*>(anchor+2*sizeof(uintptr_t))=8;
  wht::LayoutPoint131 current;check(!wht::ReadLayoutPoint131(base,*p,f,root,8,current)&&wht::ReadLayoutPoint131(base,*p,f,root,8,current,c),"console-relative tooltip allowed only explicitly");
  const auto original=current;wht::MouseTooltip131 feature;unsigned writes=0;auto set=[&](unsigned point,const wht::LayoutPoint131& v){check(point==8,"only tooltip bottom-right anchor changes");current=v;++writes;return true;};
  auto apply=[&](bool enabled,POINT cursor,bool foreground=true){return feature.Apply(u,f,root,current,enabled,cursor,800,600,foreground,1,set);};
  check(apply(true,{100,100})&&current.relative==root&&current.point==0&&writes==1,"follow anchors relative to root top-left");apply(true,{100,100});check(writes==1,"stationary mouse no redundant layout call");
  apply(true,{600,400});check(writes==2,"moving cursor updates anchor");apply(false,{600,400});check(current==original&&!feature.active,"disable/menu pause restores console-relative original");
  apply(true,{100,100});apply(true,{-1,0});check(current==original&&!feature.active,"leaving client restores original");
  apply(true,{100,100});apply(true,{100,100},false);check(current==original&&!feature.active,"losing focus restores original");
  apply(true,{100,100});current.x=.123f;const auto mapChanged=current;apply(false,{0,0});check(current==mapChanged,"map anchor changes not overwritten on disable");
  feature.Reset();current=original;check(!feature.Apply(u,f,root,current,true,{1,1},800,600,true,1,[](unsigned,const wht::LayoutPoint131&){return false;})&&!feature.active,"failed native write not marked active");
  apply(true,{1,1});const unsigned before=writes;feature.Apply(u+8,f+8,root,current,false,{0,0},800,600,true,1,set);check(writes==before&&!feature.active,"changed UI never restores stale pointer");
  ++ptr(console,0);check(!wht::TooltipConsole131(base,tip.build,c),"foreign console refused");++ptr(frame,0);check(!wht::TooltipRoot131(base,*p,tip,u,f,root),"foreign tooltip refused");
 }
}
