#pragma once
#include "common.hpp"
namespace wht {
// CUnit::UpdateColor (26DCA0) consumes packed ARGB at +4E0.
// Match the legacy tool's scoped alpha override; retain no unit between calls.
struct ScopedVertexAlpha129 {
 uintptr_t object=0,table=0;BYTE saved=0;
 ScopedVertexAlpha129(uintptr_t base,uintptr_t unit,bool enabled){
  if(!enabled||!base||!unit)return;
  __try{
   if(*reinterpret_cast<uintptr_t*>(unit)!=base+0xa9c504)return;
   auto& alpha=*reinterpret_cast<BYTE*>(unit+0x4e3);
   if(alpha==255)return;
   table=base+0xa9c504;saved=alpha;object=unit;alpha=255;
  }__except(EXCEPTION_EXECUTE_HANDLER){object=0;}
 }
 ~ScopedVertexAlpha129(){
  __try{
   if(object&&*reinterpret_cast<uintptr_t*>(object)==table){
    auto& alpha=*reinterpret_cast<BYTE*>(object+0x4e3);
    if(alpha==255)alpha=saved;
   }
  }__except(EXCEPTION_EXECUTE_HANDLER){}
 }
};
}
