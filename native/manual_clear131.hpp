#pragma once
#include "message_clear131.hpp"
#include "quick_chat131.hpp"
namespace wht {
inline unsigned ManualClearKind131(const std::wstring& command){
 if(command==L"#clear"||command==L"#cls")return 1;
 if(command==L"#clearc"||command==L"#clsc")return 2;
 return 0;
}
struct ManualClear131 {
 uintptr_t owner=0;unsigned pending=0;uint64_t expires=0;
 void Reset(){owner=0;pending=0;expires=0;}
 bool Queue(uintptr_t ui,unsigned kind,uint64_t now){
  if(!ui||(kind!=1&&kind!=2))return false;
  if(owner!=ui||now>=expires)Reset();
  owner=ui;pending|=kind;expires=now+2000;return true;
 }
 template<class Chat,class Clear> unsigned Tick(uintptr_t ui,bool allowed,bool ready,uint64_t now,Chat chat,Clear clear){
  if(!pending)return 0;
  if(!allowed||ui!=owner||!ui||now>=expires){Reset();return 0;}
  bool typing=false;if(!ready||!chat(typing)||typing)return 0;
  // Consume before invoking native UI, including native refusal/re-entry.
  const unsigned requested=pending;Reset();unsigned result=0;
  for(unsigned kind:{1u,2u})if((requested&kind)&&clear(kind==2))result|=kind;
  return result;
 }
 unsigned Update(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,bool ready,uint64_t now){
  return Tick(ui,allowed,ready,now,[&](bool& typing){QuickChatView131 view;if(!ReadQuickChat131(base,build,ui,view))return false;typing=view.open;return true;},[&](bool chat){
   const auto* p=MessageProfileFor131(build);return p&&ClearMessageFrame131(base,MessageFrame131(base,ui,*p,chat),*p);
  });
 }
};
}
