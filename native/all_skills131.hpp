#pragma once
#include "drop_icon131.hpp"
#include "quick_drop131.hpp"
namespace wht {
struct SkillProfile131 {unsigned build;uintptr_t click,eventTable,buttonTable,unitTable,heroTable,heroMember,points;};
#ifdef _WIN64
inline constexpr SkillProfile131 skillProfiles131[]={
 {12164,0xfb6da0,0x1c57ef8,0x1c0df60,0x1c16140,0x1a47c90,0x558,0xe4},
 {12173,0xff3910,0x1c86e08,0x1c3ce20,0x1c45000,0x1a76ca0,0x558,0xe4}
};
#else
inline constexpr SkillProfile131 skillProfiles131[]={
 {12164,0xdfc1d0,0x1853a04,0x182a764,0x182eeb0,0x17624d0,0x450,0x94},
 {12173,0xdbafd0,0x1808924,0x17df644,0x17e3d90,0x1717508,0x448,0x94}
};
#endif
inline const SkillProfile131* SkillProfile(unsigned build){for(const auto& p:skillProfiles131)if(p.build==build)return &p;return nullptr;}
inline int HeroPoints131(uintptr_t unit,uintptr_t base,const SkillProfile131& p){
 __try{if(!InventoryIdentity131(unit,base,p.unitTable))return -1;const auto hero=*reinterpret_cast<uintptr_t*>(unit+p.heroMember);
  if(!InventoryIdentity131(hero,base,p.heroTable))return -1;const auto points=*reinterpret_cast<int*>(hero+p.points);return points>=0&&points<=10000?points:-1;
 }__except(1){return -1;}
}
struct SkillButton131 {unsigned ability=0,order=0;};
inline SkillButton131 SkillButtonData131(uintptr_t button){
 __try{if(!button||!*reinterpret_cast<unsigned char*>(button+(sizeof(uintptr_t)==8?0xf1:0x9d)))return {};
  const auto data=*reinterpret_cast<uintptr_t*>(button+(sizeof(uintptr_t)==8?0x270:0x190));if(!data)return {};
  return {*reinterpret_cast<unsigned*>(data+(sizeof(uintptr_t)==8?8:4)),*reinterpret_cast<unsigned*>(data+(sizeof(uintptr_t)==8?12:8))};
 }__except(1){return {};}
}
inline bool SkillClick131(uintptr_t button,uintptr_t base,const SkillProfile131& p){
 __try{
  if(!InventoryIdentity131(button,base,p.buttonTable))return false;
#ifdef _WIN64
  const unsigned char prefix[]={0x48,0x83,0xec,0x58,0x48,0xc7,0x44,0x24,0x20,0xfe,0xff,0xff,0xff,0x4c,0x8b,0x81,0xd0,0x01,0x00,0x00,0x4d,0x85,0xc0};
  if(memcmp(reinterpret_cast<void*>(base+p.click),prefix,sizeof(prefix)))return false;
  if(*reinterpret_cast<unsigned char*>(base+p.click+33)!=0x48||p.click+40+*reinterpret_cast<int32_t*>(base+p.click+36)!=p.eventTable)return false;
#else
  const unsigned char prefix[]={0x55,0x8b,0xec,0x6a,0xff},callback[]={0x8b,0x91,0x20,0x01,0x00,0x00,0x85,0xd2,0x74,0x32};
  if(memcmp(reinterpret_cast<void*>(base+p.click),prefix,sizeof(prefix))||memcmp(reinterpret_cast<void*>(base+p.click+37),callback,sizeof(callback)))return false;
  if(*reinterpret_cast<uintptr_t*>(base+p.click+57)!=base+p.eventTable)return false;
#endif
  const auto listener=*reinterpret_cast<uintptr_t*>(button+(sizeof(uintptr_t)==8?0x1d0:0x120));if(!listener)return false;
  const auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);const auto pe=reinterpret_cast<IMAGE_NT_HEADERS*>(base+dos->e_lfanew);const auto end=base+pe->OptionalHeader.SizeOfImage;
  const auto table=*reinterpret_cast<uintptr_t*>(listener);if(table<base||table+(sizeof(uintptr_t)==8?0x18:0xc)+sizeof(uintptr_t)>end)return false;
  const auto invoke=*reinterpret_cast<uintptr_t*>(table+(sizeof(uintptr_t)==8?0x18:0xc));if(invoke<base||invoke>=end)return false;
  reinterpret_cast<void(__thiscall*)(uintptr_t,int)>(base+p.click)(button,1);return true;
 }__except(1){return false;}
}
struct SkillsIcon131:DropIcon131 {
 SkillsIcon131(){Configure();}
 void Configure(){area={.558f,.012f,.012f,.016f};upAsset="BTNAllSkills.blp";downAsset="BTNAllSkillsDow.blp";}
 void Reset(){DropIcon131::Reset();Configure();}
};
// Finish the native learn/back sequence in one UI update, before rendering.
// The player never needs to open the hero's plus card manually.
struct AllSkills131 {
 static constexpr unsigned plus=0xd0020,back=0xd0007,heroAbility=0x41486572;
 uintptr_t unit=0;bool active=false,openingSent=false,everOpened=false,reported=false;unsigned index=0,passes=0,submitted=0;int passPoints=0;uint64_t due=0,deadline=0;
 void Reset(){*this={};}
 template<class Selected,class Points,class Data> bool Start(uint64_t now,Selected selected,Points points,Data data){
  if(active)return true;const auto selectedUnit=selected();const int available=points(selectedUnit);if(!selectedUnit||available<=0)return false;
  bool canOpen=data(11).order==back;for(unsigned slot=0;slot<12&&!canOpen;++slot)canOpen=data(slot).order==plus;
  if(!canOpen)return false;Reset();unit=selectedUnit;passPoints=available;active=true;deadline=now+15000;due=now;return true;
 }
 template<class Selected,class Points,class Data,class Click> bool Tick(uint64_t now,bool allowed,Selected selected,Points points,Data data,Click click){
  if(!active)return false;if(!allowed||selected()!=unit||points(unit)<0){Reset();return false;}if(now<due)return false;
  const auto finish=[&](){if(selected()==unit&&data(11).order==back)click(11);active=false;return true;};
  if(now>=deadline||passes>=20||points(unit)==0)return finish();
  if(data(11).order!=back){
   if(everOpened)return finish();
   if(!openingSent){for(unsigned slot=0;slot<12;++slot)if(data(slot).order==plus){openingSent=click(slot);if(!openingSent)return finish();break;}}
   if(data(11).order!=back)return finish();
  }
  everOpened=true;
  for(unsigned pass=0;pass<20;++pass){
   const int before=points(unit);if(before<=0)return finish();
   bool sent=false;
   for(unsigned entry=0;entry<11;++entry){
    if(selected()!=unit){Reset();return false;}
    if(points(unit)<=0||data(11).order!=back)return finish();
    const unsigned slot=(entry%3)*4+entry/3;const auto value=data(slot);
    if(value.ability==heroAbility&&value.order&&value.order!=plus&&value.order!=back){if(!click(slot))return finish();++submitted;sent=true;}
   }
   // The game may apply points after UI dispatch. Keep the inherited bounded
   // passes instead of waiting for that update with the learn card visible.
   if(!sent)return finish();
  }
  return finish();
 }
};
}
