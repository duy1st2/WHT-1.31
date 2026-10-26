#pragma once
#include "common.hpp"
namespace wht {
struct NativeCastSlot {bool inventory=false;int slot=-1;};
inline bool ToolCastKeyAssigned(const Config& c,bool shop,unsigned vk){
    if(c.inventoryEnabled)for(const auto& b:c.inventory)if(b.enabled&&b.vk&&b.vk==vk)return true;
    const auto& bindings=shop?c.shop:c.command;
    if(shop?c.shopEnabled:c.commandEnabled)for(const auto& b:bindings)if(b.enabled&&b.vk&&b.vk==vk)return true;
    return false;
}
template<class NativeKey> NativeCastSlot FindNativeCastSlot(const Config& c,bool shop,unsigned vk,NativeKey key){
    // Only the new quick-cast mode extends to original game hotkeys. A failed
    // tool binding must not fall through and activate a different native slot.
    if(c.modeCast!=1||!vk||ToolCastKeyAssigned(c,shop,vk))return {};
    static const unsigned inventoryKeys[]={VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD4,VK_NUMPAD5,VK_NUMPAD1,VK_NUMPAD2};
    for(int i=0;i<6;++i)if(vk==inventoryKeys[i]&&key(true,i))return {true,i};
    for(int i=0;i<12;++i)if(key(false,i)==vk)return {false,i};
    return {};
}
}
