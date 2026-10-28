#include "../native/unit_scale131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private native fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
unsigned calls=0;uintptr_t wantedModel=0;float wantedScale=0;
void __cdecl Setter(uintptr_t model,unsigned index,float scale){Check(model==wantedModel&&index==2&&scale==wantedScale,"cdecl model/index/float ABI");++calls;}
unsigned propertyCalls=0;
#ifdef _WIN64
void PropertySet(uintptr_t property,const float* value){
 const unsigned start=0x20,end=0x34,offset=0x38;
#else
void __fastcall PropertySet(uintptr_t property,void*,const float* value){
 const unsigned start=0x10,end=0x1c,offset=0x1c;
#endif
 for(unsigned i=start;i<end;++i)Check(*reinterpret_cast<unsigned char*>(property+i)==0,"native clears interpolation state before virtual scalar setter");
 *reinterpret_cast<float*>(property+offset)=*value;++propertyCalls;
}
int main(){try{
 for(const auto& p:wht::unitScaleProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=0;for(size_t i=0;i<p.length;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.length;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 Check(wht::UnitScaleCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.length;++i){image.data[p.code[i].rva]^=1;Check(!wht::UnitScaleCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}

 memcpy(image.data,header,sizeof(header));DWORD oldProtect=0;Check(bool(VirtualProtect(image.data,size+64,PAGE_EXECUTE_READ,&oldProtect)),"private fixture RX");FlushInstructionCache(GetCurrentProcess(),image.data,size+64);
 alignas(uintptr_t) unsigned char model[64]{},property[96]{};uintptr_t properties[3]={0,0,reinterpret_cast<uintptr_t>(property)};uintptr_t vtable[4]={0,base+p.set,base+p.set,reinterpret_cast<uintptr_t>(PropertySet)};
 const auto m=reinterpret_cast<uintptr_t>(model),prop=reinterpret_cast<uintptr_t>(property);*reinterpret_cast<unsigned*>(model+p.count)=3;*reinterpret_cast<uintptr_t*>(model+p.table)=reinterpret_cast<uintptr_t>(properties);*reinterpret_cast<uintptr_t*>(property)=reinterpret_cast<uintptr_t>(vtable);property[p.type]=6;property[p.flags]=0;
 memset(property+(sizeof(uintptr_t)==8?0x20:0x10),0xab,sizeof(uintptr_t)==8?20:12);
 auto nativeSet=reinterpret_cast<wht::UnitScaleSetter131>(base+p.set);auto nativeGet=reinterpret_cast<wht::UnitScaleGetter131>(base+p.get);unsigned oldCalls=propertyCalls;
 Check(wht::InvokeUnitScale131(nativeSet,m,2.5f)&&propertyCalls==oldCalls+1,"captured native setter dispatches uniform model scale");Check(nativeGet(m,2)==2.5f,"captured getter confirms actual scalar property");
 // Runtime layout validation needs executable in-module methods; no method is invoked here.
 vtable[3]=base+p.set;uintptr_t array=0,item=0,table=0;Check(wht::ReadScaleProperty131(base,p,m,array,item,table)&&array==reinterpret_cast<uintptr_t>(properties)&&item==prop,"verified model table/property layout");
 property[p.flags]=2;Check(!wht::ReadScaleProperty131(base,p,m,array,item,table),"read-only scale refused");property[p.flags]=0;property[p.type]=5;Check(!wht::ReadScaleProperty131(base,p,m,array,item,table),"wrong property type refused");property[p.type]=6;*reinterpret_cast<unsigned*>(model+p.count)=2;Check(!wht::ReadScaleProperty131(base,p,m,array,item,table),"missing scale index refused");*reinterpret_cast<unsigned*>(model+p.count)=3;properties[2]=0;Check(!wht::ReadScaleProperty131(base,p,m,array,item,table),"empty scalar property refused");
 }
 float scale=0;Check(wht::ParseUnitScale131(L"",scale)&&scale==1,"default normal scale");Check(wht::ParseUnitScale131(L"200",scale)&&scale==2,"percentage");Check(wht::ParseUnitScale131(L"2",scale)&&scale==2,"direct multiplier");Check(wht::ParseUnitScale131(L"10",scale)&&scale==10,"threshold ten remains multiplier");Check(wht::ParseUnitScale131(L"0",scale)&&scale==.05f,"minimum clamp");Check(wht::ParseUnitScale131(L"-2",scale)&&scale==.05f,"reference negative clamp");Check(wht::ParseUnitScale131(L"3000",scale)&&scale==20,"maximum clamp");
 for(const auto* bad:{L"nan",L"inf",L"1e100",L"2 extra",L"2\n",L"abc"})Check(!wht::ParseUnitScale131(bad,scale),"malformed or nonfinite input refused");Check(!wht::ParseUnitScale131(std::wstring(L"2\0x",3),scale),"embedded NUL refused");Check(!wht::UnitScaleProfileFor131(129),"unknown profile refused");
 wantedModel=123;wantedScale=2;Check(wht::InvokeUnitScale131(Setter,123,2)&&calls==1,"native uniform scale ABI");Check(!wht::InvokeUnitScale131(Setter,0,2)&&!wht::InvokeUnitScale131(Setter,123,21)&&!wht::InvokeUnitScale131(Setter,123,NAN)&&calls==1,"invalid invoke no mutation");
 wht::UnitScaleSource131 target;target.unit=123;target.world=456;target.id={1,2};target.local=0;target.model=789;target.properties=1000;target.property=2000;target.propertyTable=3000;target.scale=1;
 unsigned reads=0,writes=0;bool unstable=false,replaced=false,noOp=false,refused=false;std::string report;
 auto read=[&](wht::UnitScaleSource131& out){out=target;++reads;if(unstable&&reads==2)++out.id.low;if(replaced&&reads==3)++out.property;return true;};
 auto apply=[&](float value,const wht::UnitScaleSource131& before){Check(before.model==789&&before.property==2000,"same model/property passed to native update");++writes;if(refused)return false;if(!noOp)target.scale=value;return true;};
 auto run=[&](const std::wstring& args){reads=0;return wht::UnitScaleCommand131(args,read,apply,report);};
 Check(run(L"200")&&target.scale==2&&reads==3&&writes==1&&report=="Unit Scale: 2.00x","update and native readback");auto old=writes;Check(run(L"2")&&writes==old&&reads==2,"already requested size no mutation");Check(run(L"")&&target.scale==1,"restore normal size");old=writes;unstable=true;Check(!run(L"3")&&writes==old,"changed unit before update refused");unstable=false;replaced=true;Check(!run(L"4"),"replaced model property after update not success");replaced=false;noOp=true;Check(!run(L"5"),"native no-op not success");noOp=false;refused=true;Check(!run(L"6"),"native failure propagated");refused=false;old=writes;Check(!run(L"bad")&&writes==old&&reads==0,"invalid input no model access");target.model=0;Check(!run(L"2")&&writes==old,"absent model no write");
 puts("unit scale contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
