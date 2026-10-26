#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
#include "button_animation131.hpp"
namespace wht {
struct DropIcon131 {
 static constexpr ToolMenuRect rect{.558f,.068f,.012f,.016f};
 ToolMenuRect area=rect;const char* upAsset="BTNDropItem.blp";const char* downAsset="BTNDropItemDow.blp";
 void* frame=nullptr;void* texture=nullptr;bool shown=false,pressed=false,consumeUp=false,latched=false;
 ButtonPressAnimation131 animation;unsigned pressMs=180;bool greenWhenLatched=false;float appliedAmount=-1;uint32_t appliedColor=0;const char* appliedAsset=nullptr;
 void Reset(){*this={};}
 void Update(uintptr_t base,bool enabled,ToolMenuOverlay& menu){
  if(!menu.initialized)return;
  if(enabled&&!frame){frame=menu.Frame(base,menu.independentHost,area);if(frame){texture=menu.Texture(frame,upAsset,area.width,area.height);if(texture)menu.api.setRelPoint(texture,4,frame,4,0,0,1);}}
  shown=enabled&&frame&&texture;
  if(!shown){if(frame)NativeFrameHide(frame);pressed=false;animation.Reset();return;}
  const auto now=GetTickCount64();const float amount=animation.Amount(now);
  const auto asset=animation.active?downAsset:upAsset;if(appliedAsset!=asset){menu.api.setTexture(texture,asset,0);appliedAsset=asset;}
  if(appliedAmount!=amount){const float inset=.001f*amount;menu.api.setWidth(texture,area.width-2*inset);menu.api.setHeight(texture,area.height-2*inset);menu.api.setRelPoint(texture,4,frame,4,.0004f*amount,-.0004f*amount,1);appliedAmount=amount;}
  const auto color=IconPressColor131(amount,greenWhenLatched&&latched);if(menu.tint&&appliedColor!=color){menu.tint(texture,&color);appliedColor=color;}NativeFrameShow(frame);
 }
 template<class Action> bool Mouse(unsigned message,POINT point,int width,int height,bool allowed,Action action){
  if(message==WM_LBUTTONUP){const bool consumed=consumeUp;consumeUp=false;pressed=false;return consumed;}
  if(message!=WM_LBUTTONDOWN&&message!=WM_LBUTTONDBLCLK)return false;
  if(!shown||!allowed||!area.Hit(point,width,height))return false;
  // A double-click must not repeat an already executed batch.
  if(message==WM_LBUTTONDOWN){pressed=true;animation.Begin(GetTickCount64(),pressMs);consumeUp=true;action();}
  else consumeUp=true;
  return true;
 }
};
}
