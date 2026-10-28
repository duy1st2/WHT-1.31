#include "../native/unit_damage131.hpp"
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned calls=0;float actual=NAN;
#ifdef _WIN64
void Setter(uintptr_t unit,const float* life){
#else
void __fastcall Setter(uintptr_t unit,void*,const float* life){
#endif
 Check(unit==123&&life,"native life receiver and pointer");actual=*life;++calls;
}
int main(){try{
 float amount=0;Check(wht::ParseUnitDamage131(L"",amount)&&amount==100,"default100");Check(wht::ParseUnitDamage131(L" 12.5 ",amount)&&amount==12.5f,"fractional amount");Check(wht::ParseUnitDamage131(L"1000000000",amount)&&amount==1e9f,"maximum");
 for(const auto* bad:{L"0",L"-1",L"nan",L"inf",L"1000000001",L"1e100",L"1e-100",L"1 extra",L"1\n"})Check(!wht::ParseUnitDamage131(bad,amount),"invalid amount refused");Check(!wht::ParseUnitDamage131(std::wstring(L"1\0x",3),amount),"NUL input refused");
 const auto setter=reinterpret_cast<wht::KillUnitNative131>(Setter);Check(wht::InvokeUnitLife131(setter,123,37.5f)&&actual==37.5f&&calls==1,"native thiscall pointer-to-float ABI");Check(wht::InvokeUnitLife131(setter,123,0)&&actual==0&&calls==2,"lethal target zero allowed");
 Check(!wht::InvokeUnitLife131(setter,123,-1)&&!wht::InvokeUnitLife131(setter,123,NAN)&&!wht::InvokeUnitLife131(setter,0,1)&&!wht::InvokeUnitLife131(nullptr,123,1)&&calls==2,"invalid native input no call");float nonzero=1;Check(!wht::InvokeKillUnit131(setter,123,&nonzero)&&calls==2,"existing kill zero-only ABI contract unchanged");
 wht::KillUnitSource131 target;target.unit=123;target.world=456;target.id={1,2};target.local=0;target.life=500;
 unsigned reads=0,writes=0;bool finished=false,unstable=false,refused=false,regen=false;std::string report;
 auto read=[&](wht::KillUnitSource131& out){Check(!finished,"no stale unit read after setting life");out=target;++reads;if(unstable&&reads==2)++out.id.low;if(regen&&reads==2)out.life=550;return true;};
 auto set=[&](uintptr_t unit,float hp){Check(unit==123,"selected unit passed to setter");actual=hp;++writes;finished=true;return !refused;};
 auto run=[&](const std::wstring& args){reads=0;finished=false;return wht::UnitDamageCommand131(args,read,set,report);};
 Check(run(L"")&&actual==400&&reads==2&&writes==1&&report.find("400.000000")!=report.npos,"default damage and no after-call access");Check(run(L"12.5")&&actual==487.5f,"fractional HP reduction");Check(run(L"1000")&&actual==0,"overkill clamps zero");regen=true;Check(run(L"100")&&actual==450,"latest valid HP used when regeneration advanced");regen=false;
 auto old=writes;unstable=true;Check(!run(L"100")&&writes==old,"changed serial no write");unstable=false;target.life=NAN;Check(!run(L"1")&&writes==old,"nonfinite HP no write");target.life=0;Check(!run(L"1")&&writes==old,"dead unit refused");target.life=1e9f;Check(!run(L"0.00001")&&writes==old,"unrepresentable subtraction not false success");target.life=500;refused=true;Check(!run(L"1")&&writes==old+1,"native failure reported");refused=false;old=writes;Check(!run(L"bad")&&writes==old&&reads==0,"invalid args no game reads");
 puts("unit damage contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
