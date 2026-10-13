#pragma once
#include "common.hpp"
namespace wht {
struct Profile64 {unsigned build;const char* hash;uintptr_t uiGet,alloc,frameCtor,fontCtor,textureCtor,backdropCtor,backdropSet,stringAssign,width,height,absolute,relative,fontSet,fontText,fontAlign,textureSet,uiVtable,simpleVtable,archiveOpen,archiveLookup,cameraSet,floatCurrent,floatMaximum,resolve,heroLevel,statCtor,statBind,tint,solid,textWidth,abilityGetter,cameraVtable,portraitVtable,unitVtable,floatVtable,heroVtable,statVtable,buttonVtable,commandVtable,infoVtable,inventoryVtable;};
inline constexpr Profile64 profiles64[]={
 {12164,"0F82359F25B0C25FED3BD379C482BDD695636F16E064526C2943E453FF4E9AAF",0xc44860,0x2d8030,0xfb9d00,0xfb7ba0,0xfb7c20,0xfde750,0xfbade0,0x316170,0xfb5c10,0xfb5830,0xfb5970,0xfb5a70,0xfb9040,0xfb9660,0xfb9140,0xfb9880,0x1c00ec0,0x1c583e8,0x3324b0,0x331c60,0xc36550,0xf114b0,0xf11500,0x3547b0,0xd096e0,0xcf8a10,0xcf91d0,0xfb7b00,0xfb97e0,0xfb8a30,0xcc3ee0,0x1bff998,0x1c10810,0x1c16140,0x19dd698,0x1a47c90,0x1c14f08,0x1c0df60,0x1c08180,0x1c084e8,0x1c08858},
 {12173,"BA77193D0C5A94F3EDCFD4933987764C6FAF8AB97CB93D6256B645BBAC8939E1",0xc8aa60,0x345b70,0xff6870,0xff4710,0xff4790,0x101b170,0xff7950,0x383bf0,0xff2780,0xff23a0,0xff24e0,0xff25e0,0xff5bb0,0xff61d0,0xff5cb0,0xff63f0,0x1c2fd78,0x1c872f8,0x39ff30,0x39f6e0,0xc7cdd0,0xf4d280,0xf4d2d0,0x3bf9a0,0xd481a0,0xd37610,0xd37dd0,0xff4670,0xff6350,0xff55a0,0xd03dc0,0x1c2e828,0x1c3f6d0,0x1c45000,0x1a0c6b0,0x1a76ca0,0x1c43dc8,0x1c3ce20,0x1c37040,0x1c373a8,0x1c37718},
};
inline const Profile64* FindProfile64(const std::string& hash){for(const auto& p:profiles64)if(hash==p.hash)return &p;return nullptr;}
inline bool ProfileCode64(uintptr_t base,const Profile64& p){
 __try{
 if(p.build==12164){
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x7c};if(memcmp(reinterpret_cast<void*>(base+p.cameraSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.floatCurrent),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.floatMaximum),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x8b,0xc1,0x44,0x8b,0xca,0xc1,0xe8,0x1f};if(memcmp(reinterpret_cast<void*>(base+p.resolve),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x40,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.heroLevel),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x8b,0xc4,0x48,0x89,0x48,0x8,0x55};if(memcmp(reinterpret_cast<void*>(base+p.statCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x55,0x53,0x57,0x48,0x8b,0xec,0x48};if(memcmp(reinterpret_cast<void*>(base+p.statBind),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0xf,0xb6,0x42,0x3,0x88,0x81,0xb8,0x0};if(memcmp(reinterpret_cast<void*>(base+p.tint),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c};if(memcmp(reinterpret_cast<void*>(base+p.solid),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x40,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.textWidth),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x8b,0x81,0x78,0x2d,0x0,0x0,0xc3};if(memcmp(reinterpret_cast<void*>(base+p.abilityGetter),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.uiGet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x74};if(memcmp(reinterpret_cast<void*>(base+p.alloc),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x4c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.frameCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.fontCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x4c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.textureCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x4c,0x24,0x8,0x56,0x57,0x41};if(memcmp(reinterpret_cast<void*>(base+p.backdropCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.backdropSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x6c,0x24,0x20,0x56,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.stringAssign),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0xf3,0xf,0x11,0x89,0x9c,0x0,0x0,0x0};if(memcmp(reinterpret_cast<void*>(base+p.width),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0xf3,0xf,0x11,0x89,0xa0,0x0,0x0,0x0};if(memcmp(reinterpret_cast<void*>(base+p.height),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x74};if(memcmp(reinterpret_cast<void*>(base+p.absolute),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x74};if(memcmp(reinterpret_cast<void*>(base+p.relative),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x6c};if(memcmp(reinterpret_cast<void*>(base+p.fontSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.fontText),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x44,0x8b,0x81,0x34,0x01,0,0,0x41};if(memcmp(reinterpret_cast<void*>(base+p.fontAlign),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x55,0x56,0x57,0x41,0x56,0x41,0x57};if(memcmp(reinterpret_cast<void*>(base+p.textureSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x55,0x56,0x57,0x41,0x54,0x48,0x81};if(memcmp(reinterpret_cast<void*>(base+p.archiveOpen),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x6c};if(memcmp(reinterpret_cast<void*>(base+p.archiveLookup),bytes,sizeof(bytes)))return false;}
  return true;
 }
 if(p.build==12173){
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x7c};if(memcmp(reinterpret_cast<void*>(base+p.cameraSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.floatCurrent),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.floatMaximum),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x8b,0xc1,0x44,0x8b,0xca,0xc1,0xe8,0x1f};if(memcmp(reinterpret_cast<void*>(base+p.resolve),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x40,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.heroLevel),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x8b,0xc4,0x48,0x89,0x48,0x8,0x55};if(memcmp(reinterpret_cast<void*>(base+p.statCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x55,0x53,0x57,0x48,0x8b,0xec,0x48};if(memcmp(reinterpret_cast<void*>(base+p.statBind),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0xf,0xb6,0x42,0x3,0x88,0x81,0xb8,0x0};if(memcmp(reinterpret_cast<void*>(base+p.tint),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c};if(memcmp(reinterpret_cast<void*>(base+p.solid),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x40,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.textWidth),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x8b,0x81,0x78,0x2d,0x0,0x0,0xc3};if(memcmp(reinterpret_cast<void*>(base+p.abilityGetter),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.uiGet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x74};if(memcmp(reinterpret_cast<void*>(base+p.alloc),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x4c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.frameCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.fontCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x4c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.textureCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x4c,0x24,0x8,0x56,0x57,0x41};if(memcmp(reinterpret_cast<void*>(base+p.backdropCtor),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x57,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.backdropSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x6c,0x24,0x20,0x56,0x48,0x83};if(memcmp(reinterpret_cast<void*>(base+p.stringAssign),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0xf3,0xf,0x11,0x89,0x9c,0x0,0x0,0x0};if(memcmp(reinterpret_cast<void*>(base+p.width),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0xf3,0xf,0x11,0x89,0xa0,0x0,0x0,0x0};if(memcmp(reinterpret_cast<void*>(base+p.height),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x74};if(memcmp(reinterpret_cast<void*>(base+p.absolute),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x74};if(memcmp(reinterpret_cast<void*>(base+p.relative),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x6c};if(memcmp(reinterpret_cast<void*>(base+p.fontSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b};if(memcmp(reinterpret_cast<void*>(base+p.fontText),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x44,0x8b,0x81,0x34,0x01,0,0,0x41};if(memcmp(reinterpret_cast<void*>(base+p.fontAlign),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x55,0x56,0x57,0x41,0x56,0x41,0x57};if(memcmp(reinterpret_cast<void*>(base+p.textureSet),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x40,0x55,0x56,0x57,0x41,0x54,0x48,0x81};if(memcmp(reinterpret_cast<void*>(base+p.archiveOpen),bytes,sizeof(bytes)))return false;}
  {const unsigned char bytes[]={0x48,0x89,0x5c,0x24,0x8,0x48,0x89,0x6c};if(memcmp(reinterpret_cast<void*>(base+p.archiveLookup),bytes,sizeof(bytes)))return false;}
  return true;
 }
 return false;
 }__except(1){return false;}
}
}
