#pragma once
#include "hero_agility_profile131.hpp"
#include "remove_unit131.hpp"
#include "all_skills131.hpp"
#include <sstream>
namespace wht {
inline const HeroAgilityProfile131* HeroAgilityProfileFor131(unsigned build){for(const auto& p:heroAgilityProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool HeroAgilityCode131(uintptr_t base,const HeroAgilityProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
inline bool ParseHeroAgility131(const std::wstring& args,int& value){value=500;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;if(args.find_first_not_of(L" \t")==std::wstring::npos)return true;std::wistringstream in(args);in.imbue(std::locale::classic());long long parsed=0;std::wstring extra;if(!(in>>parsed)||in>>extra||parsed<1||parsed>10000)return false;value=int(parsed);return true;}
using HeroAgilityGet131=int(__thiscall*)(uintptr_t,unsigned);
using HeroAgilitySet131=void(__thiscall*)(uintptr_t,int,unsigned);
inline bool InvokeHeroAgility131(HeroAgilitySet131 set,uintptr_t unit,int value){if(!set||!unit||value<1||value>10000)return false;__try{set(unit,value,1);return true;}__except(1){return false;}}
struct HeroAgilitySource131 {RemoveUnitSource131 source;uintptr_t hero=0;int base=0,total=0;};
inline bool SameHeroAgility131(const HeroAgilitySource131& a,const HeroAgilitySource131& b){return a.hero&&a.hero==b.hero&&SameRemoveUnit131(a.source,b.source);}
inline bool HeroAgilityIdentity131(uintptr_t base,unsigned build,const HeroAgilitySource131& value){const auto* p=SkillProfile(build);if(!p||!value.hero||!ValidRemoveUnit131(base,build,value.source))return false;
 __try{const auto unit=value.source.unit;const unsigned kind=*reinterpret_cast<unsigned char*>(unit+(sizeof(uintptr_t)==8?0x5b:0x37));return kind>='A'&&kind<='Z'&&!(*reinterpret_cast<uint32_t*>(unit+(sizeof(uintptr_t)==8?0x19c:0x168))&0x40000000)&&*reinterpret_cast<uintptr_t*>(unit+p->heroMember)==value.hero&&InventoryIdentity131(value.hero,base,p->heroTable);}__except(1){return false;}}
template<class Selected> bool ReadHeroAgility131(uintptr_t base,unsigned build,Selected selected,HeroAgilitySource131& out){const auto* p=HeroAgilityProfileFor131(build);const auto* sp=SkillProfile(build);if(!p||!sp||!HeroAgilityCode131(base,*p))return false;
 HeroAgilitySource131 value;if(!ReadRemoveUnit131(base,build,selected,value.source))return false;
 __try{value.hero=*reinterpret_cast<uintptr_t*>(value.source.unit+sp->heroMember);if(!HeroAgilityIdentity131(base,build,value))return false;
 const auto get=reinterpret_cast<HeroAgilityGet131>(base+p->get);value.base=get(value.source.unit,0);if(selected()!=value.source.unit||!HeroAgilityIdentity131(base,build,value))return false;value.total=get(value.source.unit,1);
 if(selected()!=value.source.unit||!HeroAgilityIdentity131(base,build,value)||value.base<0||value.base>1000000000||value.total<0||value.total>1000000000)return false;out=value;return true;
 }__except(1){return false;}}
inline bool SetHeroAgilityNative131(uintptr_t base,unsigned build,const HeroAgilitySource131& value,int requested){const auto* p=HeroAgilityProfileFor131(build);return p&&HeroAgilityCode131(base,*p)&&HeroAgilityIdentity131(base,build,value)&&InvokeHeroAgility131(reinterpret_cast<HeroAgilitySet131>(base+p->set),value.source.unit,requested);}
template<class Read,class Set> bool HeroAgilityCommand131(const std::wstring& args,Read read,Set set,std::string& report){
 int requested=500;if(!ParseHeroAgility131(args,requested)){report="Usage: select a hero, then #agi [1..10000] (default 500)";return false;}
 HeroAgilitySource131 before,current,after;if(!read(before)||!read(current)||!SameHeroAgility131(before,current)||before.base!=current.base){report="Hero AGI: select a stable registered hero";return false;}
 if(current.base!=requested){if(!set(current,requested)){report="Hero AGI: native update failed; update may have applied";return false;}if(!read(after)||!SameHeroAgility131(current,after)||after.base!=requested){report="Hero AGI: game did not confirm requested base Agility";return false;}current=after;}
 report="Hero AGI: "+std::to_string(current.base)+" (total: "+std::to_string(current.total)+")";return true;
}
}
