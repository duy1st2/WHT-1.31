#pragma once
#include "save_notice_profile131.hpp"
#include "resource131.hpp"
#include "notification131.hpp"
#include <mutex>
#include <deque>
#include <fstream>
#include <filesystem>
namespace wht {
struct SaveNoticeContext131 {uintptr_t root=0,world=0;uint64_t epoch=0;};
struct SaveNoticeEvent131 {unsigned slot=24;uint64_t time=0;};
struct SaveNoticeQueue131 {
 std::mutex mutex;SaveNoticeContext131 context;std::deque<SaveNoticeEvent131> events;
 void Set(uintptr_t root,uintptr_t world){std::lock_guard<std::mutex> guard(mutex);if(context.root!=root||context.world!=world){context={root,world,context.epoch+1};events.clear();}}
 SaveNoticeContext131 Context(){std::lock_guard<std::mutex> guard(mutex);return context;}
 bool Add(const SaveNoticeContext131& source,unsigned slot,uint64_t now){std::lock_guard<std::mutex> guard(mutex);if(!source.root||!source.world||source.root!=context.root||source.world!=context.world||source.epoch!=context.epoch||slot>=24||events.size()>=64)return false;events.push_back({slot,now});return true;}
 std::deque<SaveNoticeEvent131> Drain(uintptr_t root,uintptr_t world,uint64_t now){std::lock_guard<std::mutex> guard(mutex);std::deque<SaveNoticeEvent131> out;if(context.root==root&&context.world==world&&root&&world)for(const auto& e:events)if(now>=e.time&&now-e.time<=2000)out.push_back(e);events.clear();return out;}
};
inline SaveNoticeQueue131 saveNoticeQueue131;
inline std::atomic<uintptr_t> saveNoticeTable131=0;
inline bool ReadSaveNoticePacket131(uintptr_t packet,uintptr_t table,unsigned& slot){
 __try{if(!packet||!table||*reinterpret_cast<uintptr_t*>(packet)!=table)return false;slot=*reinterpret_cast<unsigned char*>(packet+(sizeof(void*)==8?0x29:0x15));return slot<24;}__except(1){return false;}
}
// Additive observer, same registry contract as FogClickCapture131: return 1 to continue.
// The stock save handler remains registered and receives its original packet.
inline int __cdecl SaveNoticeCapture131(void* packet){
 try{const auto context=saveNoticeQueue131.Context();if(context.root&&context.world){unsigned slot=24;if(ReadSaveNoticePacket131(reinterpret_cast<uintptr_t>(packet),saveNoticeTable131.load(),slot))saveNoticeQueue131.Add(context,slot,GetTickCount64());}}catch(...){}return 1;
}
inline bool ReadSaveNoticeNameRaw131(uintptr_t base,unsigned build,uintptr_t expected,unsigned slot,char* out){
 const auto* p=ResourceProfileFor131(build);if(!p||slot>=24||!ResourceCode131(base,*p))return false;
 __try{unsigned local=24;const auto world=ResourceWorld131(base,*p,local);if(!world||world!=expected)return false;const auto player=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,unsigned)>(base+p->playerFn)(world,slot);if(!ResourceIdentity131(player,base,p->playerTable))return false;const auto text=reinterpret_cast<const char*(__thiscall*)(uintptr_t)>(base+p->stringFn)(player+p->nameOffset);if(!text)return false;unsigned n=0;for(;n<255&&text[n];++n)out[n]=text[n];out[n]=0;return n&&n<255&&ResourceWorld131(base,*p,local)==world;}__except(1){return false;}
}
inline std::string SaveNoticeName131(unsigned slot,const char* name){
 const std::string fallback="Player "+std::to_string(slot+1);if(!name||!*name)return fallback;for(const unsigned char* c=reinterpret_cast<const unsigned char*>(name);*c;++c)if(*c<32||*c==127)return fallback;
 return MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,name,-1,nullptr,0)>1?std::string(name):fallback;
}
struct SaveNoticeService131 {
 uintptr_t base=0,root=0,world=0;unsigned build=0;bool checked=false,valid=false,registered=false,warned=false;
 const MenuFogVertexProfile131* registry=nullptr;const SaveNoticeProfile131* profile=nullptr;
 bool Ensure(uintptr_t image,unsigned version){if(checked)return base==image&&build==version&&valid;base=image;build=version;checked=true;registry=MenuFogVertexProfileFor131(build);profile=SaveNoticeProfileFor131(build);valid=registry&&profile&&MenuFogVertexGuard131(base,*registry)&&SaveNoticeCode131(base,*profile);return valid;}
 bool RegisterNative(bool enable){if(!registry)return false;__try{if(enable){using Fn=void(__cdecl*)(unsigned char,void*,void*,unsigned);reinterpret_cast<Fn>(base+registry->reg)(6,reinterpret_cast<void*>(SaveNoticeCapture131),nullptr,0);}else{using Fn=void(__cdecl*)(unsigned char,void*,void*);reinterpret_cast<Fn>(base+registry->unreg)(6,reinterpret_cast<void*>(SaveNoticeCapture131),nullptr);}return true;}__except(1){return false;}}
 template<class Register> bool Transition(uintptr_t ui,uintptr_t current,bool enabled,Register reg){
  const bool want=enabled&&ui&&current;
  if(registered&&(!want||ui!=root||current!=world)){saveNoticeQueue131.Set(0,0);if(!reg(false))return false;registered=false;root=world=0;}
  if(!want){saveNoticeQueue131.Set(0,0);return true;}
  if(!registered){if(!reg(true))return false;registered=true;root=ui;world=current;}
  saveNoticeQueue131.Set(root,world);return true;
 }
 void Reset(){Transition(0,0,false,[&](bool on){return RegisterNative(on);});}
 template<class Notify> void Update(uintptr_t image,unsigned version,uintptr_t ui,bool enabled,const std::wstring& folder,Notify notify){
  if(!enabled&&!registered){saveNoticeQueue131.Set(0,0);return;}if(!Ensure(image,version)){if(enabled&&!warned){Log(folder,"#csave: native profile refused");warned=true;}return;}
  const auto* rp=ResourceProfileFor131(build);unsigned local=24;const auto current=enabled&&ui&&rp&&ResourceCode131(base,*rp)?ResourceWorld131(base,*rp,local):0;
  saveNoticeTable131=base+profile->table;if(!Transition(ui,current,enabled,[&](bool on){return RegisterNative(on);})){if(!warned)Log(folder,"#csave: native observer transition failed");warned=true;return;}warned=false;
  const auto events=saveNoticeQueue131.Drain(ui,current,GetTickCount64());if(events.empty())return;
  std::ofstream out(std::filesystem::path(folder)/L"TS-SaveGame.log",std::ios::app|std::ios::binary);
  for(const auto& e:events){char text[256]{};if(!ReadSaveNoticeNameRaw131(base,build,current,e.slot,text))text[0]=0;const auto name=SaveNoticeName131(e.slot,text);if(out)out<<"Save Game: "<<name<<"\n";notify("|cFFff9900[Save Game]:|r "+name);}
  if(!out)Log(folder,"#csave: TS-SaveGame.log append failed");
 }
};
template<class Save,class Reload> bool SaveNoticeCommand131(const std::wstring& args,Config& config,Save save,Reload reload,std::string& report){
 if(args.find_first_not_of(L" \t\r\n")!=args.npos){report="Usage: #csave (no arguments; toggles save notifications/log)";return false;}const bool enabled=!config.notiLogSaveGame;
 const bool saved=save(enabled?L"1":L"0");reload();if(!saved||config.notiLogSaveGame!=enabled){report="Check Players Save Game: setting could not be saved";return false;}
 report=enabled?"|CFFFCD211Check Players Save Game|r: |cff44ff3dOn":"|CFFFCD211Check Players Save Game|r: |cffff5c5cOff";return true;
}
}
