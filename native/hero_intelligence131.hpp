#pragma once
#include "hero_intelligence_profile131.hpp"
#include "remove_unit131.hpp"
#include "all_skills131.hpp"
#include <sstream>
namespace wht {
inline const HeroIntelligenceProfile131* HeroIntelligenceProfileFor131(unsigned build){for(const auto& p:heroIntelligenceProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool HeroIntelligenceCode131(uintptr_t base,const HeroIntelligenceProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
inline bool ParseHeroIntelligence131(const std::wstring& args,int& value){value=500;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;if(args.find_first_not_of(L" \t")==std::wstring::npos)return true;std::wistringstream in(args);in.imbue(std::locale::classic());long long parsed=0;std::wstring extra;if(!(in>>parsed)||in>>extra||parsed<1||parsed>10000)return false;value=int(parsed);return true;}
using HeroIntelligenceGet131=int(__thiscall*)(uintptr_t,unsigned);
using HeroIntelligenceSet131=void(__thiscall*)(uintptr_t,int,unsigned);
inline bool InvokeHeroIntelligence131(HeroIntelligenceSet131 set,uintptr_t unit,int value){if(!set||!unit||value<1||value>10000)return false;__try{set(unit,value,1);return true;}__except(1){return false;}}
struct HeroIntelligenceSource131 {RemoveUnitSource131 source;uintptr_t hero=0;int base=0,total=0;};
inline bool SameHeroIntelligence131(const HeroIntelligenceSource131& a,const HeroIntelligenceSource131& b){return a.hero&&a.hero==b.hero&&SameRemoveUnit131(a.source,b.source);}
inline bool HeroIntelligenceIdentity131(uintptr_t base,unsigned build,const HeroIntelligenceSource131& value){const auto* p=SkillProfile(build);if(!p||!value.hero||!ValidRemoveUnit131(base,build,value.source))return false;
 __try{const auto unit=value.source.unit;const unsigned kind=*reinterpret_cast<unsigned char*>(unit+(sizeof(uintptr_t)==8?0x5b:0x37));return kind>='A'&&kind<='Z'&&!(*reinterpret_cast<uint32_t*>(unit+(sizeof(uintptr_t)==8?0x19c:0x168))&0x40000000)&&*reinterpret_cast<uintptr_t*>(unit+p->heroMember)==value.hero&&InventoryIdentity131(value.hero,base,p->heroTable);}__except(1){return false;}}
template<class Selected> bool ReadHeroIntelligence131(uintptr_t base,unsigned build,Selected selected,HeroIntelligenceSource131& out){const auto* p=HeroIntelligenceProfileFor131(build);const auto* sp=SkillProfile(build);if(!p||!sp||!HeroIntelligenceCode131(base,*p))return false;
 HeroIntelligenceSource131 value;if(!ReadRemoveUnit131(base,build,selected,value.source))return false;
 __try{value.hero=*reinterpret_cast<uintptr_t*>(value.source.unit+sp->heroMember);if(!HeroIntelligenceIdentity131(base,build,value))return false;
 const auto get=reinterpret_cast<HeroIntelligenceGet131>(base+p->get);value.base=get(value.source.unit,0);if(selected()!=value.source.unit||!HeroIntelligenceIdentity131(base,build,value))return false;value.total=get(value.source.unit,1);
 if(selected()!=value.source.unit||!HeroIntelligenceIdentity131(base,build,value)||value.base<0||value.base>1000000000||value.total<0||value.total>1000000000)return false;out=value;return true;
 }__except(1){return false;}}
inline bool SetHeroIntelligenceNative131(uintptr_t base,unsigned build,const HeroIntelligenceSource131& value,int requested){const auto* p=HeroIntelligenceProfileFor131(build);return p&&HeroIntelligenceCode131(base,*p)&&HeroIntelligenceIdentity131(base,build,value)&&InvokeHeroIntelligence131(reinterpret_cast<HeroIntelligenceSet131>(base+p->set),value.source.unit,requested);}
template<class Read,class Set> bool HeroIntelligenceCommand131(const std::wstring& args,Read read,Set set,std::string& report){
 int requested=500;if(!ParseHeroIntelligence131(args,requested)){report="Usage: select a hero, then #int [1..10000] (default 500)";return false;}
 HeroIntelligenceSource131 before,current,after;if(!read(before)||!read(current)||!SameHeroIntelligence131(before,current)||before.base!=current.base){report="Hero INT: select a stable registered hero";return false;}
 if(current.base!=requested){if(!set(current,requested)){report="Hero INT: native update failed; update may have applied";return false;}if(!read(after)||!SameHeroIntelligence131(current,after)||after.base!=requested){report="Hero INT: game did not confirm requested base Intelligence";return false;}current=after;}
 report="Hero INT: "+std::to_string(current.base)+" (total: "+std::to_string(current.total)+")";return true;
}
}
