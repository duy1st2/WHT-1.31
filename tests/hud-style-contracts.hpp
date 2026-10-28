#pragma once
#include "../native/hud_style.hpp"
#include <cmath>
template<class Check> void TestHudStyle(Check check){
 using namespace wht;
 check(HudStyle::timerY==.568f&&HudStyle::memoryY==.551f&&HudStyle::fpsY==.535f&&HudStyle::pingLeft==.428f,"HUD coordinates match 1.29 reference");
 check(HudStyle::Center(false)==.34f&&HudStyle::Center(true)==.4f,"HUD follows time-of-day visibility");
 struct Paint {float size=0,width=0,height=0,x[9]{},y[9]{};unsigned count=0;int align=0;} paint;
 auto font=+[](void* f,const char*,float size,int)->unsigned{static_cast<Paint*>(f)->size=size;return 1;};
 auto width=+[](void* f,float value){static_cast<Paint*>(f)->width=value;};
 auto height=+[](void* f,float value){static_cast<Paint*>(f)->height=value;};
 auto point=+[](void* f,int index,float x,float y,int){auto& p=*static_cast<Paint*>(f);p.x[index]=x;p.y[index]=y;++p.count;};
 auto align=+[](void* f,int value){static_cast<Paint*>(f)->align=value;};
 struct Api {decltype(font) setFont;decltype(width) setWidth;decltype(height) setHeight;decltype(point) setAbsPoint;decltype(align) setTextAlign;} api{font,width,height,point,align};
 check(StyleNativeFps(&paint,api)&&paint.count==9&&paint.size==.011f&&paint.align==2,"FPS style sets font and all anchors through native API");
 check(std::abs(paint.x[4]-.4f)<.00001f&&std::abs(paint.y[4]-.535f)<.00001f&&paint.width==.12f&&paint.height==.016f,"FPS rectangle is centered below memory");
 check(!StyleNativeFps(nullptr,api),"missing FPS font is ignored");
 char text[96]{};check(std::string(StyledFpsText(" FPS: 60",text,sizeof(text)))=="|cffdcda3c FPS: 60|r","FPS color matches reference");
 const char* other="Damage: 60";check(StyledFpsText(other,text,sizeof(text))==other,"unrelated font strings retain native colors");
}
