#pragma once
#include "goku_pick131.hpp"
#include "notification131.hpp"
#include "quick_drop_network131.hpp"
namespace wht {
struct InventoryInfo131 {unsigned capacity=0;std::array<uint32_t,6> codes{};};
struct InventoryInfoSource131 {uintptr_t unit=0;DropObjectId id{};};
template<class Selected,class Read> bool CaptureInventoryInfo131(Selected selected,Read read,InventoryInfo131& out){
 const auto before=selected();if(!before.unit||!before.id.Valid())return false;InventoryInfo131 snapshot;
 if(!read(before.unit,snapshot)||snapshot.capacity>6)return false;
 const auto after=selected();if(before.unit!=after.unit||!(before.id==after.id))return false;
 for(unsigned i=snapshot.capacity;i<6;++i)if(snapshot.codes[i])return false;
 out=snapshot;return true;
}
template<class Selected> bool ReadInventoryInfo131(uintptr_t base,unsigned build,Selected selected,InventoryInfo131& out){
 const auto* p=InventoryProfile(build);if(!p)return false;const auto api=InventoryNative131(base,*p);if(!api||!ItemCodeGuard131(base,*p))return false;
 auto source=[&](){InventoryInfoSource131 result;const auto unit=selected();if(InventoryIdentity131(unit,base,p->unitTable)){result.unit=unit;result.id=DropIdentity131(unit);}return result;};
 auto read=[&](uintptr_t unit,InventoryInfo131& snapshot){
  __try{const auto inventory=*reinterpret_cast<uintptr_t*>(unit+p->inventoryOffset);if(!inventory)return true;if(!InventoryIdentity131(inventory,base,p->inventoryTable))return false;
   snapshot.capacity=api.size(unit);if(snapshot.capacity>6)return false;
   for(unsigned i=0;i<snapshot.capacity;++i){const auto item=api.item(unit,i);if(!item)continue;if(!InventoryIdentity131(item,base,p->itemTable))return false;const auto code=ItemCodeValue131(base,*p,item);if(!code)return false;snapshot.codes[i]=code;}return true;
  }__except(1){return false;}
 };
 return CaptureInventoryInfo131(source,read,out);
}
inline std::string InventoryInfoText131(const InventoryInfo131& info,bool english){
 std::string text=english?"Item Slots: ":Utf8(L"Slot v\u1eadt ph\u1ea9m: ");bool found=false;
 for(unsigned slot=0;slot<info.codes.size();++slot)if(info.codes[slot]){char raw[5]{};for(unsigned i=0;i<4;++i){const auto c=(info.codes[slot]>>(24-8*i))&255;raw[i]=c>=32&&c<=126?char(c):'?';}text+="["+std::to_string(slot)+": "+raw+"] ";found=true;}
 if(!found)text+=english?"[Empty]":Utf8(L"[Tr\u1ed1ng]");return text;
}
struct LocalInfo131 {
 std::string text,applied;uint64_t until=0;void* frame=nullptr;void* label=nullptr;bool shown=false,published=false;
 void Reset(){*this={};}
 void Event(std::string value,uint64_t now){published=false;text=value.size()<=512?std::move(value):std::string{};until=text.empty()?0:now+4000;}
 bool Visible(bool allowed,bool menu,uint64_t now){if(!allowed||now>=until){text.clear();until=0;}return !text.empty()&&!menu;}
 void Update(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,ToolMenuOverlay& menu,uint64_t now){
  if(Visible(allowed,menu.open,now)&&!published){published=true;ShowNotification131(base,build,ui,text);}
 }

};
}
