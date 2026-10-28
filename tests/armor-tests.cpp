#include "../native/armor131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned calls=0;
#ifdef _WIN64
void Add(uintptr_t unit,const float* delta){
#else
void __fastcall Add(uintptr_t unit,void*,const float* delta){
#endif
 Check(unit==123&&delta&&*delta==-10.5f,"native unit receiver and signed real pointer ABI");++calls;
}
int main(){try{
 for(const auto& p:wht::armorProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"captured fixture available");std::array<unsigned char,4096> header{};input.read(reinterpret_cast<char*>(header.data()),header.size());unsigned pe=0;memcpy(&pe,header.data()+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header.data()+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t imageSize=0;for(size_t i=0;i<p.count;++i)imageSize=(std::max)(imageSize,size_t(p.code[i].rva+p.code[i].length));imageSize=(std::max)(imageSize,size_t(p.panelTable+0x100));imageSize=(std::max)(imageSize,size_t(p.floatTable+sizeof(uintptr_t)));std::vector<unsigned char> image(imageSize+64);const auto base=reinterpret_cast<uintptr_t>(image.data());
 for(size_t i=0;i<p.count;++i){const auto& code=p.code[i];std::vector<unsigned char> bytes(code.length);input.seekg(code.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"captured attack block read");
 for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v-=uint32_t(capturedBase);memcpy(bytes.data()+code.reloc[j],&v,4);}Check(!memcmp(bytes.data(),code.bytes,code.length),"captured attack and hero-bar caller signatures");
 memcpy(image.data()+code.rva,bytes.data(),bytes.size());for(size_t j=0;j<code.relocCount;++j){uint32_t v=0;memcpy(&v,bytes.data()+code.reloc[j],4);v+=uint32_t(base);memcpy(image.data()+code.rva+code.reloc[j],&v,4);}}
 *reinterpret_cast<uintptr_t*>(base+p.floatTable)=base+p.set;*reinterpret_cast<uintptr_t*>(base+p.panelTable+(sizeof(uintptr_t)==8?0xe0:0x70))=base+p.panel;
 Check(wht::ArmorCode131(base,p),"relocated attack guard");image[p.add]^=1;Check(!wht::ArmorCode131(base,p),"tampered native armor refused");image[p.add]^=1;*reinterpret_cast<uintptr_t*>(base+p.floatTable)+=1;Check(!wht::ArmorCode131(base,p),"foreign property setter refused");
 }




 for(const auto& p:wht::armorProfiles131){
 const auto* inv=wht::InventoryProfile(p.build);Check(inv!=nullptr,"known unit profile");alignas(uintptr_t) unsigned char unit[0x400]{};const uintptr_t u=reinterpret_cast<uintptr_t>(unit),base=0x10000000;
 *reinterpret_cast<uintptr_t*>(unit)=base+inv->unitTable;*reinterpret_cast<uintptr_t*>(unit+p.property)=base+p.floatTable;
 *reinterpret_cast<float*>(unit+(sizeof(uintptr_t)==8?0x278:0x1e8))=1234;auto& field=*reinterpret_cast<float*>(unit+p.property+sizeof(uintptr_t));field=12.5f;float value=NAN;
 Check(wht::ReadArmorFields131(base,p,u,value)&&value==12.5f,"correct armor field distinct from former mana candidate");field=-15;Check(wht::ReadArmorFields131(base,p,u,value)&&value==-15,"negative armor readable");field=0;Check(wht::ReadArmorFields131(base,p,u,value)&&value==0,"zero armor valid");
 field=NAN;Check(!wht::ReadArmorFields131(base,p,u,value),"invalid armor refused");field=12;*reinterpret_cast<uintptr_t*>(unit+p.property)+=1;Check(!wht::ReadArmorFields131(base,p,u,value),"foreign property refused");*reinterpret_cast<uintptr_t*>(unit+p.property)=base+p.floatTable;*reinterpret_cast<uintptr_t*>(unit)+=1;Check(!wht::ReadArmorFields131(base,p,u,value),"foreign unit refused");
#ifdef _WIN64
 const unsigned char expected[]={0x41,0x8b,0x84,0x24,0x98,0x02,0x00,0x00};
#else
 const unsigned char expected[]={0x8b,0x86,0xf8,0x01,0x00,0x00};
#endif
 Check(p.code[2].length==sizeof(expected)&&!memcmp(p.code[2].bytes,expected,sizeof(expected)),"native armor-panel reader confirms semantic field");
 }
 Check(!wht::ArmorProfileFor131(129),"unsupported armor profile");float armor=0;
 Check(wht::ParseArmor131(L"",armor)&&armor==50,"default armor50");Check(wht::ParseArmor131(L"-1000000",armor)&&armor==-1000000&&wht::ParseArmor131(L"1000000",armor)&&armor==1000000,"signed inclusive limits");
 for(const auto* bad:{L"nan",L"inf",L"1000001",L"-1000001",L"1 extra",L"1\n",L"abc"})Check(!wht::ParseArmor131(bad,armor),"malformed nonfinite and out of range refused");
 auto native=reinterpret_cast<wht::ArmorAdd131>(Add);Check(wht::InvokeArmorAdd131(native,123,-10.5f)&&calls==1,"native real-pointer ABI");Check(!wht::InvokeArmorAdd131(native,0,1)&&!wht::InvokeArmorAdd131(native,123,NAN)&&calls==1,"invalid native args no call");
 wht::ArmorSource131 target;target.unit=123;target.world=456;target.id={1,2};target.local=0;target.armor=10;
 unsigned reads=0,writes=0;bool changed=false,noOp=false,refused=false;std::string report;
 auto read=[&](wht::ArmorSource131& out){out=target;if(++reads>=3&&changed)++out.id.low;return true;};auto add=[&](uintptr_t unit,float delta){Check(unit==123,"same selected unit");++writes;if(refused)return false;if(!noOp)target.armor+=delta;return true;};
 Check(wht::ArmorCommand131(L"",read,add,report)&&writes==1&&target.armor==50,"native delta sets default armor and reads back");
 reads=0;unsigned old=writes;Check(wht::ArmorCommand131(L"50",read,add,report)&&writes==old,"zero delta no write");
 reads=0;Check(wht::ArmorCommand131(L"-12.5",read,add,report)&&target.armor==-12.5f,"negative fractional armor");
 reads=0;changed=true;Check(!wht::ArmorCommand131(L"",read,add,report),"recycled unit after update refused");changed=false;target.armor=10;reads=0;noOp=true;Check(!wht::ArmorCommand131(L"",read,add,report),"native no-op not success");
 noOp=false;refused=true;reads=0;Check(!wht::ArmorCommand131(L"",read,add,report),"native refusal reported");
 old=writes;Check(!wht::ArmorCommand131(L"bad",read,add,report)&&writes==old,"malformed input no writes");
 auto unstable=[&](wht::ArmorSource131& out){out=target;if(++reads==2)++out.world;return true;};reads=0;Check(!wht::ArmorCommand131(L"",unstable,add,report)&&writes==old,"world change before update no writes");
 target.armor=NAN;reads=0;Check(!wht::ArmorCommand131(L"",read,add,report)&&writes==old,"invalid source armor refused");
 puts("armor contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
