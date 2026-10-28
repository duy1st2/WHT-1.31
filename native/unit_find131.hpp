#pragma once
#include "unit_info131.hpp"
#include "remove_unit131.hpp"
#include "remove_ability131.hpp"
namespace wht {
struct UnitFindSource131 {RemoveUnitSource131 source;UnitInfo131 info;};
inline bool ValidUnitFindPosition131(const UnitInfo131& v){return v.code&&std::isfinite(v.x)&&std::isfinite(v.y);}
inline bool SameUnitFind131(const UnitFindSource131& a,const UnitFindSource131& b){return SameRemoveUnit131(a.source,b.source)&&a.info.code==b.info.code;}
template<class Selected> bool ReadUnitFind131(uintptr_t base,unsigned build,Selected selected,UnitFindSource131& out){UnitFindSource131 v;if(!ReadRemoveUnit131(base,build,selected,v.source)||!ReadUnitInfoFields131(base,build,v.source.unit,true,v.info)||!ValidUnitFindPosition131(v.info)||!ValidRemoveUnit131(base,build,v.source)||selected()!=v.source.unit)return false;out=v;return true;}
template<class Read> bool UnitFindCommand131(const std::wstring& args,Read read,std::string& report){uint32_t code=0;if(!ParseRemoveAbility131(args,code)){report="Usage: #ufind rawcode (e.g. hfoo or 0x68666F6F); active selection only";return false;}UnitFindSource131 before,current;if(!read(before)||!read(current)||!SameUnitFind131(before,current)||!ValidUnitFindPosition131(before.info)||!ValidUnitFindPosition131(current.info)){report="Unit Find: select a stable registered unit; position unavailable";return false;}if(current.info.code!=code){report="Unit Find: ["+RemoveAbilityName131(code)+"] not found in active selection; selected ["+RemoveAbilityName131(current.info.code)+"]";return false;}char coordinates[100]{};std::snprintf(coordinates,sizeof(coordinates)," at (%.1f, %.1f)",current.info.x,current.info.y);report="Found ["+RemoveAbilityName131(code)+"] in active selection"+coordinates;return true;}
}
