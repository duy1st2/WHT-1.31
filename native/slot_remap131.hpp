#pragma once
#include "common.hpp"
#include <array>
namespace wht {
inline bool RemapControl131(const std::wstring& section,const std::wstring& key){
 return (section==L"SlotHotkeyShop"&&(key==L"Enable"||key.rfind(L"SlotCommandShop",0)==0))||
        (section==L"SlotHotkey"&&(key==L"Enable"||key.rfind(L"SlotCommand",0)==0||key.rfind(L"QuickCastCommand",0)==0||key.rfind(L"AutoTargetCommand",0)==0))||
        (section==L"SlotHotkeyInv"&&(key==L"Enable"||key.rfind(L"SlotInventory",0)==0||key.rfind(L"QuickCastInventory",0)==0||key.rfind(L"AutoTargetCommand",0)==0));
}
inline unsigned RemappedKey131(const Config& c,unsigned slot,bool shop=false){
 if(slot>=18)return 0;const bool inventory=slot>=12;const auto& b=inventory?c.inventory[slot-12]:shop?c.shop[slot]:c.command[slot];
 return (inventory?c.inventoryEnabled:shop?c.shopEnabled:c.commandEnabled)&&b.enabled?b.vk:0;
}
// Match 1.29's IsShop policy: normal unit orders select the hero grid;
// populated panels without those orders use shop/learn/book bindings.
template<class Data> bool AlternatePanel131(Data data){
 bool any=false;for(unsigned slot=0;slot<12;++slot){const unsigned order=data(slot);if(!order)continue;any=true;
  switch(order){case 0xd000f:case 0xd0016:case 0xd0012:case 0xd0004:case 0xd0019:return false;}}
 return any;
}
inline bool RemapCandidate131(const Config& c,unsigned key){
 if(!key)return false;
 if(c.inventoryEnabled)for(const auto& b:c.inventory)if(b.enabled&&b.vk==key)return true;
 if(c.commandEnabled)for(const auto& b:c.command)if(b.enabled&&b.vk==key)return true;
 if(c.shopEnabled)for(const auto& b:c.shop)if(b.enabled&&b.vk==key)return true;
 return false;
}
inline bool ResolveAlternate131(bool value){return value;}
template<class Get> bool ResolveAlternate131(Get get){return get();}
struct NoLearn131 {bool operator()(unsigned)const{return false;}};
struct SlotRemap131 {
 std::array<bool,256> pressed{};
 void Reset(){pressed.fill(false);}
 template<class Click,class Shop=bool,class Learn=NoLearn131> bool Input(unsigned message,unsigned key,bool repeat,bool allowed,bool modifiers,const Config& config,Click click,Shop getShop=false,Learn learn={}){
  if(key>=pressed.size())return false;
  if(message==WM_KEYUP||message==WM_SYSKEYUP){const bool used=pressed[key];pressed[key]=false;return used;}
  if(message!=WM_KEYDOWN)return false;
  if(pressed[key])return true;
  if(!allowed||modifiers||repeat||!RemapCandidate131(config,key))return false;
  const bool shop=ResolveAlternate131(getShop);
  // Inventory wins duplicate bindings, matching 1.29's input order.
  for(unsigned i=0;i<18;++i){const unsigned slot=i<6?i+12:i-6;
   if(RemappedKey131(config,slot,shop)==key&&click(slot)){pressed[key]=true;return true;}}
  if(!shop&&config.shopEnabled)for(unsigned slot=0;slot<11;++slot){const auto& b=config.shop[slot];if(b.enabled&&b.vk==key&&learn(slot)){pressed[key]=true;return true;}}
  return false;
 }
};
}
