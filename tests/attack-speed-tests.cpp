#include "../native/attack_speed131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned setCalls=0,refreshCalls=0;
#ifdef _WIN64
void Set(uintptr_t property,const float* factor,int policy){
#else
void __fastcall Set(uintptr_t property,void*,const float* factor,int policy){
#endif
 Check(property==123&&factor&&*factor==2&&policy==1,"bounded real setter receiver pointer and policy ABI");++setCalls;
}
#ifdef _WIN64
void Refresh(uintptr_t attack,uintptr_t unit){
#else
void __fastcall Refresh(uintptr_t attack,void*,uintptr_t unit){
#endif
 Check(attack==456&&unit==789,"native animation refresh attack receiver and unit ABI");++refreshCalls;
}
int main(){try{
 for(const auto& p:wht::attackSpeedProfiles131){
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
 Check(wht::AttackSpeedCode131(base,p),"relocated attack guard");image[p.set]^=1;Check(!wht::AttackSpeedCode131(base,p),"tampered native attack refused");
 }


 Check(!wht::AttackSpeedProfileFor131(129),"unsupported profile refused");float factor=0;
 Check(wht::ParseAttackSpeed131(L"",factor)&&factor==2,"default multiplier2");Check(wht::ParseAttackSpeed131(L"0",factor)&&factor==.1f&&wht::ParseAttackSpeed131(L"51",factor)&&factor==50,"reference clamping");
 for(const auto* bad:{L"nan",L"inf",L"2 extra",L"1\n",L"abc"})Check(!wht::ParseAttackSpeed131(bad,factor),"malformed/nonfinite rejected");
 Check(wht::InvokeAttackSpeedSet131(reinterpret_cast<wht::AttackSpeedSet131>(Set),123,2)&&setCalls==1,"native setter ABI");Check(wht::InvokeAttackSpeedRefresh131(reinterpret_cast<wht::AttackSpeedRefresh131>(Refresh),456,789)&&refreshCalls==1,"native refresh ABI");
 Check(!wht::InvokeAttackSpeedSet131(reinterpret_cast<wht::AttackSpeedSet131>(Set),0,2)&&!wht::InvokeAttackSpeedSet131(reinterpret_cast<wht::AttackSpeedSet131>(Set),123,NAN)&&!wht::InvokeAttackSpeedRefresh131(reinterpret_cast<wht::AttackSpeedRefresh131>(Refresh),456,0)&&setCalls==1&&refreshCalls==1,"invalid native args no calls");
 wht::AttackSpeedSource131 target;target.unit=789;target.attack=456;target.world=111;target.id={1,2};target.local=0;target.factor=1;target.minimum=.2f;target.maximum=1.5f;
 unsigned reads=0,writes=0,updates=0;bool changed=false,refused=false,accepted=true,refreshAccepted=true;std::string report;
 auto read=[&](wht::AttackSpeedSource131& out){out=target;if(++reads>=3){out.factor=accepted?2.f:1.f;if(changed)++out.id.low;}return true;};
 auto set=[&](uintptr_t attack,float value){Check(attack==456&&value==2,"command selected attack and default");++writes;return !refused;};
 auto refresh=[&](uintptr_t attack,uintptr_t unit){Check(attack==456&&unit==789,"refresh same source");++updates;return refreshAccepted;};
 Check(wht::AttackSpeedCommand131(L"",read,set,refresh,report)&&writes==1&&updates==1&&report.find("2.00x")!=std::string::npos&&report.find("1.50x")!=std::string::npos,"requested multiplier and effective map cap reported after refresh");
 reads=0;changed=true;unsigned prior=updates;Check(!wht::AttackSpeedCommand131(L"",read,set,refresh,report)&&updates==prior,"recycled identity before refresh refused");
 reads=0;changed=false;accepted=false;Check(!wht::AttackSpeedCommand131(L"",read,set,refresh,report)&&updates==prior,"native no-op does not refresh or claim success");
 reads=0;accepted=true;refreshAccepted=false;Check(!wht::AttackSpeedCommand131(L"",read,set,refresh,report),"refresh refusal reported");
 reads=0;refused=true;Check(!wht::AttackSpeedCommand131(L"",read,set,refresh,report),"setter refusal reported");
 unsigned old=writes;Check(!wht::AttackSpeedCommand131(L"bad",read,set,refresh,report)&&writes==old,"malformed args no mutation");
 auto unstable=[&](wht::AttackSpeedSource131& out){out=target;if(++reads==2)++out.attack;return true;};reads=0;Check(!wht::AttackSpeedCommand131(L"",unstable,set,refresh,report)&&writes==old,"replaced attack before update no writes");
 target.maximum=NAN;reads=0;Check(!wht::AttackSpeedCommand131(L"",read,set,refresh,report)&&writes==old,"invalid map limits refused");
 puts("attack speed contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
