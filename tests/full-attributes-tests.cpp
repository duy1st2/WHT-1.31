#include "../native/full_attributes131.hpp"
#include <stdexcept>
#include <cstdio>
#include <vector>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
wht::FullAttributesSource131 Hero(){wht::FullAttributesSource131 result;wht::RemoveUnitSource131 source;source.unit=123;source.world=456;source.agent=789;source.id={1,2};source.local=0;
 result.strength.source=source;result.agility.source=source;result.intelligence.source=source;result.strength.hero=result.agility.hero=result.intelligence.hero=1000;result.strength.base=20;result.agility.base=30;result.intelligence.base=40;result.strength.total=25;result.agility.total=40;result.intelligence.total=55;return result;}
int main(){try{
 auto target=Hero();unsigned reads=0,writes=0,changedAt=0;int refuseAt=-1,noOpAt=-1;bool changedAfterFirst=false,revertAtEnd=false,mixed=false,externalChange=false;std::vector<unsigned> order;std::string report;
 auto read=[&](wht::FullAttributesSource131& out){out=target;++reads;if(reads==changedAt||(changedAfterFirst&&writes==1)){++out.strength.source.id.low;++out.agility.source.id.low;++out.intelligence.source.id.low;}if(mixed)++out.agility.hero;if(revertAtEnd&&writes==3)out.strength.base=1;if(externalChange&&reads==3)++out.intelligence.base;return true;};
 auto set=[&](const wht::FullAttributesSource131& source,unsigned index,int value){Check(wht::SameFullAttributes131(source,target),"all writes retain same original hero");order.push_back(index);++writes;if(int(index)==refuseAt)return false;if(int(index)==noOpAt)return true;
 if(index==0){target.strength.base=value;target.strength.total=value+5;}else if(index==1){target.agility.base=value;target.agility.total=value+10;}else {target.intelligence.base=value;target.intelligence.total=value+15;}return true;};
 auto run=[&](const std::wstring& args){reads=writes=0;order.clear();return wht::FullAttributesCommand131(args,read,set,report);};
 Check(wht::CoherentFullAttributes131(target),"one registered hero across three readers");Check(run(L"")&&order==std::vector<unsigned>({0,1,2})&&writes==3&&target.strength.base==500&&target.agility.base==500&&target.intelligence.base==500&&report=="Hero STR/AGI/INT: 500 (totals: 505 / 510 / 515)","default500 updates in reference order and preserves distinct bonuses");
 Check(run(L"500")&&!writes,"already full no native mutations");target.agility.base=30;Check(run(L"500")&&order==std::vector<unsigned>({1}),"only mismatching base updated");
 target=Hero();Check(run(L"1")&&target.intelligence.base==1,"minimum value");Check(run(L"10000")&&target.strength.base==10000,"maximum value");
 for(const auto* bad:{L"0",L"10001",L"1.5",L"nan",L"500 extra",L"500\n"})Check(!run(bad)&&!reads&&!writes,"invalid input no game access");Check(!run(std::wstring(L"5\0x",3))&&!reads,"NUL no game access");
 target=Hero();changedAt=2;Check(!run(L"500")&&!writes,"selection changed before operation");changedAt=0;mixed=true;Check(!run(L"500")&&!writes,"mixed heroes from getter callbacks refused");mixed=false;externalChange=true;Check(!run(L"500")&&!writes,"base changed before first setter refused");externalChange=false;
 target=Hero();changedAfterFirst=true;Check(!run(L"500")&&writes==1&&target.agility.base==30&&target.intelligence.base==40,"selection changed after first update prevents later hero writes");changedAfterFirst=false;
 target=Hero();refuseAt=1;Check(!run(L"500")&&writes==2&&target.strength.base==500&&target.agility.base==30&&target.intelligence.base==40&&report.find("1 updates confirmed")!=report.npos,"partial update reported without rollback or third setter");refuseAt=-1;
 target=Hero();noOpAt=1;Check(!run(L"500")&&writes==2&&target.intelligence.base==40,"native no-op stops remaining mutations");noOpAt=-1;
 target=Hero();revertAtEnd=true;Check(!run(L"500")&&writes==3&&report.find("changed a requested base stat")!=report.npos,"final confirmation catches later callback reverting previous attribute");revertAtEnd=false;
 target=Hero();target.strength.hero=0;Check(!run(L"500")&&!writes,"nonhero source refused");
 // Runtime dispatch rejects invalid attribute indices without touching game memory.
 Check(!wht::SetFullAttributeNative131(0,12164,Hero(),3,500),"invalid runtime attribute index refused");
 puts("full attributes contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
