#pragma once
#include "unit_model_profile131.hpp"
#include "remove_unit131.hpp"
namespace wht {
inline const UnitModelProfile131* UnitModelProfileFor131(unsigned build){for(const auto& p:unitModelProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool UnitModelCode131(uintptr_t base,const UnitModelProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
inline bool UnitModelPath131(const std::string& path){
 if(path.size()<5||path.size()>=512)return false;
 for(unsigned char c:path)if(c<32||c>=127||c=='|')return false;
 std::string suffix=path.substr(path.size()-4);for(char& c:suffix)c=static_cast<char>(tolower(static_cast<unsigned char>(c)));
 return suffix==".mdx"||suffix==".mdl";
}
struct UnitModelSource131 {RemoveUnitSource131 owner;uintptr_t model=0,name=0,text=0;std::string path;};
inline bool SameUnitModel131(const UnitModelSource131& a,const UnitModelSource131& b){return SameRemoveUnit131(a.owner,b.owner)&&a.model&&a.name&&a.text&&a.model==b.model&&a.name==b.name&&a.text==b.text&&a.path==b.path;}
inline bool ReadUnitModelFields131(const UnitModelProfile131& p,uintptr_t base,uintptr_t unit,uintptr_t& model,uintptr_t& name,uintptr_t& text,char* buffer){
 __try{
 model=*reinterpret_cast<uintptr_t*>(unit+p.modelOffset);if(!model||*reinterpret_cast<uintptr_t*>(model)!=base+p.modelTable)return false;
 name=*reinterpret_cast<uintptr_t*>(model+p.nameOffset);if(!name||*reinterpret_cast<uintptr_t*>(name)!=base+p.stringTable)return false;
 text=*reinterpret_cast<uintptr_t*>(name+p.stringOffset);if(!text)return false;
 for(unsigned i=0;i<512;++i){buffer[i]=*reinterpret_cast<const char*>(text+i);if(!buffer[i])return i>0;}
 return false;
 }__except(1){return false;}
}
template<class Selected> bool ReadUnitModel131(uintptr_t base,unsigned build,Selected selected,UnitModelSource131& out){
 const auto* p=UnitModelProfileFor131(build);if(!p||!UnitModelCode131(base,*p))return false;
 UnitModelSource131 value;char buffer[512]{};
 if(!ReadRemoveUnit131(base,build,selected,value.owner)||!ReadUnitModelFields131(*p,base,value.owner.unit,value.model,value.name,value.text,buffer))return false;
 value.path=buffer;if(!UnitModelPath131(value.path)||selected()!=value.owner.unit||!ValidRemoveUnit131(base,build,value.owner))return false;
 uintptr_t model=0,name=0,text=0;char again[512]{};
 if(!ReadUnitModelFields131(*p,base,value.owner.unit,model,name,text,again)||model!=value.model||name!=value.name||text!=value.text||value.path!=again)return false;
 out=value;return true;
}
template<class Read> bool UnitModelCommand131(const std::wstring& args,Read read,std::string& report){
 if(!UnitInfoArgs131(args)){report="Usage: select a unit, then #gmodel";return false;}
 UnitModelSource131 before,after;if(!read(before)||!read(after)||!SameUnitModel131(before,after)||!UnitModelPath131(before.path)){report="Get Model: selected unit model filename unavailable or changed";return false;}
 report="Get Model: "+before.path;return true;
}
}
