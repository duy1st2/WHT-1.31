#include "../native/reselect131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned nativeCalls=0;
#ifdef _WIN64
void Native(uintptr_t manager,const wht::NativeUnitList131* list,unsigned a,unsigned b,unsigned c,unsigned d,unsigned e){
#else
void __fastcall Native(uintptr_t manager,void*,const wht::NativeUnitList131* list,unsigned a,unsigned b,unsigned c,unsigned d,unsigned e){
#endif
 Check(manager==123&&list&&list->count<=1&&a==0&&b==1&&!c&&!d&&!e,"native list ABI and replace policy");++nativeCalls;
}
int main(){try{
 for(const auto& p:wht::craftSelectionProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"captured fixture available");std::array<unsigned char,4096> header{};input.read(reinterpret_cast<char*>(header.data()),header.size());unsigned pe=0;memcpy(&pe,header.data()+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header.data()+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 std::vector<unsigned char> image(p.select+4096);const auto base=reinterpret_cast<uintptr_t>(image.data());
 for(size_t i=0;i<p.count;++i){const auto& code=p.code[i];std::vector<unsigned char> bytes(code.length);input.seekg(code.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"captured selection block read");
 for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v-=uint32_t(capturedBase);memcpy(bytes.data()+code.reloc[j],&v,4);}Check(!memcmp(bytes.data(),code.bytes,code.length),"captured selection and hero-bar caller signatures");
 memcpy(image.data()+code.rva,bytes.data(),bytes.size());for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v+=uint32_t(base);memcpy(image.data()+code.rva+code.reloc[j],&v,4);}}
 Check(wht::CraftSelectionGuard131(base,p),"relocated selection guard");image[p.select]^=1;Check(!wht::CraftSelectionGuard131(base,p),"tampered native selection refused");
 }
 wht::NativeUnitList131 list{0,nullptr};wht::InvokeCraftSelect131(reinterpret_cast<wht::CraftSelect131>(Native),123,list);uintptr_t unit=456;list={1,&unit};wht::InvokeCraftSelect131(reinterpret_cast<wht::CraftSelect131>(Native),123,list);Check(nativeCalls==2,"empty clear and one-unit selection native ABI");
 const wht::ReselectSource131 target{456,{1,2}};wht::ReselectSource131 current=target;bool live=true,fail=false,confirm=true;unsigned writes=0,restores=0;std::string report;
 auto read=[&](){return current;};auto valid=[&](const wht::ReselectSource131& value){return live&&wht::SameReselect131(value,target);};
 auto select=[&](const wht::ReselectSource131* value){++writes;if(fail)return false;current=value&&confirm?*value:wht::ReselectSource131{};return true;};auto restore=[&](){++restores;};
 Check(!wht::ReselectFlow131(L"extra",read,valid,select,restore,report)&&!writes,"bad args no writes");
 Check(wht::ReselectFlow131(L"",read,valid,select,restore,report)&&writes==2&&!restores,"clear then reselect and confirm");
 fail=true;Check(!wht::ReselectFlow131(L"",read,valid,select,restore,report)&&restores==1,"clear refusal restores");fail=false;confirm=false;current=target;
 Check(!wht::ReselectFlow131(L"",read,valid,select,restore,report)&&restores==2,"native no confirmation never reports success");current=target;live=false;unsigned before=writes;
 Check(!wht::ReselectFlow131(L"",read,valid,select,restore,report)&&writes==before,"stale source no writes");
 current=target;live=true;before=writes;unsigned recovery=restores;
 auto ignoredClear=[&](const wht::ReselectSource131*){++writes;return true;};
 Check(!wht::ReselectFlow131(L"",read,valid,ignoredClear,restore,report)&&writes==before+1&&restores==recovery+1,"native clear no-op rejected before reselection");
 puts("reselect contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
