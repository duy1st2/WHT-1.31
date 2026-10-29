#pragma once
#include <windows.h>
#include <cstdint>
#include <cstring>
namespace wht {
// Identify the native implementation, not the map's item/ability rawcode.
// Scripted/channel abilities and ambiguous regeneration remain unclassified.
inline unsigned PotionClassCapability(const char* name){
 if(!name)return 0;
 if(!strcmp(name,".?AVCAbilityItemHeal@@")||!strcmp(name,".?AVCAbilityItemHealAoe@@"))return 1;
 if(!strcmp(name,".?AVCAbilityItemManaRestore@@")||!strcmp(name,".?AVCAbilityItemManaRestoreAoe@@"))return 2;
 if(!strcmp(name,".?AVCAbilityItemRestore@@")||!strcmp(name,".?AVCAbilityItemRestoreAoe@@"))return 3;
 return 0;
}
inline unsigned NativePotionCapability(uintptr_t base,uintptr_t ability){
 if(!base||!ability)return 0;
 __try {
  const auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
  if(dos->e_magic!=IMAGE_DOS_SIGNATURE||dos->e_lfanew<=0||dos->e_lfanew>0x100000)return 0;
  const auto pe=reinterpret_cast<const IMAGE_NT_HEADERS*>(base+dos->e_lfanew);
  if(pe->Signature!=IMAGE_NT_SIGNATURE)return 0;
  const auto size=pe->OptionalHeader.SizeOfImage;if(size<4096||size>0x40000000)return 0;
  const auto end=base+size;const auto vt=*reinterpret_cast<const uintptr_t*>(ability);
  if(vt<base+sizeof(uintptr_t)||vt>=end)return 0;
  const auto col=*reinterpret_cast<const uintptr_t*>(vt-sizeof(uintptr_t));
  if(col<base||col>end-24)return 0;
#ifdef _WIN64
  if(*reinterpret_cast<const unsigned*>(col)!=1)return 0;
  if(base+*reinterpret_cast<const uint32_t*>(col+20)!=col)return 0;
  const auto type=base+*reinterpret_cast<const uint32_t*>(col+12);constexpr unsigned prefix=16;
#else
  if(*reinterpret_cast<const unsigned*>(col)!=0)return 0;
  const auto type=*reinterpret_cast<const uintptr_t*>(col+12);constexpr unsigned prefix=8;
#endif
  if(type<base||type>end-prefix-96)return 0;
  char name[96]{};bool terminated=false;
  for(unsigned i=0;i<sizeof(name);++i){name[i]=*reinterpret_cast<const char*>(type+prefix+i);if(!name[i]){terminated=true;break;}}
  return terminated?PotionClassCapability(name):0;
 }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
}
