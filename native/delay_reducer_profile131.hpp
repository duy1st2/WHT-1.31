#pragma once
#include "common.hpp"
#include <cstring>
namespace wht {
struct DelayEntry131 {uintptr_t table,getter,locator,type;};
struct DelayProfile131 {unsigned build;DelayEntry131 entries[3];};
inline constexpr const char* delayTypes131[]={".?AVNetProviderLOOP@Net@@",".?AVNetProviderLTCP@Net@@",".?AVNetProviderBNET@Net@@"};
#ifdef _WIN64
inline constexpr DelayProfile131 delayProfiles131[]={
{12164, {{0x1c6ca18,0x1096400,0x1e368e8,0x2123b28},{0x1c6e878,0x10a9350,0x1e38870,0x2124e70},{0x1c6db30,0x109ecc0,0x1e36e48,0x2124138}}},
{12173, {{0x1c9b928,0x10d2e30,0x1e64708,0x2150b28},{0x1c9d788,0x10e5d80,0x1e66690,0x2151e70},{0x1c9ca40,0x10db6f0,0x1e64c68,0x2151138}}},
};
#else
inline constexpr DelayProfile131 delayProfiles131[]={
{12164, {{0x1860064,0xebf370,0x19a8f18,0x1b05c84},{0x18608f8,0xed0730,0x19aa2b0,0x1b06d34},{0x186032c,0xec6890,0x19a924c,0x1b061a4}}},
{12173, {{0x1814f84,0xe7e210,0x1960d28,0x1abdc84},{0x1815818,0xe8f5d0,0x19620c0,0x1abed34},{0x181524c,0xe85730,0x196105c,0x1abe1a4}}},
};
#endif
inline const DelayProfile131* DelayProfileFor131(unsigned build){for(const auto& p:delayProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool DelayEntryCode131(uintptr_t base,const DelayEntry131& p,unsigned index){if(!base||index>=3)return false;
 __try{const unsigned char code[]={0xb8,0x64,0,0,0,0xc3};if(memcmp(reinterpret_cast<const void*>(base+p.getter),code,sizeof(code)))return false;if(*reinterpret_cast<uintptr_t*>(base+p.table-sizeof(uintptr_t))!=base+p.locator)return false;
#ifdef _WIN64
 if(*reinterpret_cast<unsigned*>(base+p.locator)!=1||*reinterpret_cast<unsigned*>(base+p.locator+12)!=p.type)return false;constexpr unsigned nameOffset=16;
#else
 if(*reinterpret_cast<unsigned*>(base+p.locator)!=0||*reinterpret_cast<unsigned*>(base+p.locator+12)!=base+p.type)return false;constexpr unsigned nameOffset=8;
#endif
 if(*reinterpret_cast<unsigned*>(base+p.locator+4)||*reinterpret_cast<unsigned*>(base+p.locator+8))return false;return !memcmp(reinterpret_cast<void*>(base+p.type+nameOffset),delayTypes131[index],strlen(delayTypes131[index])+1);
 }__except(1){return false;}
}
}
