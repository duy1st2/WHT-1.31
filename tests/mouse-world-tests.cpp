#include "../native/mouse_world131.hpp"
#include <fstream>
#include <limits>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned queries=0;
#ifdef _WIN64
bool FakeQuery(uintptr_t world,float* xyz,unsigned mode){
#else
bool __fastcall FakeQuery(uintptr_t world,void*,float* xyz,unsigned mode){
#endif
 Check(world==0x1234&&xyz&&!mode,"native cursor query receiver and argument ABI");++queries;xyz[0]=123;xyz[1]=-456;xyz[2]=78;return true;
}
int main(){try{
 Check(!wht::MouseWorldProfileFor131(129),"unknown native cursor profile refused");
 for(const auto& profile:wht::mouseWorldProfiles131){
#ifdef _WIN64
  const auto file=profile.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const auto file=profile.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);Check(bool(input),"captured cursor fixture available");input.seekg(profile.query);alignas(uintptr_t) std::array<unsigned char,96> bytes{};input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(input.gcount()==bytes.size(),"captured cursor function complete");
  auto original=bytes;
#ifndef _WIN64
  for(unsigned i=13;i<17;++i)original[i]=0;
#endif
  Check(original==profile.code,"all 96 native cursor bytes match verified captured profile");
  const auto base=reinterpret_cast<uintptr_t>(bytes.data())-profile.query;
#ifndef _WIN64
  *reinterpret_cast<uintptr_t*>(bytes.data()+13)=base+profile.cursor;
#endif
  Check(wht::MouseWorldCode131(base,profile),"native cursor guard accepts supported code with ASLR");bytes[95]^=1;Check(!wht::MouseWorldCode131(base,profile)&&!wht::MouseWorldCode131(0,profile),"altered/missing native query refused");bytes[95]^=1;
#ifndef _WIN64
  ++*reinterpret_cast<uintptr_t*>(bytes.data()+13);Check(!wht::MouseWorldCode131(base,profile),"redirected native cursor global refused");
#endif
 }
 wht::MouseWorldPosition131 position{1,2,3};const auto native=reinterpret_cast<wht::MouseWorldQuery131>(FakeQuery);
 Check(wht::InvokeMouseWorld131(native,0x1234,position)&&queries==1&&position.x==123&&position.y==-456&&position.z==78,"native world cursor coordinates published");
 Check(!wht::InvokeMouseWorld131(nullptr,0x1234,position)&&!wht::InvokeMouseWorld131(native,0,position)&&queries==1,"missing native query never dispatched");
 Check(wht::MouseWorldPoint131({-100000,100000,0})&&!wht::MouseWorldPoint131({100001,0,0})&&!wht::MouseWorldPoint131({0,0,std::numeric_limits<float>::quiet_NaN()}),"native world result bounds and finite Z validated");
 Check(wht::MouseWorldInput131(-4,4)&&!wht::MouseWorldInput131(4.1f,0)&&!wht::MouseWorldInput131(0,std::numeric_limits<float>::infinity()),"engine cursor input validation");
 float x=1,y=2;Check(wht::PanPointArgs131(L" -65536 65536 ",x,y)&&x==-65536&&y==65536,"reference camera coordinate bounds");
 for(auto args:{L"",L"1",L"1 2 3",L"65537 0",L"nan 1",L"1 2\n"})Check(!wht::PanPointArgs131(args,x,y),"invalid camera coordinate arguments refused");
 Check(!wht::PanPointArgs131(std::wstring(L"1 2\0x",5),x,y),"NUL camera arguments refused");
 std::string report;unsigned reads=0,pans=0;
 auto read=[&](wht::MouseWorldPosition131& out){++reads;out={123,-456,78};return true;};auto camera=[]()->uintptr_t{return 0x1234;};auto pan=[&](uintptr_t target,float a,float b){Check(target==0x1234&&a==12&&b==-34,"explicit pan target uses supplied coordinates");++pans;return true;};
 Check(wht::MouseWorldCommand131(L"#mpos",L"",1280,720,read,camera,pan,report)&&report=="X: 123.00000, Y: -456.00000, Z: 78.00000 | Res: 1280, 720"&&!pans,"mpos matches reference world and resolution formatting");
 Check(wht::MouseWorldCommand131(L"#ppos",L" ",1280,720,read,camera,pan,report)&&reads==2&&!pans,"bare ppos reads cursor as reference");
 Check(wht::MouseWorldCommand131(L"#ppos",L"12 -34",1280,720,read,camera,pan,report)&&reads==2&&pans==1,"explicit ppos pans without reading cursor");
 Check(!wht::MouseWorldCommand131(L"#mpos",L"12 -34",1280,720,read,camera,pan,report)&&pans==1,"mpos never pans camera");
 Check(!wht::MouseWorldCommand131(L"#ppos",L"12 -34",1280,720,read,[]()->uintptr_t{return 0;},pan,report)&&pans==1,"missing camera cannot pan");
 Check(!wht::MouseWorldCommand131(L"#mpos",L"",1280,720,[](wht::MouseWorldPosition131&){return false;},camera,pan,report)&&report.find("unavailable")!=std::string::npos,"missing projection is reported without fake coordinates");
 Check(!wht::MouseWorldCommand131(L"#ppos",L"12 -34",1280,720,read,camera,[](uintptr_t,float,float){return false;},report)&&report.find("unavailable")!=std::string::npos,"refused native pan never reports success");
 std::puts("PASS world cursor query ABI, all captured native guards, coordinates, formatting and pan policy");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
