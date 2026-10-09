#pragma once
#include <mutex>
#include <cmath>
#include <sstream>
#include <string>
namespace wht {
inline bool ParseSpeedMultiplier129(const std::wstring& args,float& speed){
 speed=1.f;if(args.find_first_not_of(L" \t\r\n")==std::wstring::npos)return true;
 std::wistringstream in(args);std::wstring extra;
 return bool(in>>speed)&&!(in>>extra)&&std::isfinite(speed)&&speed>=1.f&&speed<=70.f;
}
struct ScaledClock129 {
 long double real=0,fake=0;double speed=1;bool started=false;
 long double Read(long double now)const{return started?fake+(now-real)*speed:now;}
 void Set(long double now,double next){fake=Read(now);real=now;speed=next;started=true;}
};
struct ScaledTick129 {
 DWORD real=0,fake=0;double speed=1;bool started=false;
 DWORD Read(DWORD now)const{return started?fake+static_cast<DWORD>(double(DWORD(now-real))*speed):now;}
 void Set(DWORD now,double next){fake=Read(now);real=now;speed=next;started=true;}
};
}
