#pragma once
#include "stats_poll131.hpp"
#include "ui_refresh131.hpp"
inline uintptr_t paintExpectedFrame131=0,paintExpectedUnit131=0;inline unsigned paintCalls131=0;
#ifdef _WIN64
inline uintptr_t PaintAbiFixture131(uintptr_t frame,uintptr_t unit){
#else
inline uintptr_t __fastcall PaintAbiFixture131(uintptr_t frame,void*,uintptr_t unit){
#endif
 check(frame==paintExpectedFrame131&&unit==paintExpectedUnit131,"native paint receiver and unit forwarded");*reinterpret_cast<uintptr_t*>(frame+(sizeof(void*)==8?0x210:0x154))=unit;++paintCalls131;return 0x12345678;
}
inline unsigned presentationPaintCalls131=0;
inline void NativePresentationFixture131(uintptr_t frame){
 check(frame==paintExpectedFrame131&&*reinterpret_cast<uintptr_t*>(frame+(sizeof(void*)==8?0x210:0x154))==paintExpectedUnit131,"presentation follows the current native binding, including pooled unit changes");
 check(paintCalls131==presentationPaintCalls131+1,"bar presentation runs immediately after native bind/update");++presentationPaintCalls131;
 wht::NotifyStatPresentation131(frame);
}
inline unsigned worldHostShows131=0,worldHostHides131=0;
#ifdef _WIN64
inline void WorldHostShowFixture131(void* frame){
#else
inline void __fastcall WorldHostShowFixture131(void* frame,void*){
#endif
 ++worldHostShows131;auto flags=static_cast<unsigned char*>(frame)+(sizeof(void*)==8?0xf0:0x9c);flags[0]=flags[1]=1;
}
#ifdef _WIN64
inline void WorldHostHideFixture131(void* frame){
#else
inline void __fastcall WorldHostHideFixture131(void* frame,void*){
#endif
 ++worldHostHides131;auto flags=static_cast<unsigned char*>(frame)+(sizeof(void*)==8?0xf0:0x9c);flags[0]=flags[1]=0;
}
inline unsigned uiAbiNativeCalls131=0,uiAbiAfterCalls131=0;
#ifdef _WIN64
inline uintptr_t UiAbiFixture131(uintptr_t frame,float elapsed,float time,uintptr_t context){
#else
inline uintptr_t __fastcall UiAbiFixture131(uintptr_t frame,void*,float elapsed,float time,uintptr_t context){
#endif
 check(frame==123&&elapsed==3.25f&&time==-2.5f&&context==456,"native UI receiver, floats and context forwarded");
 ++uiAbiNativeCalls131;return 0x12345678;
}
inline void UiAbiAfter131(){check(uiAbiNativeCalls131==uiAbiAfterCalls131+1,"presentation runs after native UI update");++uiAbiAfterCalls131;wht::DispatchUiRefresh131(123);}
inline void CheckGraphicsRepair131(){
 {auto saved=wht::nativeFrameLayout;wht::nativeFrameLayout.visible=sizeof(void*)==8?0xf0:0x9c;wht::nativeFrameLayout.byteFlags=true;wht::nativeFrameLayout.show=27*sizeof(void*);wht::nativeFrameLayout.hide=26*sizeof(void*);
  alignas(uintptr_t) unsigned char frame[0x200]{},parent[0x200]{};uintptr_t table[28]{};table[27]=reinterpret_cast<uintptr_t>(WorldHostShowFixture131);table[26]=reinterpret_cast<uintptr_t>(WorldHostHideFixture131);*reinterpret_cast<uintptr_t*>(frame)=reinterpret_cast<uintptr_t>(table);
  const auto flags=frame+(sizeof(void*)==8?0xf0:0x9c);const auto root=parent+(sizeof(void*)==8?0xf0:0x9c);flags[0]=1;
  for(unsigned i=0;i<512;++i)wht::MaintainWorldHost131(frame,reinterpret_cast<uintptr_t>(parent));
  check(worldHostShows131==0&&flags[0]==1&&flags[1]==0,"detached native parent never turns off requested overhead values or forces child attachment");
  root[0]=root[1]=1;wht::MaintainWorldHost131(frame,reinterpret_cast<uintptr_t>(parent));
  for(unsigned i=0;i<512;++i)wht::MaintainWorldHost131(frame,reinterpret_cast<uintptr_t>(parent));
  check(worldHostShows131==1&&flags[0]==1&&flags[1]==1,"visible parent attaches its owned host once without repeated registration");
  wht::HideWorldHost131(frame);wht::HideWorldHost131(frame);check(worldHostHides131==1&&flags[0]==0,"explicit feature disable hides once");wht::nativeFrameLayout=saved;
 }

 for(const auto& control:wht::kToolMenuControls){check(control.label&&control.section&&control.key,"menu catalog has no empty trailing entries");check(std::wstring(control.section)!=L"ManaBar","removed mana bar has no menu controls");}
 check(!wht::Config{}.manaBarEnable&&!wht::Config{}.manaBarHero&&!wht::Config{}.manaBarNonHero,"removed mana bar defaults cannot enable native frames");

 check(wht::OverheadEligible131(true,false,8),"native detached/invulnerable frame retains overlay policy when invulnerable filtering is off");
 check(wht::OverheadEligible131(true,true,0)&&!wht::OverheadEligible131(true,true,8)&&!wht::OverheadEligible131(false,false,0),"overhead policy depends on authorization and invulnerable option, never transient draw attachment");

 {wht::uiRefreshOwner131=123;wht::uiRefreshThread131=GetCurrentThreadId();wht::uiRefreshUpdate131=reinterpret_cast<uintptr_t>(UiAbiFixture131);wht::uiRefreshCallback131=UiAbiAfter131;
  for(unsigned n=0;n<512;++n){
#ifdef _WIN64
   check(wht::UiRefreshCall131(123,3.25f,-2.5f,456)==0x12345678,"native UI return register preserved");
#else
   check(wht::UiRefreshCall131(123,nullptr,3.25f,-2.5f,456)==0x12345678,"native UI return register preserved");
#endif
  }
  check(uiAbiNativeCalls131==512&&uiAbiAfterCalls131==512,"one bounded presentation per native update, no recursion or stack drift");
  wht::uiRefreshOriginalTable131=111;wht::uiRefreshProxyTable131=222;
  check(wht::NativeClass131(123,222,111)&&!wht::NativeClass131(124,222,111)&&!wht::NativeClass131(123,222,112),"only exact live UI owner recognizes proxy identity");
  wht::ForgetUiRefresh131();wht::uiRefreshOriginalTable131=wht::uiRefreshProxyTable131=wht::uiRefreshUpdate131=0;
  check(!wht::InstallUiRefresh131(0,99999,0,nullptr),"unsupported UI update profile refused");
 }

 {
  wht::StatsLabels131 labels;wht::LifeSource131 source;source.ui=1;source.unit=2;labels.Sync(source);
  std::string native="12345 |CFF00FF00+1000.5|R";unsigned writes=0;
  auto set=[&](uintptr_t font,const char* value){check(font==123,"cached presentation only writes owned stat font");native=value;++writes;};
  labels.Handle(123,native.c_str(),1,true,true,set);
  for(unsigned n=0;n<512;++n){native="12345 |CFF00FF00+1000.5|R";
   labels.Present(true,[&](uintptr_t,std::string& text){text=native;return true;},set);
   check(native=="12,345 |CFF00FF00+1,000.5|R","native selection text repaired in same update without discovery timer");
  }
  check(writes==513,"exactly one stat rewrite per native change");
  labels.Present(true,[&](uintptr_t,std::string& text){text=native;return true;},set);check(writes==513,"unchanged displayed text does not allocate or rewrite");
  source.unit=3;labels.Sync(source);native="67890";
  labels.Present(true,[&](uintptr_t,std::string& text){text=native;return true;},set);check(native=="67890","cached presentation cannot write old fonts after selection identity changes");
 }
 for(const auto& panels:wht::statsPanels131){
  const auto* life=wht::LifeProfileFor131(panels.build);check(life!=nullptr,"poll profiles paired");const uintptr_t base=0x10000000;
  alignas(uintptr_t) unsigned char info[0x680]{},panel[0x300]{},font[0x140]{};
  *reinterpret_cast<uintptr_t*>(info+(sizeof(void*)==8?0x620:8))=reinterpret_cast<uintptr_t>(panel);*reinterpret_cast<uintptr_t*>(panel)=base+panels.damage;
  *reinterpret_cast<uintptr_t*>(panel+(sizeof(void*)==8?0x1e8:0x12c))=reinterpret_cast<uintptr_t>(font);*reinterpret_cast<uintptr_t*>(font)=base+life->font;
  const char* numeric="12345 - 67890";*reinterpret_cast<const char**>(font+(sizeof(void*)==8?0x108:0xa8))=numeric;
  wht::StatsLabels131 labels;wht::LifeSource131 source;source.ui=1;source.unit=2;labels.Sync(source);std::string actual;unsigned writes=0;
  auto set=[&](uintptr_t target,const char* value){check(target==reinterpret_cast<uintptr_t>(font),"only native stat font written");actual=value;++writes;};
  wht::PollStatsLabels131(labels,base,*life,panels.build,reinterpret_cast<uintptr_t>(info),true,set);
  check(actual=="12,345 - 67,890"&&writes==1,"read only native code supports stat polling");
  *reinterpret_cast<const char**>(font+(sizeof(void*)==8?0x108:0xa8))=actual.c_str();
  wht::PollStatsLabels131(labels,base,*life,panels.build,reinterpret_cast<uintptr_t>(info),true,set);check(writes==1,"owned stat text not rewritten");
  *reinterpret_cast<uintptr_t*>(panel)=base+panels.armor;*reinterpret_cast<const char**>(font+(sizeof(void*)==8?0x108:0xa8))="12345 |CFF00FF00+1000.5|R";labels.Reset();labels.Sync(source);
  wht::PollStatsLabels131(labels,base,*life,panels.build,reinterpret_cast<uintptr_t>(info),true,set);check(actual=="12,345 |CFF00FF00+1,000.5|R","armor uses common native value label, preserving fractional bonus");

 }

 {wht::ComponentLookup131 lookup;alignas(uintptr_t) uintptr_t ui[32]{},first[2]{},hud[2]{};const uintptr_t base=0x100000,table=0x1234;
 first[0]=hud[0]=base+table;ui[8]=reinterpret_cast<uintptr_t>(first);ui[20]=reinterpret_cast<uintptr_t>(hud);auto root=reinterpret_cast<uintptr_t>(ui);
 check(lookup.Find(root,base,table,sizeof(ui),0)==reinterpret_cast<uintptr_t>(first),"generic component keeps first match");
 check(lookup.Find(root,base,table,sizeof(ui),0,true)==reinterpret_cast<uintptr_t>(hud),"console lookup selects final HUD console instead of auxiliary console");}
#ifdef _WIN64
 for(const auto& p:wht::profiles64){
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char parent[0x240]{},child[0x240]{},unit[0x60]{};
  auto root=reinterpret_cast<uintptr_t>(parent),nested=reinterpret_cast<uintptr_t>(child),u=reinterpret_cast<uintptr_t>(unit);
  *reinterpret_cast<uintptr_t*>(parent)=*reinterpret_cast<uintptr_t*>(child)=base+p.statVtable;*reinterpret_cast<uintptr_t*>(unit)=base+p.unitVtable;
  for(auto b:{parent,child}){*reinterpret_cast<unsigned*>(b+0x1e8)=5;*reinterpret_cast<uintptr_t*>(b+0x210)=u;}
  uintptr_t selected=0;bool visible=false;check(wht::Hp64(root,base,p,selected,visible),"native HP root eligible");
  *reinterpret_cast<uintptr_t*>(child+0xc0)=root;check(!wht::Hp64(nested,base,p,selected,visible),"nested stat-bar child is excluded from native HP root sampling");
 }
#else
 for(unsigned n=0;n<2;++n){const auto& p=wht::stateBarProfiles131[n];const auto& r=wht::regenProfiles131[n];
  const uintptr_t base=0x10000000;alignas(uintptr_t) unsigned char parent[0x180]{},child[0x180]{},unit[0x60]{};
  auto root=reinterpret_cast<uintptr_t>(parent),nested=reinterpret_cast<uintptr_t>(child),u=reinterpret_cast<uintptr_t>(unit);
  *reinterpret_cast<uintptr_t*>(parent)=*reinterpret_cast<uintptr_t*>(child)=base+p.bar;*reinterpret_cast<uintptr_t*>(unit)=base+r.unit;
  for(auto b:{parent,child}){*reinterpret_cast<unsigned*>(b+0x134)=5;*reinterpret_cast<uintptr_t*>(b+0x154)=u;}
  uintptr_t selected=0;bool visible=false,hero=false;check(wht::SampleStateBar131(root,base,p,r,selected,visible,hero),"native HP root eligible");
  *reinterpret_cast<uintptr_t*>(child+0x78)=root;check(!wht::SampleStateBar131(nested,base,p,r,selected,visible,hero),"nested stat-bar child is excluded from native HP root sampling");
 }
#endif

 {alignas(uintptr_t) unsigned char frame[0x240]{},unit[0x60]{},otherUnit[0x60]{};const uintptr_t f=reinterpret_cast<uintptr_t>(frame),u=reinterpret_cast<uintptr_t>(unit);
  *reinterpret_cast<uintptr_t*>(unit)=12345;*reinterpret_cast<uintptr_t*>(frame+(sizeof(void*)==8?0x210:0x154))=u;
  wht::StatPaintRecord131 record;record.unit=u;record.unitTable=12345;record.color=true;record.presentation=true;wht::statPaintRecords131[f]=record;wht::statPaintPresentation131=NativePresentationFixture131;presentationPaintCalls131=0;wht::statPaintThread131=GetCurrentThreadId();
  wht::statPaintBind131=wht::statPaintUpdate131=reinterpret_cast<uintptr_t>(PaintAbiFixture131);paintExpectedFrame131=f;paintExpectedUnit131=u;paintCalls131=0;
  for(unsigned n=0;n<256;++n){paintExpectedUnit131=n&1?u:reinterpret_cast<uintptr_t>(otherUnit);
#ifdef _WIN64
   check(wht::StatPaintBindCall131(f,paintExpectedUnit131)==0x12345678&&wht::StatPaintUpdateCall131(f,paintExpectedUnit131)==0x12345678,"native paint return register preserved");
#else
   check(wht::StatPaintBindCall131(f,nullptr,paintExpectedUnit131)==0x12345678&&wht::StatPaintUpdateCall131(f,nullptr,paintExpectedUnit131)==0x12345678,"native paint return register preserved");
#endif
  }
  check(presentationPaintCalls131==512,"every pooled native bind/update gets one bounded presentation without recursion");
  check(paintCalls131==512,"native paint callbacks executed once per call without stack drift");wht::ForgetStatPaint131();wht::statPaintBind131=wht::statPaintUpdate131=0;
  check(!wht::StatPaintCode131(0,0),"unknown native paint profile refused");
 }

}
