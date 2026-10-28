#include "../native/move_speed131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned calls=0;
#ifdef _WIN64
void Set(uintptr_t unit,const float* speed){
#else
void __fastcall Set(uintptr_t unit,void*,const float* speed){
#endif
 Check(unit==123&&speed&&*speed==522,"native unit receiver and real pointer ABI");++calls;
}
int main(){try{
 for(const auto& p:wht::moveSpeedProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"captured fixture available");std::array<unsigned char,4096> header{};input.read(reinterpret_cast<char*>(header.data()),header.size());unsigned pe=0;memcpy(&pe,header.data()+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header.data()+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t imageSize=0;for(size_t i=0;i<p.count;++i)imageSize=(std::max)(imageSize,size_t(p.code[i].rva+p.code[i].length));std::vector<unsigned char> image(imageSize+64);const auto base=reinterpret_cast<uintptr_t>(image.data());
 for(size_t i=0;i<p.count;++i){const auto& code=p.code[i];std::vector<unsigned char> bytes(code.length);input.seekg(code.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"captured movement block read");
 for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v-=uint32_t(capturedBase);memcpy(bytes.data()+code.reloc[j],&v,4);}Check(!memcmp(bytes.data(),code.bytes,code.length),"captured movement and hero-bar caller signatures");
 memcpy(image.data()+code.rva,bytes.data(),bytes.size());for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v+=uint32_t(base);memcpy(image.data()+code.rva+code.reloc[j],&v,4);}}
 Check(wht::MoveSpeedCode131(base,p),"relocated movement guard");image[p.get]^=1;Check(!wht::MoveSpeedCode131(base,p),"tampered native movement refused");
 }

 Check(!wht::MoveSpeedProfileFor131(129),"unsupported build");float speed=0;
 Check(wht::ParseMoveSpeed131(L"",speed)&&speed==522,"default 522");
 Check(wht::ParseMoveSpeed131(L"-5",speed)&&speed==0&&wht::ParseMoveSpeed131(L"2500",speed)&&speed==2000,"reference clamps");
 for(const auto* bad:{L"nan",L"inf",L"3 extra",L"1\n",L"abc"})Check(!wht::ParseMoveSpeed131(bad,speed),"malformed or nonfinite speed rejected");
 auto native=reinterpret_cast<wht::MoveSpeedSet131>(Set);Check(wht::InvokeMoveSpeed131(native,123,522)&&calls==1,"native set ABI");Check(!wht::InvokeMoveSpeed131(native,0,522)&&!wht::InvokeMoveSpeed131(native,123,NAN)&&!wht::InvokeMoveSpeed131(native,123,2001)&&calls==1,"invalid native parameters no call");
 wht::MoveSpeedSource131 target;target.unit=123;target.movement=456;target.world=789;target.id={1,2};target.local=0;target.speed=300;
 unsigned reads=0,writes=0;bool changed=false,refused=false;std::string report;
 auto read=[&](wht::MoveSpeedSource131& out){out=target;++reads;if(reads>=3){out.speed=500;if(changed)++out.id.low;}return true;};
 auto set=[&](uintptr_t unit,float value){Check(unit==123&&value==522,"command selected unit and default");++writes;return !refused;};
 Check(wht::MoveSpeedCommand131(L"",read,set,report)&&writes==1&&report.find("500.0")!=std::string::npos&&report.find("522.0")!=std::string::npos,"report actual game speed distinct from requested");
 reads=0;changed=true;Check(!wht::MoveSpeedCommand131(L"",read,set,report),"recycled unit after native update refused");
 reads=0;changed=false;refused=true;Check(!wht::MoveSpeedCommand131(L"",read,set,report),"native refusal reported");
 unsigned old=writes;Check(!wht::MoveSpeedCommand131(L"bad",read,set,report)&&writes==old,"malformed input no writes");
 Check(!wht::MoveSpeedCommand131(L"",[](wht::MoveSpeedSource131&){return false;},set,report)&&writes==old,"unavailable selection no writes");
 auto unstable=[&](wht::MoveSpeedSource131& out){out=target;if(++reads==2)++out.movement;return true;};reads=0;Check(!wht::MoveSpeedCommand131(L"",unstable,set,report)&&writes==old,"movement component replaced before update no writes");
 puts("move speed contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
