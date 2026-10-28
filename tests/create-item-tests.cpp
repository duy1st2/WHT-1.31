#include "../native/create_item131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <limits>
void Check(bool b,const char* s){if(!b)throw std::runtime_error(s);}
uint32_t seenCode=0;float seenX=0,seenY=0;
uint32_t __cdecl Create(uint32_t code,const float* x,const float* y){seenCode=code;seenX=*x;seenY=*y;return 123;}
uint32_t __cdecl Type(uint32_t handle){return handle==123?0x49303030:0;}
uintptr_t __cdecl Resolve(uint32_t handle){return handle==123?(sizeof(uintptr_t)==8?uintptr_t(0x1234567812345678ULL):uintptr_t(0x12345678)):0;}
int main(){try{
 uint32_t code=0;unsigned count=0,slot=99;Check(wht::ParseCreateUnit131(L"I000 50",code,count)&&code==0x49303030&&count==50,"item parser");Check(wht::ParseCreateUnit131(L"0x49303030",code,count)&&count==1,"hex/default");Check(wht::ParseCopyItem131(L"6 2",slot,count)&&slot==5&&count==2,"human inventory slot");for(auto s:{L"0",L"7",L"01",L"1 0",L"1 51",L"1 1 extra",L"1\n"})Check(!wht::ParseCopyItem131(s,slot,count),"bad copy args");Check(!wht::ParseCopyItem131(std::wstring(L"1\0",2),slot,count),"nul rejected");
 Check(wht::InvokeCreateItem131(Create,0x49303030,1.5f,-4.f)==123&&seenCode==0x49303030&&seenX==1.5f&&seenY==-4.f,"native uint32 real-pointer ABI");Check(wht::InvokeItemType131(Type,123)==0x49303030&&wht::InvokeItemResolve131(Resolve,123)==(sizeof(uintptr_t)==8?uintptr_t(0x1234567812345678ULL):uintptr_t(0x12345678)),"handle getter and pointer width");Check(!wht::InvokeCreateItem131(Create,1,std::numeric_limits<float>::infinity(),0),"invalid coords no native call");
 wht::ResourceCommandSource131 owner;owner.world=1;owner.player=2;owner.local=0;owner.physical=0;auto read=[&](wht::ResourceCommandSource131& out){out=owner;return true;};bool valid=true;auto source=[&](const auto&){return valid;};std::vector<float> xs;unsigned creates=0;auto create=[&](uint32_t type,float x,float y){Check(type==0x49303030&&y==3,"spawn args");xs.push_back(x);return uint32_t(++creates);};auto confirm=[&](uint32_t handle,uint32_t type,wht::CreatedItemReference131& out){out.handle=handle;out.code=type;out.item=100+handle;out.node=200+handle;out.id={handle,10};return true;};std::string report;
 Check(wht::CreateItemBatch131(0x49303030,3,2,3,read,source,create,confirm,report)&&xs==std::vector<float>{2,42,82},"WHT count spacing and confirmation");auto before=creates;Check(!wht::CreateItemBatch131(1,51,0,0,read,source,create,confirm,report)&&creates==before,"preflight count no mutation");valid=false;Check(!wht::CreateItemBatch131(0x49303030,2,2,3,read,source,create,confirm,report)&&creates==before,"source preflight no mutation");valid=true;
 unsigned confirmed=0;auto partial=[&](uint32_t handle,uint32_t type,wht::CreatedItemReference131& out){confirm(handle,type,out);return ++confirmed<2;};Check(!wht::CreateItemBatch131(0x49303030,3,2,3,read,source,create,partial,report)&&creates==before+2&&report.find("1/3 confirmed")!=report.npos,"partial count and stop");before=creates;auto changed=[&](uint32_t type,float x,float y){auto h=create(type,x,y);owner.world=99;return h;};Check(!wht::CreateItemBatch131(0x49303030,3,2,3,read,source,changed,confirm,report)&&creates==before+1,"changed world after factory");before=creates;auto duplicate=[&](uint32_t handle,uint32_t type,wht::CreatedItemReference131& out){confirm(handle,type,out);out.id={1,10};return true;};Check(!wht::CreateItemBatch131(0x49303030,3,2,3,read,source,create,duplicate,report)&&creates==before+2,"same item serial refused");before=creates;auto sourceChanges=[&](uint32_t handle,uint32_t type,wht::CreatedItemReference131& out){confirm(handle,type,out);valid=false;return true;};Check(!wht::CreateItemBatch131(0x49303030,3,2,3,read,source,create,sourceChanges,report)&&creates==before+1,"inventory changed after spawn stops");
 for(const auto& p:wht::createItemProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream in(file,std::ios::binary);Check(bool(in),"capture");std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());for(size_t i=0;i<p.count;++i){const auto& c=p.code[i];for(size_t j=0;j<c.relocCount;++j){unsigned value=0;memcpy(&value,c.bytes+c.reloc[j],4);value+=unsigned(base);memcpy(image.data()+c.rva+c.reloc[j],&value,4);}}Check(wht::CreateItemCode131(base,p),"rebased capture guards");for(size_t i=0;i<p.count;++i){image[p.code[i].rva]^=1;Check(!wht::CreateItemCode131(base,p),"modified guard refuses");image[p.code[i].rva]^=1;}
 }
 puts("Create/copy item contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
