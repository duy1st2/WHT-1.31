#include "../native/base_damage131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned calls=0;
#ifdef _WIN64
void Add(uintptr_t attack,int delta,unsigned mask){
#else
void __fastcall Add(uintptr_t attack,void*,int delta,unsigned mask){
#endif
 Check(attack==123&&delta==-10&&mask==1,"native attack receiver signed delta and weapon1 mask ABI");++calls;
}
int main(){try{
 for(const auto& p:wht::baseDamageProfiles131){
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
 Check(wht::BaseDamageCode131(base,p),"relocated attack guard");image[p.add]^=1;Check(!wht::BaseDamageCode131(base,p),"tampered native attack refused");
 }



 Check(!wht::BaseDamageProfileFor131(129),"unsupported profile refused");int damage=0;
 Check(wht::ParseBaseDamage131(L"",damage)&&damage==50,"default 50");Check(wht::ParseBaseDamage131(L"0",damage)&&damage==0&&wht::ParseBaseDamage131(L"1000000000",damage)&&damage==1000000000,"inclusive limits");
 for(const auto* bad:{L"-1",L"1000000001",L"1.5",L"2 extra",L"1\n",L"abc"})Check(!wht::ParseBaseDamage131(bad,damage),"malformed and out of range rejected");
 auto native=reinterpret_cast<wht::BaseDamageAdd131>(Add);Check(wht::InvokeBaseDamageAdd131(native,123,-10)&&calls==1,"native add ABI");Check(!wht::InvokeBaseDamageAdd131(native,0,-10)&&!wht::InvokeBaseDamageAdd131(nullptr,123,-10)&&calls==1,"invalid native parameters no calls");
 wht::BaseDamageSource131 target;target.unit=789;target.attack=123;target.world=456;target.id={1,2};target.local=0;target.damage=40;
 unsigned reads=0,writes=0;bool changed=false,refused=false,noOp=false;std::string report;
 auto read=[&](wht::BaseDamageSource131& out){out=target;if(++reads>=3&&changed)++out.id.low;return true;};
 auto add=[&](uintptr_t attack,int delta){Check(attack==123,"same attack receiver");++writes;if(refused)return false;if(!noOp)target.damage+=delta;return true;};
 Check(wht::BaseDamageCommand131(L"",read,add,report)&&writes==1&&target.damage==50,"default uses delta and reads actual");
 reads=0;unsigned old=writes;Check(wht::BaseDamageCommand131(L"50",read,add,report)&&writes==old,"already correct value no native mutation");
 reads=0;Check(wht::BaseDamageCommand131(L"0",read,add,report)&&target.damage==0,"negative delta reaches zero");
 target.damage=INT_MIN;reads=0;old=writes;Check(!wht::BaseDamageCommand131(L"1000000000",read,add,report)&&writes==old,"signed delta overflow refused before native call");
 target.damage=40;reads=0;changed=true;Check(!wht::BaseDamageCommand131(L"",read,add,report),"recycled unit after native update refused");
 changed=false;target.damage=40;reads=0;noOp=true;Check(!wht::BaseDamageCommand131(L"",read,add,report),"native no-op not reported successful");
 noOp=false;refused=true;reads=0;Check(!wht::BaseDamageCommand131(L"",read,add,report),"native refusal reported");
 old=writes;Check(!wht::BaseDamageCommand131(L"bad",read,add,report)&&writes==old,"malformed args no writes");
 auto unstable=[&](wht::BaseDamageSource131& out){out=target;if(++reads==2)++out.attack;return true;};reads=0;Check(!wht::BaseDamageCommand131(L"",unstable,add,report)&&writes==old,"replaced attack before update refused");
 auto varying=[&](wht::BaseDamageSource131& out){out=target;if(++reads==2)++out.damage;return true;};reads=0;Check(!wht::BaseDamageCommand131(L"",varying,add,report)&&writes==old,"changing base value before update refused");
 puts("base damage contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
