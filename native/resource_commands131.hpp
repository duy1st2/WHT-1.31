#pragma once
#include "resource_commands_profile131.hpp"
#include "resource131.hpp"
#include <sstream>
#include <climits>
namespace wht {
inline const ResourceCommandProfile131* ResourceCommandProfileFor131(unsigned build){for(const auto& p:resourceCommandProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool ResourceCommandCode131(uintptr_t base,const ResourceCommandProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
enum class ResourceCommand131:unsigned {Gold,Lumber,Food,Full};
inline bool IsResourceCommand131(const std::wstring& name){return name==L"#gold"||name==L"#lumber"||name==L"#food"||name==L"#fullres";}
inline ResourceCommand131 ResourceCommandName131(const std::wstring& name){return name==L"#gold"?ResourceCommand131::Gold:name==L"#lumber"?ResourceCommand131::Lumber:name==L"#food"?ResourceCommand131::Food:ResourceCommand131::Full;}
inline bool ParseResourceAmount131(ResourceCommand131 command,const std::wstring& args,int& amount){
 if(unsigned(command)>3||args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;amount=command==ResourceCommand131::Food?200:100000;
 if(args.find_first_not_of(L" \t")==std::wstring::npos)return true;if(command==ResourceCommand131::Full)return false;
 std::wistringstream in(args);in.imbue(std::locale::classic());long long value=0;std::wstring extra;if(!(in>>value)||in>>extra||value<1||value>INT_MAX)return false;amount=int(value);return true;
}
struct ResourceCommandSource131 {uintptr_t world=0,player=0;unsigned local=24,physical=24;int values[3]{};};
inline bool SameResourcePlayer131(const ResourceCommandSource131& a,const ResourceCommandSource131& b){return a.world&&a.player&&a.local<24&&a.physical<24&&a.world==b.world&&a.player==b.player&&a.local==b.local&&a.physical==b.physical;}
inline bool ReadResourceCommand131(uintptr_t base,unsigned build,ResourceCommandSource131& out){
 const auto* p=ResourceProfileFor131(build);const auto* cp=ResourceCommandProfileFor131(build);if(!p||!cp||!ResourceCode131(base,*p)||!ResourceCommandCode131(base,*cp))return false;
 ResourceNative131 api{reinterpret_cast<decltype(api.player)>(base+p->playerFn),reinterpret_cast<decltype(api.integer)>(base+p->integerFn),reinterpret_cast<decltype(api.string)>(base+p->stringFn)};
 __try{ResourceCommandSource131 value;value.world=ResourceWorld131(base,*p,value.local);if(!value.world)return false;value.player=api.player(value.world,value.local);if(!ResourceIdentity131(value.player,base,p->playerTable)||*reinterpret_cast<int*>(value.player+p->statusOffset)!=1)return false;
 value.physical=*reinterpret_cast<unsigned char*>(value.player+p->idOffset);if(value.physical>=24)return false;const unsigned states[]={1,2,4};for(unsigned i=0;i<3;++i){value.values[i]=api.integer(value.player+p->stateBase+states[i]*p->stateStride);if(i<2)value.values[i]/=10;if(value.values[i]<0)return false;}
 unsigned local=24;if(ResourceWorld131(base,*p,local)!=value.world||local!=value.local||api.player(value.world,local)!=value.player||!ResourceIdentity131(value.player,base,p->playerTable)||*reinterpret_cast<unsigned char*>(value.player+p->idOffset)!=value.physical)return false;out=value;return true;
 }__except(1){return false;}
}
using ResourceSetNative131=void(__thiscall*)(uintptr_t,unsigned,int);
inline bool InvokeResourceSet131(ResourceSetNative131 set,uintptr_t player,unsigned state,int value){if(!set||!player||(state!=1&&state!=2&&state!=4)||value<1||((state==1||state==2)&&value>9999999))return false;__try{set(player,state,value);return true;}__except(1){return false;}}
inline bool SetResourceNative131(uintptr_t base,unsigned build,const ResourceCommandSource131& source,unsigned state,int value){ResourceCommandSource131 now;if(!ReadResourceCommand131(base,build,now)||!SameResourcePlayer131(source,now))return false;const unsigned index=state==1?0:state==2?1:2;if(now.values[index]!=source.values[index])return false;const auto* p=ResourceCommandProfileFor131(build);return p&&InvokeResourceSet131(reinterpret_cast<ResourceSetNative131>(base+p->set),now.player,state,value);}
template<class Read,class Set> bool ResourceCommandFlow131(ResourceCommand131 command,const std::wstring& args,Read read,Set set,std::string& report){
 int amount=0;if(!ParseResourceAmount131(command,args,amount)){report="Usage: #gold/#lumber [positive integer], #food [cap], or #fullres";return false;}
 ResourceCommandSource131 before,current;if(!read(before)||!read(current)||!SameResourcePlayer131(before,current)){report="Resources: active local player required";return false;}
 const unsigned first=command==ResourceCommand131::Full?0:unsigned(command),last=command==ResourceCommand131::Full?3:first+1;int desired[3]{};
 for(unsigned i=first;i<last;++i){if(before.values[i]!=current.values[i]||current.values[i]<0){report="Resources: values changed before update";return false;}const int64_t value=command==ResourceCommand131::Full?(i==2?300:1000000):i==2?amount:int64_t(current.values[i])+amount;
 if(value>INT_MAX||(i<2&&value>9999999)){report="Resources: requested value exceeds native limit";return false;}desired[i]=int(value);}
 unsigned changed=0;const unsigned states[]={1,2,4};
 for(unsigned i=first;i<last;++i){ResourceCommandSource131 check;if(!read(check)||!SameResourcePlayer131(current,check)||check.values[i]!=current.values[i]){report="Resources: player or value changed; "+std::to_string(changed)+" fields applied";return false;}
 if(check.values[i]==desired[i]){current=check;continue;}
 if(!set(check,states[i],desired[i])){report="Resources: native update failed; "+std::to_string(changed)+" fields confirmed";return false;}
 ResourceCommandSource131 after;if(!read(after)||!SameResourcePlayer131(check,after)||after.values[i]!=desired[i]){report="Resources: native result not confirmed; update may have applied";return false;}current=after;++changed;}
 report="Resources: Gold "+std::to_string(current.values[0])+", Lumber "+std::to_string(current.values[1])+", Food cap "+std::to_string(current.values[2]);return true;
}
}
