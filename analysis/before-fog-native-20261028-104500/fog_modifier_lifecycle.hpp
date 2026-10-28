#pragma once
#include <cstdint>
#include <string>
namespace wht {
// UI-thread ownership contract for the pending #mh backend. Native addresses,
// handle conversion and destruction must be verified before wiring this state.
struct FogModifierContext {
 uintptr_t world=0,session=0,player=0;
 uint64_t generation=0;
 unsigned local=24;
 bool Valid()const{return world&&session&&player&&generation&&local<24;}
};
inline bool SameFogWorld(const FogModifierContext& a,const FogModifierContext& b){return a.Valid()&&b.Valid()&&a.world==b.world&&a.session==b.session&&a.generation==b.generation;}
inline bool SameFogContext(const FogModifierContext& a,const FogModifierContext& b){return SameFogWorld(a,b)&&a.player==b.player&&a.local==b.local;}
struct FogModifierReference {
 uint32_t handle=0;
 uintptr_t object=0;
 uint32_t serialLow=0,serialHigh=0;
 bool Owned()const{return handle!=0;}
 bool Identified()const{return Owned()&&object&&(serialLow||serialHigh);}
};
enum class FogReferenceState {Live,Gone,Unknown};
enum class FogModifierPhase {Off,Active,PendingCleanup};

class FogModifierLifecycle {
 FogModifierContext context_;
 FogModifierReference bounds_,modifier_;
 uint64_t epoch_=0;
 bool active_=false,busy_=false;
 void Forget(){context_={};bounds_={};modifier_={};active_=false;++epoch_;}
 template<class Backend> bool Check(Backend& api,const FogModifierContext& expected,uint64_t epoch,bool samePlayer){
  if(epoch_!=epoch)return false;
  FogModifierContext now;if(!api.ReadContext(now)||!now.Valid())return false;
  if(!SameFogWorld(expected,now)){Forget();return false;}
  return !samePlayer||SameFogContext(expected,now);
 }
 template<class Backend> bool DestroyOne(Backend& api,FogModifierReference& reference,bool rectangle,const FogModifierContext& expected,uint64_t epoch){
  if(!reference.Owned())return true;
  if(!Check(api,expected,epoch,false))return false;
  const auto owned=reference;
  const auto state=api.Inspect(expected,owned,rectangle);
  if(!Check(api,expected,epoch,false))return false;
  if(state==FogReferenceState::Gone){reference={};return true;}
  if(state!=FogReferenceState::Live||!owned.Identified())return false;
  // The backend must refuse a reused handle/serial, even in the same world.
  api.Destroy(expected,owned,rectangle);
  if(!Check(api,expected,epoch,false))return false;
  const auto after=api.Inspect(expected,owned,rectangle);
  if(!Check(api,expected,epoch,false)||after!=FogReferenceState::Gone)return false;
  reference={};return true;
 }
 template<class Backend> bool Cleanup(Backend& api){
  const auto expected=context_;const auto epoch=epoch_;
  active_=false;
  if(modifier_.Owned()){
   if(!Check(api,expected,epoch,false))return !HasResources();
   const auto owned=modifier_;const auto state=api.Inspect(expected,owned,false);
   if(!Check(api,expected,epoch,false))return !HasResources();
   if(state==FogReferenceState::Unknown)return false;
   if(state==FogReferenceState::Live){
    if(!owned.Identified()||!api.Stop(expected,owned))return false;
    if(!Check(api,expected,epoch,false))return !HasResources();
   }
   if(!DestroyOne(api,modifier_,false,expected,epoch))return !HasResources();
   active_=false;
  }
  if(!DestroyOne(api,bounds_,true,expected,epoch))return !HasResources();
  Forget();return true;
 }
public:
 bool HasResources()const{return bounds_.Owned()||modifier_.Owned();}
 FogModifierPhase Phase()const{return active_?FogModifierPhase::Active:HasResources()?FogModifierPhase::PendingCleanup:FogModifierPhase::Off;}
 // Call only after confirmed world teardown. No engine call is made here, and
 // an operation in a reentrant native callback cannot publish stale resources.
 void OnWorldGone(){Forget();}
 template<class Backend> bool Request(bool enable,Backend& api,std::string& report){
  if(busy_){report="Maphack: operation already running";return false;}
  busy_=true;struct Unlock{bool& busy;~Unlock(){busy=false;}} unlock{busy_};
  FogModifierContext now;
  if(!api.ReadContext(now)||!now.Valid()){report="Maphack: match context unavailable";return false;}
  if(context_.Valid()&&!SameFogWorld(context_,now))Forget();
  if(!enable){
   if(!HasResources()){report="Maphack already OFF";return true;}
   if(!Cleanup(api)){report="Maphack: cleanup not confirmed; owned handles retained";return false;}
   report="Maphack: OFF";return true;
  }
  if(active_&&SameFogContext(context_,now)){
   const auto epoch=epoch_;const auto owned=modifier_;
   const auto state=api.Inspect(now,owned,false);
   if(Check(api,now,epoch,true)&&state==FogReferenceState::Live&&api.Active(now,owned)&&Check(api,now,epoch,true)){report="Maphack already ON";return true;}
   active_=false;if(epoch_==epoch&&state==FogReferenceState::Gone)modifier_={};
   report="Maphack: previous activation no longer confirmed";return false;
  }
  if(HasResources()){
   if(SameFogContext(context_,now)){report="Maphack: cleanup required before enabling";return false;}
   if(!Cleanup(api)){report="Maphack: previous player's cleanup not confirmed";return false;}
   if(!api.ReadContext(now)||!now.Valid()){report="Maphack: match context unavailable";return false;}
  }
  context_=now;const auto epoch=epoch_;
  FogModifierReference created;
  const bool boundsOK=api.CreateBounds(now,created);
  if(epoch_!=epoch){report="Maphack: world ended during bounds creation";return false;}
  bounds_=created;
  if(!Check(api,now,epoch,true)||!boundsOK||!bounds_.Identified()){report="Maphack: bounds creation not confirmed";return false;}
  created={};const bool modifierOK=api.CreateModifier(now,bounds_,created);
  if(epoch_!=epoch){report="Maphack: world ended during modifier creation";return false;}
  modifier_=created;
  if(!Check(api,now,epoch,true)||!modifierOK||!modifier_.Identified()){report="Maphack: modifier creation not confirmed; cleanup required";return false;}
  // Configuration copies the rectangle. Release its handle before activation.
  if(!DestroyOne(api,bounds_,true,now,epoch)||!Check(api,now,epoch,true)){report="Maphack: bounds cleanup not confirmed; modifier not started";return false;}
  const auto owned=modifier_;
  const bool started=api.Start(now,owned);
  if(!Check(api,now,epoch,true)||!started){report="Maphack: activation not confirmed; cleanup required";return false;}
  active_=true;report="Maphack: ON";return true;
 }
};
}
