#pragma once
#include "component_lookup131.hpp"
#include <unordered_map>
namespace wht {
inline bool StatPaintCode131(uintptr_t base,uintptr_t table){
#ifdef _WIN64
 const uintptr_t profiles[][4]={{0x1c14f08,0xfbb160,0xcf91d0,0xcf94d0},{0x1c43dc8,0xff7cd0,0xd37dd0,0xd380d0}};
 const unsigned char bind[]={0x40,0x55,0x53,0x57,0x48,0x8b,0xec,0x48,0x83,0xec,0x60,0x48},update[]={0x40,0x57,0x48,0x83,0xec,0x40,0x48,0xc7,0x44,0x24,0x20,0xfe};
#else
 const uintptr_t profiles[][4]={{0x182e5fc,0xdfff60,0xb9c790,0xb9ca70},{0x17e34dc,0xdbed60,0xb5bfd0,0xb5c2b0}};
 const unsigned char bind[]={0x55,0x8b,0xec,0x83,0xec,0x1c,0x56,0x57,0x8b,0x7d,8,0x8b},update[]={0x55,0x8b,0xec,0x6a,0xff};
#endif
 __try{for(const auto& p:profiles)if(table==p[0])return base&&*reinterpret_cast<uintptr_t*>(base+table+27*sizeof(uintptr_t))==base+p[1]&&*reinterpret_cast<uintptr_t*>(base+table+30*sizeof(uintptr_t))==base+p[2]&&*reinterpret_cast<uintptr_t*>(base+table+31*sizeof(uintptr_t))==base+p[3]&&!memcmp(reinterpret_cast<void*>(base+p[2]),bind,sizeof(bind))&&!memcmp(reinterpret_cast<void*>(base+p[3]),update,sizeof(update));}__except(1){}return false;
}
struct StatPaintRecord131 {bool hide=false,color=false;uintptr_t unit=0,unitTable=0,texture=0,textureTable=0,tint=0,hideFn=0;uint32_t desired=0;};
inline std::unordered_map<uintptr_t,StatPaintRecord131> statPaintRecords131;
inline std::array<uintptr_t,34> statPaintSlots131{};
inline uintptr_t statPaintShow131=0,statPaintBind131=0,statPaintUpdate131=0;inline DWORD statPaintThread131=0;
inline bool StatPaintHidden131(const StatPaintRecord131& r){__try{return r.hide&&r.unit&&*reinterpret_cast<uintptr_t*>(r.unit)==r.unitTable&&(*reinterpret_cast<unsigned*>(r.unit+(sizeof(void*)==8?0x38:0x20))&8);}__except(1){return false;}}
inline bool StatPaintHiddenAt131(uintptr_t frame,const StatPaintRecord131& r){__try{return *reinterpret_cast<uintptr_t*>(frame+(sizeof(void*)==8?0x210:0x154))==r.unit&&StatPaintHidden131(r);}__except(1){return false;}}
inline void ApplyStatPaint131(uintptr_t frame){
 if(GetCurrentThreadId()!=statPaintThread131)return;auto found=statPaintRecords131.find(frame);if(found==statPaintRecords131.end())return;const auto& r=found->second;
 __try{
  if(*reinterpret_cast<uintptr_t*>(frame+(sizeof(void*)==8?0x210:0x154))!=r.unit||!r.unit||*reinterpret_cast<uintptr_t*>(r.unit)!=r.unitTable)return;
  if(StatPaintHidden131(r)&&r.hideFn)reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(r.hideFn)(frame);
  if(!r.color||!r.texture||*reinterpret_cast<uintptr_t*>(frame+(sizeof(void*)==8?0x1e0:0x130))!=r.texture||*reinterpret_cast<uintptr_t*>(r.texture)!=r.textureTable)return;
  auto bytes=reinterpret_cast<const unsigned char*>(r.texture+(sizeof(void*)==8?0xb8:0x74));const uint32_t color=(uint32_t(bytes[0])<<24)|(uint32_t(bytes[3])<<16)|(uint32_t(bytes[2])<<8)|bytes[1];
  if(color!=r.desired&&r.tint)reinterpret_cast<void(__thiscall*)(uintptr_t,const uint32_t*)>(r.tint)(r.texture,&r.desired);
 }__except(1){}
}
#ifdef _WIN64
inline uintptr_t StatPaintShowCall131(uintptr_t frame){
#else
inline uintptr_t __fastcall StatPaintShowCall131(uintptr_t frame,void*){
#endif
 if(GetCurrentThreadId()==statPaintThread131){auto found=statPaintRecords131.find(frame);if(found!=statPaintRecords131.end()&&StatPaintHiddenAt131(frame,found->second))return 1;}
 const auto result=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(statPaintShow131)(frame);ApplyStatPaint131(frame);return result;
}
#ifdef _WIN64
inline uintptr_t StatPaintBindCall131(uintptr_t frame,uintptr_t unit){
#else
inline uintptr_t __fastcall StatPaintBindCall131(uintptr_t frame,void*,uintptr_t unit){
#endif
 const auto result=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,uintptr_t)>(statPaintBind131)(frame,unit);ApplyStatPaint131(frame);return result;
}
#ifdef _WIN64
inline uintptr_t StatPaintUpdateCall131(uintptr_t frame,uintptr_t unit){
#else
inline uintptr_t __fastcall StatPaintUpdateCall131(uintptr_t frame,void*,uintptr_t unit){
#endif
 const auto result=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,uintptr_t)>(statPaintUpdate131)(frame,unit);ApplyStatPaint131(frame);return result;
}
inline bool SetStatPaint131(uintptr_t base,uintptr_t table,uintptr_t frame,bool colorMode,const StatPaintRecord131& desired,bool enabled){
 if(!ComponentIdentity131(frame,base+table))return false;
 __try{
  if(!statPaintOriginalTable131){if(!StatPaintCode131(base,table))return false;memcpy(statPaintSlots131.data(),reinterpret_cast<void*>(base+table-sizeof(uintptr_t)),sizeof(statPaintSlots131));statPaintOriginalTable131=base+table;statPaintShow131=statPaintSlots131[28];statPaintBind131=statPaintSlots131[31];statPaintUpdate131=statPaintSlots131[32];statPaintThread131=GetCurrentThreadId();statPaintSlots131[28]=reinterpret_cast<uintptr_t>(StatPaintShowCall131);statPaintSlots131[31]=reinterpret_cast<uintptr_t>(StatPaintBindCall131);statPaintSlots131[32]=reinterpret_cast<uintptr_t>(StatPaintUpdateCall131);statPaintProxyTable131=reinterpret_cast<uintptr_t>(statPaintSlots131.data()+1);}
  if(statPaintOriginalTable131!=base+table)return false;auto found=statPaintRecords131.find(frame);
  if(found==statPaintRecords131.end()){if(!enabled)return true;if(statPaintRecords131.size()>=4096)return false;found=statPaintRecords131.emplace(frame,StatPaintRecord131{}).first;}
  auto& r=found->second;if(colorMode){r.color=enabled;r.texture=desired.texture;r.textureTable=desired.textureTable;r.tint=desired.tint;r.desired=desired.desired;}else{r.hide=enabled;r.hideFn=desired.hideFn;}r.unit=desired.unit;r.unitTable=desired.unitTable;
  if(r.color||r.hide){statPaintOwners131.insert(frame);*reinterpret_cast<uintptr_t*>(frame)=statPaintProxyTable131;}
  else{if(*reinterpret_cast<uintptr_t*>(frame)==statPaintProxyTable131)*reinterpret_cast<uintptr_t*>(frame)=statPaintOriginalTable131;statPaintRecords131.erase(found);statPaintOwners131.erase(frame);}return true;
 }__except(1){return false;}
}
inline void PruneStatPaint131(const std::unordered_set<uintptr_t>& frames){for(auto i=statPaintRecords131.begin();i!=statPaintRecords131.end();)if(!frames.count(i->first)){statPaintOwners131.erase(i->first);i=statPaintRecords131.erase(i);}else ++i;}
inline void ForgetStatPaint131(){statPaintRecords131.clear();statPaintOwners131.clear();}
}
