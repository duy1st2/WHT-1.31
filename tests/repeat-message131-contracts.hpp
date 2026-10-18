#pragma once
#include "repeat_message131.hpp"
inline void CheckRepeatMessage131(){
 wht::RepeatMessage131 r;unsigned sends=0;auto send=[&](const std::string& text){check(text=="hello","repeat preserves text");++sends;return true;};
 check(r.Command(L"3 hello"),"repeat accepts count and message");
 check(!r.Tick(true,false,1000,send)&&!sends,"typing/menu/focus gate pauses repeat");
 check(r.Tick(true,true,1000,send)&&r.queue.remaining==2,"repeat sends one native message");
 check(!r.Tick(true,true,1249,send)&&sends==1,"repeat enforces cadence");
 check(r.Tick(true,true,9000,send)&&sends==2&&r.queue.remaining==1,"late update never catches up");
 check(r.Tick(true,true,9250,send)&&r.queue.text.empty()&&!r.queue.remaining,"repeat stops exactly at count");
 check(!r.Tick(true,true,9500,send)&&sends==3,"completed repeat stays stopped");
 check(r.Command(L"10000 hello")&&r.Command(L"stop")&&!r.queue.remaining,"stop cancels pending repeat");
 for(auto value:{L"0 hello",L"10001 hello",L"-1 hello",L"1",L"1 #speed 2",L"1 a\nb",L"stop extra"})check(!r.Command(value),"invalid repeat rejected");
 check(r.Command(L"2 hello"),"repeat restart");
 check(!r.Tick(true,true,10000,[](const auto&){return false;})&&r.queue.remaining==2&&r.queue.due==10250,"refused native chat retains count and backs off");
 check(!r.Tick(false,true,11000,send)&&!r.queue.remaining,"lost session cancels queue");
 check(r.Command(L"1 " + std::wstring(128,L'x'))&&!r.Command(L"1 "+std::wstring(129,L'x')),"strict UTF8 boundary no truncation");
 check(r.Command(L"1 \u0111\u00e3 g\u1eedi")&&r.queue.text==wht::Utf8(L"\u0111\u00e3 g\u1eedi"),"Vietnamese survives repeat");
 r.Reset();check(!r.queue.remaining&&r.queue.text.empty(),"new match clears queue");
}
