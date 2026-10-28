#include "component_lookup131.hpp"
#include "hp_player_color131.hpp"
#include "hp_dimensions131.hpp"
#include "invulnerable_bar131.hpp"
#pragma once
#include "world_present131.hpp"
#include "profile64.hpp"
#include "tool_menu.hpp"
#include <unordered_map>
#include <unordered_set>
#include <cstdio>
namespace wht {
inline bool Object64(uintptr_t object,uintptr_t base,uintptr_t table){__try{return object&&NativeClass131(object,*reinterpret_cast<uintptr_t*>(object),base+table);}__except(1){return false;}}
inline uintptr_t Component64(uintptr_t ui,uintptr_t base,uintptr_t table){return FindComponent131(ui,base,table,0x7e8);}
inline uintptr_t Selected64(uintptr_t ui,uintptr_t base,const Profile64& p){auto portrait=Component64(ui,base,p.portraitVtable);if(!portrait)return 0;__try{auto unit=*reinterpret_cast<uintptr_t*>(portrait+0x3b8);return Object64(unit,base,p.unitVtable)?unit:0;}__except(1){return 0;}}
inline int Level64(uintptr_t unit,uintptr_t base,const Profile64& p){if(!Object64(unit,base,p.unitVtable))return 0;__try{auto hero=*reinterpret_cast<uintptr_t*>(unit+0x558);if(!Object64(hero,base,p.heroVtable))return 0;int n=reinterpret_cast<int(*)(uintptr_t)>(base+p.heroLevel)(unit);return n>0&&n<=10000?n:0;}__except(1){return 0;}}
inline bool State64(uintptr_t unit,uintptr_t base,const Profile64& p,bool mana,float& value,float& maximum,float& rate){if(!Object64(unit,base,p.unitVtable))return false;
 __try{auto property=unit+(mana?0x248:0x200);if(!Object64(property,base,p.floatVtable))return false;
  auto state=reinterpret_cast<uintptr_t(*)(uint32_t,uint32_t)>(base+p.resolve)(*reinterpret_cast<uint32_t*>(property+0x10),*reinterpret_cast<uint32_t*>(property+0x14));if(!state)return false;
  reinterpret_cast<void*(*)(uintptr_t,float*)>(base+p.floatCurrent)(property,&value);reinterpret_cast<void*(*)(uintptr_t,float*)>(base+p.floatMaximum)(property,&maximum);rate=*reinterpret_cast<float*>(state+0xd4);
  return std::isfinite(value)&&std::isfinite(maximum)&&maximum>=0&&maximum<1.e12f&&std::isfinite(rate)&&rate>-10000&&rate<100000;
 }__except(1){return false;}}
inline std::string RateText64(bool mana,float rate){char text[96]{};std::snprintf(text,sizeof(text),"|cfff9d112%s/s:|r %s%s%.2f|r",mana?"MP":"HP",rate<0?"|cffff0303":mana?"|cffc3dbff":"|cff00ff00",rate<0?"":"+",rate);return text;}
inline std::string StateText64(float value,float maximum,bool showMax,bool mana,bool commas){if(maximum<=0)return {};char text[96]{};value=std::floor(std::clamp(value,0.f,maximum));if(showMax)std::snprintf(text,sizeof(text),"%.0f/%.0f",value,std::floor(maximum));else std::snprintf(text,sizeof(text),"%.0f",value);
 std::string plain=text,result;for(size_t i=0;i<plain.size();){if(plain[i]<'0'||plain[i]>'9'){result+=plain[i++];continue;}size_t end=i;while(end<plain.size()&&plain[end]>='0'&&plain[end]<='9')++end;for(auto start=i;i<end;++i){if(commas&&i>start&&(end-i)%3==0)result+=',';result+=plain[i];}}return std::string(mana?"|cffc3dbff":"|cff00ff00")+result+"|r";}
inline bool Frames64(void* host,std::unordered_set<uintptr_t>& frames){__try{auto manager=*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(host)+0xb8);if(!manager)return false;auto node=*reinterpret_cast<uintptr_t*>(manager+0x2b0);
 for(unsigned n=0;node&&!(node&1);++n){if(n>=32768)return false;auto frame=*reinterpret_cast<uintptr_t*>(node+0x10);if(frame)frames.insert(frame);auto next=*reinterpret_cast<uintptr_t*>(node+8);if(next==node)return false;node=next;}return true;}__except(1){return false;}}
inline bool Hp64(uintptr_t bar,uintptr_t base,const Profile64& p,uintptr_t& unit,bool& visible){if(!Object64(bar,base,p.statVtable))return false;__try{if(Object64(*reinterpret_cast<uintptr_t*>(bar+0xc0),base,p.statVtable))return false;if(*reinterpret_cast<unsigned*>(bar+0x1e8)!=5||*reinterpret_cast<unsigned*>(bar+0xe4)>7)return false;unit=*reinterpret_cast<uintptr_t*>(bar+0x210);visible=*reinterpret_cast<uint8_t*>(bar+0xf1)!=0;return Object64(unit,base,p.unitVtable);}__except(1){return false;}}
inline bool OwnedChild64(void* child,uintptr_t parent,uintptr_t base,uintptr_t vt){if(!Object64(reinterpret_cast<uintptr_t>(child),base,vt))return false;__try{return *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(child)+0xc0)==parent;}__except(1){return false;}}
inline uintptr_t Button64(uintptr_t ui,uintptr_t base,const Profile64& p,unsigned slot){__try{uintptr_t button=0;if(slot<12){auto bar=Component64(ui,base,p.commandVtable);if(!bar||*reinterpret_cast<unsigned*>(bar+0x1f8)!=3)return 0;auto rows=*reinterpret_cast<uintptr_t*>(bar+0x200);if(!rows)return 0;auto row=*reinterpret_cast<uintptr_t*>(rows+(slot/4)*0x20+8);if(!row)return 0;button=*reinterpret_cast<uintptr_t*>(row+(slot%4)*8);}else{auto info=Component64(ui,base,p.infoVtable);if(!info)return 0;auto bar=*reinterpret_cast<uintptr_t*>(info+0x210);if(!Object64(bar,base,p.inventoryVtable)||*reinterpret_cast<unsigned*>(bar+0x1d8)!=6)return 0;auto list=*reinterpret_cast<uintptr_t*>(bar+0x1e0);if(!list)return 0;button=*reinterpret_cast<uintptr_t*>(list+(slot-12)*16+8);}return Object64(button,base,p.buttonVtable)?button:0;}__except(1){return 0;}}
inline float Seconds64(uintptr_t button,uintptr_t base,const Profile64& p){__try{auto data=*reinterpret_cast<uintptr_t*>(button+0x270);if(!data)return 0;auto ability=reinterpret_cast<uintptr_t(*)(uintptr_t)>(base+p.abilityGetter)(data);if(!ability)return 0;
 auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);auto end=base+nt->OptionalHeader.SizeOfImage;auto vt=*reinterpret_cast<uintptr_t*>(ability);if(vt<base||vt+0x5c8>=end)return 0;auto col=*reinterpret_cast<uintptr_t*>(vt-8);if(col<base||col+24>=end||*reinterpret_cast<unsigned*>(col)!=1)return 0;auto type=base+*reinterpret_cast<uint32_t*>(col+12);if(type<base||type+32>=end||memcmp(reinterpret_cast<void*>(type+16),".?AVCAbility",12))return 0;
 auto getter=*reinterpret_cast<uintptr_t*>(vt+0x5c0);if(getter<base||getter>=end)return 0;float value=0;reinterpret_cast<float*(*)(uintptr_t,float*,unsigned)>(getter)(ability,&value,*reinterpret_cast<unsigned*>(data+0xc));return std::isfinite(value)&&value>0&&value<86400?value:0;}__except(1){return 0;}}
inline std::string SecondsText64(float value){if(!std::isfinite(value)||value<=0||value>=86400)return {};char text[32]{};int seconds=int(std::ceil(value));if(seconds>=60)std::snprintf(text,sizeof(text),"%d:%02d",seconds/60,seconds%60);else if(value<1)std::snprintf(text,sizeof(text),"%.1f",value);else std::snprintf(text,sizeof(text),"%d",seconds);return text;}
struct Gameplay64 {
 HpPlayerColor131 hpColors;HpDimensions131 hpDimensions;
 InvulnerableBars131 invulnerable;
 struct World{void* host=nullptr;void* background=nullptr;std::array<void*,2> values{},levels{};std::array<std::string,2> valueText{};std::string levelText;bool placed=false;float backgroundWidth=0;std::array<float,2> fontSizes{};uint32_t color=0xffffffff;};
 struct Cooldown{uintptr_t button=0;std::array<void*,2> fonts{};std::string text;};
 std::unordered_map<uintptr_t,World> world;std::array<Cooldown,18> cooldown{};std::array<void*,2> rates{},rateFonts{};std::array<std::string,2> rateText{};unsigned reported=0,observedNative=0;
 void Reset(){ForgetStatPaint131();hpColors.Reset();hpDimensions.Reset();invulnerable.Reset();world.clear();cooldown={};rates={};rateFonts={};rateText={};reported=observedNative=0;}
 unsigned Update(uintptr_t base,uintptr_t ui,const Profile64& p,const Config& c,bool allowed,ToolMenuOverlay& menu,const NativeFrameApi& raw){unsigned observed=0;
  auto selected=Selected64(ui,base,p);
  for(unsigned n=0;n<2;++n){float value=0,maximum=0,rate=0;bool enabled=allowed&&(n?c.showRegenMana:c.showRegenHP)&&State64(selected,base,p,n!=0,value,maximum,rate)&&rate!=0;
   ToolMenuRect rect{.510f,n?0.f:.009f,.08f,.009f};if(enabled&&!rates[n]){rates[n]=menu.Frame(base,menu.independentHost,rect);if(rates[n])rateFonts[n]=menu.TextIn(rates[n],rect,"",false,.009f);}if(!rates[n]||!rateFonts[n])continue;
   if(!enabled){NativeFrameHide(rates[n]);continue;}auto text=RateText64(n!=0,rate);if(text!=rateText[n]){menu.api.setText(rateFonts[n],text.c_str());rateText[n]=text;}NativeFrameShow(rates[n]);observed|=1;
  }
  for(unsigned slot=0;slot<18;++slot){auto& e=cooldown[slot];auto button=Button64(ui,base,p,slot);auto value=button&&allowed&&c.showCooldown?Seconds64(button,base,p):0.f;auto text=SecondsText64(value);if(value>0)observed|=2;
   for(unsigned n=0;n<2;++n){bool created=false;if(!e.fonts[n]&&!text.empty()){e.fonts[n]=NativeCreateFontString(menu.api,menu.independentHost,.016f,"",4);created=e.fonts[n]!=nullptr;if(created){menu.api.setWidth(e.fonts[n],.030f);menu.api.setHeight(e.fonts[n],.024f);menu.api.setTextAlign(e.fonts[n],2);}}
    if(e.fonts[n]&&button&&(created||button!=e.button)){float shadow=n?0.f:.001f;menu.api.setRelPoint(e.fonts[n],4,reinterpret_cast<void*>(button),4,shadow,-shadow,1);}
    if(e.fonts[n]&&(created||e.text!=text)){auto label=text.empty()?std::string{}:std::string(n?"|cffffffff":"|cff000000")+text+"|r";menu.api.setText(e.fonts[n],label.c_str());}}
   e.button=button;e.text=text;
  }
  std::unordered_set<uintptr_t> frames;if(!Frames64(menu.independentHost,frames))return observed;
  for(auto it=world.begin();it!=world.end();)if(!frames.count(it->first))it=world.erase(it);else ++it;
  PruneStatPaint131(frames);hpColors.Prune(frames);hpDimensions.Prune(frames);
  invulnerable.Prune(frames);
  for(auto bar:frames){uintptr_t unit=0;bool visible=false;if(!Hp64(bar,base,p,unit,visible))continue;
   StatPaintRecord131 record;record.unit=unit;record.unitTable=base+p.unitVtable;
   const bool enabled=allowed; // Apply reference custom HP colors in normal mode too.
   if(!enabled){auto previous=statPaintRecords131.find(bar);if(previous!=statPaintRecords131.end()&&previous->second.presentation)WriteBar(bar,base,p,c,allowed,raw);}
   SetStatPaint131(base,p.statVtable,bar,false,record,enabled,true);
  }
  return observed|observedNative;
 }
 unsigned WriteBar(uintptr_t bar,uintptr_t base,const Profile64& p,const Config& c,bool allowed,const NativeFrameApi& raw){unsigned observed=0;
uintptr_t unit=0;bool visible=false;if(!Hp64(bar,base,p,unit,visible))return observed;invulnerable.Update(base,p.build,bar,unit,allowed&&c.hideInvulnerableBars,visible);hpColors.Update(base,p.build,bar,unit,allowed,GetTickCount64(),c);hpDimensions.Update(base,p.build,bar,allowed?c.barWidthHp:0.f,allowed?c.barHeightHp:0.f,raw);int level=Level64(unit,base,p);bool hero=level>0;
   const bool eligible=OverheadEligible131(allowed,c.hideInvulnerableBars,OverheadUnitFlags131(unit));
   bool stateEnabled=eligible&&c.stateTagEnable&&(hero?c.stateTagHero:c.stateTagNonHero);
   std::string levelText=eligible&&c.stateTagShowLevel&&hero?std::to_string(level):std::string{};auto found=world.find(bar);if(found==world.end()){if((!stateEnabled&&levelText.empty())||world.size()>=4096)return observed;found=world.emplace(bar,World{}).first;}auto& e=found->second;
   if(e.host&&!OwnedChild64(e.host,bar,base,p.simpleVtable)){e.host=nullptr;e.values={};e.levels={};e.background=nullptr;e.valueText={};e.levelText.clear();e.color=0xffffffff;e.placed=false;e.backgroundWidth=0;e.fontSizes={};}
   float value=0,maximum=0,rate=0;
   if(!stateEnabled&&levelText.empty()){if(e.host)HideWorldHost131(e.host);return observed;}if(!e.host){auto memory=raw.alloc(0x1d0,"WHTState64",1,0);if(memory)e.host=raw.frameCtor(memory,reinterpret_cast<void*>(bar));
    if(e.host&&!NativeSetOwnedFrameLevel(e.host,base,p.simpleVtable,kToolFeatureFrameLevel)){NativeFrameHide(e.host);e.host=nullptr;}}if(!e.host)return observed;
   if(!e.placed){raw.setRelPoint(e.host,4,reinterpret_cast<void*>(bar),4,0,0,1);raw.setWidth(e.host,.08f);raw.setHeight(e.host,.03f);e.placed=true;}float size=(std::isfinite(c.stateTagFontSize)?std::clamp(c.stateTagFontSize,1.f,20.f):4.f)*.0015f;
   for(unsigned n=0;n<2;++n){std::string text;if(stateEnabled&&(n?c.stateTagManaValue:c.stateTagHpValue)&&State64(unit,base,p,n!=0,value,maximum,rate))text=StateText64(value,maximum,c.stateTagShowMax,n!=0,c.lifeManaCommas);
    if(!e.values[n]&&!text.empty())e.values[n]=NativeCreateFontString(raw,e.host,size,"",2);if(e.values[n]){if(e.fontSizes[n]!=size){raw.setFont(e.values[n],"FRIZQT__.ttf",size,1);raw.setTextAlign(e.values[n],2);raw.setRelPoint(e.values[n],4,reinterpret_cast<void*>(bar),4,0,n?-(size+.004f):size+.002f,1);e.fontSizes[n]=size;}if(e.valueText[n]!=text){raw.setText(e.values[n],text.c_str());e.valueText[n]=text;}if(!text.empty())observed|=8;}}
   if(!e.background&&!levelText.empty()){auto memory=raw.alloc(0x150,"WHTLevel64",1,0);if(memory)e.background=raw.textureCtor(memory,e.host,2,1);}
   for(unsigned n=0;n<2;++n){bool created=false;if(!e.levels[n]&&!levelText.empty()){e.levels[n]=NativeCreateFontString(raw,e.host,.010546875f,"",2);created=e.levels[n]!=nullptr;if(created){raw.setTextAlign(e.levels[n],2);float shadow=n?0.f:.0006f;raw.setRelPoint(e.levels[n],8,reinterpret_cast<void*>(bar),6,-.0025f+shadow,-.005f-shadow,1);}}if(e.levels[n]&&(created||e.levelText!=levelText)){auto text=levelText.empty()?std::string{}:std::string(n?"|cffffffff":"|cff000000")+levelText+"|r";raw.setText(e.levels[n],text.c_str());}}
   if(e.background){float width=e.levels[1]&&!levelText.empty()?reinterpret_cast<float(*)(void*)>(base+p.textWidth)(e.levels[1]):0.f,scale=*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(e.background)+0xa4);bool show=!levelText.empty()&&std::isfinite(width)&&width>0&&width<.8f&&std::isfinite(scale)&&scale>0;uint32_t color=show?0xc0000000:0;if(e.color!=color){reinterpret_cast<bool(*)(void*,const uint32_t*)>(base+p.solid)(e.background,&color);e.color=color;}if(show&&e.backgroundWidth!=width/scale+.002f){e.backgroundWidth=width/scale+.002f;raw.setWidth(e.background,e.backgroundWidth);raw.setHeight(e.background,.012546875f);raw.setRelPoint(e.background,4,e.levels[1],4,0,0,1);}}
   e.levelText=levelText;if(!levelText.empty())observed|=16;MaintainWorldHost131(e.host,bar);
  observedNative|=observed;return observed;
 }
};
}
