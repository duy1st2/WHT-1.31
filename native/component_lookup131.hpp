#pragma once
#include "common.hpp"
#include <unordered_set>
#include <algorithm>
#include <unordered_set>
#include <algorithm>
namespace wht {
inline uintptr_t statPaintOriginalTable131=0,statPaintProxyTable131=0;inline std::unordered_set<uintptr_t> statPaintOwners131;
inline uintptr_t uiRefreshOwner131=0,uiRefreshOriginalTable131=0,uiRefreshProxyTable131=0;
inline bool NativeClass131(uintptr_t object,uintptr_t actual,uintptr_t expected){return actual==expected||(object==uiRefreshOwner131&&actual==uiRefreshProxyTable131&&expected==uiRefreshOriginalTable131)||(actual==statPaintProxyTable131&&expected==statPaintOriginalTable131&&statPaintOwners131.count(object));}

inline bool ReadableComponent131(uintptr_t address,size_t size){
 if(address<0x10000||size>~uintptr_t(0)-address)return false;
 MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(address),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
 const auto protection=m.Protect&0xff;
 if(protection!=PAGE_READONLY&&protection!=PAGE_READWRITE&&protection!=PAGE_WRITECOPY&&protection!=PAGE_EXECUTE_READ&&protection!=PAGE_EXECUTE_READWRITE&&protection!=PAGE_EXECUTE_WRITECOPY)return false;
 return address+size<=reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize;
}
inline bool ComponentIdentity131(uintptr_t object,uintptr_t table){
 if(object%alignof(uintptr_t)||!ReadableComponent131(object,sizeof(uintptr_t)))return false;
 __try{const auto actual=*reinterpret_cast<uintptr_t*>(object);return NativeClass131(object,actual,table);}__except(1){return false;}
}
struct ComponentLookup131 {
 struct Entry {uintptr_t table=0;unsigned offset=~0u;uint64_t due=0;bool last=false;};
 uintptr_t owner=0,base=0;std::array<Entry,32> entries{};
 uintptr_t Find(uintptr_t ui,uintptr_t image,uintptr_t table,unsigned limit,uint64_t now,bool last=false){
  if(owner!=ui||base!=image){owner=ui;base=image;entries={};}
  if(!ui||!image||!table)return 0;
  Entry* entry=nullptr;for(auto& e:entries)if(e.table==table&&e.last==last){entry=&e;break;}
  if(!entry)for(auto& e:entries)if(!e.table){entry=&e;e.table=table;e.last=last;break;}
  if(!entry)return 0;
  if(entry->offset!=~0u){uintptr_t object=Slot(ui,entry->offset);if(ComponentIdentity131(object,image+table))return object;entry->offset=~0u;entry->due=0;}
  if(now<entry->due)return 0;entry->due=now+250;
  if(!ReadableComponent131(ui,limit))return 0;
  for(unsigned off=sizeof(uintptr_t);off+sizeof(uintptr_t)<=limit;off+=sizeof(uintptr_t)){
   const auto object=Slot(ui,off);if(ComponentIdentity131(object,image+table)){entry->offset=off;if(!last)return object;}
  }return entry->offset!=~0u?Slot(ui,entry->offset):0;
 }
 static uintptr_t Slot(uintptr_t ui,unsigned offset){__try{return *reinterpret_cast<uintptr_t*>(ui+offset);}__except(1){return 0;}}
};
inline unsigned NativeUiSpan131(uintptr_t ui,uintptr_t base,unsigned maximum){
 if(!ui||!base)return maximum;const auto table=ComponentLookup131::Slot(ui,0);
#ifdef _WIN64
 return NativeClass131(ui,table,base+0x1c00ec0)?(std::min)(maximum,0x7d8u):maximum;
#else
 return NativeClass131(ui,table,base+0x18240a0)?(std::min)(maximum,0x4b0u):maximum;
#endif
}
inline uintptr_t FindComponent131(uintptr_t ui,uintptr_t base,uintptr_t table,unsigned limit){thread_local ComponentLookup131 cache;return cache.Find(ui,base,table,NativeUiSpan131(ui,base,limit),GetTickCount64());}
inline uintptr_t ConsoleComponent131(uintptr_t ui,uintptr_t base,uintptr_t table){thread_local ComponentLookup131 cache;return cache.Find(ui,base,table,NativeUiSpan131(ui,base,sizeof(void*)==8?0x7e8:0x4d4),GetTickCount64(),true);}

}
