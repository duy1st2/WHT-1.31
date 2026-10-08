#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
namespace wht {
// Classic 1.29 palette; indices 0..15 retain the legacy resource colors.
// Extended reference: https://www.hiveworkshop.com/threads/color-code-for-players-12.304942/
inline bool PlayerColor129(unsigned index,uint32_t& color){
 constexpr uint32_t colors[]={0xffff0303,0xff0042ff,0xff1ce6b9,0xff540081,
  0xfffffc01,0xfffe8a0e,0xff20c000,0xffe55bb0,0xff959697,0xff7ebff1,
  0xff106246,0xff4e2a04,0xff9b0000,0xff0000c3,0xff00eaff,0xffbe00fe,
  0xffebcd87,0xfff8a48b,0xffbfff80,0xffdcb9eb,0xff282828,0xffebf0ff,
  0xff00781e,0xffa46f33};
 if(index>=sizeof(colors)/sizeof(colors[0]))return false;
 color=colors[index];return true;
}
inline std::string PlayerColorTag129(unsigned index,bool left){
 if(left)return "|cff7d7d7d";
 uint32_t color=0xffffffff;PlayerColor129(index,color);
 char tag[11]{};std::snprintf(tag,sizeof(tag),"|c%08x",static_cast<unsigned>(color));return tag;
}
}
