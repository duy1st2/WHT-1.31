#pragma once
#include "services129.hpp"
namespace wht {
struct SkillSelection129 {uintptr_t ui=0,unit=0;uint64_t identity=0;};
inline SkillSelection129 SelectedSkill129(uintptr_t base){
 SkillSelection129 selected;
 if(!base)return selected;
 __try {
  const auto ui=*reinterpret_cast<uintptr_t*>(base+0xd3d7f8);if(!ui)return selected;
  const auto portrait=*reinterpret_cast<uintptr_t*>(ui+0x434);
  if(!portrait||*reinterpret_cast<uintptr_t*>(portrait)!=base+0xa981d4)return selected;
  const auto unit=*reinterpret_cast<uintptr_t*>(portrait+0x248);if(!unit)return selected;
  selected.ui=ui;selected.unit=unit;
  selected.identity=(uint64_t(*reinterpret_cast<uint32_t*>(unit+0xc))<<32)|*reinterpret_cast<uint32_t*>(unit+0x10);
 }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
 return selected;
}
inline unsigned SkillValue129(uintptr_t base,int slot,unsigned offset){
 if(offset!=4&&offset!=8)return 0;
 const auto button=Button129(base,false,slot);if(!button)return 0;
 __try {const auto data=*reinterpret_cast<uintptr_t*>(button+0x194);return *reinterpret_cast<unsigned*>(data+offset);}
 __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline bool LearnSlot129(uintptr_t base,int slot,ClickFn129 click){
 if(slot<0||slot>=11||!click)return false;
 const auto selected=SelectedSkill129(base);
 if(!selected.unit||!selected.identity)return false;
 const auto same=[&](){const auto current=SelectedSkill129(base);return current.ui==selected.ui&&current.unit==selected.unit&&current.identity==selected.identity;};
 const bool alreadyOpen=SkillValue129(base,11,8)==0xd0007;
 if(!alreadyOpen){
  bool opened=false;
  for(int plus=0;plus<12;++plus)if(SkillValue129(base,plus,8)==0xd0020){opened=ClickButton129(base,false,plus,click);break;}
  if(!opened)return false;
  if(!same()||SkillValue129(base,11,8)!=0xd0007)return true;
 }
 const auto order=SkillValue129(base,slot,8);
 if(same()&&SkillValue129(base,slot,4)==0x41486572&&order&&order!=0xd0020&&order!=0xd0007)
  ClickButton129(base,false,slot,click);
 // Return only the menu opened by this shortcut; keep a manually opened menu.
 if(!alreadyOpen&&same()&&SkillValue129(base,11,8)==0xd0007)ClickButton129(base,false,11,click);
 return true;
}
struct AllSkills129 {
 SkillSelection129 selected;bool active=false,opening=false,waiting=false,clickedInPass=false;
 unsigned index=0,passes=0;uint64_t due=0,deadline=0;
 void Reset(){*this={};}
 bool Same(uintptr_t base)const{const auto current=SelectedSkill129(base);return current.ui==selected.ui&&current.unit==selected.unit&&current.identity==selected.identity;}
 bool Start(uintptr_t base,ClickFn129 click,uint64_t now){
  if(active)return true;
  selected=SelectedSkill129(base);if(!selected.unit||!selected.identity||!click)return false;
  active=true;index=passes=0;clickedInPass=false;due=now;deadline=now+1000;
  if(SkillValue129(base,11,8)==0xd0007){opening=waiting=false;return true;}
  // Queue the opening click: Start is also called inside native chat dispatch,
  // before the chat bar closes. Clicking here can be ignored by the engine.
  // The plus button's order identifies it; its ability tag can differ by map.
  for(int slot=0;slot<12;++slot)if(SkillValue129(base,slot,8)==0xd0020){opening=true;waiting=false;return true;}
  Reset();return false;
 }
 void Tick(uintptr_t base,bool allowed,ClickFn129 click,uint64_t now){
  if(!active)return;
  if(!allowed||!click||!Same(base)){Reset();return;}
  if(now<due)return;
  // Match 1.24/1.26: finish the native learn/back sequence in one UI
  // update, before rendering. Start stays deferred until chat has closed.
  if(SkillValue129(base,11,8)!=0xd0007){
   bool opened=false;
   for(int slot=0;slot<12;++slot)if(SkillValue129(base,slot,8)==0xd0020){
    opened=ClickButton129(base,false,slot,click);break;
   }
   if(!opened||SkillValue129(base,11,8)!=0xd0007){Reset();return;}
  }
  for(unsigned pass=0;pass<20;++pass){
   bool submitted=false;
   for(unsigned entry=0;entry<11;++entry){
    if(!Same(base)||SkillValue129(base,11,8)!=0xd0007){Reset();return;}
    const int slot=int((entry%3)*4+entry/3);
    const auto order=SkillValue129(base,slot,8);
    if(SkillValue129(base,slot,4)==0x41486572&&order&&order!=0xd0020&&order!=0xd0007){
     if(!ClickButton129(base,false,slot,click)){
      if(Same(base)&&SkillValue129(base,11,8)==0xd0007)ClickButton129(base,false,11,click);
      Reset();return;
     }
     submitted=true;
    }
   }
   if(!submitted)break;
  }
  if(Same(base)&&SkillValue129(base,11,8)==0xd0007)ClickButton129(base,false,11,click);
  Reset();
 }
};
}
