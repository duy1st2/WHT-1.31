#pragma once
#include "unit_scale_profile131.hpp"
#include "remove_ability131.hpp"
#include <iomanip>
namespace wht {
inline const UnitScaleProfile131* UnitScaleProfileFor131(unsigned build){for(const auto& p:unitScaleProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool UnitScaleCode131(uintptr_t base,const UnitScaleProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.length};return ResourceTextGuard131(base,code);}
inline bool ParseUnitScale131(const std::wstring& args,float& scale){
 if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);in.imbue(std::locale::classic());float value=100;std::wstring extra;
 if(args.find_first_not_of(L" \t")!=std::wstring::npos&&(!(in>>value)||in>>extra))return false;if(!std::isfinite(value))return false;
 scale=value>10?value/100.f:value;scale=(std::max)(.05f,(std::min)(20.f,scale));return true;
}
using UnitScaleSetter131=void(__cdecl*)(uintptr_t,unsigned,float);
using UnitScaleGetter131=float(__cdecl*)(uintptr_t,unsigned);
inline bool InvokeUnitScale131(UnitScaleSetter131 setter,uintptr_t model,float scale){if(!setter||!model||!std::isfinite(scale)||scale<.05f||scale>20)return false;__try{setter(model,2,scale);return true;}__except(1){return false;}}
inline bool ScaleMethod131(uintptr_t base,uintptr_t fn){__try{const auto pe=*reinterpret_cast<uint32_t*>(base+60);if(pe>0x1000)return false;const auto size=*reinterpret_cast<uint32_t*>(base+pe+24+56);if(size<0x1000||size>0x10000000||fn<base+0x1000||fn>=base+size)return false;
 MEMORY_BASIC_INFORMATION memory{};if(!VirtualQuery(reinterpret_cast<void*>(fn),&memory,sizeof(memory))||memory.State!=MEM_COMMIT||(memory.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;return (memory.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY))!=0;
 }__except(1){return false;}}
struct UnitScaleSource131 {uintptr_t unit=0,world=0,model=0,properties=0,property=0,propertyTable=0;DropObjectId id;unsigned local=24;float scale=NAN;};
inline bool SameUnitScale131(const UnitScaleSource131& a,const UnitScaleSource131& b){return a.unit&&a.world&&a.model&&a.property&&a.id.Valid()&&a.local<24&&a.unit==b.unit&&a.id==b.id&&a.world==b.world&&a.local==b.local&&a.model==b.model&&a.properties==b.properties&&a.property==b.property&&a.propertyTable==b.propertyTable;}
inline bool ReadScaleProperty131(uintptr_t base,const UnitScaleProfile131& p,uintptr_t model,uintptr_t& properties,uintptr_t& property,uintptr_t& table){
 __try{if(!model)return false;const auto count=*reinterpret_cast<unsigned*>(model+p.count);if(count<=2||count>65536)return false;properties=*reinterpret_cast<uintptr_t*>(model+p.table);if(!properties)return false;property=*reinterpret_cast<uintptr_t*>(properties+2*sizeof(uintptr_t));if(!property)return false;
 if(*reinterpret_cast<unsigned char*>(property+p.type)!=6||(*reinterpret_cast<unsigned char*>(property+p.flags)&2))return false;table=*reinterpret_cast<uintptr_t*>(property);
 if(!table)return false;return ScaleMethod131(base,*reinterpret_cast<uintptr_t*>(table+sizeof(uintptr_t)))&&ScaleMethod131(base,*reinterpret_cast<uintptr_t*>(table+3*sizeof(uintptr_t)));
 }__except(1){return false;}
}
template<class Selected> bool ReadUnitScale131(uintptr_t base,unsigned build,Selected selected,UnitScaleSource131& out){
 const auto* p=UnitScaleProfileFor131(build);const auto* inv=InventoryProfile(build);const auto* rp=ResourceProfileFor131(build);if(!p||!inv||!rp||!UnitScaleCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 __try{UnitScaleSource131 value;value.unit=selected();if(!InventoryIdentity131(value.unit,base,inv->unitTable))return false;value.id=DropIdentity131(value.unit);if(!value.id.Valid())return false;value.world=ResourceWorld131(base,*rp,value.local);if(!value.world)return false;
 value.model=*reinterpret_cast<uintptr_t*>(value.unit+p->model);if(!ReadScaleProperty131(base,*p,value.model,value.properties,value.property,value.propertyTable))return false;
 value.scale=reinterpret_cast<UnitScaleGetter131>(base+p->get)(value.model,2);if(!std::isfinite(value.scale))return false;
 uintptr_t properties=0,property=0,table=0;unsigned local=24;
 if(selected()!=value.unit||!(DropIdentity131(value.unit)==value.id)||!InventoryIdentity131(value.unit,base,inv->unitTable)||ResourceWorld131(base,*rp,local)!=value.world||local!=value.local||*reinterpret_cast<uintptr_t*>(value.unit+p->model)!=value.model||!ReadScaleProperty131(base,*p,value.model,properties,property,table)||properties!=value.properties||property!=value.property||table!=value.propertyTable)return false;
 out=value;return true;
 }__except(1){return false;}
}
inline bool ApplyUnitScale131(uintptr_t base,unsigned build,float scale,const UnitScaleSource131& expected){
 UnitScaleSource131 now;if(!ReadUnitScale131(base,build,[&](){return expected.unit;},now)||!SameUnitScale131(expected,now)||expected.scale!=now.scale)return false;
 const auto* p=UnitScaleProfileFor131(build);return p&&InvokeUnitScale131(reinterpret_cast<UnitScaleSetter131>(base+p->set),expected.model,scale);
}
template<class Read,class Apply> bool UnitScaleCommand131(const std::wstring& args,Read read,Apply apply,std::string& report){
 float scale=1;if(!ParseUnitScale131(args,scale)){report="Usage: #size 200 (2x) or #size 100 (1x)";return false;}
 UnitScaleSource131 before,current,after;if(!read(before)||!read(current)||!SameUnitScale131(before,current)||!std::isfinite(before.scale)||before.scale!=current.scale){report="Unit Scale: select a stable unit with a writable model scale";return false;}
 if(before.scale!=scale){if(!apply(scale,before)){report="Unit Scale: native update failed or selection changed";return false;}
 if(!read(after)||!SameUnitScale131(before,after)||!std::isfinite(after.scale)||fabsf(after.scale-scale)>0.00001f){report="Unit Scale: game did not confirm result; update may have applied";return false;}}
 std::ostringstream text;text.imbue(std::locale::classic());text<<"Unit Scale: "<<std::fixed<<std::setprecision(2)<<scale<<"x";report=text.str();return true;
}
}
