#pragma once
#include "life_labels131.hpp"
#include "stats_labels131.hpp"
#include "stats_poll131.hpp"
#include <cmath>
#include <cstdio>
namespace wht {
using LifeFontSet131=void(__thiscall*)(uintptr_t,const char*);
inline LifeFontSet131 originalLifeFont131=nullptr;
inline LifeLabels131 lifeLabels131;
inline StatsLabels131 statsLabels131;
inline int lifeHookStage131=0,lifeHookError131=0;inline DWORD lifeHookWinError131=0;
inline const StatsProfile131* statsProfile131=nullptr;
inline uintptr_t lifeBase131=0;inline const LifeProfile131* lifeProfile131=nullptr;
inline uintptr_t lifeUi131=0,lifePortrait131=0;
inline uint64_t lifePollDue131=0;
inline void RefreshSelectionLabels131(){lifePollDue131=0;}
inline DWORD lifeThread131=0;inline unsigned lifeFlags131=0;inline bool lifeInstalled131=false;
inline bool InstallLifeFont131(uintptr_t base,const LifeProfile131& p){
 if(lifeInstalled131)return lifeBase131==base&&lifeProfile131==&p;
 lifeHookStage131=1;if(!LifeCode131(base,p))return false;
 // Supplied clients map code read/execute and reject protection changes: use native labels without patching.
 lifeBase131=base;lifeProfile131=&p;lifeThread131=GetCurrentThreadId();
 originalLifeFont131=reinterpret_cast<LifeFontSet131>(base+p.setText);
 statsProfile131=StatsProfileFor131(p.build);if(statsProfile131&&!StatsCode131(base,*statsProfile131,p))statsProfile131=nullptr;
 lifeHookStage131=6;lifeHookError131=0;lifeInstalled131=true;return true;
}
inline void ResetLifeMana131(){lifeFlags131=0;lifeUi131=lifePortrait131=0;lifeLabels131.Reset();statsLabels131.Reset();}
template<class Sample> inline void UpdateLifeMana131(uintptr_t base,unsigned build,uintptr_t ui,uintptr_t portrait,uintptr_t info,unsigned flags,Sample sample,bool present=false){
 const auto* p=LifeProfileFor131(build);if(!p){ResetLifeMana131();return;}
 if(!lifeInstalled131){if(!InstallLifeFont131(base,*p))return;}
 if(lifeThread131!=GetCurrentThreadId())return;
 lifeUi131=ui;lifePortrait131=portrait;const auto previous=lifeFlags131;
 const auto source=ReadLifeSource131(base,*p,ui,portrait);const bool changed=!(lifeLabels131.owner==source);
 const auto now=GetTickCount64();const bool discover=changed||flags!=previous||now>=lifePollDue131;
 if(!discover&&!present)return;if(discover)lifePollDue131=now+100;
 lifeLabels131.Sync(source);lifeFlags131=flags;statsLabels131.Sync(source);
 for(unsigned n=0;n<2;++n)if(source.fonts[n]){
  std::array<char,129> current{};if(!ReadLifeText131(source.fonts[n],current))continue;
  auto& label=lifeLabels131.labels[n];
  if(!label.valid||current.data()!=label.written){
   if(!current[0]&&(flags&2)){
    float value=0,maximum=0;if(sample(source.unit,n!=0,value,maximum)&&maximum>0){char numeric[96]{};std::snprintf(numeric,sizeof(numeric),"%.0f / %.0f",std::floor(value),std::floor(maximum));
     lifeLabels131.Handle(source.fonts[n],numeric,n?p->mpNumeric:p->hpNumeric,*p,flags,originalLifeFont131);label.widthCleared=true;}
   }else lifeLabels131.Handle(source.fonts[n],current.data(),n?p->mpNumeric:p->hpNumeric,*p,flags,originalLifeFont131);
  }
 }
 if(statsProfile131&&((flags|previous)&4)){
  if(discover)PollStatsLabels131(statsLabels131,base,*p,build,info,(flags&4)!=0,originalLifeFont131);
  else statsLabels131.Present((flags&4)!=0,[&](uintptr_t font,std::string& text){
   if(!StatsFont131(base,*p,font))return false;std::array<char,129> current{};
   if(!ReadLifeText131(font,current))return false;text=current.data();return true;
  },originalLifeFont131);
 }

 for(unsigned n=0;n<2;++n)if(lifeLabels131.owner.fonts[n]&&!lifeLabels131.labels[n].valid){std::array<char,129> safe{};if(ReadLifeText131(lifeLabels131.owner.fonts[n],safe))lifeLabels131.Seed(n,safe.data());}
 if(flags||previous!=flags)lifeLabels131.Apply(flags,[&](uintptr_t font,std::string& text){std::array<char,129> safe{};if(!ReadLifeText131(font,safe))return false;text=safe.data();return true;},originalLifeFont131);
 if((flags|previous)&4)statsLabels131.Apply((flags&4)!=0,[&](uintptr_t font,std::string& text){if(!StatsFont131(base,*p,font))return false;std::array<char,129> safe{};if(!ReadLifeText131(font,safe))return false;text=safe.data();return true;},originalLifeFont131);
 // Poll only current, class-checked native labels; no executable patch or
 // process-wide text callback. Cached strings are restored only while owned.
}
}
