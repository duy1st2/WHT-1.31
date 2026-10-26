#pragma once
#include "button_animation131.hpp"
inline void CheckButtonAnimation131(){
 wht::ButtonPressAnimation131 a;a.Begin(100);
 check(a.Amount(100)==1&&a.Amount(190)==.25f&&a.Amount(280)==0&&!a.active,"icon presses ease back over reference 180ms");
 a.Begin(500,140);check(a.Down(639)&&!a.Down(640),"MTH visual remains pressed for reference 140ms");
 a.Begin(1000);a.Reset();check(a.Amount(1001)==0,"hiding or resetting cancels animation");
 check(wht::IconPressColor131(0,false)==0xffffffff&&wht::IconPressColor131(1,false)==0xff999999,"icon brightness matches reference 40 percent press dim");
 check(wht::IconPressColor131(0,true)==0xff44ff3d,"active Auto Pick keeps reference green tint");
 wht::DropIcon131 icon;icon.shown=true;unsigned calls=0;const auto area=icon.area;
 check(icon.Mouse(WM_LBUTTONDOWN,{691,786},1000,900,true,[&](){++calls;}),"animated icon accepts original hit rectangle");
 const auto started=icon.animation.started;check(icon.Mouse(WM_LBUTTONUP,{0,0},1000,900,true,[&](){++calls;})&&icon.animation.Down(started+90)&&calls==1,"release outside consumes click without cancelling return animation or repeating action");
 check(icon.area.right==area.right&&icon.area.width==area.width,"press animation keeps logical hitbox fixed");
 wht::GokuIcon131 mth;check(mth.pressMs==140,"all MTH buttons inherit reference press duration");
}
