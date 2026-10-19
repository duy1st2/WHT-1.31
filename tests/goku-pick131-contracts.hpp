#pragma once
#include "goku_pick131.hpp"
#include <fstream>
inline void CheckGokuPick131(){
 const uintptr_t base=0x10000000;
 for(const auto& p:wht::inventoryProfiles131){alignas(uintptr_t) unsigned char item[0x100]{},unit[0x700]{};
  const auto raw=reinterpret_cast<uintptr_t>(item);*reinterpret_cast<uintptr_t*>(item)=base+p.itemTable;*reinterpret_cast<uintptr_t*>(unit)=base+p.unitTable;
  *reinterpret_cast<uint32_t*>(item+(sizeof(uintptr_t)==8?0x58:0x34))=wht::gokuType131;
  check(wht::ItemCodeValue131(base,p,raw)==0x49303136,"native rawcode preserves I016 byte order");
  for(auto& entry:dropTestItems)entry=0;dropTestItems[5]=raw;
  wht::InventoryApi131 api;api.item=reinterpret_cast<decltype(api.item)>(DropTestItem);api.size=reinterpret_cast<decltype(api.size)>(DropTestSize);api.drop=reinterpret_cast<decltype(api.drop)>(DropTestDrop);
  check(wht::InventoryHasType131(base,p,api,reinterpret_cast<uintptr_t>(unit),6,wht::gokuType131),"full inventory last slot confirms Goku completion");
  check(!wht::InventoryHasType131(base,p,api,reinterpret_cast<uintptr_t>(unit),5,wht::gokuType131),"only actual inventory capacity inspected");
  *reinterpret_cast<uint32_t*>(item+(sizeof(uintptr_t)==8?0x58:0x34))=0x49303137;
  check(!wht::InventoryHasType131(base,p,api,reinterpret_cast<uintptr_t>(unit),6,wht::gokuType131),"other map item never completes Goku mode");
  *reinterpret_cast<uintptr_t*>(item)=0;check(!wht::ItemCodeValue131(base,p,raw),"unrelated object class refused");
 }
 for(auto& entry:dropTestItems)entry=0;
 for(const auto& p:wht::itemCodeProfiles131){
#ifdef _WIN64
  const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);if(!input)continue;std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});
  const auto image=reinterpret_cast<uintptr_t>(memory.data());const auto* inventory=wht::InventoryProfile(p.build);check(inventory!=nullptr,"inventory profile accompanies code profile");
  *reinterpret_cast<uintptr_t*>(memory.data()+inventory->itemTable+37*sizeof(uintptr_t))=image+p.constructor;
  check(wht::ItemCodeGuard131(image,*inventory),"captured native item constructor code and table guard");
  memory[p.constructor]^=1;check(!wht::ItemCodeGuard131(image,*inventory),"changed native code refused");
 }
 wht::GokuIcon131 icon;icon.Reset();check(icon.area.right==.305f&&icon.area.bottom==.142f&&icon.area.width==.038f,"Goku button uses reference first MTH toolbar position");
 icon.shown=true;unsigned toggles=0;check(icon.Mouse(WM_LBUTTONDOWN,{357,669},1000,900,true,[&](){++toggles;})&&toggles==1,"Goku button input toggles once");
 icon.Mouse(WM_LBUTTONUP,{357,669},1000,900,true,[&](){++toggles;});
 check(icon.Mouse(WM_LBUTTONDBLCLK,{357,669},1000,900,true,[&](){++toggles;})&&toggles==1,"double click does not toggle mode twice");
}
