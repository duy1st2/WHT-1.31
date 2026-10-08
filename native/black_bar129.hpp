#pragma once
#include "mouse_tooltip129.hpp"
namespace wht {
struct BlackBar129 {
 uintptr_t ui=0,layout=0;bool active=false;
 std::array<TooltipAnchor129,2> original{},written{};
 void Reset(){*this={};}
 template<class SetPoint> void Apply(uintptr_t base,uintptr_t owner,bool enabled,SetPoint setPoint){
  const auto viewport=owner?*reinterpret_cast<uintptr_t*>(owner+0x3fc):0;
  const auto currentLayout=viewport?viewport+0xbc:0;
  if(ui!=owner||layout!=currentLayout){Reset();ui=owner;layout=currentLayout;}
  if(!layout)return;
  const auto root=ui+0xbc;
  constexpr int points[]={6,2};std::array<TooltipAnchor129,2> current{};
  // Stock scene anchors at 1E7B35..1E7BA0 target the game UI layout.
  for(int i=0;i<2;++i)if(!ReadTooltipAnchor129(base,layout,root,0,current[i],points[i]))return;
  if(!enabled){
   if(active)for(int i=0;i<2;++i)if(current[i]==written[i]&&!(current[i]==original[i]))setPoint(layout,points[i],original[i]);
   active=false;return;
  }
  const std::array<TooltipAnchor129,2> desired{{{root,6,0,0},{root,2,0,-.02f}}};
  for(int i=0;i<2;++i){
   if(!active||!(current[i]==written[i]))original[i]=current[i];
   if(!(current[i]==desired[i]))setPoint(layout,points[i],desired[i]);
   written[i]=desired[i];
  }
  active=true;
 }
};
}
