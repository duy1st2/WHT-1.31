#include "../native/remove_ability131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned removeCalls=0,refreshCalls=0;
#ifdef _WIN64
void Remove(uintptr_t unit,uintptr_t ability){
#else
void __fastcall Remove(uintptr_t unit,void*,uintptr_t ability){
#endif
 Check(unit==123&&ability==789,"native unit/ability removal ABI");++removeCalls;
}
void __cdecl Refresh(uintptr_t unit){Check(unit==123,"native cdecl unit panel refresh ABI");++refreshCalls;}
int main(){try{
 for(const auto& p:wht::removeAbilityProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"captured fixture available");std::array<unsigned char,4096> header{};input.read(reinterpret_cast<char*>(header.data()),header.size());unsigned pe=0;memcpy(&pe,header.data()+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header.data()+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t imageSize=0;for(size_t i=0;i<p.count;++i)imageSize=(std::max)(imageSize,size_t(p.code[i].rva+p.code[i].length));std::vector<unsigned char> image(imageSize+64);const auto base=reinterpret_cast<uintptr_t>(image.data());
 for(size_t i=0;i<p.count;++i){const auto& code=p.code[i];std::vector<unsigned char> bytes(code.length);input.seekg(code.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"captured attack block read");
 for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v-=uint32_t(capturedBase);memcpy(bytes.data()+code.reloc[j],&v,4);}Check(!memcmp(bytes.data(),code.bytes,code.length),"captured attack and hero-bar caller signatures");
 memcpy(image.data()+code.rva,bytes.data(),bytes.size());for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v+=uint32_t(base);memcpy(image.data()+code.rva+code.reloc[j],&v,4);}}
 Check(wht::RemoveAbilityCode131(base,p),"relocated attack guard");image[p.remove]^=1;Check(!wht::RemoveAbilityCode131(base,p),"tampered native attack refused");
 }




 Check(!wht::RemoveAbilityProfileFor131(129),"unsupported profile refused");uint32_t code=0;
 Check(wht::ParseRemoveAbility131(L"AHhb",code)&&code==0x41486862,"four byte rawcode");Check(wht::ParseRemoveAbility131(L"0x41486862",code)&&code==0x41486862,"hex rawcode");
 for(const auto* bad:{L"",L"ABC",L"ABCDE",L"0x41",L"0x00000000",L"0xGGGGGGGG",L"AHhb extra",L"AHhb\n"})Check(!wht::ParseRemoveAbility131(bad,code),"malformed rawcode refused");
 Check(wht::InvokeRemoveAbility131(reinterpret_cast<wht::RemoveAbilityNative131>(Remove),123,789)&&removeCalls==1,"native remove ABI");Check(wht::InvokeRefreshAbility131(Refresh,123)&&refreshCalls==1,"native refresh ABI");Check(!wht::InvokeRemoveAbility131(reinterpret_cast<wht::RemoveAbilityNative131>(Remove),123,0)&&!wht::InvokeRefreshAbility131(Refresh,0)&&removeCalls==1&&refreshCalls==1,"invalid native args no call");
 wht::RemoveAbilitySource131 target;target.unit=123;target.world=456;target.id={1,2};target.local=0;target.ability=789;target.abilityId={3,4};target.abilityTable=1000;
 unsigned reads=0,writes=0,updates=0;bool changed=false,noOp=false,refused=false;std::string report;
 auto read=[&](uint32_t raw,wht::RemoveAbilitySource131& out){Check(raw==0x41486862,"same requested code");out=target;if(++reads>=3&&changed)++out.id.low;return true;};
 auto remove=[&](uintptr_t unit,uintptr_t ability){Check(unit==123&&ability==789,"same stable ability");++writes;if(refused)return false;if(!noOp){target.ability=0;target.abilityId={};target.abilityTable=0;}return true;};auto refresh=[&](uintptr_t unit){Check(unit==123,"refresh stable unit");++updates;return true;};
 Check(wht::RemoveAbilityCommand131(L"AHhb",read,remove,refresh,report)&&writes==1&&updates==1&&reads==4&&report=="Removed Ability: AHhb","remove then absence check refresh and final confirmation");reads=0;unsigned old=writes;Check(wht::RemoveAbilityCommand131(L"AHhb",read,remove,refresh,report)&&writes==old&&report.find("already absent")!=std::string::npos,"absent skill no mutation");
 target.ability=789;target.abilityId={3,4};target.abilityTable=1000;changed=true;reads=0;unsigned oldUpdates=updates;Check(!wht::RemoveAbilityCommand131(L"AHhb",read,remove,refresh,report)&&updates==oldUpdates,"changed unit after remove prevents panel refresh");
 changed=false;target.ability=789;noOp=true;reads=0;Check(!wht::RemoveAbilityCommand131(L"AHhb",read,remove,refresh,report)&&updates==oldUpdates,"native no-op not success");noOp=false;refused=true;reads=0;Check(!wht::RemoveAbilityCommand131(L"AHhb",read,remove,refresh,report),"native removal refusal");
 old=writes;Check(!wht::RemoveAbilityCommand131(L"bad",read,remove,refresh,report)&&writes==old,"malformed input no writes");auto unstable=[&](uint32_t,wht::RemoveAbilitySource131& out){out=target;if(++reads==2)++out.abilityId.low;return true;};reads=0;Check(!wht::RemoveAbilityCommand131(L"AHhb",unstable,remove,refresh,report)&&writes==old,"recycled ability before dispatch refused");
 std::vector<unsigned char> image(0x4000);const auto base=reinterpret_cast<uintptr_t>(image.data());*reinterpret_cast<uint32_t*>(base+60)=0x80;*reinterpret_cast<uint32_t*>(base+0x80+24+56)=0x4000;alignas(uintptr_t) unsigned char ability[0x60]{};const uintptr_t a=reinterpret_cast<uintptr_t>(ability);*reinterpret_cast<uintptr_t*>(ability)=base+0x2000;wht::DropObjectId id{5,6};memcpy(ability+(sizeof(uintptr_t)==8?0x18:0xc),&id,sizeof(id));uintptr_t table=0;wht::DropObjectId got;
 Check(wht::ReadRemovalAbilityIdentity131(base,a,table,got)&&table==base+0x2000&&got==id,"native query ability class/serial snapshot");*reinterpret_cast<uintptr_t*>(ability)=base+0x5000;Check(!wht::ReadRemovalAbilityIdentity131(base,a,table,got),"foreign ability class refused");
 puts("remove ability contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
