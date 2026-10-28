#include "../native/camera_focus131.hpp"
#include <fstream>
#include <stdexcept>
#include <limits>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned panCalls=0;
#ifdef _WIN64
void FakePan(uintptr_t camera,float x,float y,float z,unsigned mode){
#else
void __fastcall FakePan(uintptr_t camera,void*,float x,float y,float z,unsigned mode){
#endif
 Check(camera==0x1234&&x==123.f&&y==-456.f&&z==0.f&&!mode,"native camera receiver and float argument ABI");++panCalls;
}
int main(){try{
 Check(!wht::CameraPanProfileFor131(129),"unverified camera build refused");
 for(const auto& profile:wht::cameraPanProfiles131){
#ifdef _WIN64
  const auto file=profile.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const auto file=profile.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);Check(bool(input),"captured camera fixture available");input.seekg(profile.pan);std::array<unsigned char,24> bytes{};input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(input.gcount()==bytes.size()&&bytes==profile.code,"all camera guards match independently captured profiles");
  const auto base=reinterpret_cast<uintptr_t>(bytes.data())-profile.pan;Check(wht::CameraPanCode131(base,profile),"native camera code guard accepts profile");bytes[23]^=1;Check(!wht::CameraPanCode131(base,profile)&&!wht::CameraPanCode131(0,profile),"altered or missing camera code refused");
 }
 auto native=reinterpret_cast<wht::CameraPanFn131>(FakePan);
 Check(wht::InvokeCameraPan131(native,0x1234,123,-456)&&panCalls==1,"actual native-style call dispatch");
 Check(!wht::InvokeCameraPan131(nullptr,0x1234,123,-456)&&!wht::InvokeCameraPan131(native,0,123,-456)&&!wht::InvokeCameraPan131(native,0x1234,1e6f,0)&&panCalls==1,"bad calls rejected before native dispatch");
 Check(wht::FocusPoint131(-65536,65536)&&!wht::FocusPoint131(65537,0)&&!wht::FocusPoint131(std::numeric_limits<float>::quiet_NaN(),0)&&!wht::FocusPoint131(0,std::numeric_limits<float>::infinity()),"reference coordinate bounds and finite policy");
 std::string report;unsigned reads=0,pans=0,cameras=0;
 auto read=[&](float& x,float& y){++reads;x=123;y=-456;return true;};
 auto camera=[&]()->uintptr_t{++cameras;return 0x1234;};auto pan=[&](uintptr_t target,float x,float y){Check(target==0x1234&&x==123&&y==-456,"selected unit position forwarded");++pans;return true;};
 Check(wht::FocusSelected131(L" ",read,camera,pan,report)&&reads==1&&pans==1&&cameras==2&&report.find("123.00000")!=std::string::npos,"focus reads selected unit and confirms same live camera");
 for(auto args:{L"1",L"extra",L"\n"})Check(!wht::FocusSelected131(args,read,camera,pan,report)&&pans==1&&reads==1,"focus rejects arguments without native effects");
 Check(!wht::FocusSelected131(std::wstring(L"\0x",2),read,camera,pan,report)&&pans==1,"embedded NUL refused");
 Check(!wht::FocusSelected131(L"",[](float&,float&){return false;},camera,pan,report)&&pans==1,"missing selected unit cannot move camera");
 Check(!wht::FocusSelected131(L"",read,[]()->uintptr_t{return 0;},pan,report)&&pans==1,"missing camera cannot pan");
 unsigned changed=0;Check(!wht::FocusSelected131(L"",read,[&]()->uintptr_t{return ++changed;},pan,report)&&pans==1,"replaced camera cannot receive stale pan");
 Check(!wht::FocusSelected131(L"",read,camera,[](uintptr_t,float,float){return false;},report)&&report.find("unavailable")!=std::string::npos,"native refusal never announces successful focus");
 std::puts("PASS focus ABI, per-profile guards, selected unit/camera stability, bounds and failure policy");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
