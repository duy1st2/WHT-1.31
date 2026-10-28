#include "../native/fog_modifier_lifecycle.hpp"
#include <stdexcept>
#include <cstdio>
#include <functional>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Backend {
 wht::FogModifierContext context{1,2,3,4,0};
 bool available=true,boundsOK=true,createOK=true,startOK=true,stopOK=true;
 bool started=false,rectLive=false,modLive=false,unknown=false,destroyRect=true,destroyMod=true;
 unsigned bounds=0,creates=0,starts=0,stops=0,rectDestroys=0,modDestroys=0;
 unsigned reads=0,failRead=0;
 std::function<void()> onBounds,onCreate,onStart,onStop,onDestroy;
 bool ReadContext(wht::FogModifierContext& out){++reads;if(!available||reads==failRead)return false;out=context;return true;}
 template<class C> bool CreateBounds(const C&,wht::FogModifierReference& out){++bounds;rectLive=true;out={10,100,1,2};if(onBounds)onBounds();return boundsOK;}
 template<class C,class R> bool CreateModifier(const C&,const R& rect,wht::FogModifierReference& out){Check(rect.handle==10&&rectLive,"valid owned rectangle supplied to native constructor");++creates;modLive=true;out={20,200,3,4};if(onCreate)onCreate();return createOK;}
 template<class C,class R> wht::FogReferenceState Inspect(const C& expected,const R& owned,bool rectangle){Check(wht::SameFogWorld(expected,context),"never inspect old-world handle");Check(owned.handle==(rectangle?10u:20u),"typed handle ownership");if(unknown)return wht::FogReferenceState::Unknown;return (rectangle?rectLive:modLive)?wht::FogReferenceState::Live:wht::FogReferenceState::Gone;}
 template<class C,class R> bool Start(const C&,const R& owned){Check(owned.handle==20&&modLive&&!rectLive,"bounds released before activation");++starts;started=startOK;if(onStart)onStart();return startOK;}
 template<class C,class R> bool Active(const C&,const R&){return started&&modLive;}
 template<class C,class R> bool Stop(const C&,const R& owned){Check(owned.handle==20&&modLive,"stop only live owned modifier");++stops;if(stopOK)started=false;if(onStop)onStop();return stopOK;}
 template<class C,class R> bool Destroy(const C& expected,const R&,bool rectangle){Check(wht::SameFogWorld(expected,context),"never destroy old-world handle");if(rectangle){++rectDestroys;if(destroyRect)rectLive=false;}else{++modDestroys;if(destroyMod)modLive=false;}if(onDestroy)onDestroy();return true;}
};
int main(){try{
 using Phase=wht::FogModifierPhase;std::string report;
 {Backend api;wht::FogModifierLifecycle state;
  Check(state.Request(true,api,report)&&state.Phase()==Phase::Active&&api.bounds==1&&api.creates==1&&api.starts==1&&api.rectDestroys==1,"one create and confirmed start");
  Check(state.Request(true,api,report)&&api.bounds==1&&api.starts==1,"already enabled no allocation");
  Check(state.Request(false,api,report)&&state.Phase()==Phase::Off&&api.stops==1&&api.modDestroys==1,"stop and confirmed destroy");
  Check(state.Request(false,api,report)&&api.stops==1,"already disabled no duplicate cleanup");}
 {Backend api;wht::FogModifierLifecycle state;api.createOK=false;
  Check(!state.Request(true,api,report)&&state.Phase()==Phase::PendingCleanup&&!api.starts,"partial constructor reference retained");
  Check(!state.Request(true,api,report)&&api.creates==1,"pending state cannot allocate another modifier");
  Check(state.Request(false,api,report)&&state.Phase()==Phase::Off&&api.modDestroys==1&&api.rectDestroys==1,"partial constructor cleanup releases both owned resources");}
 {Backend api;wht::FogModifierLifecycle state;api.boundsOK=false;
  Check(!state.Request(true,api,report)&&!api.creates&&state.HasResources(),"uncertain rectangle returned by native stays owned");
  Check(state.Request(false,api,report)&&api.rectDestroys==1,"failed bounds allocation still cleaned");}
 {Backend api;wht::FogModifierLifecycle state;api.destroyRect=false;
  Check(!state.Request(true,api,report)&&!api.starts&&state.Phase()==Phase::PendingCleanup&&api.rectDestroys==1,"unconfirmed rectangle destruction blocks activation without retry");
  api.destroyRect=true;Check(state.Request(false,api,report)&&api.modDestroys==1,"explicit cleanup can resolve pending resources");}
 {Backend api;wht::FogModifierLifecycle state;api.startOK=false;
  Check(!state.Request(true,api,report)&&state.Phase()==Phase::PendingCleanup,"uncertain start never claimed active");
  Check(state.Request(false,api,report)&&api.stops==1&&api.modDestroys==1,"uncertain activation can be stopped and destroyed");}
 {Backend api;wht::FogModifierLifecycle state;Check(state.Request(true,api,report),"setup active");api.stopOK=false;
  Check(!state.Request(false,api,report)&&state.Phase()==Phase::PendingCleanup&&!api.modDestroys,"unconfirmed stop retains modifier");
  api.stopOK=true;api.destroyMod=false;Check(!state.Request(false,api,report)&&state.HasResources()&&api.modDestroys==1,"unconfirmed destroy retained after one invocation");
  api.modLive=false;Check(state.Request(false,api,report)&&api.modDestroys==1,"already destroyed serial is not destroyed twice");}
 {Backend api;wht::FogModifierLifecycle state;Check(state.Request(true,api,report),"setup active");api.unknown=true;
  Check(!state.Request(false,api,report)&&state.HasResources()&&!api.stops&&!api.modDestroys,"unknown handle identity never mutated");
  api.available=false;Check(!state.Request(false,api,report)&&state.HasResources(),"unreadable context does not discard ownership");}
 {Backend api;wht::FogModifierLifecycle state;Check(state.Request(true,api,report),"setup active");++api.context.generation;
  Check(state.Request(false,api,report)&&state.Phase()==Phase::Off&&!api.modDestroys&&!api.stops,"new generation never uses old handles even when addresses match");}
 {Backend api;wht::FogModifierLifecycle state;Check(state.Request(true,api,report),"setup active");++api.context.player;++api.context.local;
  Check(state.Request(true,api,report)&&state.Phase()==Phase::Active&&api.modDestroys==1&&api.creates==2,"player change cleans old modifier in same world before rebinding");}
 for(unsigned stage=0;stage<3;++stage){Backend api;wht::FogModifierLifecycle state;
  auto end=[&](){state.OnWorldGone();++api.context.generation;};if(stage==0)api.onBounds=end;else if(stage==1)api.onCreate=end;else api.onStart=end;
  Check(!state.Request(true,api,report)&&state.Phase()==Phase::Off&&!state.HasResources()&&!api.modDestroys,"teardown during callback cannot publish stale references or active state");}
 {Backend api;wht::FogModifierLifecycle state;api.onCreate=[&](){std::string inner;Check(!state.Request(true,api,inner)&&inner.find("already running")!=inner.npos,"reentrant request rejected");};
  Check(state.Request(true,api,report)&&api.creates==1,"outer operation completes without duplicate callback allocation");}
 {Backend api;wht::FogModifierLifecycle state;api.failRead=2;
  Check(!state.Request(true,api,report)&&state.HasResources()&&!api.creates,"context failure after allocation retains ownership without further engine calls");
  api.failRead=0;Check(state.Request(false,api,report),"later explicit cleanup releases retained bounds");}
 puts("fog modifier ownership lifecycle contracts passed (native backend not enabled)");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
