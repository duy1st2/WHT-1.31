#pragma once
#include "component_lookup131.hpp"
#include <cstring>
namespace wht {
struct QuickChatProfile131 {unsigned build;uintptr_t ui,bar,edit,open,text,enter,baseEdit,baseEnter;};
#ifdef _WIN64
inline constexpr QuickChatProfile131 quickChatProfiles131[]={
 {12164,0x1c00ec0,0x1c0adc0,0x1c0ab00,0xcc1f80,0xfc5770,0xcc2160,0x1c598a0,0xfc47b0},
 {12173,0x1c2fd78,0x1c39c80,0x1c399c0,0xd01e40,0x1002190,0xd02020,0x1c887b0,0x10011d0}};
inline constexpr unsigned quickChatMode131=0x3e8,quickChatEdit131=0x3f0,quickChatLabel131=0x3f8;
#else
inline constexpr QuickChatProfile131 quickChatProfiles131[]={
 {12164,0x18240a0,0x1828b00,0x18289a0,0xb6b720,0xe09660,0xb6b8d0,0x1854580,0xe08770},
 {12173,0x17d8f80,0x17dd9e0,0x17dd880,0xb2b410,0xdc8480,0xb2b5c0,0x18094a0,0xdc7590}};
inline constexpr unsigned quickChatMode131=0x264,quickChatEdit131=0x268,quickChatLabel131=0x26c;
#endif
inline const QuickChatProfile131* QuickChatProfileFor131(unsigned build){for(const auto& p:quickChatProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool QuickChatCode131(uintptr_t base,const QuickChatProfile131& p){
#ifdef _WIN64
 const unsigned char open[]={0x48,0x89,0x5c,0x24,8,0x57,0x48,0x83,0xec,0x20,0x88,0x91,0xe8,3,0,0};
 const unsigned char text[]={0x48,0x89,0x5c,0x24,8,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57};
 const unsigned char enter[]={0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x57,0x41,0x56,0x41,0x57,0x48};
 const unsigned char derived[]={0x8b,0x42,0x20,0x83,0xf8,0x1b,0x74,0x0c,0x83,0xc0,0xdb,0x83};
 const unsigned char event[]={0xba,0x64,0,0x0b,0x40};constexpr unsigned eventOffset=0x19d;
#else
 const unsigned char open[]={0x55,0x8b,0xec,0x53,0x8b,0x5d,8,0x56,0x8b,0xf1,0x53,0x8b,0x8e,0x68,2,0};
 const unsigned char text[]={0x55,0x8b,0xec,0x51,0x53,0x56,0x8b,0x75,8,0x8b,0xd6,0x57,0x8b,0xf9,0x8d,0x4a};
 const unsigned char enter[]={0x55,0x8b,0xec,0x51,0x53,0x56,0x57,0x8b,0xd9,0x6a,1,0x89,0x5d,0xfc};
 const unsigned char derived[]={0x55,0x8b,0xec,0x8b,0x55,8,0x8b,0x42,0x10,0x83,0xf8,0x1b};
 const unsigned char event[]={0x68,0x64,0,0x0b,0x40};constexpr unsigned eventOffset=0x174;
#endif
 __try{
  if(!base)return false;
#ifdef _WIN64
  const auto branch=base+p.enter+0xe;
  if(memcmp(reinterpret_cast<void*>(branch),"\x0f\x87",2)||branch+6+*reinterpret_cast<int32_t*>(branch+2)!=base+p.baseEnter)return false;
#else
  const auto branch=base+p.enter+0x1c;
  if(*reinterpret_cast<unsigned char*>(branch)!=0xe9||branch+5+*reinterpret_cast<int32_t*>(branch+1)!=base+p.baseEnter)return false;
#endif
  return *reinterpret_cast<uintptr_t*>(base+p.bar+74*sizeof(uintptr_t))==base+p.open&&*reinterpret_cast<uintptr_t*>(base+p.edit+21*sizeof(uintptr_t))==base+p.enter&&*reinterpret_cast<uintptr_t*>(base+p.baseEdit+21*sizeof(uintptr_t))==base+p.baseEnter&&
  !memcmp(reinterpret_cast<void*>(base+p.open),open,sizeof(open))&&!memcmp(reinterpret_cast<void*>(base+p.text),text,sizeof(text))&&!memcmp(reinterpret_cast<void*>(base+p.enter),derived,sizeof(derived))&&!memcmp(reinterpret_cast<void*>(base+p.baseEnter),enter,sizeof(enter))&&!memcmp(reinterpret_cast<void*>(base+p.baseEnter+eventOffset),event,sizeof(event));}__except(1){return false;}
}
struct QuickChatView131 {
 uintptr_t ui=0,bar=0,edit=0,label=0;bool open=false;
 bool Same(const QuickChatView131& other)const{return ui&&ui==other.ui&&bar==other.bar&&edit==other.edit&&label==other.label;}
};
inline bool ReadQuickChat131(uintptr_t base,unsigned build,uintptr_t ui,QuickChatView131& out){
 const auto* p=QuickChatProfileFor131(build);if(!p||!ComponentIdentity131(ui,base+p->ui))return false;
 const auto bar=FindComponent131(ui,base,p->bar,sizeof(void*)==8?0x7e8:0x4d4);if(!bar)return false;
 __try{
  const auto mode=*reinterpret_cast<unsigned char*>(bar+quickChatMode131);
  const auto edit=ComponentLookup131::Slot(bar,quickChatEdit131),label=ComponentLookup131::Slot(bar,quickChatLabel131);
  if(mode>1||!ComponentIdentity131(edit,base+p->edit)||!ReadableComponent131(label,sizeof(uintptr_t)))return false;
  out={ui,bar,edit,label,mode!=0};return true;
 }__except(1){return false;}
}
struct QuickChatApi131 {
 void(__thiscall* open)(uintptr_t,int)=nullptr;
 void(__thiscall* text)(uintptr_t,const char*,int)=nullptr;
 bool(__thiscall* enter)(uintptr_t,const void*)=nullptr;
 explicit operator bool()const{return open&&text&&enter;}
};
inline QuickChatApi131 QuickChatNative131(uintptr_t base,unsigned build){
 const auto* p=QuickChatProfileFor131(build);if(!p)return {};
 static uintptr_t checked=0;static unsigned version=0;static bool ready=false;
 if(checked!=base||version!=build){checked=base;version=build;ready=QuickChatCode131(base,*p);}if(!ready)return {};
 return {reinterpret_cast<decltype(QuickChatApi131::open)>(base+p->open),reinterpret_cast<decltype(QuickChatApi131::text)>(base+p->text),reinterpret_cast<decltype(QuickChatApi131::enter)>(base+p->enter)};
}
inline bool ValidQuickChat131(const std::string& text){return !text.empty()&&text.size()<=128&&QuickChatText(Wide(text))==text;}
// The verified CEditBox key handler reads event ID at 2*ptr, key at 4*ptr,
// modifiers at key+4. Its Enter arm emits native frame event 0x400B0064,
// using the same CGameUI chat submission callback as a physical Enter key.
inline std::array<unsigned char,48> QuickChatEnterEvent131(){
 std::array<unsigned char,48> event{};const unsigned id=0x40060064,key=VK_RETURN;
 memcpy(event.data()+2*sizeof(void*),&id,sizeof(id));memcpy(event.data()+4*sizeof(void*),&key,sizeof(key));return event;
}
template<class Read> bool SendQuickChat131(const std::string& text,const QuickChatApi131& api,Read read){
 if(!api||!ValidQuickChat131(text))return false;
 QuickChatView131 owner,current;if(!read(owner)||owner.open)return false;
 api.open(owner.bar,1);
 if(!read(current)||!owner.Same(current)||!current.open)return false;
 api.text(owner.edit,text.c_str(),1);
 if(!read(current)||!owner.Same(current)||!current.open)return false;
 alignas(uintptr_t) const auto event=QuickChatEnterEvent131();const bool handled=api.enter(owner.edit,event.data());
 if(!read(current)||!owner.Same(current))return false;
 // If the normal native handler refused submission, only close the chat
 // opened here while all original owners still match. Never touch a new UI.
 if(current.open){api.open(owner.bar,0);return false;}return handled;
}
template<class Send> bool QuickChatKey131(unsigned key,bool repeat,bool allowed,bool modifiers,const Config& config,Send send){
 if(!allowed||modifiers||repeat||!config.quickChatEnabled||!key||key>=256||key==VK_RETURN||key==VK_ESCAPE)return false;
 for(const auto& binding:config.quickChat)if(binding.vk==key&&ValidQuickChat131(binding.text))return send(binding.text);
 return false;
}
struct QuickChatEcho131 {
 unsigned scan=0;uint64_t due=0;
 void Reset(){scan=0;due=0;}
 void Sent(LPARAM data,uint64_t now){scan=unsigned((data>>16)&255);due=now+250;}
 bool Character(LPARAM data,uint64_t now){if(!scan||now>due||unsigned((data>>16)&255)!=scan)return false;Reset();return true;}
};
}
