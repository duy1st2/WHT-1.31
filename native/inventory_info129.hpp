#pragma once
#include "skills129.hpp"
#include <array>
#include <string>
#include <cstring>
namespace wht {
struct InventoryInfo129 {unsigned capacity=0;std::array<uint32_t,6> codes{};};
inline bool ReadInventoryInfo129(uintptr_t base,InventoryInfo129& out){
 if(!base)return false;const auto selected=SelectedSkill129(base);
 if(!selected.unit||!selected.identity)return false;
 __try {
  const unsigned char slotSig[]={0x55,0x8b,0xec,0x8b,0x89,0x04,0x04,0,0};
  const unsigned char sizeSig[]={0x8b,0x89,0x04,0x04,0,0};
  const unsigned char hasSig[]={0x8b,0x81,0x64,0x01,0,0};
  if(memcmp(reinterpret_cast<void*>(base+0x291820),slotSig,sizeof(slotSig))||
     memcmp(reinterpret_cast<void*>(base+0x2960f0),sizeSig,sizeof(sizeSig))||
     memcmp(reinterpret_cast<void*>(base+0x27ec60),hasSig,sizeof(hasSig)))return false;
  const auto unit=selected.unit;if(*reinterpret_cast<uintptr_t*>(unit)!=base+0xa9c504)return false;
  using Has=int(__thiscall*)(uintptr_t);using Size=unsigned(__thiscall*)(uintptr_t);
  using Item=uintptr_t(__thiscall*)(uintptr_t,unsigned);
  out={};
  if(reinterpret_cast<Has>(base+0x27ec60)(unit)){
   out.capacity=reinterpret_cast<Size>(base+0x2960f0)(unit);if(out.capacity>6)return false;
   for(unsigned slot=0;slot<out.capacity;++slot){
    const auto item=reinterpret_cast<Item>(base+0x291820)(unit,slot);
    if(item)out.codes[slot]=*reinterpret_cast<uint32_t*>(item+0x34);
   }
  }
  const auto current=SelectedSkill129(base);return current.unit==unit&&current.identity==selected.identity;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string InventoryInfoText129(const InventoryInfo129& info){
 std::string text="Item Slots: ";bool found=false;
 for(unsigned slot=0;slot<info.codes.size();++slot)if(info.codes[slot]){
  char raw[5]{};for(unsigned i=0;i<4;++i){auto ch=(info.codes[slot]>>(24-8*i))&255;raw[i]=ch>=32&&ch<=126?char(ch):'?';}
  text+="["+std::to_string(slot)+": "+raw+"] ";found=true;
 }
 if(!found)text+="[Empty]";return text;
}
}
