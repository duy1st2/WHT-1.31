#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
namespace wht {
struct DropIcon131 {
 static constexpr ToolMenuRect rect{.558f,.068f,.012f,.016f};
 ToolMenuRect area=rect;const char* upAsset="BTNDropItem.blp";const char* downAsset="BTNDropItemDow.blp";
 void* frame=nullptr;void* texture=nullptr;bool shown=false,pressed=false,consumeUp=false,latched=false;
 void Reset(){*this={};}
 void Update(uintptr_t base,bool enabled,ToolMenuOverlay& menu){
  if(!menu.initialized)return;
  if(enabled&&!frame){frame=menu.Frame(base,menu.independentHost,area);if(frame){texture=menu.Texture(frame,upAsset,area.width,area.height);if(texture)menu.api.setRelPoint(texture,4,frame,4,0,0,1);}}
  shown=enabled&&frame&&texture;
  if(!shown){if(frame)NativeFrameHide(frame);pressed=false;return;}
  menu.api.setTexture(texture,(pressed||latched)?downAsset:upAsset,0);NativeFrameShow(frame);
 }
 template<class Action> bool Mouse(unsigned message,POINT point,int width,int height,bool allowed,Action action){
  if(message==WM_LBUTTONUP){const bool consumed=consumeUp;consumeUp=false;pressed=false;return consumed;}
  if(message!=WM_LBUTTONDOWN&&message!=WM_LBUTTONDBLCLK)return false;
  if(!shown||!allowed||!area.Hit(point,width,height))return false;
  // A double-click must not repeat an already executed batch.
  if(message==WM_LBUTTONDOWN){pressed=true;consumeUp=true;action();}
  else consumeUp=true;
  return true;
 }
};
}
