#pragma once
#include "common.hpp"
#include "show_resource.hpp"
namespace wht {
struct ResourceApi129 {
 uint32_t(__cdecl* player)(unsigned)=nullptr;
 uint32_t(__cdecl* local)()=nullptr;
 int(__cdecl* ally)(uint32_t,uint32_t)=nullptr;
 int(__cdecl* observer)(uint32_t)=nullptr;
 int(__cdecl* slot)(uint32_t)=nullptr;
 int(__cdecl* state)(uint32_t,int)=nullptr;
 unsigned(__cdecl* color)(uint32_t)=nullptr;
 uintptr_t(__cdecl* resolve)(uint32_t)=nullptr;
 const char*(__thiscall* name)(uintptr_t,int)=nullptr;
 bool Complete()const{return player&&local&&ally&&observer&&slot&&state&&color&&resolve&&name;}
};
inline bool ResourceRaw129(const ResourceApi129& api,unsigned slot,uint32_t local,ResourceRow& row,char* bytes){
 // Keep SEH separate from string/vector construction (MSVC C2712).
 uint32_t player=0;const char* name=nullptr;
 __try {
  player=api.player(slot);if(!player)return false;
  if(player!=local&&!api.observer(player)&&!api.ally(local,player))return false;
  const int status=api.slot(player);if(status!=1&&status!=2)return false;
  const auto object=api.resolve(player);if(!object)return false;
  // Older maps remap neutral slots 12..15 to physical 24..27 in 1.29.
  // These are not participant rows, even if neutral-passive reports observer.
  if(*reinterpret_cast<const unsigned char*>(object+0x30)>=24)return false;
  row.slot=slot;row.left=status==2;row.color=api.color(player);
  row.gold=api.state(player,1);row.lumber=api.state(player,2);
  row.cap=(std::min)(api.state(player,4),api.state(player,6));row.food=api.state(player,5);
  name=api.name(object,0);if(name){unsigned i=0;for(;i<256&&name[i];++i)bytes[i]=name[i];if(i==256)return false;}
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 return true;
}
inline bool ResourceRow129(const ResourceApi129& api,unsigned slot,uint32_t local,ResourceRow& row){
 char bytes[257]{};if(!ResourceRaw129(api,slot,local,row,bytes))return false;
 row.name=ResourceName(Wide(bytes));if(row.name.empty())row.name=L"Player "+std::to_wstring(slot+1);return true;
}
inline uint32_t ResourceLocal129(uintptr_t base,const ResourceApi129& api){
 if(!base||!api.Complete())return 0;
 __try {const auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);
  if(!world||!ui||!*reinterpret_cast<uintptr_t*>(ui+0x294))return 0;
  const unsigned slot=*reinterpret_cast<uint16_t*>(world+0x28);if(slot>=24)return 0;
  const auto local=api.local();return local&&local==api.player(slot)?local:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline ResourceTable ReadResources129(uintptr_t base,bool enabled,const ResourceApi129& api){
 ResourceTable table;if(!enabled)return table;const auto local=ResourceLocal129(base,api);if(!local)return table;
 table.ready=true;for(unsigned slot=0;slot<24;++slot){ResourceRow row;if(ResourceRow129(api,slot,local,row))table.rows.push_back(std::move(row));}return table;
}
}
