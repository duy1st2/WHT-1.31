#pragma once
#include "quick_drop131.hpp"
#include "drop_icon131.hpp"
#include "mth_presentation131.hpp"
namespace wht {
inline constexpr uint32_t gokuType131=0x49303136; // I016, same reference map item.
struct ItemCodeProfile131 {unsigned build;uintptr_t constructor;};
#ifdef _WIN64
inline constexpr ItemCodeProfile131 itemCodeProfiles131[]={{12164,0xd39b70},{12173,0xd78510}};
#else
inline constexpr ItemCodeProfile131 itemCodeProfiles131[]={{12164,0xbd31f0},{12173,0xb92de0}};
#endif
inline bool ItemCodeGuard131(uintptr_t base,const InventoryProfile131& inventory){
 if(!base)return false;const ItemCodeProfile131* p=nullptr;for(const auto& candidate:itemCodeProfiles131)if(candidate.build==inventory.build)p=&candidate;if(!p)return false;
 __try{
#ifdef _WIN64
  const unsigned char prefix[]={0x48,0x8b,0xc4,0x48,0x89,0x58,8,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x55,0x41,0x56,0x41,0x57,0x48,0x8d,0x68,0xa8,0x48,0x81,0xec,0x40,1,0,0,0x0f,0x29,0x70,0xd8,0x49,0x8b,0xf0,0x0f,0x29,0x78,0xc8,0x48,0x8b,0xf9,0x89,0x51,0x58};
#else
  const unsigned char prefix[]={0x55,0x8b,0xec,0x8b,0x45,8,0x83,0xec,0x6c,0x53,0x56,0x57,0x8b,0xd9,0x53,0x6a,0,0x50,0x89,0x43,0x34};
#endif
  return *reinterpret_cast<uintptr_t*>(base+inventory.itemTable+37*sizeof(uintptr_t))==base+p->constructor&&!memcmp(reinterpret_cast<void*>(base+p->constructor),prefix,sizeof(prefix));
 }__except(1){return false;}
}
inline uint32_t ItemCodeValue131(uintptr_t base,const InventoryProfile131& p,uintptr_t item){
 __try{return InventoryIdentity131(item,base,p.itemTable)?*reinterpret_cast<uint32_t*>(item+(sizeof(uintptr_t)==8?0x58:0x34)):0;}__except(1){return 0;}
}
inline bool InventoryHasType131(uintptr_t base,const InventoryProfile131& p,const InventoryApi131& api,uintptr_t unit,unsigned count,uint32_t type){
 if(!type||!api||!count||count>6||!InventoryIdentity131(unit,base,p.unitTable))return false;
 __try{for(unsigned slot=0;slot<count;++slot)if(ItemCodeValue131(base,p,api.item(unit,slot))==type)return true;}__except(1){return false;}return false;
}
inline bool MthResolvedRect131(uintptr_t frame,float& left,float& bottom,float& right,float& top){
 __try{if(!frame)return false;const auto bounds=reinterpret_cast<const float*>(frame+(sizeof(uintptr_t)==8?0x88:0x44));bottom=bounds[0];left=bounds[1];top=bounds[2];right=bounds[3];return std::isfinite(left)&&std::isfinite(bottom)&&std::isfinite(right)&&std::isfinite(top)&&right>left&&top>bottom&&right-left<.2f&&top-bottom<.1f;}__except(1){return false;}
}
struct GokuIcon131:DropIcon131 {
 MthPresentation131 presentation;
 void* label=nullptr;const char* title="Goku";std::string appliedTitle;ToolMenuRect appliedArea{};
 GokuIcon131(){area={.305f,.142f,.038f,.024f};}
 void Reset(){*this=GokuIcon131{};}
 void Update(uintptr_t base,bool enabled,ToolMenuOverlay& menu,const NativeFrameApi* raw=nullptr){
  if(!menu.initialized)return;
  if(enabled&&!frame){frame=menu.Frame(base,menu.independentHost,area);if(frame)NativeSetFrameBackdropEx(menu.api,frame,"UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp","UI\\Widgets\\EscMenu\\Human\\human-options-button-border-up.blp",.0125f,.0045f);}
  if(frame&&!label){label=menu.Font(frame,.010f,title);if(label){menu.api.setWidth(label,area.width);menu.api.setHeight(label,area.height);menu.api.setTextAlign(label,2);}}
  shown=enabled&&frame&&label;
  if(frame){if(shown){
   if(appliedArea.right!=area.right||appliedArea.bottom!=area.bottom||appliedArea.width!=area.width||appliedArea.height!=area.height){menu.api.setWidth(frame,area.width);menu.api.setHeight(frame,area.height);menu.api.setAbsPoint(frame,0,area.right-area.width,area.bottom+area.height,1);appliedArea=area;}
   const auto text=std::string(latched?"|cff00ff00":"|cffffcc00")+title+"|r";const bool textChanged=text!=appliedTitle;if(textChanged){menu.api.setText(label,text.c_str());appliedTitle=text;}
   presentation.Visibility(true,[&](){NativeFrameShow(frame);},[&](){NativeFrameHide(frame);});
   float left=0,bottom=0,right=0,top=0;if(raw&&MthResolvedRect131(reinterpret_cast<uintptr_t>(frame),left,bottom,right,top))presentation.Layout(left,bottom,right,top,textChanged,[&](){raw->setWidth(label,right-left);raw->setHeight(label,top-bottom);raw->setAbsPoint(label,4,(left+right)*.5f,(bottom+top)*.5f,1);});
  }else presentation.Visibility(false,[&](){NativeFrameShow(frame);},[&](){NativeFrameHide(frame);});}
 }
};
}
