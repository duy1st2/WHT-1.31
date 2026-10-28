#pragma once
#include "hero_level_profile131.hpp"
#include "hero_strength131.hpp"
#include <climits>
namespace wht {
inline const HeroLevelProfile131* HeroLevelProfileFor131(unsigned build){for(const auto& p:heroLevelProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool HeroLevelCode131(uintptr_t base,const HeroLevelProfile131& p){ResourceTextProfile131 c{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,c);}
inline bool ParseHeroLevel131(const std::wstring& args,int& value){value=10;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;if(args.find_first_not_of(L" \t")==std::wstring::npos)return true;std::wistringstream in(args);in.imbue(std::locale::classic());long long n;std::wstring extra;if(!(in>>n)||in>>extra||n<1||n>10000)return false;value=int(n);return true;}
using HeroLevelGet131=int(__thiscall*)(uintptr_t);
using HeroLevelRequired131=int(__thiscall*)(uintptr_t,int);
using HeroLevelReal131=uint32_t*(__cdecl*)(uint32_t*,int);
using HeroLevelAdd131=void(__thiscall*)(uintptr_t,const uint32_t*,int,int,int,int,int);
using HeroLevelStrip131=bool(__thiscall*)(uintptr_t,int);
struct HeroLevelSource131 {HeroStrengthSource131 identity;int level=0;};
inline bool SameHeroLevel131(const HeroLevelSource131& a,const HeroLevelSource131& b){return SameHeroStrength131(a.identity,b.identity);}
inline bool ReadHeroLevelValue131(uintptr_t base,unsigned build,HeroLevelSource131& value){const auto* p=HeroLevelProfileFor131(build);if(!p||!HeroLevelCode131(base,*p)||!HeroStrengthIdentity131(base,build,value.identity))return false;__try{value.level=reinterpret_cast<HeroLevelGet131>(base+p->level)(value.identity.source.unit);return HeroStrengthIdentity131(base,build,value.identity)&&value.level>=1&&value.level<=10000;}__except(1){return false;}}
template<class Selected> bool ReadHeroLevel131(uintptr_t base,unsigned build,Selected selected,HeroLevelSource131& out){const auto* sp=SkillProfile(build);if(!sp)return false;HeroLevelSource131 value;if(!ReadRemoveUnit131(base,build,selected,value.identity.source))return false;__try{value.identity.hero=*reinterpret_cast<uintptr_t*>(value.identity.source.unit+sp->heroMember);}__except(1){return false;}if(!ReadHeroLevelValue131(base,build,value)||selected()!=value.identity.source.unit)return false;out=value;return true;}
inline bool HeroLevelDelta131(int target,int xp,int& delta){const long long d=static_cast<long long>(target)-xp;if(xp<0||d<=0||d>INT_MAX)return false;delta=int(d);return true;}
inline bool SetHeroLevelNative131(uintptr_t base,unsigned build,const HeroLevelSource131& source,int requested){const auto* p=HeroLevelProfileFor131(build);if(!p||requested<1||requested>10000||!HeroLevelCode131(base,*p))return false;HeroLevelSource131 current=source;if(!ReadHeroLevelValue131(base,build,current)||current.level!=source.level)return false;
 __try{const auto unit=current.identity.source.unit;if(requested<current.level){reinterpret_cast<HeroLevelStrip131>(base+p->strip)(unit,current.level-requested);}
 else if(requested>current.level){const int target=reinterpret_cast<HeroLevelRequired131>(base+p->required)(unit,requested-1);if(!HeroStrengthIdentity131(base,build,current.identity))return false;const int xp=reinterpret_cast<HeroLevelGet131>(base+p->experience)(unit);int delta=0;if(!HeroStrengthIdentity131(base,build,current.identity)||!HeroLevelDelta131(target,xp,delta))return false;uint32_t real=0;if(reinterpret_cast<HeroLevelReal131>(base+p->real)(&real,delta)!=&real||!HeroStrengthIdentity131(base,build,current.identity))return false;reinterpret_cast<HeroLevelAdd131>(base+p->add)(unit,&real,1,0,0,0,0);}
 return ReadHeroLevelValue131(base,build,current)&&current.level==requested;
 }__except(1){return false;}}
template<class Read,class Set> bool HeroLevelCommand131(const std::wstring& args,Read read,Set set,std::string& report){int requested=10;if(!ParseHeroLevel131(args,requested)){report="Usage: select a hero, then #lvl [1..10000] (default 10)";return false;}HeroLevelSource131 before,current,after;if(!read(before)||!read(current)||!SameHeroLevel131(before,current)||before.level!=current.level){report="Hero Level: select a stable registered hero";return false;}if(current.level!=requested){const bool applied=set(current,requested);if(!read(after)||!SameHeroLevel131(current,after)){report="Hero Level: hero changed during update; update may have applied";return false;}if(!applied||after.level!=requested){report="Hero Level: game reports "+std::to_string(after.level)+", requested "+std::to_string(requested);return false;}current=after;}report="Hero Level: "+std::to_string(current.level);return true;}
}
