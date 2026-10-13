#pragma once
#include "common.hpp"
#include "show_resource.hpp"
namespace wht {
struct ResourceProfile131 {
 unsigned build;uintptr_t worldTable,playerTable,allianceTable,encodedWorld,decodeKey,localOffset;
 uintptr_t playerFn,integerFn,stringFn,stateBase,stateStride,nameOffset,idOffset,allianceOffset,allianceMask,colorOffset,statusOffset;
};
#ifdef _WIN64
inline constexpr ResourceProfile131 resourceProfiles131[]={
 {12164,0x1bb3d30,0x1bb2f48,0x1bcd568,0x20b9d98,0x6b208c73c00571eeULL,0xdc,0x7975e0,0xf11010,0x316560,0xa0,0x18,0x58,0x50,0x88,0x60,0x400,0x40c},
 {12173,0x1be2d08,0x1be1f20,0x1bfc548,0x20e6d98,0xc943ab4704e7c9d0ULL,0xc4,0x7ef900,0xf4cde0,0x383fe0,0x90,0x18,0x48,0x44,0x78,0x60,0x3e8,0x3f4}};
#else
inline constexpr ResourceProfile131 resourceProfiles131[]={
 {12164,0x1806c24,0x1806950,0x180a2d4,0x1aafa78,0x3393cddb,0x94,0x6e4c30,0xd68150,0x2c0b90,0x50,0x10,0x2c,0x28,0x44,0x38,0x2d8,0x2e4},
 {12173,0x17bbc54,0x17bb980,0x17bf304,0x1a67a78,0x51325ca2,0xd4,0x6a84b0,0xd22ce0,0x2aa9c0,0x58,0x10,0x34,0x30,0x4c,0x38,0x2dc,0x2e8}};
#endif
inline const ResourceProfile131* ResourceProfileFor131(unsigned build){for(const auto& p:resourceProfiles131)if(p.build==build)return &p;return nullptr;}
struct ResourceNative131 {
 uintptr_t(__thiscall* player)(uintptr_t,unsigned)=nullptr;
 int(__thiscall* integer)(uintptr_t)=nullptr;
 const char*(__thiscall* string)(uintptr_t)=nullptr;
};
inline bool ResourceCode131(uintptr_t base,const ResourceProfile131& p){
 __try{
#ifdef _WIN64
  const unsigned char player[]={0x48,0x83,0xec,0x28,0x83,0xfa,0x1b,0x77};
  const unsigned char integer[]={0x48,0x83,0xec,0x28,0x8b,0x51,0x14,0x8b};
  const unsigned char string[]={0x48,0x8b,0x41,0x10,0x48,0x85,0xc0,0x74};
#else
  const unsigned char player[]={0x55,0x8b,0xec,0x8b,0x45,0x08,0x83,0xf8};
  const unsigned char integer[]={0xff,0x71,0x0c,0xff,0x71,0x08,0xe8};
  const unsigned char string[]={0x8b,0x41,0x08,0x85,0xc0,0x74,0x04,0x8b};
#endif
  return !memcmp(reinterpret_cast<void*>(base+p.playerFn),player,sizeof(player))&&!memcmp(reinterpret_cast<void*>(base+p.integerFn),integer,sizeof(integer))&&!memcmp(reinterpret_cast<void*>(base+p.stringFn),string,sizeof(string));
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool ResourceIdentity131(uintptr_t object,uintptr_t base,uintptr_t table){__try{return object&&*reinterpret_cast<uintptr_t*>(object)==base+table;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
inline uintptr_t ResourceWorld131(uintptr_t base,const ResourceProfile131& p,unsigned& local){
 __try{auto world=*reinterpret_cast<uintptr_t*>(base+p.encodedWorld)^p.decodeKey;
  if(!ResourceIdentity131(world,base,p.worldTable))return 0;
  local=*reinterpret_cast<uint16_t*>(world+p.localOffset);return local<24?world:0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool ResourceEligible131(unsigned slot,unsigned physical,unsigned local,int status,bool observer,bool ally){return slot<24&&physical<24&&(status==1||status==2)&&(slot==local||observer||ally);}
inline bool ResourceRaw131(uintptr_t base,uintptr_t world,const ResourceProfile131& p,const ResourceNative131& api,unsigned slot,unsigned local,ResourceRow& row,char* name){
 __try{auto player=api.player(world,slot);if(!ResourceIdentity131(player,base,p.playerTable))return false;
  auto physical=*reinterpret_cast<uint8_t*>(player+p.idOffset);if(physical>=24)return false;
  auto status=*reinterpret_cast<int*>(player+p.statusOffset);bool observer=api.integer(player+p.stateBase+11*p.stateStride)!=0,ally=false;
  if(slot!=local&&!observer){auto own=api.player(world,local);if(!ResourceIdentity131(own,base,p.playerTable))return false;
   auto alliances=*reinterpret_cast<uintptr_t*>(own+p.allianceOffset);if(ResourceIdentity131(alliances,base,p.allianceTable))ally=(uint32_t(api.integer(alliances+p.allianceMask))&(1u<<physical))!=0;}
  if(!ResourceEligible131(slot,physical,local,status,observer,ally))return false;
  row.slot=slot;row.left=status==2;row.color=*reinterpret_cast<unsigned*>(player+p.colorOffset);
  row.gold=api.integer(player+p.stateBase+p.stateStride)/10;row.lumber=api.integer(player+p.stateBase+2*p.stateStride)/10;
  row.food=api.integer(player+p.stateBase+5*p.stateStride);row.cap=(std::min)(api.integer(player+p.stateBase+4*p.stateStride),api.integer(player+p.stateBase+6*p.stateStride));
  auto text=api.string(player+p.nameOffset);if(text){unsigned n=0;for(;n<256&&text[n];++n)name[n]=text[n];if(n==256)return false;}
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline ResourceTable ReadResources131(uintptr_t base,unsigned build){
 ResourceTable table;auto p=ResourceProfileFor131(build);if(!p||!ResourceCode131(base,*p))return table;
 unsigned local=24;auto world=ResourceWorld131(base,*p,local);if(!world)return table;
 ResourceNative131 api{reinterpret_cast<decltype(api.player)>(base+p->playerFn),reinterpret_cast<decltype(api.integer)>(base+p->integerFn),reinterpret_cast<decltype(api.string)>(base+p->stringFn)};
 table.ready=true;for(unsigned slot=0;slot<24;++slot){ResourceRow row;char name[257]{};if(!ResourceRaw131(base,world,*p,api,slot,local,row,name))continue;
  row.name=ResourceName(Wide(name));if(row.name.empty())row.name=L"Player "+std::to_wstring(slot+1);table.rows.push_back(std::move(row));}
 return table;
}
}
