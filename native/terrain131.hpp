#pragma once
#include "terrain_profile131.hpp"
#include "resource131.hpp"
#include "quick_drop_network131.hpp"
#include "ui_perf131.hpp"
#include <sstream>
namespace wht {
inline bool TerrainRawCode131(const std::wstring& text,uint32_t& code){if(text.size()!=4)return false;uint32_t value=0;for(auto c:text){if(c<33||c>126)return false;value=(value<<8)|uint32_t(c);}code=value;return true;}
struct TerrainRequest131 {bool query=false,off=false;std::wstring text;uint32_t code=0;};
inline bool ParseTerrain131(const std::wstring& args,TerrainRequest131& out){out={};if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;std::wistringstream in(args);std::wstring extra;if(!(in>>out.text)){out.query=true;return true;}if(in>>extra)return false;out.off=out.text==L"0"||out.text==L"off"||out.text==L"none"||out.text==L"clear";return out.off||TerrainRawCode131(out.text,out.code);}
using TerrainSetNative131=void(__cdecl*)(const float*,const float*,uint32_t,int,int,int);
using TerrainGetNative131=uint32_t(__cdecl*)(const float*,const float*);
inline bool InvokeTerrainSet131(TerrainSetNative131 fn,float x,float y,uint32_t code){if(!fn||!code||!std::isfinite(x)||!std::isfinite(y))return false;__try{fn(&x,&y,code,-1,7,1);return true;}__except(1){return false;}}
inline bool InvokeTerrainGet131(TerrainGetNative131 fn,float x,float y,uint32_t& code){if(!fn||!std::isfinite(x)||!std::isfinite(y))return false;__try{code=fn(&x,&y);return true;}__except(1){return false;}}
struct TerrainContext131 {uintptr_t root=0,world=0,player=0,session=0;unsigned local=24;};
inline bool SameTerrainContext131(const TerrainContext131& a,const TerrainContext131& b){return a.root&&a.world&&a.player&&a.session&&a.local<24&&a.root==b.root&&a.world==b.world&&a.player==b.player&&a.session==b.session&&a.local==b.local;}
inline bool ReadTerrainContext131(uintptr_t base,unsigned build,uintptr_t ui,TerrainContext131& out){const auto* p=ResourceProfileFor131(build);const auto* np=DropNetworkProfileFor131(build);const auto* inv=InventoryProfile(build);if(!ui||!p||!np||!inv||!ResourceCode131(base,*p))return false;const auto network=DropNetworkNative131(base,*np,*inv);if(!network)return false;
 __try{TerrainContext131 value;value.root=ui;value.session=DropSession131(network);if(!value.session)return false;value.world=ResourceWorld131(base,*p,value.local);if(!value.world||value.local>=24)return false;value.player=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,unsigned)>(base+p->playerFn)(value.world,value.local);if(!ResourceIdentity131(value.player,base,p->playerTable)||*reinterpret_cast<int*>(value.player+p->statusOffset)!=1||*reinterpret_cast<unsigned char*>(value.player+p->idOffset)>=24)return false;unsigned local=24;if(ResourceWorld131(base,*p,local)!=value.world||local!=value.local||DropSession131(network)!=value.session)return false;out=value;return true;}__except(1){return false;}
}
enum class TerrainStep131 {Idle,Pending,Complete,Failed,Interrupted};
struct TerrainJob131 {
 static constexpr unsigned side=118,total=side*side,scanLimit=256,writeLimit=8;
 unsigned index=0,confirmed=0;uint32_t code=0;uint64_t revision=0;bool running=false,busy=false;
 void Cancel(){running=false;++revision;}
 void Begin(uint32_t value){Cancel();index=confirmed=0;code=value;running=value!=0;}
 static void Point(unsigned at,float& x,float& y){x=-15000.f+float(at/side)*256.f;y=15000.f-float(at%side)*256.f;}
 template<class Get,class Set,class Clock> TerrainStep131 Tick(Get get,Set set,Clock now){
  if(!running)return TerrainStep131::Idle;if(busy)return TerrainStep131::Pending;busy=true;struct Unlock{bool& b;~Unlock(){b=false;}} unlock{busy};const auto serial=revision;const auto wanted=code;const auto start=now();unsigned scanned=0,writes=0;
  while(running&&revision==serial&&index<total&&scanned<scanLimit&&writes<writeLimit){if(scanned){const auto time=now();if(time<start||time-start>=2000)break;}float x=0,y=0;Point(index,x,y);uint32_t previous=0;const bool read=get(x,y,previous);if(revision!=serial)return TerrainStep131::Interrupted;if(!read){running=false;return TerrainStep131::Failed;}
   if(previous){const bool changed=set(x,y,wanted);if(revision!=serial)return TerrainStep131::Interrupted;if(!changed){running=false;return TerrainStep131::Failed;}++writes;uint32_t after=0;const bool verified=get(x,y,after);if(revision!=serial)return TerrainStep131::Interrupted;if(!verified||after!=wanted){running=false;return TerrainStep131::Failed;}++confirmed;}
   ++index;++scanned;
  }
  if(revision!=serial)return TerrainStep131::Interrupted;if(index==total){running=false;return confirmed?TerrainStep131::Complete:TerrainStep131::Failed;}return TerrainStep131::Pending;
 }
};
inline uint64_t TerrainMicroseconds131(){const auto frequency=UiPerfFrequency131(),ticks=UiPerfCounter131();return frequency?(ticks/frequency)*1000000+(ticks%frequency)*1000000/frequency:0;}
template<class Root> struct TerrainBackend131 {
 uintptr_t base;unsigned build;TerrainContext131 expected;Root root;
 bool Valid(){TerrainContext131 current;const auto* p=TerrainProfileFor131(build);return p&&TerrainCode131(base,*p)&&ReadTerrainContext131(base,build,root(),current)&&SameTerrainContext131(expected,current);}
 bool Get(float x,float y,uint32_t& code){const auto* p=TerrainProfileFor131(build);return p&&Valid()&&InvokeTerrainGet131(reinterpret_cast<TerrainGetNative131>(base+p->get),x,y,code)&&Valid();}
 bool Set(float x,float y,uint32_t code){const auto* p=TerrainProfileFor131(build);return p&&Valid()&&InvokeTerrainSet131(reinterpret_cast<TerrainSetNative131>(base+p->set),x,y,code)&&Valid();}
};
struct TerrainFeature131 {
 TerrainJob131 job;TerrainContext131 context;std::wstring wanted;bool attempted=false;
 void Reset(){job.Cancel();context={};wanted.clear();attempted=false;}
 template<class Root> void Update(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,const Config& config,Root root,std::string& report){
  if(!allowed||!ui||config.codeNen.empty()){if(attempted||job.running||context.world)Reset();return;}TerrainContext131 current;if(!ReadTerrainContext131(base,build,ui,current)){if(context.world)Reset();return;}
  if(!SameTerrainContext131(context,current)||wanted!=config.codeNen){job.Cancel();context=current;wanted=config.codeNen;attempted=false;}
  if(!attempted){attempted=true;uint32_t code=0;const auto* p=TerrainProfileFor131(build);if(!TerrainRawCode131(wanted,code)||!p||!TerrainCode131(base,*p)){report="Background code: invalid setting or native profile mismatch; not applied";return;}job.Begin(code);}
  if(!job.running)return;TerrainBackend131<Root> backend{base,build,context,root};const auto result=job.Tick([&](float x,float y,uint32_t& code){return backend.Get(x,y,code);},[&](float x,float y,uint32_t code){return backend.Set(x,y,code);},TerrainMicroseconds131);
  if(result==TerrainStep131::Complete)report="|CFFFCD211Background code|r: "+Utf8(wanted)+"; native readback confirmed "+std::to_string(job.confirmed)+" grid points";
  else if(result==TerrainStep131::Failed)report="Background code: native terrain operation/readback failed or no terrain in WHT grid; stopped (painted tiles remain)";
 }
};
template<class Save,class Reload> bool TerrainCommand131(uintptr_t base,unsigned build,const std::wstring& args,bool allowed,Config& config,TerrainFeature131& feature,Save save,Reload reload,std::string& report){
 TerrainRequest131 request;if(!ParseTerrain131(args,request)){report="Usage: #codenen / #code_nen / #nen [4 ASCII characters|off]";return false;}
 if(request.query){report=config.codeNen.empty()?"|CFFFCD211Current background code|r: Empty":"|CFFFCD211Current background code|r: |cff44ff3d"+Utf8(config.codeNen)+"|r";if(feature.job.running)report+="; applying "+std::to_string(feature.job.index)+"/"+std::to_string(TerrainJob131::total);return true;}
 if(!request.off){const auto* p=TerrainProfileFor131(build);if(!allowed||!p||!TerrainCode131(base,*p)){report="Background code: active settings and verified native profile required";return false;}}
 const auto value=request.off?std::wstring():request.text;const bool saved=save(value);reload();if(!saved||config.codeNen!=value){report="Background code: setting could not be saved; current job unchanged";return false;}feature.Reset();
 report=request.off?"|CFFFCD211Background code|r: |cffff5c5cOFF|r; further painting stopped, painted tiles remain until map reload":"|CFFFCD211Background code|r: |cff44ff3d"+Utf8(value)+"|r; queued for gradual application";return true;
}
}
