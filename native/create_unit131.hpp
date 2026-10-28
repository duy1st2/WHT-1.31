#pragma once
#include "create_unit_profile131.hpp"
#include "unit_find131.hpp"
#include "resource_commands131.hpp"
#include "mouse_world131.hpp"
namespace wht {
inline const CreateUnitProfile131* CreateUnitProfileFor131(unsigned build){for(const auto& p:createUnitProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool CreateUnitCode131(uintptr_t base,const CreateUnitProfile131& p){
 ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};const auto* rp=ResourceProfileFor131(p.build);if(!rp||!ResourceTextGuard131(base,code))return false;
 __try{return *reinterpret_cast<uintptr_t*>(base+rp->playerTable+(sizeof(uintptr_t)==8?0xc8:0x64))==base+p.ownerGetter;}__except(1){return false;}
}
inline bool ParseUnitCount131(const std::wstring& args,unsigned& count){
 count=1;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;if(!(in>>value))return true;
 if(value.size()>2||value.find_first_not_of(L"0123456789")!=std::wstring::npos||in>>extra)return false;
 unsigned n=0;for(auto c:value)n=n*10+unsigned(c-L'0');if(n<1||n>50)return false;count=n;return true;
}
inline bool ParseCreateUnit131(const std::wstring& args,uint32_t& code,unsigned& count){
 code=0;count=1;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring raw,rest;if(!(in>>raw)||!ParseRemoveAbility131(raw,code))return false;std::getline(in,rest);return ParseUnitCount131(rest,count);
}
inline bool SpawnPoint131(float x,float y){return std::isfinite(x)&&std::isfinite(y)&&std::abs(x)<=100000.f&&std::abs(y)<=100000.f;}
using CreateUnitNative131=uintptr_t(__cdecl*)(unsigned,uint32_t,const float*,const float*,const float*);
inline uintptr_t InvokeCreateUnit131(CreateUnitNative131 create,unsigned owner,uint32_t code,float x,float y,float facing){
 if(!create||owner>=24||!code||!SpawnPoint131(x,y)||!std::isfinite(facing))return 0;
 __try{return create(owner,code,&x,&y,&facing);}__except(1){return 0;}
}
inline bool CreateFacing131(uintptr_t base,const CreateUnitProfile131& p,float& out){__try{const float scale=*reinterpret_cast<const float*>(base+p.angleScale);if(!std::isfinite(scale)||scale<.01745328f||scale>.01745331f)return false;out=270.f*scale;return true;}__except(1){return false;}}
struct CreatedUnit131 {RemoveUnitSource131 source;UnitInfo131 info;unsigned owner=~0u;};
inline bool ConfirmCreatedUnit131(uintptr_t base,unsigned build,uintptr_t unit,uint32_t code,const ResourceCommandSource131& context,CreatedUnit131& out){
 CreatedUnit131 value;if(!ReadRemoveUnit131(base,build,[&](){return unit;},value.source)||value.source.world!=context.world||value.source.local!=context.local||!ReadUnitInfoFields131(base,build,unit,true,value.info)||value.info.code!=code||!SpawnPoint131(value.info.x,value.info.y))return false;
 const auto* inv=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(!inv||!np)return false;const auto api=DropNetworkNative131(base,*np,*inv);if(!api)return false;
 __try{value.owner=api.owner(unit);}__except(1){return false;}
 if(value.owner!=context.physical||!ValidRemoveUnit131(base,build,value.source))return false;out=value;return true;
}
template<class Read,class SourceValid,class Create,class Confirm> bool CreateUnitBatch131(uint32_t code,unsigned count,float x,float y,float spacing,Read read,SourceValid sourceValid,Create create,Confirm confirm,std::string& report){
 if(!code||count<1||count>50||!std::isfinite(spacing)||spacing<0||spacing>1000||!SpawnPoint131(x,y)||!SpawnPoint131(x+float(count-1)*spacing,y)){report="Create Unit: invalid count or spawn coordinates";return false;}
 ResourceCommandSource131 context,current;if(!read(context)||!read(current)||!SameResourcePlayer131(context,current)||!sourceValid(context)){report="Create Unit: stable local player/source required";return false;}
 std::array<DropObjectId,50> ids{};unsigned made=0;
 auto fail=[&](bool invoked){report="Create Unit: "+std::to_string(made)+"/"+std::to_string(count)+" confirmed; stopped"+(invoked?" (latest invocation may have created a unit)":" before next spawn");return false;};
 for(unsigned i=0;i<count;++i){
  if(!read(current)||!SameResourcePlayer131(context,current)||!sourceValid(context))return fail(false);
  const auto unit=create(context.physical,code,x+float(i)*spacing,y);if(!unit)return fail(true);
  CreatedUnit131 value;if(!confirm(unit,code,context,value)||!value.source.id.Valid()||!sourceValid(context)||!read(current)||!SameResourcePlayer131(context,current))return fail(true);
  for(unsigned k=0;k<made;++k)if(ids[k]==value.source.id)return fail(true);ids[made++]=value.source.id;
 }
 report="Create Unit: "+std::to_string(made)+"/"+std::to_string(count)+" created";return true;
}
inline std::atomic_flag createUnitBusy131=ATOMIC_FLAG_INIT;
template<class Selected,class Cursor> bool CreateOrCopyUnitCommand131(uintptr_t base,unsigned build,const std::wstring& name,const std::wstring& args,Selected selected,Cursor cursor,std::string& report){
 const bool copy=name==L"#copy";uint32_t code=0;unsigned count=1;
 if((name!=L"#create"&&!copy)||!(copy?ParseUnitCount131(args,count):ParseCreateUnit131(args,code,count))){report=copy?"Usage: #copy [1..50] (select a unit first)":"Usage: #create rawcode [1..50]";return false;}
 if(createUnitBusy131.test_and_set()){report="Create Unit: native batch already active";return false;}struct Release {~Release(){createUnitBusy131.clear();}} release;
 const auto* p=CreateUnitProfileFor131(build);float facing=0;if(!p||!CreateUnitCode131(base,*p)||!CreateFacing131(base,*p,facing)){report="Create Unit: native profile mismatch";return false;}
 ResourceCommandSource131 commandContext;if(!ReadResourceCommand131(base,build,commandContext)){report="Create Unit: active local player unavailable";return false;}
 float x=0,y=0;UnitFindSource131 source;
 if(copy){UnitFindSource131 check;if(!ReadUnitFind131(base,build,selected,source)||!ReadUnitFind131(base,build,selected,check)||!SameUnitFind131(source,check)){report="Copy Unit: select a stable registered unit";return false;}code=source.info.code;x=source.info.x+50.f;y=source.info.y;}
 else {MouseWorldPosition131 point;if(!cursor(point)||!SpawnPoint131(point.x,point.y)){report="Create Unit: point the cursor at the map";return false;}x=point.x;y=point.y;}
 auto sourceValid=[&](const ResourceCommandSource131& context){if(!SameResourcePlayer131(commandContext,context))return false;if(!copy)return true;if(source.source.world!=context.world||source.source.local!=context.local||!ValidRemoveUnit131(base,build,source.source)||selected()!=source.source.unit)return false;UnitInfo131 info;return ReadUnitInfoFields131(base,build,source.source.unit,false,info)&&info.code==code&&ValidRemoveUnit131(base,build,source.source);};
 const bool result=CreateUnitBatch131(code,count,x,y,copy?50.f:40.f,[&](ResourceCommandSource131& out){return ReadResourceCommand131(base,build,out);},sourceValid,[&](unsigned owner,uint32_t type,float px,float py){return InvokeCreateUnit131(reinterpret_cast<CreateUnitNative131>(base+p->create),owner,type,px,py,facing);},[&](uintptr_t unit,uint32_t type,const ResourceCommandSource131& context,CreatedUnit131& out){return ConfirmCreatedUnit131(base,build,unit,type,context,out);},report);
 if(copy&&report.compare(0,12,"Create Unit:")==0)report.replace(0,12,"Copy Unit:");return result;
}
}
