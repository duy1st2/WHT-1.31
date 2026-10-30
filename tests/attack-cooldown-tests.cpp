#include "../native/attack_cooldown131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned sets=0,gets=0;
#ifdef _WIN64
void Set(uintptr_t attack,const float* value,unsigned weapon){
 Check(value&&*value==.25f,"Win64 setter receives real pointer");
#else
void __fastcall Set(uintptr_t attack,void*,float value,unsigned weapon){
 Check(value==.25f,"x86 setter receives float by value on stack");
#endif
 Check(attack==123&&weapon==1,"native setter attack receiver and zero-based weapon2");++sets;
}
#ifdef _WIN64
float* Get(uintptr_t attack,float* out,unsigned weapon){
#else
float* __fastcall Get(uintptr_t attack,void*,float* out,unsigned weapon){
#endif
 Check(attack==123&&weapon==1&&out,"native getter output pointer and weapon ABI");++gets;*out=.25f;return out;
}
int main(){try{
 for(const auto& p:wht::attackCooldownProfiles131){
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
 Check(wht::AttackCooldownCode131(base,p),"relocated attack guard");image[p.set]^=1;Check(!wht::AttackCooldownCode131(base,p),"tampered native attack refused");
 }


 Check(!wht::AttackCooldownProfileFor131(129),"unsupported build refused");
 float seconds=0;unsigned weapon=99;
 Check(wht::ParseAttackCooldown131(L"0.25",seconds,weapon)&&seconds==.25f&&weapon==0,"default weapon1");
 Check(wht::ParseAttackCooldown131(L" 3600 2 ",seconds,weapon)&&seconds==3600&&weapon==1,"weapon2 and upper bound");
 Check(wht::ParseAttackCooldown131(L"0.01 1",seconds,weapon)&&weapon==0,"minimum cooldown");
 for(const auto* bad:{L"",L"0",L"-1",L"0.009",L"3601",L"nan",L"inf",L"1 0",L"1 3",L"1 1.5",L"1 2 extra",L"1\n2",L"1\r"})Check(!wht::ParseAttackCooldown131(bad,seconds,weapon),"malformed cooldown rejected");
 Check(!wht::ParseAttackCooldown131(std::wstring(L"1\0 2",5),seconds,weapon),"embedded NUL rejected");
 Check(wht::InvokeAttackCooldownSet131(reinterpret_cast<wht::AttackCooldownSet131>(Set),123,1,.25f)&&sets==1,"typed setter ABI");
 float actual=0;Check(wht::InvokeAttackCooldownGet131(reinterpret_cast<wht::AttackCooldownGet131>(Get),123,1,actual)&&gets==1&&actual==.25f,"typed getter ABI and readback");
 Check(!wht::InvokeAttackCooldownSet131(reinterpret_cast<wht::AttackCooldownSet131>(Set),123,2,.25f)&&!wht::InvokeAttackCooldownSet131(reinterpret_cast<wht::AttackCooldownSet131>(Set),123,1,NAN)&&sets==1,"bad arguments never call native");
 wht::AttackCooldownSource131 state;state.selected.unit=1;state.selected.attack=2;state.selected.world=3;state.selected.local=0;state.selected.id={4,5};state.seconds=1;
 unsigned reads=0,writes=0;float periods[2]={1,2};bool missing=false,noOp=false,replace=false;std::string report;
 auto read=[&](unsigned slot,wht::AttackCooldownSource131& out){++reads;if(missing)return false;out=state;out.weapon=slot;out.seconds=periods[slot];if(replace&&reads>1)out.selected.id={4,6};return true;};
 auto set=[&](uintptr_t attack,unsigned slot,float value){Check(attack==2&&slot<2,"selected native receiver");++writes;if(!noOp)periods[slot]=value;return true;};
 Check(wht::AttackCooldownCommand131(L".25 2",read,set,report)&&writes==1&&periods[0]==1&&periods[1]==.25f&&report.find("weapon 2")!=report.npos,"weapon2 only and confirmed feedback");
 Check(wht::AttackCooldownCommand131(L".5",read,set,report)&&periods[0]==.5f&&periods[1]==.25f,"default weapon1 does not modify weapon2");
 const auto oldWrites=writes,oldReads=reads;Check(!wht::AttackCooldownCommand131(L"1 3",read,set,report)&&reads==oldReads&&writes==oldWrites,"invalid input before any game read/write");
 missing=true;Check(!wht::AttackCooldownCommand131(L"1",read,set,report)&&writes==oldWrites,"no selected attack no write");missing=false;
 reads=0;replace=true;Check(!wht::AttackCooldownCommand131(L"1",read,set,report)&&writes==oldWrites,"recycled selection before write refused");replace=false;
 noOp=true;Check(!wht::AttackCooldownCommand131(L"1",read,set,report)&&report.find("did not confirm")!=report.npos,"native no-op not reported successful");noOp=false;
 auto changedAfter=[&](unsigned slot,wht::AttackCooldownSource131& out){if(!read(slot,out))return false;if(reads>2)out.selected.attack=99;return true;};reads=0;
 Check(!wht::AttackCooldownCommand131(L".75",changedAfter,set,report)&&report.find("did not confirm")!=report.npos,"changed attack after native call detected");
 puts("Attack cooldown profiles, typed native ABI, parser, selected identity and per-weapon readback passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
