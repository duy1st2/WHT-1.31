#pragma once
#include "common.hpp"
#include "component_lookup131.hpp"
#include <cstring>
namespace wht {
struct ConsoleProfile131 {unsigned build;uintptr_t table,show,hide,requestedCheck,attachedSet;};
#ifdef _WIN64
inline constexpr ConsoleProfile131 consoleProfiles131[]={
 {12164,0x1c13208,0xfbb160,0xfba8d0,0x34,0xd8},
 {12173,0x1c420c8,0xff7cd0,0xff7440,0x71,0x115}
};
#else
inline constexpr ConsoleProfile131 consoleProfiles131[]={
 {12164,0x182d714,0xdfff60,0xdff850,0x50,0xb3},
 {12173,0x17e25f4,0xdbed60,0xdbe650,0x21,0x84}
};
#endif
inline const ConsoleProfile131* ConsoleProfileFor131(unsigned build){for(const auto& p:consoleProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool ConsoleCode131(uintptr_t base,const ConsoleProfile131& p){
#ifdef _WIN64
 const unsigned char show[]={0x48,0x83,0xec,0x28,0x48,0x89,0x5c,0x24,0x30,0x48,0x8b,0xd9};
 const unsigned char hide[]={0x40,0x56,0x48,0x83,0xec,0x20,0x80,0xb9,0xf1,0,0,0,0};
 const unsigned char check[]={0x38,0x81,0xf0,0,0,0},attach[]={0x44,0x88,0xb3,0xf1,0,0,0},detach[]={0x88,0x9e,0xf1,0,0,0};
 constexpr unsigned hideSlot=0xd0,showSlot=0xd8,detachOffset=0x77;
#else
 const unsigned char show[]={0x55,0x8b,0xec,0x83,0xec,8,0x53,0x56,0x57,0x89,0x4d,0xf8};
 const unsigned char hide[]={0x56,0x8b,0xf1,0x80,0xbe,0x9d,0,0,0,0};
 const unsigned char check[]={0x80,0xbb,0x9c,0,0,0,0},attach[]={0xc6,0x83,0x9d,0,0,0,1},detach[]={0xc6,0x86,0x9d,0,0,0,0};
 constexpr unsigned hideSlot=0x68,showSlot=0x6c,detachOffset=0x3f;
#endif
 __try{return base&&*reinterpret_cast<uintptr_t*>(base+p.table+hideSlot)==base+p.hide&&*reinterpret_cast<uintptr_t*>(base+p.table+showSlot)==base+p.show&&!memcmp(reinterpret_cast<void*>(base+p.show),show,sizeof(show))&&!memcmp(reinterpret_cast<void*>(base+p.hide),hide,sizeof(hide))&&!memcmp(reinterpret_cast<void*>(base+p.show+p.requestedCheck),check,sizeof(check))&&!memcmp(reinterpret_cast<void*>(base+p.show+p.attachedSet),attach,sizeof(attach))&&!memcmp(reinterpret_cast<void*>(base+p.hide+detachOffset),detach,sizeof(detach));}__except(1){return false;}
}
inline bool ReadConsole131(uintptr_t base,const ConsoleProfile131& p,uintptr_t frame,bool& requested,bool& attached){
 __try{
  if(!base||!frame||*reinterpret_cast<uintptr_t*>(frame)!=base+p.table)return false;
  const auto flags=reinterpret_cast<const unsigned char*>(frame+(sizeof(uintptr_t)==8?0xf0:0x9c));if(flags[0]>1||flags[1]>1)return false;requested=flags[0]!=0;attached=flags[1]!=0;return true;
 }__except(1){return false;}
}
inline bool WriteConsoleRequested131(uintptr_t base,const ConsoleProfile131& p,uintptr_t frame,bool shown){
 bool requested=false,attached=false;if(!ReadConsole131(base,p,frame,requested,attached))return false;
 __try{*reinterpret_cast<unsigned char*>(frame+(sizeof(uintptr_t)==8?0xf0:0x9c))=shown?1:0;return true;}__except(1){return false;}
}
struct ConsoleUi131 {
 unsigned diagnostic=0;uintptr_t owner=0;bool active=false,wasShown=false;
 void Reset(){*this={};}
 template<class Write,class Show,class Hide> bool Apply(uintptr_t frame,bool valid,bool requested,bool attached,bool enabled,Write write,Show show,Hide hide){
  if(owner!=frame){Reset();owner=frame;}if(!valid){Reset();return false;}
  if(enabled){
   if(!active){active=true;wasShown=requested;}
   if(requested&&!write(false)){Reset();return false;}
   if(attached)hide(frame);
  }else if(active){
   if(wasShown){if(!requested&&!write(true)){Reset();return false;}if(!attached)show(frame);}
   active=false;
  }
  return true;
 }
 bool Update(uintptr_t base,unsigned build,uintptr_t frame,bool enabled){
  diagnostic=1;const auto* p=ConsoleProfileFor131(build);bool requested=false,attached=false;
  if(!p||!ConsoleCode131(base,*p)){diagnostic=2;return false;}if(!ReadConsole131(base,*p,frame,requested,attached)){diagnostic=3;return false;}diagnostic=4;
  using Callback=void(__thiscall*)(uintptr_t);
  return Invoke(base,*p,frame,enabled,requested,attached,reinterpret_cast<Callback>(base+p->show),reinterpret_cast<Callback>(base+p->hide));
 }
 using Callback=void(__thiscall*)(uintptr_t);
 bool Invoke(uintptr_t base,const ConsoleProfile131& p,uintptr_t frame,bool enabled,bool requested,bool attached,Callback show,Callback hide){
  __try{return Apply(frame,true,requested,attached,enabled,[&](bool visible){return WriteConsoleRequested131(base,p,frame,visible);},show,hide);}__except(1){Reset();return false;}
 }
};
}
