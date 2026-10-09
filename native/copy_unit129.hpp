#pragma once
#include "create_unit129.hpp"
#include "teleport129.hpp"
namespace wht {
inline bool ParseCopyUnit129(const std::wstring& args,unsigned& count){
 uint32_t ignored=0;
 return ParseCreateUnit129(L"hfoo "+args,ignored,count);
}
inline bool ReadCopySource129(uintptr_t base,const SkillSelection129& selected,const TeleportApi129& api,uint32_t& code,TeleportPosition129& position,const char** error=nullptr){
 code=0;position={};if(error)*error=nullptr;
 __try {
  if(!LiveTeleportUnit129(base,selected,api))return TeleportFailure129(error,"Select a live unit first");
  const auto type=*reinterpret_cast<uint32_t*>(selected.unit+0x34);
  if(!type)return TeleportFailure129(error,"Unit type unavailable");
  float xyz[3]={NAN,NAN,NAN};api.position(selected.unit+0x26c,xyz);
  if(!LiveTeleportUnit129(base,selected,api))return TeleportFailure129(error,"Selected unit changed");
  if(!std::isfinite(xyz[0])||!std::isfinite(xyz[1])||std::abs(xyz[0])>99950||std::abs(xyz[1])>100000)return TeleportFailure129(error,"Unit position unavailable");
  code=type;position={xyz[0],xyz[1]};return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return TeleportFailure129(error,"Cannot read selected unit");}
}
inline unsigned CopyUnits129(uintptr_t base,const SkillSelection129& selected,const TeleportApi129& sourceApi,const CreateUnitApi129& createApi,unsigned count,const char** error=nullptr){
 if(error)*error=nullptr;
 if(count<1||count>50){TeleportFailure129(error,"Count must be 1..50");return 0;}
 if(!createApi){TeleportFailure129(error,"Create function signature mismatch");return 0;}
 uint32_t code=0;TeleportPosition129 source;
 if(!ReadCopySource129(base,selected,sourceApi,code,source,error))return 0;
 // Legacy #copy creates fresh units for the local player beside the source.
 const auto countCreated=CreateUnits129(base,createApi,code,count,source.x+50.f,source.y,50.f);
 if(!countCreated)TeleportFailure129(error,"Game did not create a unit");
 return countCreated;
}
}
