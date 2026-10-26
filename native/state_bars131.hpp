#include "world_present131.hpp"
#include "hp_player_color131.hpp"
#include "hp_dimensions131.hpp"
#include "invulnerable_bar131.hpp"
#pragma once
#include "regen131.hpp"
#include <unordered_set>
#include <unordered_map>
namespace wht {
struct StateBarProfile131 {unsigned build;uintptr_t bar,ctor,bind,color,maximum,hero,getLevel;unsigned heroMember;uintptr_t textWidth,solidColor;};
inline constexpr StateBarProfile131 stateBarProfiles131[]={
 {12164,0x182e5fc,0xb9c0a0,0xb9c790,0xdfce40,0xd68530,0x17624d0,0xbabfa0,0x450,0xdfdd70,0xdfe980},
 {12173,0x17e34dc,0xb5b8e0,0xb5bfd0,0xdbbc40,0xd230c0,0x1717508,0xb6b410,0x448,0xdbcb70,0xdbd780}
};
inline int HeroLevel131(uintptr_t unit,uintptr_t base,const RegenProfile131& r,const StateBarProfile131& p){
 if(!Identity131(unit,base,r.unit))return 0;
 __try{
  const auto ability=*reinterpret_cast<uintptr_t*>(unit+p.heroMember);
  if(!Identity131(ability,base,p.hero))return 0;
  const int level=reinterpret_cast<int(__thiscall*)(uintptr_t)>(base+p.getLevel)(unit);
  return level>0&&level<=10000?level:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline std::string HeroLevelText131(int level){return level>0&&level<=10000?std::to_string(level):std::string{};}
// CSimpleFrame ctor stores its manager at +74. Native RegisterFrame traverses
// manager +18C, node +4 (next), node +8 (frame); this list includes hidden frames.
// Snapshot before creating children, on the window/UI thread only.
inline bool RegisteredFrames131(void* host,std::unordered_set<uintptr_t>& frames){
 __try{
  const auto manager=*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(host)+0x74);if(!manager)return false;
  auto node=*reinterpret_cast<uintptr_t*>(manager+0x18c);
  for(unsigned count=0;node&&!(node&1);++count){
   if(count>=32768)return false;
   const auto frame=*reinterpret_cast<uintptr_t*>(node+8);if(frame)frames.insert(frame);
   const auto next=*reinterpret_cast<uintptr_t*>(node+4);if(next==node)return false;node=next;
  }return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool SampleStateBar131(uintptr_t bar,uintptr_t base,const StateBarProfile131& p,const RegenProfile131& r,uintptr_t& unit,bool& visible,bool& hero){
 if(!Identity131(bar,base,p.bar))return false;
 __try{
  if(*reinterpret_cast<unsigned*>(bar+0x134)!=5)return false;
  if(Identity131(*reinterpret_cast<uintptr_t*>(bar+0x78),base,p.bar))return false; // tool mana child, not a native HP root
  unit=*reinterpret_cast<uintptr_t*>(bar+0x154);if(!Identity131(unit,base,r.unit))return false;
  if(*reinterpret_cast<unsigned*>(bar+0x90)>7)return false;
  visible=*reinterpret_cast<unsigned char*>(bar+0x9d)!=0;
  const auto id=*reinterpret_cast<unsigned char*>(unit+0x37);hero=id>='A'&&id<='Z';return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ReadState131(uintptr_t unit,uintptr_t base,const RegenProfile131& r,const Profile131& p,const StateBarProfile131& s,bool mana,float& value,float& maximum){
 if(!Identity131(unit,base,r.unit))return false;
 __try{
  const auto property=unit+(mana?0x1cc:0x1a4);if(!Identity131(property,base,r.floatType))return false;
  reinterpret_cast<void*(__thiscall*)(uintptr_t,float*)>(base+p.readFloat)(property,&value);
  reinterpret_cast<void*(__thiscall*)(uintptr_t,float*)>(base+s.maximum)(property,&maximum);
  return std::isfinite(value)&&std::isfinite(maximum)&&maximum>0&&maximum<1.e12f;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string StateText131(float value,float maximum,bool showMax,bool mana,bool commas){
 if(!std::isfinite(value)||!std::isfinite(maximum)||maximum<=0||maximum>=1.e12f)return {};
 char buffer[96]{};value=std::floor(std::clamp(value,0.f,maximum));
 if(showMax)std::snprintf(buffer,sizeof(buffer),"%.0f/%.0f",value,std::floor(maximum));else std::snprintf(buffer,sizeof(buffer),"%.0f",value);
 std::string plain=buffer,result;
 for(size_t i=0;i<plain.size();){if(plain[i]<'0'||plain[i]>'9'){result+=plain[i++];continue;}
  size_t end=i;while(end<plain.size()&&plain[end]>='0'&&plain[end]<='9')++end;
  for(const auto start=i;i<end;++i){if(commas&&i>start&&(end-i)%3==0)result+=',';result+=plain[i];}}
 return std::string(mana?"|cffc3dbff":"|cff00ff00")+result+"|r";
}
inline bool WorldChild131(void* child,uintptr_t parent,uintptr_t base,uintptr_t table){
 if(!Identity131(reinterpret_cast<uintptr_t>(child),base,table))return false;
 __try{return *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(child)+0x78)==parent;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct StateBars131 {
 struct Entry {void* host=nullptr;void* levelBackground=nullptr;std::array<void*,2> fonts{},levelFonts{};std::array<std::string,2> text{};std::string levelText;float layoutSize=0,backgroundWidth=0;std::array<float,2> fontSizes{};uint32_t backgroundColor=0xffffffff;};
 HpPlayerColor131 hpColors;HpDimensions131 hpDimensions;
 InvulnerableBars131 invulnerable;
 std::unordered_map<uintptr_t,Entry> entries;bool stateReported=false,levelReported=false;unsigned observedNative=0;
 void Reset(){ForgetStatPaint131();hpColors.Reset();hpDimensions.Reset();invulnerable.Reset();entries.clear();stateReported=levelReported=false;observedNative=0;}
 // Children are owned by each native HP parent. Never dereference a cached
 // child unless its native class and parent still match. The timer only
 // discovers/prunes roots; native paint callbacks own presentation writes.
 unsigned Update(uintptr_t base,void* root,const Profile131& p,const RegenProfile131& r,const StateBarProfile131& s,const Config& config,bool allowed,const NativeFrameApi& raw){
  std::unordered_set<uintptr_t> frames;if(!root||!RegisteredFrames131(root,frames))return 0;
  for(auto it=entries.begin();it!=entries.end();)if(!frames.count(it->first))it=entries.erase(it);else ++it;
  PruneStatPaint131(frames);hpColors.Prune(frames);hpDimensions.Prune(frames);
  invulnerable.Prune(frames);
  for(const auto bar:frames){uintptr_t unit=0;bool visible=false,hero=false;
   if(!SampleStateBar131(bar,base,s,r,unit,visible,hero))continue;
   StatPaintRecord131 record;record.unit=unit;record.unitTable=base+r.unit;
   const bool enabled=allowed&&(config.barWidthHp>0||config.barHeightHp>0||config.colorPlayer||config.hideInvulnerableBars||config.stateTagEnable||config.stateTagShowLevel||(config.timeBoss&&(config.mth49Enable||config.g17Enable)));
   if(!enabled){auto previous=statPaintRecords131.find(bar);if(previous!=statPaintRecords131.end()&&previous->second.presentation)WriteBar(bar,base,p,r,s,config,allowed,raw);}
   SetStatPaint131(base,s.bar,bar,false,record,enabled,true);
  }
  return observedNative;
 }
 unsigned WriteBar(uintptr_t bar,uintptr_t base,const Profile131& p,const RegenProfile131& r,const StateBarProfile131& s,const Config& config,bool allowed,const NativeFrameApi& raw){unsigned observed=0;
uintptr_t unit=0;bool visible=false,hero=false;
   if(!SampleStateBar131(bar,base,s,r,unit,visible,hero))return observed;
   invulnerable.Update(base,p.build,bar,unit,allowed&&config.hideInvulnerableBars,visible);
   hpColors.Update(base,p.build,bar,unit,allowed&&config.colorPlayer,GetTickCount64());
   hpDimensions.Update(base,p.build,bar,allowed?config.barWidthHp:0.f,allowed?config.barHeightHp:0.f,raw);
   const bool eligible=OverheadEligible131(allowed,config.hideInvulnerableBars,OverheadUnitFlags131(unit));
   const bool stateEnabled=eligible&&config.stateTagEnable&&(hero?config.stateTagHero:config.stateTagNonHero);
   const auto levelText=HeroLevelText131(eligible&&hero&&config.stateTagShowLevel?HeroLevel131(unit,base,r,s):0);
   auto found=entries.find(bar);if(found==entries.end()){if((!stateEnabled&&levelText.empty())||entries.size()>=4096)return observed;found=entries.emplace(bar,Entry{}).first;}
   auto& entry=found->second;
   if(entry.host&&(!WorldChild131(entry.host,bar,base,p.simpleVtable))){entry.host=nullptr;entry.fonts={};entry.text={};entry.levelFonts={};entry.levelBackground=nullptr;entry.levelText.clear();entry.backgroundColor=0xffffffff;entry.layoutSize=entry.backgroundWidth=0;entry.fontSizes={};}
   float value=0,maximum=0;
   if(!stateEnabled&&levelText.empty()){if(entry.host)HideWorldHost131(entry.host);return observed;}
   if(!entry.host){auto memory=raw.alloc(nativeFrameLayout.frameSize,"WHTState131",1,0);if(memory)entry.host=raw.frameCtor(memory,reinterpret_cast<void*>(bar));
    if(entry.host&&!NativeSetOwnedFrameLevel(entry.host,base,nativeFrameLayout.simpleVtable,kToolFeatureFrameLevel)){NativeFrameHide(entry.host);entry.host=nullptr;}}
   if(!entry.host)return observed;
   if(!entry.layoutSize){raw.setRelPoint(entry.host,4,reinterpret_cast<void*>(bar),4,0,0,1);raw.setWidth(entry.host,.08f);raw.setHeight(entry.host,.03f);entry.layoutSize=1;}
   const float size=(std::isfinite(config.stateTagFontSize)?std::clamp(config.stateTagFontSize,1.f,20.f):4.f)*.0015f;
   for(unsigned n=0;n<2;++n){std::string text;
    if(stateEnabled&&(n?config.stateTagManaValue:config.stateTagHpValue)&&ReadState131(unit,base,r,p,s,n!=0,value,maximum))text=StateText131(value,maximum,config.stateTagShowMax,n!=0,config.lifeManaCommas);
    if(!entry.fonts[n]&&!text.empty())entry.fonts[n]=NativeCreateFontString(raw,entry.host,size,"",2);
    if(entry.fonts[n]){if(entry.fontSizes[n]!=size){raw.setFont(entry.fonts[n],"FRIZQT__.ttf",size,1);raw.setTextAlign(entry.fonts[n],2);
     raw.setRelPoint(entry.fonts[n],4,reinterpret_cast<void*>(bar),4,0,n?-(size+.004f):size+.002f,1);entry.fontSizes[n]=size;}
     if(entry.text[n]!=text){raw.setText(entry.fonts[n],text.c_str());entry.text[n]=text;}if(!text.empty())observed|=2;}
   }
   if(!entry.levelBackground&&!levelText.empty()){
    auto memory=raw.alloc(nativeFrameLayout.textureSize,"WHTLevelBackground131",1,0);
    // Same solid texture and creation order as 1.24/1.26/1.29:
    // background first, then shadow and foreground on draw layer 2.
    if(memory)entry.levelBackground=raw.textureCtor(memory,entry.host,2,1);
   }
   for(unsigned n=0;n<2;++n){bool created=false;
    if(!entry.levelFonts[n]&&!levelText.empty()){
     entry.levelFonts[n]=NativeCreateFontString(raw,entry.host,.010546875f,"",2);created=entry.levelFonts[n]!=nullptr;
     if(created){raw.setTextAlign(entry.levelFonts[n],2);
      // Bottom-right of the number to bottom-left of HP, outside HP/MP values.
      const float shadow=n?0.f:.0006f;
      raw.setRelPoint(entry.levelFonts[n],8,reinterpret_cast<void*>(bar),6,-.0025f+shadow,-.005f-shadow,1);}}
    if(entry.levelFonts[n]&&(created||entry.levelText!=levelText)){
     const auto label=levelText.empty()?std::string{}:std::string(n?"|cffffffff":"|cff000000")+levelText+"|r";
     raw.setText(entry.levelFonts[n],label.c_str());}
   }
   if(entry.levelBackground){
    const float width=entry.levelFonts[1]&&!levelText.empty()?reinterpret_cast<float(__thiscall*)(void*)>(base+s.textWidth)(entry.levelFonts[1]):0.f;
    // Native glyph measurement already includes scale; SetWidth takes a local
    // width. Remove the texture's scale before sizing, to avoid applying it twice.
    const float scale=*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(entry.levelBackground)+0x60);
    const bool visible=!levelText.empty()&&std::isfinite(width)&&width>0&&width<.8f&&std::isfinite(scale)&&scale>0;
    const uint32_t color=visible?0xc0000000:0;
    // SetColor creates a solid GPU texture; region tint alone cannot draw an
    // empty CSimpleTexture. Avoid recreating the image on every UI tick.
    if(entry.backgroundColor!=color){
     reinterpret_cast<bool(__thiscall*)(void*,const uint32_t*)>(base+s.solidColor)(entry.levelBackground,&color);
     entry.backgroundColor=color;}
    if(visible&&entry.backgroundWidth!=width/scale+.002f){entry.backgroundWidth=width/scale+.002f;raw.setWidth(entry.levelBackground,entry.backgroundWidth);raw.setHeight(entry.levelBackground,.012546875f);
     raw.setRelPoint(entry.levelBackground,4,entry.levelFonts[1],4,0,0,1);}
   }
   entry.levelText=levelText;if(!levelText.empty())observed|=4;
   MaintainWorldHost131(entry.host,bar);
  observedNative|=observed;return observed;
 }
};
}
