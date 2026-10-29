#pragma once
#include "delay_reducer_profile131.hpp"
#include "menu_fog_vertex131.hpp"
namespace wht {
inline std::atomic<unsigned> delayLoop131{100},delayLan131{100},delayNet131{100};
inline std::atomic<unsigned>* DelayValue131(unsigned i){return i==0?&delayLoop131:i==1?&delayLan131:&delayNet131;}
#ifdef _WIN64
#define WHT_DELAY_ABI
#define WHT_DELAY_ARGS uintptr_t
#else
#define WHT_DELAY_ABI __fastcall
#define WHT_DELAY_ARGS uintptr_t,void*
#endif
inline unsigned WHT_DELAY_ABI DelayLoopCall131(WHT_DELAY_ARGS){return delayLoop131.load();}
inline unsigned WHT_DELAY_ABI DelayLanCall131(WHT_DELAY_ARGS){return delayLan131.load();}
inline unsigned WHT_DELAY_ABI DelayNetCall131(WHT_DELAY_ARGS){return delayNet131.load();}
#undef WHT_DELAY_ABI
#undef WHT_DELAY_ARGS
inline uintptr_t DelayCallback131(unsigned i){return i==0?reinterpret_cast<uintptr_t>(DelayLoopCall131):i==1?reinterpret_cast<uintptr_t>(DelayLanCall131):reinterpret_cast<uintptr_t>(DelayNetCall131);}
inline bool DelaySlotRead131(uintptr_t address,uintptr_t& value){__try{if(!address||address%alignof(uintptr_t))return false;value=*reinterpret_cast<uintptr_t*>(address);return true;}__except(1){return false;}}
inline bool ParseDelayReducer131(const std::wstring& args,unsigned& value){if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;const auto first=args.find_first_not_of(L" \t"),last=args.find_last_not_of(L" \t");if(first==args.npos)return false;const auto text=args.substr(first,last-first+1);unsigned n=0;for(auto c:text){if(c<L'0'||c>L'9')return false;n=n*10+unsigned(c-L'0');if(n>1000)return false;}if(!n)return false;value=n;return true;}
struct DelayReducer131 {
 std::array<NativeDataSlot131,3> slots;bool warned=false;
 void Normalize(unsigned i){auto& slot=slots[i];uintptr_t current=0;if(slot.address&&DelaySlotRead131(slot.address,current)&&current==slot.original)slot.address=slot.original=slot.replacement=0;}
 bool Available(uintptr_t base,const DelayEntry131& p,unsigned i){if(!DelayEntryCode131(base,p,i))return false;Normalize(i);const auto at=base+p.table+10*sizeof(uintptr_t);uintptr_t current=0;if(!DelaySlotRead131(at,current))return false;return current==base+p.getter||(slots[i].address==at&&slots[i].replacement==DelayCallback131(i)&&current==slots[i].replacement);}
 bool Ready(uintptr_t base,unsigned build,unsigned scope){const auto* p=DelayProfileFor131(build);if(!p||(scope!=1&&scope!=2))return false;const unsigned first=scope==1?0:2,last=scope==1?2:3;for(unsigned i=first;i<last;++i)if(!Available(base,p->entries[i],i))return false;return true;}
 bool Restore(unsigned first,unsigned last){bool ok=true;for(unsigned i=first;i<last;++i){DelayValue131(i)->store(100);Normalize(i);const bool restored=slots[i].Restore();ok=restored&&ok;}return ok;}
 bool Reset(){return Restore(0,3);}
 bool ApplyGroup(uintptr_t base,const DelayProfile131& p,unsigned first,unsigned last,bool enabled,const int* values){
  bool ready=true;if(enabled)for(unsigned i=first;i<last;++i)if(values[i]<1||values[i]>1000||!Available(base,p.entries[i],i))ready=false;
  if(!enabled||!ready){const bool restored=Restore(first,last);return restored&&(!enabled||ready);}
  bool ok=true;for(unsigned i=first;i<last;++i){DelayValue131(i)->store(unsigned(values[i]));const bool changed=values[i]==100?slots[i].Restore():slots[i].Install(base+p.entries[i].table+10*sizeof(uintptr_t),base+p.entries[i].getter,DelayCallback131(i));ok=changed&&ok;}
  if(!ok)Restore(first,last);return ok;
 }
 bool Update(uintptr_t base,unsigned build,bool allowed,const Config& config,std::string& report){
  const auto scopes=allowed?(config.delayReducerScopes&3u):0u;bool ok=true;if(!scopes){if(slots[0].address||slots[1].address||slots[2].address)ok=Reset();}
  else if(const auto* p=DelayProfileFor131(build)){const int values[]={config.localDelay,config.lanDelay,config.netDelay};const bool local=ApplyGroup(base,*p,0,2,(scopes&1)!=0,values);const bool net=ApplyGroup(base,*p,2,3,(scopes&2)!=0,values);ok=local&&net;}
  else{Reset();ok=false;}
  if(!ok&&!warned)report="Delay Reducer: provider profile or callback ownership refused; requested changes may be unavailable";warned=!ok;return ok;
 }
};
template<class Available,class Apply,class Save,class Reload> bool DelayReducerCommand131(bool net,const std::wstring& args,Config& config,Available available,Apply apply,Save save,Reload reload,std::string& report){
 unsigned value=0;if(!ParseDelayReducer131(args,value)){report=net?"Usage: #drn <integer 1..1000 ms>":"Usage: #dr <integer 1..1000 ms> (Local and LAN)";return false;}const unsigned scope=net?2u:1u;if(!available(scope)){report="Delay Reducer: active settings and verified unowned provider callbacks required";return false;}
 const auto key=net?L"Net Game Delay Reducer":L"Local Game Delay Reducer";const int old=net?config.netDelay:config.localDelay,oldLan=config.lanDelay;const unsigned oldScopes=config.delayReducerScopes,newScopes=oldScopes|scope;const auto text=std::to_wstring(value);
 const bool primary=save(key,text),lan=net?true:save(L"Lan Game Delay Reducer",text);const bool activation=primary&&lan?save(L"Delay Reducer Scopes",std::to_wstring(newScopes)):false;reload();
 if(!primary||!lan||!activation||(net?config.netDelay:config.localDelay)!=int(value)||(!net&&config.lanDelay!=int(value))||config.delayReducerScopes!=newScopes){
  const bool a=save(key,std::to_wstring(old)),b=net?true:save(L"Lan Game Delay Reducer",std::to_wstring(oldLan)),c=save(L"Delay Reducer Scopes",std::to_wstring(oldScopes));reload();
  const bool restored=a&&b&&c&&(net?config.netDelay:config.localDelay)==old&&(net||config.lanDelay==oldLan)&&config.delayReducerScopes==oldScopes;
  report=restored?"Delay Reducer: could not save request; original settings restored":"Delay Reducer: settings save incomplete; inspect INI before continuing";return false;
 }
 report=LegacySettingNotice131(key,text);if(!apply()){report+="; saved, but native callbacks were not fully applied";return false;}return true;
}
}
