#pragma once
#include "map_trade_profile131.hpp"
#include "resource131.hpp"
#include "unit_info131.hpp"
namespace wht {
inline constexpr uint32_t mapTradeRestriction131=0x100;
inline const MapTradeProfile131* MapTradeProfileFor131(unsigned build){for(const auto& p:mapTradeProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool MapTradeCode131(uintptr_t base,const MapTradeProfile131& p){ResourceTextProfile131 code{p.build,0,0,0,0,0,p.code,p.count};return ResourceTextGuard131(base,code);}
struct MapTradeSource131 {uintptr_t world=0,setup=0;unsigned local=24;uint32_t flags=0;};
inline bool SameMapTrade131(const MapTradeSource131& a,const MapTradeSource131& b){return a.world&&a.setup&&a.local<24&&a.world==b.world&&a.setup==b.setup&&a.local==b.local;}
inline bool ReadMapTradeFields131(uintptr_t base,const MapTradeProfile131& p,uintptr_t world,uintptr_t& setup,uint32_t& flags){
 if(!world)return false;__try{setup=*reinterpret_cast<uintptr_t*>(world+p.setupOffset);if(!ResourceIdentity131(setup,base,p.setupTable)||((setup+p.flagsOffset)&3))return false;flags=*reinterpret_cast<uint32_t*>(setup+p.flagsOffset);return true;}__except(1){return false;}
}
inline bool ReadMapTrade131(uintptr_t base,unsigned build,MapTradeSource131& out){
 const auto* p=MapTradeProfileFor131(build);const auto* rp=ResourceProfileFor131(build);if(!p||!rp||!MapTradeCode131(base,*p)||!ResourceCode131(base,*rp))return false;
 MapTradeSource131 value;value.world=ResourceWorld131(base,*rp,value.local);if(!value.world||!ReadMapTradeFields131(base,*p,value.world,value.setup,value.flags))return false;
 unsigned local=24;uintptr_t setup=0;uint32_t flags=0;if(ResourceWorld131(base,*rp,local)!=value.world||local!=value.local||!ReadMapTradeFields131(base,*p,value.world,setup,flags)||setup!=value.setup||flags!=value.flags)return false;out=value;return true;
}
// Native SetMapFlag(false) performs flags &= ~mask, without other side effects.
// Compare/exchange preserves unrelated flags and refuses a changed source word.
inline bool ClearMapTradeField131(uintptr_t base,const MapTradeProfile131& p,uintptr_t setup,uint32_t expected){
 __try{if(!ResourceIdentity131(setup,base,p.setupTable)||((setup+p.flagsOffset)&3))return false;const auto desired=expected&~mapTradeRestriction131;return uint32_t(InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(setup+p.flagsOffset),LONG(desired),LONG(expected)))==expected;}__except(1){return false;}
}
inline bool ClearMapTradeNative131(uintptr_t base,unsigned build,const MapTradeSource131& source){
 const auto* p=MapTradeProfileFor131(build);MapTradeSource131 current;if(!p||!ReadMapTrade131(base,build,current)||!SameMapTrade131(source,current)||current.flags!=source.flags)return false;return ClearMapTradeField131(base,*p,current.setup,current.flags);
}
template<class Read,class Clear> bool MapTradeCommand131(const std::wstring& args,Read read,Clear clear,std::string& report){
 if(!UnitInfoArgs131(args)){report="Usage: #mtrade";return false;}
 MapTradeSource131 before,current,after;if(!read(before)||!read(current)||!SameMapTrade131(before,current)||before.flags!=current.flags){report="Trade map flag unavailable or changed; no action taken";return false;}
 if(!(current.flags&mapTradeRestriction131)){report="Trade restriction already disabled";return true;}
 if(!clear(current)){report="Trade restriction update refused: map or flags changed";return false;}
 if(!read(after)||!SameMapTrade131(current,after)||after.flags!=(current.flags&~mapTradeRestriction131)){report="Trade restriction update not confirmed; flag may have changed";return false;}
 report="Trade restriction disabled";return true;
}
}
