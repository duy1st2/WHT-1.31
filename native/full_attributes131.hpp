#pragma once
#include "hero_strength131.hpp"
#include "hero_agility131.hpp"
#include "hero_intelligence131.hpp"
namespace wht {
struct FullAttributesSource131 {HeroStrengthSource131 strength;HeroAgilitySource131 agility;HeroIntelligenceSource131 intelligence;};
inline bool CoherentFullAttributes131(const FullAttributesSource131& value){return value.strength.hero&&value.strength.hero==value.agility.hero&&value.strength.hero==value.intelligence.hero&&SameRemoveUnit131(value.strength.source,value.agility.source)&&SameRemoveUnit131(value.strength.source,value.intelligence.source);}
inline bool SameFullAttributes131(const FullAttributesSource131& a,const FullAttributesSource131& b){return CoherentFullAttributes131(a)&&CoherentFullAttributes131(b)&&SameHeroStrength131(a.strength,b.strength)&&SameHeroAgility131(a.agility,b.agility)&&SameHeroIntelligence131(a.intelligence,b.intelligence);}
inline int FullAttributeBase131(const FullAttributesSource131& value,unsigned index){return index==0?value.strength.base:index==1?value.agility.base:value.intelligence.base;}
inline bool SameFullAttributeValues131(const FullAttributesSource131& a,const FullAttributesSource131& b){for(unsigned i=0;i<3;++i)if(FullAttributeBase131(a,i)!=FullAttributeBase131(b,i))return false;return true;}
template<class Selected> bool ReadFullAttributes131(uintptr_t base,unsigned build,Selected selected,FullAttributesSource131& out){FullAttributesSource131 value;
 if(!ReadHeroStrength131(base,build,selected,value.strength)||!ReadHeroAgility131(base,build,selected,value.agility)||!ReadHeroIntelligence131(base,build,selected,value.intelligence)||!CoherentFullAttributes131(value))return false;
 if(selected()!=value.strength.source.unit||!HeroStrengthIdentity131(base,build,value.strength))return false;out=value;return true;
}
inline bool SetFullAttributeNative131(uintptr_t base,unsigned build,const FullAttributesSource131& value,unsigned index,int requested){if(!CoherentFullAttributes131(value)||index>2)return false;
 return index==0?SetHeroStrengthNative131(base,build,value.strength,requested):index==1?SetHeroAgilityNative131(base,build,value.agility,requested):SetHeroIntelligenceNative131(base,build,value.intelligence,requested);
}
template<class Read,class Set> bool FullAttributesCommand131(const std::wstring& args,Read read,Set set,std::string& report){
 int requested=500;if(!ParseHeroStrength131(args,requested)){report="Usage: select a hero, then #fullatt [1..10000] (default 500)";return false;}
 FullAttributesSource131 before,current;if(!read(before)||!read(current)||!SameFullAttributes131(before,current)||!SameFullAttributeValues131(before,current)){report="Hero Attributes: select a stable registered hero";return false;}
 unsigned confirmed=0;
 for(unsigned i=0;i<3;++i){FullAttributesSource131 check;if(!read(check)||!SameFullAttributes131(before,check)||!SameFullAttributeValues131(current,check)){report="Hero Attributes: hero or values changed; "+std::to_string(confirmed)+" updates confirmed";return false;}
 if(FullAttributeBase131(check,i)==requested){current=check;continue;}
 if(!set(check,i,requested)){report="Hero Attributes: native update failed; "+std::to_string(confirmed)+" updates confirmed; current update may have applied";return false;}
 FullAttributesSource131 after;if(!read(after)||!SameFullAttributes131(before,after)||FullAttributeBase131(after,i)!=requested){report="Hero Attributes: native result not confirmed; partial changes may have applied";return false;}current=after;++confirmed;}
 FullAttributesSource131 final;if(!read(final)||!SameFullAttributes131(before,final)){report="Hero Attributes: hero changed during final verification; partial changes may have applied";return false;}
 for(unsigned i=0;i<3;++i)if(FullAttributeBase131(final,i)!=requested){report="Hero Attributes: game changed a requested base stat; partial changes may have applied";return false;}
 report="Hero STR/AGI/INT: "+std::to_string(requested)+" (totals: "+std::to_string(final.strength.total)+" / "+std::to_string(final.agility.total)+" / "+std::to_string(final.intelligence.total)+")";return true;
}
}
