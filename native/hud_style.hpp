#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
namespace wht {
// Appearance reference: WHT-1.29/native/hud_layout129.hpp (2026-10-27).
struct HudStyle {
 static constexpr float timerX=.34f,timerY=.568f,memoryY=.551f,fpsX=.4f,fpsY=.535f,pingLeft=.428f,pingBottom=.548f;
 static constexpr float timerSize=.016f,memorySize=.015f,fpsSize=.011f,pingSize=.012f;
 static float Center(bool hide){return hide?.4f:timerX;}
};
template<class Api> bool StyleNativeFps(void* font,Api& api){
 if(!font||!api.setFont||!api.setWidth||!api.setHeight||!api.setAbsPoint||!api.setTextAlign)return false;
 if(!api.setFont(font,"fonts_VN.ttf",HudStyle::fpsSize,1)&&!api.setFont(font,"FRIZQT__.ttf",HudStyle::fpsSize,1))return false;
 api.setWidth(font,.12f);api.setHeight(font,.016f);api.setTextAlign(font,2);
 // All anchors describe the same rectangle, so existing native points cannot stretch it.
 for(int i=0;i<9;++i)api.setAbsPoint(font,i,HudStyle::fpsX+float(i%3-1)*.06f,HudStyle::fpsY+float(1-i/3)*.008f,1);
 return true;
}
inline const char* StyledFpsText(const char* text,char* buffer,size_t capacity){
 if(!text||std::strncmp(text," FPS: ",6))return text;
 std::snprintf(buffer,capacity,"|cffdcda3c%s|r",text);return buffer;
}
}
