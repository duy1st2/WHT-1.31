#pragma once
#include "component_lookup131.hpp"
#include "overlay_frames.hpp"
namespace wht {
// Native overhead frames are pooled and detached between draw passes.
// Attached visibility is not feature policy: the native parent clips children.
inline bool OverheadEligible131(bool allowed,bool hideInvulnerable,unsigned unitFlags){return allowed&&!(hideInvulnerable&&(unitFlags&8));}
inline unsigned OverheadUnitFlags131(uintptr_t unit){__try{return *reinterpret_cast<unsigned*>(unit+(sizeof(void*)==8?0x38:0x20));}__except(1){return 0;}}
// Keep requested child visibility without repeatedly re-registering it.
// The engine's parent Show/Hide performs normal clipping and attachment.
inline void MaintainWorldHost131(void* child,uintptr_t parent){
 __try{auto flags=reinterpret_cast<const unsigned char*>(reinterpret_cast<uintptr_t>(child)+(sizeof(void*)==8?0xf0:0x9c));
  if(!flags[0]||(!flags[1]&&*reinterpret_cast<const unsigned char*>(parent+(sizeof(void*)==8?0xf1:0x9d))))NativeFrameShow(child);
 }__except(1){}
}
inline void HideWorldHost131(void* child){
 __try{auto flags=reinterpret_cast<const unsigned char*>(reinterpret_cast<uintptr_t>(child)+(sizeof(void*)==8?0xf0:0x9c));if(flags[0]||flags[1])NativeFrameHide(child);}__except(1){}
}
}
