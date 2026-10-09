#pragma once
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
namespace wht {
// Native stat tooltip fonts do not reliably contain Vietnamese glyphs.
// Keep these labels ASCII; tool menu labels/help remain localized.
inline std::string AttackSpeedTooltip(float multiplier,float cooldown,float maximum,float minimum=0.f){
 if(!std::isfinite(multiplier)||!std::isfinite(cooldown)||!std::isfinite(maximum)||!std::isfinite(minimum)||multiplier<=0||cooldown<=0||maximum<=0||minimum<0||minimum>maximum)return {};
 const double effective=(std::clamp)(double(multiplier),double(minimum),double(maximum));
 const double rate=effective/cooldown,interval=cooldown/effective,maxRate=double(maximum)/cooldown;
 if(!std::isfinite(rate)||!std::isfinite(interval)||!std::isfinite(maxRate))return {};
 char text[512]{};
 snprintf(text,sizeof(text),
  "Attack speed: |cff00FF00%.1f/sec|r|nAttack interval: |cff00FF00%.2f sec|r|nAttack speed bonus: |cff00FF00%+.0f%%|r|nMaximum attack speed: |cff00FF00%.1f/sec|r",
  rate,interval,(double(multiplier)-1)*100,maxRate);
 return text;
}
inline std::string MovementSpeedTooltip(float speed){
 if(!std::isfinite(speed)||speed<0)return {};
 char text[128]{};snprintf(text,sizeof(text),"Movement speed: |cff00FF00%.1f|r",double(speed));return text;
}
}
