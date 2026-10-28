#include "../native/kill_unit131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned calls=0;float zero=0;
#ifdef _WIN64
void Kill(uintptr_t unit,const float* value){
#else
void __fastcall Kill(uintptr_t unit,void*,const float* value){
#endif
 Check(unit==123&&value==&zero&&*value==0,"native CUnit receiver and exact zero pointer ABI");++calls;
}
int main(){try{
 for(const auto& p:wht::killUnitProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"captured fixture available");std::array<unsigned char,4096> header{};input.read(reinterpret_cast<char*>(header.data()),header.size());unsigned pe=0;memcpy(&pe,header.data()+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header.data()+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t imageSize=0;for(size_t i=0;i<p.count;++i)imageSize=(std::max)(imageSize,size_t(p.code[i].rva+p.code[i].length));imageSize=(std::max)(imageSize,size_t(p.unit+0x260));imageSize=(std::max)(imageSize,size_t(p.zero+4));std::vector<unsigned char> image(imageSize+64);const auto base=reinterpret_cast<uintptr_t>(image.data());
 for(size_t i=0;i<p.count;++i){const auto& code=p.code[i];std::vector<unsigned char> bytes(code.length);input.seekg(code.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"captured attack block read");
 for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v-=uint32_t(capturedBase);memcpy(bytes.data()+code.reloc[j],&v,4);}Check(!memcmp(bytes.data(),code.bytes,code.length),"captured attack and hero-bar caller signatures");
 memcpy(image.data()+code.rva,bytes.data(),bytes.size());for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v+=uint32_t(base);memcpy(image.data()+code.rva+code.reloc[j],&v,4);}}
 *reinterpret_cast<uintptr_t*>(base+p.unit+(sizeof(uintptr_t)==8?0x248:0x124))=base+p.kill;
 Check(wht::KillUnitCode131(base,p),"relocated attack guard");image[p.kill]^=1;Check(!wht::KillUnitCode131(base,p),"tampered native kill refused");image[p.kill]^=1;image[p.zero]=1;Check(!wht::KillUnitCode131(base,p),"altered native zero refused");image[p.zero]=0;*reinterpret_cast<uintptr_t*>(base+p.unit+(sizeof(uintptr_t)==8?0x248:0x124))+=1;Check(!wht::KillUnitCode131(base,p),"foreign CUnit virtual method refused");
 }




 Check(!wht::KillUnitProfileFor131(129),"unsupported build refused");auto native=reinterpret_cast<wht::KillUnitNative131>(Kill);Check(wht::InvokeKillUnit131(native,123,&zero)&&calls==1,"native zero pointer ABI");float negativeZero=-0.f,nonzero=1;
 Check(!wht::InvokeKillUnit131(native,0,&zero)&&!wht::InvokeKillUnit131(native,123,&negativeZero)&&!wht::InvokeKillUnit131(native,123,&nonzero)&&calls==1,"invalid receiver or engine-zero value refused");
 wht::KillUnitSource131 target;target.unit=123;target.world=456;target.id={1,2};target.local=0;target.life=100;
 unsigned reads=0,writes=0;std::string report;bool refused=false;
 auto read=[&](wht::KillUnitSource131& out){++reads;out=target;return true;};auto kill=[&](uintptr_t unit){Check(unit==123,"same selected unit");++writes;if(refused)return false;target.unit=0;return true;};
 Check(wht::KillUnitCommand131(L"",read,kill,report)&&reads==2&&writes==1,"one native dispatch and no post-death dereference");
 unsigned old=writes;Check(!wht::KillUnitCommand131(L"extra",read,kill,report)&&writes==old,"invalid args no dispatch");
 target.unit=123;target.life=0;Check(!wht::KillUnitCommand131(L"",read,kill,report)&&writes==old,"already dead unit refused");target.life=NAN;Check(!wht::KillUnitCommand131(L"",read,kill,report)&&writes==old,"invalid HP refused");
 target.life=100;auto unstable=[&](wht::KillUnitSource131& out){out=target;if(++reads==2)++out.id.low;return true;};reads=0;Check(!wht::KillUnitCommand131(L"",unstable,kill,report)&&writes==old,"recycled selection before native call refused");
 refused=true;Check(!wht::KillUnitCommand131(L"",read,kill,report),"native refusal reported");
 puts("kill unit contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
