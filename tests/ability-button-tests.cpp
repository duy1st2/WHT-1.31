#include "../native/ability_button131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
int main(){try{
 for(const auto& p:wht::abilityButtonProfiles131){
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
 Check(wht::AbilityButtonCode131(base,p),"relocated attack guard");image[p.notify]^=1;Check(!wht::AbilityButtonCode131(base,p),"tampered native attack refused");
 }





 Check(!wht::AbilityButtonProfileFor131(129),"unknown profile refused");
 for(const auto& p:wht::abilityButtonProfiles131){
#ifdef _WIN64
 Check(p.counter==0x64&&!p.setter,"independently verified x64 inline counter policy");
 Check(p.code[1].length==11&&p.code[1].bytes[0]==0xff&&p.code[1].bytes[1]==0x40&&p.code[1].bytes[2]==0x64,"game increment +64");
 Check(p.code[2].length==11&&p.code[2].bytes[0]==0xff&&p.code[2].bytes[1]==0x4b&&p.code[2].bytes[2]==0x64,"game decrement +64");
 for(unsigned n=1;n<=2;++n){const auto& code=p.code[n];int32_t relative=0;memcpy(&relative,code.bytes+7,4);Check(code.bytes[6]==0xe8&&code.rva+11+relative==p.notify,"inline operations call same native notifier");}
#else
 Check(p.counter==0x40&&p.setter,"verified x86 native setter policy");
 const auto& code=p.code[0];Check(code.length==57&&code.bytes[0x21]==0xff&&code.bytes[0x22]==0x40&&code.bytes[0x23]==0x40&&code.bytes[0x2d]==0xff&&code.bytes[0x2e]==0x48&&code.bytes[0x2f]==0x40,"full native setter both counter branches");
#endif
 }
 wht::AbilityButtonSource131 target;target.source.unit=123;target.source.world=456;target.source.id={1,2};target.source.local=0;target.source.ability=789;target.source.abilityId={3,4};target.source.abilityTable=1000;
 unsigned reads=0,writes=0,updates=0;bool unstable=false,noOp=false,changedAfter=false;std::string report;
 auto read=[&](uint32_t code,wht::AbilityButtonSource131& out){Check(code==0x41486862,"same rawcode");out=target;++reads;if((unstable&&reads==2)||(changedAfter&&reads==3))++out.source.abilityId.low;return true;};
 auto apply=[&](uint32_t code,bool hide,const wht::AbilityButtonSource131& value){Check(code==0x41486862&&value.source.ability==789&&value.hidden==target.hidden,"stable ability update");++writes;if(!noOp)target.hidden+=hide?1:-1;return true;};
 auto refresh=[&](uintptr_t unit){Check(unit==123,"stable unit refresh");++updates;return true;};
 auto run=[&](bool hide){reads=0;return wht::AbilityButtonCommand131(L"AHhb",hide,read,apply,refresh,report);};
 Check(run(true)&&target.hidden==1&&reads==4&&writes==1&&updates==1,"hide native update readback refresh confirmation");
 Check(run(true)&&target.hidden==2,"repeated hide increments like 1.29");Check(run(false)&&target.hidden==1,"show decrements one instead of clearing");Check(run(false)&&target.hidden==0,"balanced show reaches zero");
 auto old=writes;Check(run(false)&&writes==old&&reads==2,"show at zero never underflows");
 target.hidden=1000000;Check(!run(true)&&writes==old,"upper bound no write");target.hidden=-1;Check(!run(true)&&writes==old,"negative counter refused");target.hidden=0;
 unstable=true;Check(!run(true)&&writes==old,"changed ability before write refused");unstable=false;noOp=true;auto oldUpdates=updates;Check(!run(true)&&updates==oldUpdates,"native no-op not reported as success");noOp=false;
 changedAfter=true;Check(!run(true)&&updates==oldUpdates,"changed ability after update prevents refresh");changedAfter=false;
 old=writes;Check(!wht::AbilityButtonCommand131(L"AHhb extra",true,read,apply,refresh,report)&&writes==old,"invalid arguments no update");
 target.source.ability=0;Check(!run(true)&&writes==old,"absent ability refused");
 puts("ability button contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
