#include "../native/overhead_sampling131.hpp"
#include "../native/status_cache131.hpp"
#ifndef _WIN64
#include "../native/state_bars131.hpp"
#else
#include "../native/gameplay64.hpp"
#endif
#include <stdexcept>
#include <cstdio>
void Check(bool b,const char* s){if(!b)throw std::runtime_error(s);}
#ifndef _WIN64
unsigned textCalls=0;
void __fastcall TextFixture(void*,void*,const char* text){Check(!*text,"clear sends empty text");++textCalls;}
struct FakeMenu {wht::NativeFrameApi api{};void* independentHost=nullptr;};
#endif
ULONGLONG sampledClock=100;
ULONGLONG WINAPI SampleClock(){return sampledClock;}
unsigned nativeFloatReads=0,nativeLabelWrites=0;uintptr_t expectedProperty=0;alignas(uintptr_t) unsigned char resolvedFloat[0xe0]{};
uintptr_t __cdecl FloatResolve(uint32_t,uint32_t){return reinterpret_cast<uintptr_t>(resolvedFloat);}
#ifdef _WIN64
void* FloatCurrent(uintptr_t property,float* out){
#else
void* __fastcall FloatCurrent(uintptr_t property,void*,float* out){
#endif
 Check(property==expectedProperty,"native property receiver");++nativeFloatReads;*out=25;return out;
}
#ifdef _WIN64
void* FloatMaximum(uintptr_t property,float* out){
#else
void* __fastcall FloatMaximum(uintptr_t property,void*,float* out){
#endif
 Check(property==expectedProperty,"native maximum receiver");++nativeFloatReads;*out=100;return out;
}
#ifdef _WIN64
void LabelWrite(void*,const char*){++nativeLabelWrites;}
#else
void __fastcall LabelWrite(void*,void*,const char*){++nativeLabelWrites;}
#endif
void NativeSamplingIntegration(){
 alignas(uintptr_t) unsigned char frame[0x280]{},unit[0x700]{},host[0x180]{};
 const auto f=reinterpret_cast<uintptr_t>(frame),u=reinterpret_cast<uintptr_t>(unit),h=reinterpret_cast<uintptr_t>(host);
 *reinterpret_cast<uintptr_t*>(frame)=0x2000;*reinterpret_cast<uintptr_t*>(unit)=0x1000;*reinterpret_cast<uintptr_t*>(host)=0x4000;
 wht::Config config;config.stateTagEnable=config.stateTagNonHero=config.stateTagHpValue=true;config.stateTagManaValue=config.stateTagShowLevel=config.stateTagShowMax=false;config.stateTagFontSize=4;
 wht::NativeFrameApi raw{};raw.setText=reinterpret_cast<decltype(raw.setText)>(LabelWrite);
#ifdef _WIN64
 *reinterpret_cast<unsigned*>(frame+0x1e8)=5;*reinterpret_cast<uintptr_t*>(frame+0x210)=u;frame[0xf0]=frame[0xf1]=host[0xf0]=host[0xf1]=1;*reinterpret_cast<uintptr_t*>(host+0xc0)=f;*reinterpret_cast<uint32_t*>(unit+0x18)=1;expectedProperty=u+0x200;*reinterpret_cast<uintptr_t*>(unit+0x200)=0x3000;
 wht::Profile64 p{};p.statVtable=0x2000;p.unitVtable=0x1000;p.floatVtable=0x3000;p.simpleVtable=0x4000;p.resolve=reinterpret_cast<uintptr_t>(FloatResolve);p.floatCurrent=reinterpret_cast<uintptr_t>(FloatCurrent);p.floatMaximum=reinterpret_cast<uintptr_t>(FloatMaximum);
 wht::Gameplay64 overlay;auto& e=overlay.world[f];e.host=reinterpret_cast<void*>(h);e.values[0]=reinterpret_cast<void*>(1);e.placed=true;e.fontSizes[0]=.006f;
 auto paint=[&](){overlay.WriteBar(f,0,p,config,true,raw);};const unsigned idOffset=0x18;
#else
 *reinterpret_cast<unsigned*>(frame+0x134)=5;*reinterpret_cast<uintptr_t*>(frame+0x154)=u;frame[0x9c]=frame[0x9d]=host[0x9c]=host[0x9d]=1;*reinterpret_cast<uintptr_t*>(host+0x78)=f;*reinterpret_cast<uint32_t*>(unit+0xc)=1;unit[0x37]='h';expectedProperty=u+0x1a4;*reinterpret_cast<uintptr_t*>(unit+0x1a4)=0x3000;
 wht::Profile131 p{};p.simpleVtable=0x4000;p.readFloat=reinterpret_cast<uintptr_t>(FloatCurrent);wht::RegenProfile131 regen{};regen.unit=0x1000;regen.floatType=0x3000;wht::StateBarProfile131 bar{};bar.bar=0x2000;bar.maximum=reinterpret_cast<uintptr_t>(FloatMaximum);
 wht::StateBars131 overlay;auto& e=overlay.entries[f];e.host=reinterpret_cast<void*>(h);e.fonts[0]=reinterpret_cast<void*>(1);e.layoutSize=1;e.fontSizes[0]=.006f;
 auto paint=[&](){overlay.WriteBar(f,0,p,regen,bar,config,true,raw);};const unsigned idOffset=0xc;
#endif
 overlay.sampling.sampleClock=SampleClock;overlay.sampling.Configure(1,0,config,true);sampledClock=100;
 for(unsigned i=0;i<100;++i)paint();Check(nativeFloatReads==2&&nativeLabelWrites==1,"actual overhead writer queries floats only once for 100 fresh callbacks");
 sampledClock=299;paint();Check(nativeFloatReads==2,"actual writer skips before sample deadline");sampledClock=300;paint();Check(nativeFloatReads==4&&nativeLabelWrites==1,"actual writer resamples at 200 ms without rewriting unchanged labels");
 ++*reinterpret_cast<uint32_t*>(unit+idOffset);sampledClock=301;paint();Check(nativeFloatReads==6,"pooled native identity change resamples immediately");
 config.stateTagShowMax=true;paint();Check(nativeFloatReads==8&&nativeLabelWrites==2,"format change resamples and rewrites immediately");
 overlay.sampling.Configure(2,0,config,true);paint();Check(nativeFloatReads==10,"new root resamples actual native fields");
 overlay.sampling.frameLoad.Poll(2,true,true,1000,[](float& fps){fps=20;return true;});overlay.sampling.frameLoad.Poll(2,true,true,2000,[](float& fps){fps=20;return true;});Check(overlay.sampling.frameLoad.policy.level==2,"actual writer LOD fixture");
 sampledClock=2000;paint();Check(nativeFloatReads==12,"actual ordinary unit samples at low FPS");sampledClock=2199;paint();Check(nativeFloatReads==12,"actual writer suppresses expensive reads at low FPS");sampledClock=3199;paint();Check(nativeFloatReads==12,"actual ordinary unit held for1200 ms");sampledClock=3200;paint();Check(nativeFloatReads==14,"actual writer samples at adaptive deadline");
 overlay.sampling.Configure(2,u,config,true);sampledClock=3400;paint();Check(nativeFloatReads==16,"selected actual native unit retains200 ms priority despite low FPS");

}
int main(){try{
 wht::Config config;wht::OverheadSampling131 samples;samples.Configure(1,7,config,true);unsigned count=0,sweeps=0;
 for(uint64_t now=0;now<1000;now+=10){count+=samples.Unit(2,3,4,false,now,config,true);sweeps+=samples.Full(now);}
 Check(count==5&&sweeps==4,"200 ms baseline samples and 300 ms sweeps");
 samples.Reset();samples.Configure(1,7,config,true);samples.Load(60);Check(samples.interval==400,"crowd level one");unsigned normal=0,selected=0,hero=0;
 for(uint64_t now=0;now<800;now+=10){normal+=samples.Unit(2,3,4,false,now,config,true);selected+=samples.Unit(7,8,9,false,now,config,true);hero+=samples.Unit(10,11,12,true,now,config,true);}
 Check(normal==2&&selected==4&&hero==4,"crowd cadence preserves selected/hero priority");samples.Load(120);Check(samples.interval==800,"crowd level two");samples.Load(100);Check(samples.interval==800,"crowd hysteresis");samples.Load(99);Check(samples.interval==400,"recover to crowd level one");samples.Load(39);Check(samples.interval==200,"recover normal cadence");
 samples.Reset();samples.Configure(1,7,config,true);Check(samples.Unit(2,3,4,false,1,config,true)&&!samples.Unit(2,3,4,false,2,config,true),"steady identity skips sample");Check(samples.Unit(2,3,5,false,2,config,true),"new native frame bypasses sample cache");Check(samples.Unit(2,4,5,false,2,config,true),"recycled unit identity bypasses cache");samples.Invalidate(2);Check(samples.Unit(2,4,5,false,2,config,true),"invalid owned child forces resample");
 samples.Full(5);config.stateTagShowMax=!config.stateTagShowMax;samples.Configure(1,7,config,true);Check(samples.Full(6)&&samples.Unit(2,4,5,false,6,config,true),"setting change refreshes sweep and sample immediately");samples.Configure(1,7,config,false);Check(samples.Full(7)&&samples.Unit(2,4,5,false,7,config,false)&&samples.Unit(2,4,5,false,8,config,false),"disable cleanup never throttled");samples.Configure(9,7,config,true);Check(samples.Full(8)&&samples.Unit(2,4,5,false,8,config,true),"new world root discards history");samples.Reset();Check(!samples.Full(9)&&samples.Unit(2,4,5,false,9,config,true),"no root cannot cache native state");

 {wht::FrameLoadSampler131 load;float fps=40;unsigned polls=0;bool valid=true;auto read=[&](float& f){++polls;f=fps;return valid;};
  for(uint64_t now=0;now<1000;now+=10)load.Poll(1,true,true,now,read);Check(polls==4&&!load.policy.level,"FPS polling capped at4 per second, dwell respected");load.Poll(1,true,true,1000,read);Check(load.policy.level==1&&load.policy.Interval(200)==600&&load.policy.SweepInterval()==400,"sustained40 FPS selects WHT level1");
  fps=20;load.Poll(1,true,true,1250,read);Check(load.policy.level==2&&load.policy.Interval(800)==1200&&load.policy.SweepInterval()==600,"below30 selects WHT level2");fps=60;load.Poll(1,true,true,1500,read);load.Poll(1,true,true,6499,read);Check(load.policy.level==2,"recovery waits five seconds");load.Poll(1,true,true,6750,read);Check(load.policy.level==1,"first recovery step");load.Poll(1,true,true,11750,read);Check(load.policy.level==0,"second recovery step");
  fps=20;load.Poll(2,true,true,12000,read);Check(!load.policy.level&&load.owner==2,"new UI owner discards old dwell");const auto before=polls;load.Poll(2,true,false,14000,read);Check(polls==before,"background has no FPS reads");load.Poll(2,true,true,15000,read);Check(!load.policy.level,"background duration excluded");valid=false;load.Poll(2,true,true,16000,read);valid=true;load.Poll(2,true,true,17000,read);Check(!load.policy.level,"failed native read breaks low FPS dwell");load.Poll(2,true,true,18000,read);Check(load.policy.level==2,"fresh valid samples resume adaptation");load.Poll(2,true,true,1,read);Check(!load.policy.level,"clock rollback resets policy");const auto disabledBefore=polls;load.Poll(2,false,true,25000,read);Check(!load.owner&&!load.policy.level&&polls==disabledBefore,"settings/authorization disable stops native reads and clears policy");load.Poll(0,true,true,26000,read);Check(!load.owner&&polls==disabledBefore,"no UI does not read native FPS");
 }
 {wht::OverheadSampling131 load;load.frameLoad.Poll(1,true,true,0,[](float& f){f=20;return true;});load.frameLoad.Poll(1,true,true,1000,[](float& f){f=20;return true;});load.Configure(1,7,config,true);Check(load.frameLoad.policy.level==2,"first configure preserves matching UI FPS sample");unsigned normalCount=0,selectedCount=0,heroCount=0,fullCount=0;
  for(uint64_t now=1000;now<3400;now+=10){normalCount+=load.Unit(2,3,4,false,now,config,true);selectedCount+=load.Unit(7,8,9,false,now,config,true);heroCount+=load.Unit(10,11,12,true,now,config,true);fullCount+=load.Full(now);}
  Check(normalCount==2&&selectedCount==12&&heroCount==12&&fullCount==4,"1200 ms ordinary sampling,200 ms priority,600 ms sweeps");load.Load(120);load.frameLoad.policy.level=1;Check(load.frameLoad.policy.Interval(load.interval)==800,"FPS policy never speeds up denser crowd cadence");
  config.stateTagShowMax=!config.stateTagShowMax;load.Configure(1,7,config,true);Check(load.Full(3400)&&load.Unit(2,3,4,false,3400,config,true),"settings changes refresh immediately even under FPS LOD");load.Configure(1,7,config,false);Check(!load.frameLoad.policy.level&&load.Full(3401)&&load.Unit(2,3,4,false,3401,config,false),"disable cleanup bypasses low FPS throttle");load.Configure(2,7,config,true);Check(!load.frameLoad.owner&&load.Full(3402),"changed owner resets sampler and sweep");
 }
 wht::ButtonUpdateCadence buttons;count=0;for(uint64_t now=0;now<1000;now+=10)count+=buttons.Due(now,1,2,nullptr,1);Check(count==10,"100 ms panel queries");Check(buttons.Due(991,1,3,nullptr,1),"selection change immediate");Check(buttons.Due(992,2,3,nullptr,1),"panel owner change immediate");Check(buttons.Due(993,2,3,nullptr,0),"disable clears immediately");Check(buttons.Due(994,2,3,&config,0),"profile change immediate");Check(buttons.Due(0,2,3,&config,0),"clock rollback rechecks");
 wht::StatusCache131 status;unsigned reads=0,writes=0;float memory=512.25f;bool readOK=true,writeOK=true;std::string shown;
 auto read=[&](float& mb,float& percent){++reads;mb=memory;percent=1;return readOK;};auto write=[&](const std::string& text){++writes;if(!writeOK)return false;shown=text;return true;};
 for(uint64_t now=0;now<1000;now+=10)status.Update(now,1,2,"WHT",true,true,false,read,write);
 Check(reads==2&&writes==1&&shown=="WHT | 512 MB","500 ms RAM samples, unchanged text written once");memory=600;status.Update(1000,1,2,"WHT",true,true,false,read,write);Check(reads==3&&writes==2&&shown=="WHT | 600 MB","changed memory displayed");
 status.Update(1001,1,2,"WHT",false,true,false,read,write);Check(reads==3&&shown.find("Login")!=shown.npos,"authorization text immediate without memory query");status.Update(1002,1,2,"WHT",true,true,false,read,write);Check(reads==4&&shown=="WHT | 600 MB","authorization recovery samples immediately");status.Update(1003,1,2,"WHT",true,true,true,read,write);Check(reads==4&&shown=="WHT","external HUD avoids duplicate memory work");status.Update(1004,1,2,"WHT",true,false,false,read,write);Check(reads==4&&shown=="WHT","hidden memory no query");
 status.Update(1005,1,3,"WHT",true,true,false,read,write);Check(reads==5&&shown=="WHT | 600 MB","new font resets cache");status.Update(1006,9,3,"WHT",true,true,false,read,write);Check(reads==6,"new root resets cache");
 readOK=false;memory=999;status.Update(1506,9,3,"WHT",true,true,false,read,write);Check(shown=="WHT | 600 MB","failed sample preserves last good label");
 status.Reset();writeOK=false;const auto before=writes;status.Update(1600,1,2,"WHT",true,true,false,read,write);writeOK=true;status.Update(1601,1,2,"WHT",true,true,false,read,write);Check(writes==before+2&&status.textKnown,"failed text write remains retryable");status.Update(1602,0,0,"WHT",true,true,false,read,write);Check(!status.owner&&!status.textKnown,"ended root clears caches");
#ifndef _WIN64
 FakeMenu menu;menu.api.setText=reinterpret_cast<decltype(menu.api.setText)>(TextFixture);wht::CooldownOverlay131 overlay;overlay.entries[0].fonts={reinterpret_cast<void*>(1),reinterpret_cast<void*>(2)};overlay.entries[0].text="5";overlay.Update(0,0,wht::cooldownProfiles131[0],false,menu);Check(textCalls==2,"disable clears both existing cooldown layers");overlay.Update(0,1,wht::cooldownProfiles131[0],false,menu);Check(textCalls==2,"empty cooldown text is not repeatedly rewritten");
#endif
 NativeSamplingIntegration();
 puts("WHT sampling/cache contracts passed: baseline5/100, sweep4/100, panel10/100, RAM2/100, steady text1/100");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
