#pragma once
#include "../native/fps_text_cadence.hpp"
#include "../native/ping_reading.hpp"
#include <limits>
template<class Check> void TestNativePingReading(Check check){
 {wht::FpsTextCadence cadence;
 check(cadence.Allow(1," FPS: 60.0",0),"first FPS label appears immediately");
 check(!cadence.Allow(1," FPS: 61.0",16)&&!cadence.Allow(1," FPS: 62.0",999),"FPS label is held for a full real second");
 check(cadence.Allow(1," FPS: 63.0",1000)&&!cadence.Allow(1," FPS: 64.0",1016),"FPS refreshes once at the one-second boundary");
 check(cadence.Allow(2," FPS: 70.0",1016),"different FPS font initializes immediately");
 check(cadence.Allow(1,"Ping: 42 ms",1020)&&cadence.Allow(1,"",1021)&&cadence.Allow(1," FPS: 72.0",1022),"other text and hide/re-enable bypass FPS throttle");
 check(cadence.Allow(1," FPS: 73.0",1),"clock rollback cannot freeze FPS updates");}

 alignas(uintptr_t) unsigned char memory[512]{};const auto base=reinterpret_cast<uintptr_t>(memory);wht::NativePingProfile p{16,256,sizeof(void*)==8};auto code=memory+16;
 if(p.wide){code[0]=0xf3;code[1]=0x0f;code[2]=0x10;code[3]=5;*reinterpret_cast<int32_t*>(code+4)=256-16-8;code[8]=0xc3;}
 else {code[0]=0xd9;code[1]=5;*reinterpret_cast<uint32_t*>(code+2)=uint32_t(base+256);code[6]=0xc3;}
 auto& sample=*reinterpret_cast<float*>(memory+256);float value=-1;sample=413.25f;check(wht::ReadNativePing(base,p,value)&&value==sample,"native Ping getter resolves guarded float in milliseconds");check(wht::NativePingText(value)=="Ping: 413.25 ms (game native)","native Ping retains two decimal places without unit scaling");
 for(float invalid:{0.f,-1.f,60001.f,std::numeric_limits<float>::quiet_NaN()}){sample=invalid;value=123;check(!wht::ReadNativePing(base,p,value)&&value==123,"invalid/uninitialized native Ping cannot publish a measurement");}
 sample=22;code[0]^=1;check(!wht::ReadNativePing(base,p,value),"changed native getter refuses data access");code[0]^=1;p.data+=4;check(!wht::ReadNativePing(base,p,value),"native getter must reference the exact profiled global");check(!wht::ReadNativePing(0,p,value),"missing image cannot expose native Ping");
}
