#pragma once
#include "craft_selection131.hpp"
#include "unit_info131.hpp"
namespace wht {
struct ReselectSource131 {uintptr_t unit=0;DropObjectId id;};
inline bool SameReselect131(const ReselectSource131& a,const ReselectSource131& b){return a.unit&&a.id.Valid()&&a.unit==b.unit&&a.id==b.id;}
template<class Read,class Valid,class Select,class Restore>
bool ReselectFlow131(const std::wstring& args,Read read,Valid valid,Select select,Restore restore,std::string& report){
 if(!UnitInfoArgs131(args)){report="Usage: #selc (select a unit first)";return false;}
 const auto target=read();if(!SameReselect131(target,read())||!valid(target)){report="Reselect: select a stable, valid unit";return false;}
 // Resolve/validate both operations before this flow. An empty native list clears selection.
 if(!select(nullptr)){restore();report="Reselect: native clear failed";return false;}
 if(read().unit){restore();report="Reselect: game did not clear the selection";return false;}
 if(!valid(target)){restore();report="Reselect: unit or match changed";return false;}
 if(!select(&target)){restore();report="Reselect: native selection failed";return false;}
 if(!SameReselect131(target,read())||!valid(target)){restore();report="Reselect: game did not confirm the selected unit";return false;}
 report="Selected unit reselected";return true;
}
inline bool ValidReselectUnit131(uintptr_t base,unsigned build,const ReselectSource131& source){
 const auto* p=InventoryProfile(build);if(!p||!source.unit||!source.id.Valid())return false;
 __try{return InventoryIdentity131(source.unit,base,p->unitTable)&&DropIdentity131(source.unit)==source.id;}__except(1){return false;}
}
template<class Selected> ReselectSource131 CaptureReselect131(uintptr_t base,unsigned build,Selected selected){
 __try{ReselectSource131 value;value.unit=selected();const auto* p=InventoryProfile(build);if(!p||!InventoryIdentity131(value.unit,base,p->unitTable))return {};value.id=DropIdentity131(value.unit);return ValidReselectUnit131(base,build,value)?value:ReselectSource131{};}__except(1){return {};}
}
template<class Selected> bool ReselectCommand131(uintptr_t base,unsigned build,uintptr_t ui,const std::wstring& args,Selected selected,std::string& report){
 if(!UnitInfoArgs131(args)){report="Usage: #selc (select a unit first)";return false;}
 const CraftSelectionProfile131* profile=nullptr;for(const auto& p:craftSelectionProfiles131)if(p.build==build)profile=&p;
 const auto* resource=ResourceProfileFor131(build);unsigned local=24;uintptr_t world=0;
 SelectionRoster131 original;
 if(!profile||!CraftSelectionGuard131(base,*profile)||!resource||!ResourceCode131(base,*resource)||!(world=ResourceWorld131(base,*resource,local))||!ReadSelectionRoster131(base,build,original)||!original.count){report="Reselect: native selection or match unavailable";return false;}
 auto session=[&](){unsigned slot=24;return ResourceWorld131(base,*resource,slot)==world&&slot==local;};
 auto read=[&](){return CaptureReselect131(base,build,selected);};
 auto valid=[&](const ReselectSource131& value){bool found=false;for(unsigned i=0;i<original.count;++i)if(original.units[i]==value.unit&&original.ids[i]==value.id)found=true;return found&&session()&&ValidReselectUnit131(base,build,value);};
 auto select=[&](const ReselectSource131* value){if(!session())return false;SelectionRoster131 roster;if(value){if(!valid(*value))return false;roster.count=1;roster.units[0]=value->unit;roster.ids[0]=value->id;}return CraftSelectRoster131(base,build,ui,roster);};
 auto restore=[&](){if(!session())return;SelectionRoster131 live;for(unsigned i=0;i<original.count;++i){ReselectSource131 value{original.units[i],original.ids[i]};if(ValidReselectUnit131(base,build,value)){live.units[live.count]=value.unit;live.ids[live.count++]=value.id;}}CraftSelectRoster131(base,build,ui,live);};
 return ReselectFlow131(args,read,valid,select,restore,report);
}
}
