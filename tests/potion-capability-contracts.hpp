#pragma once
#include "auto_heal.hpp"
#include "potion_capability.hpp"
template<class Check> void TestPotionCapability(Check check){
 using namespace wht;
 check(PotionClassCapability(".?AVCAbilityItemHeal@@")==1,"native heal capability");
 check(PotionClassCapability(".?AVCAbilityItemManaRestore@@")==2,"native mana capability");
 check(PotionClassCapability(".?AVCAbilityItemRestore@@")==3,"native mixed potion capability");
 check(!PotionClassCapability(".?AVCAbilityManaShield@@")&&!PotionClassCapability(".?AVCAbilityRegeneration@@"),"unrelated or ambiguous ability not guessed");
 AutoPotionSample sample;sample.identity=42;sample.value=1;sample.maximum=100;
 sample.items[0]=0x49303030;sample.items[3]=0x49393939;sample.detectedMask=9;sample.unavailableMask=1;
 AutoPotionState state;unsigned uses=0;std::vector<uint32_t> empty;
 check(state.Tick(true,true,40,empty,0,[&](AutoPotionSample& out){out=sample;return true;},[&](uint64_t id,unsigned slot,uint32_t code){++uses;return id==42&&slot==3&&code==0x49393939;},true),"custom rawcode discovered; unavailable first slot skipped");
 check(uses==1,"one use per tick");
 AutoPotionState unknown;sample.detectedMask=0;sample.unavailableMask=0;
 check(!unknown.Tick(true,true,40,empty,0,[&](AutoPotionSample& out){out=sample;return true;},[&](uint64_t,unsigned,uint32_t){++uses;return true;},true)&&uses==1,"unknown custom items never trial clicked");
 check(!NativePotionCapability(0,0)&&!NativePotionCapability(1,1),"invalid native image refused");
 std::vector<unsigned char> image(4096);const auto base=reinterpret_cast<uintptr_t>(image.data());
 auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);dos->e_magic=IMAGE_DOS_SIGNATURE;dos->e_lfanew=128;
 auto pe=reinterpret_cast<IMAGE_NT_HEADERS*>(base+128);pe->Signature=IMAGE_NT_SIGNATURE;pe->OptionalHeader.SizeOfImage=4096;
 const uintptr_t vt=base+1024,col=base+1200,type=base+1400;
 *reinterpret_cast<uintptr_t*>(vt-sizeof(uintptr_t))=col;
#ifdef _WIN64
 *reinterpret_cast<unsigned*>(col)=1;*reinterpret_cast<unsigned*>(col+12)=1400;*reinterpret_cast<unsigned*>(col+20)=1200;constexpr unsigned prefix=16;
#else
 *reinterpret_cast<uintptr_t*>(col+12)=type;constexpr unsigned prefix=8;
#endif
 strcpy_s(reinterpret_cast<char*>(type+prefix),96,".?AVCAbilityItemManaRestore@@");
 uintptr_t ability=vt;
 check(NativePotionCapability(base,reinterpret_cast<uintptr_t>(&ability))==2,"RTTI read supports custom mana implementation");
 *reinterpret_cast<uintptr_t*>(vt-sizeof(uintptr_t))=1;
 check(!NativePotionCapability(base,reinterpret_cast<uintptr_t>(&ability)),"invalid RTTI refused");
}
