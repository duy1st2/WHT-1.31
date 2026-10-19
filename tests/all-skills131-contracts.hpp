#pragma once
#include "all_skills131.hpp"
inline void CheckAllSkills131(){
 for(const auto& p:wht::skillProfiles131){
  alignas(uintptr_t) unsigned char unit[0x700]{},hero[0x200]{};const uintptr_t base=0x10000000;
  *reinterpret_cast<uintptr_t*>(unit)=base+p.unitTable;*reinterpret_cast<uintptr_t*>(hero)=base+p.heroTable;
  const unsigned member=sizeof(uintptr_t)==8?0x558:p.build==12164?0x450:0x448,points=sizeof(uintptr_t)==8?0xe4:0x94;
  *reinterpret_cast<uintptr_t*>(unit+member)=reinterpret_cast<uintptr_t>(hero);*reinterpret_cast<int*>(hero+points)=3;
  check(wht::HeroPoints131(reinterpret_cast<uintptr_t>(unit),base,p)==3,"verified hero skill point field");
  *reinterpret_cast<int*>(hero+points)=-1;check(wht::HeroPoints131(reinterpret_cast<uintptr_t>(unit),base,p)==-1,"negative hero points rejected");
  *reinterpret_cast<uintptr_t*>(hero)=0;check(wht::HeroPoints131(reinterpret_cast<uintptr_t>(unit),base,p)==-1,"nonhero skill points rejected");
 }
 // Reproduce the UI refresh proxy introduced by the anti-flicker fix.
 {alignas(uintptr_t) uintptr_t image[128]{},proxy[64]{},listener=0,other=0;
  const auto base=reinterpret_cast<uintptr_t>(image),end=base+sizeof(image),nativeTable=base+16*sizeof(uintptr_t);
  image[19]=base+100*sizeof(uintptr_t);proxy[3]=image[19];listener=nativeTable;
  const auto oldOwner=wht::uiRefreshOwner131,oldOriginal=wht::uiRefreshOriginalTable131,oldProxy=wht::uiRefreshProxyTable131;
  check(wht::SkillListener131(base,end,reinterpret_cast<uintptr_t>(&listener)),"ordinary native button listener accepted");
  listener=reinterpret_cast<uintptr_t>(proxy);wht::uiRefreshOwner131=reinterpret_cast<uintptr_t>(&listener);wht::uiRefreshOriginalTable131=nativeTable;wht::uiRefreshProxyTable131=listener;
  check(wht::SkillListener131(base,end,reinterpret_cast<uintptr_t>(&listener)),"registered UI refresh proxy preserves native click callback");
  other=listener;check(!wht::SkillListener131(base,end,reinterpret_cast<uintptr_t>(&other)),"same proxy on another object refused");
  proxy[3]=base+101*sizeof(uintptr_t);check(!wht::SkillListener131(base,end,reinterpret_cast<uintptr_t>(&listener)),"changed receiver callback refused");proxy[3]=image[19];
  wht::uiRefreshOriginalTable131=end;check(!wht::SkillListener131(base,end,reinterpret_cast<uintptr_t>(&listener)),"invalid original native table refused");
  wht::uiRefreshOwner131=oldOwner;wht::uiRefreshOriginalTable131=oldOriginal;wht::uiRefreshProxyTable131=oldProxy;
 }
 wht::AllSkills131 batch;uintptr_t selectedUnit=123;int points=2;bool menuOpen=false;unsigned opened=0,closed=0,learned=0;
 auto selected=[&](){return selectedUnit;};auto available=[&](uintptr_t){return points;};
 auto data=[&](unsigned slot)->wht::SkillButton131 {
  if(!menuOpen)return slot==7?wht::SkillButton131{0,wht::AllSkills131::plus}:wht::SkillButton131{};
  if(slot==11)return {0,wht::AllSkills131::back};
  if(slot==0||slot==4)return {wht::AllSkills131::heroAbility,100+slot};return {};
 };
 auto click=[&](unsigned slot){if(!menuOpen&&slot==7){menuOpen=true;++opened;}else if(slot==11){menuOpen=false;++closed;}else if(points){--points;++learned;}return true;};
 check(batch.Start(0,selected,available,data),"queue all skills with available points");
 check(batch.Tick(0,true,selected,available,data,click)&&opened==1&&learned==2&&closed==1&&!menuOpen&&!batch.active,"plus learn and back finish before the first rendered frame");
 check(!batch.Start(2000,selected,available,data),"zero points does not start a sequence");
 points=3;menuOpen=true;closed=learned=0;
 auto refused=[&](unsigned slot){if(slot==11){menuOpen=false;++closed;}else ++learned;return true;};
 check(batch.Start(0,selected,available,data),"start from manually open card");
 for(unsigned now=0;now<=1000&&batch.active;now+=60)batch.Tick(now,true,selected,available,data,refused);
 check(!batch.active&&learned==40&&points==3&&closed==1,"deferred native point update stays bounded and closes card in one tick");
 points=2;menuOpen=false;batch.Start(0,selected,available,data);selectedUnit=456;
 const auto before=opened;batch.Tick(0,true,selected,available,data,click);check(!batch.active&&opened==before,"selection change cancels without clicking another unit");
 selectedUnit=123;batch.Start(0,selected,available,data);batch.Tick(0,false,selected,available,data,click);check(!batch.active&&opened==before,"chat/menu/permission gate cancels pending sequence");
 batch.Start(0,selected,available,data);unsigned clicks=0;auto ignored=[&](unsigned){++clicks;return true;};
 batch.Tick(0,true,selected,available,data,ignored);batch.Tick(1000,true,selected,available,data,ignored);batch.Tick(16000,true,selected,available,data,ignored);
 check(!batch.active&&clicks==1,"unresponsive native menu opens once and times out");
 wht::SkillsIcon131 icon;check(icon.area.bottom==.012f&&!strcmp(icon.upAsset,"BTNAllSkills.blp")&&!strcmp(icon.downAsset,"BTNAllSkillsDow.blp"),"skills icon uses legacy position and both states");
 icon.Reset();check(icon.area.bottom==.012f&&!strcmp(icon.upAsset,"BTNAllSkills.blp"),"UI reset preserves skills icon configuration");
}
