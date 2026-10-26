#pragma once
#include <cstdint>
namespace wht {
struct ButtonPressAnimation131 {
 uint64_t started=0;unsigned duration=180;bool active=false;
 void Begin(uint64_t now,unsigned milliseconds=180){started=now;duration=milliseconds;active=true;}
 void Reset(){active=false;}
 bool Down(uint64_t now){if(active&&now-started>=duration)active=false;return active;}
 float Amount(uint64_t now){if(!Down(now)||!duration)return 0;const float phase=float(now-started)/float(duration);return (1-phase)*(1-phase);}
};
inline uint32_t IconPressColor131(float amount,bool active){
 const uint32_t base=active?0xff44ff3d:0xffffffff;const float brightness=1-.40f*amount;uint32_t color=0xff000000;
 for(int shift:{0,8,16})color|=uint32_t(float((base>>shift)&255)*brightness+.5f)<<shift;return color;
}
}
