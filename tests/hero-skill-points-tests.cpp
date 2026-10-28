#include "../native/hero_skill_points131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned nativeCalls=0;
#ifdef _WIN64
bool FakeModify(uintptr_t unit,int amount){
#else
bool __fastcall FakeModify(uintptr_t unit,void*,int amount){
#endif
 Check(unit==0x1234&&amount==5,"native hero point receiver and signed amount ABI");++nativeCalls;return true;
}
int main(){try{
 Check(!wht::HeroSkillPointsProfileFor131(129),"unsupported skill point build refused");
 for(const auto& p:wht::heroSkillPointsProfiles131){
#ifdef _WIN64
  const auto file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const auto file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);Check(bool(input),"captured hero point fixture available");std::array<unsigned char,48> wrapper{};std::array<unsigned char,40> body{};input.seekg(p.modify);input.read(reinterpret_cast<char*>(wrapper.data()),wht::heroSkillWrapperSize131);Check(input.gcount()==wht::heroSkillWrapperSize131&&wrapper==p.code,"all CUnit wrappers match captured profile");input.seekg(p.implementation);input.read(reinterpret_cast<char*>(body.data()),body.size());Check(body==p.body,"native capped hero modifier matches captured profile");
  std::vector<unsigned char> image((std::max)(p.modify,p.implementation)+64);memcpy(image.data()+p.modify,p.code.data(),wht::heroSkillWrapperSize131);memcpy(image.data()+p.implementation,p.body.data(),p.body.size());const auto base=reinterpret_cast<uintptr_t>(image.data());Check(wht::HeroSkillPointsCode131(base,p),"guard accepts both verified native routines");image[p.modify+20]^=1;Check(!wht::HeroSkillPointsCode131(base,p),"altered unit wrapper refused");image[p.modify+20]^=1;image[p.implementation+20]^=1;Check(!wht::HeroSkillPointsCode131(base,p)&&!wht::HeroSkillPointsCode131(0,p),"altered hero implementation or missing image refused");
  const auto* skill=wht::SkillProfile(p.build);Check(skill!=nullptr,"existing hero reader profile");alignas(uintptr_t) unsigned char unit[0x600]{},hero[0x140]{};const uintptr_t u=reinterpret_cast<uintptr_t>(unit),h=reinterpret_cast<uintptr_t>(hero),fakeBase=0x10000000;
  *reinterpret_cast<uintptr_t*>(unit)=fakeBase+skill->unitTable;*reinterpret_cast<uintptr_t*>(hero)=fakeBase+skill->heroTable;*reinterpret_cast<uintptr_t*>(unit+skill->heroMember)=h;
  const wht::DropObjectId id{1,2};memcpy(unit+(sizeof(uintptr_t)==8?0x18:0xc),&id,sizeof(id));*reinterpret_cast<int*>(hero+skill->points)=7;wht::HeroSkillPointsSource131 out;
  Check(wht::ReadHeroSkillPoints131(fakeBase,p.build,[&](){return u;},out)&&out.unit==u&&out.hero==h&&out.points==7&&out.id==id,"existing typed hero component and points read");
  *reinterpret_cast<int*>(hero+skill->points)=-1;Check(!wht::ReadHeroSkillPoints131(fakeBase,p.build,[&](){return u;},out),"invalid hero point value refused");*reinterpret_cast<int*>(hero+skill->points)=7;
  unsigned selected=0;Check(!wht::ReadHeroSkillPoints131(fakeBase,p.build,[&](){return ++selected==1?u:uintptr_t(0);},out),"selection replacement invalidates source");++*reinterpret_cast<uintptr_t*>(hero);Check(!wht::ReadHeroSkillPoints131(fakeBase,p.build,[&](){return u;},out),"foreign hero component refused");
 }
 int amount=0;Check(wht::HeroSkillPointsAmount131(L"",amount)&&amount==500,"reference default 500 skill points");Check(wht::HeroSkillPointsAmount131(L"10000",amount)&&amount==10000,"reference maximum accepted");
 for(auto args:{L"0",L"-1",L"10001",L"1.5",L"1 extra",L"1\n"})Check(!wht::HeroSkillPointsAmount131(args,amount),"invalid skill point amount refused");Check(!wht::HeroSkillPointsAmount131(std::wstring(L"1\0x",3),amount),"NUL amount refused");
 Check(wht::InvokeHeroSkillPoints131(reinterpret_cast<wht::ModifyHeroSkillPoints131>(FakeModify),0x1234,5)&&nativeCalls==1,"typed native skill point ABI dispatch");Check(!wht::InvokeHeroSkillPoints131(nullptr,0x1234,5)&&!wht::InvokeHeroSkillPoints131(reinterpret_cast<wht::ModifyHeroSkillPoints131>(FakeModify),0,5)&&nativeCalls==1,"invalid native arguments never dispatched");
 wht::HeroSkillPointsSource131 state{123,456,{1,2},7};unsigned writes=0,reads=0;std::string report;auto read=[&](wht::HeroSkillPointsSource131& out){++reads;out=state;return true;};auto modify=[&](uintptr_t unit,int requested){Check(unit==123&&requested==5,"command forwards selected hero and amount");++writes;state.points+=2;return true;};
 Check(wht::HeroSkillPointsCommand131(L"5",read,modify,report)&&writes==1&&report=="Hero Skill Points: +2 | Total: 9","native cap reports actual increase rather than requested amount");
 const auto before=reads;Check(!wht::HeroSkillPointsCommand131(L"0",read,modify,report)&&reads==before&&writes==1,"invalid command cannot read or mutate game state");
 Check(!wht::HeroSkillPointsCommand131(L"5",[](wht::HeroSkillPointsSource131&){return false;},modify,report)&&writes==1,"missing hero cannot receive points");
 unsigned sample=0;Check(!wht::HeroSkillPointsCommand131(L"5",[&](wht::HeroSkillPointsSource131& out){out=state;if(++sample==2)out.id={9,9};return true;},modify,report)&&writes==1,"recycled hero identity refused before native call");
 Check(!wht::HeroSkillPointsCommand131(L"5",read,[](uintptr_t,int){return false;},report),"native refusal cannot announce success");
 Check(!wht::HeroSkillPointsCommand131(L"5",read,[](uintptr_t,int){return true;},report)&&report.find("cap")!=std::string::npos,"no native readback increase reports possible cap");
 Check(!wht::HeroSkillPointsCommand131(L"5",read,[&](uintptr_t,int){state.hero=789;return true;},report)&&report.find("changed")!=std::string::npos,"hero replacement after native call never publishes a false total");
 std::puts("PASS hero skill native profiles, ABI, stable typed selection, amount bounds, caps and readback");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
