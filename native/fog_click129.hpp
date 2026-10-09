#pragma once
#include "inventory_actions129.hpp"
#include "share_control129.hpp"
#include <deque>
#include <mutex>
namespace wht {
struct FogClick129 {
 struct Event {unsigned slot;uint32_t low,high;uintptr_t world;};
 struct Alert {Event event;uint64_t tick;};
 std::mutex mutex;std::deque<Event> events;std::vector<Alert> recent;
 static bool Readable(const void* p,size_t n){MEMORY_BASIC_INFORMATION m{};if(!p||!n||VirtualQuery(p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;uintptr_t at=reinterpret_cast<uintptr_t>(p),end=reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize;return at<=end&&n<=end-at;}
 static bool Accept(unsigned action,unsigned slot,unsigned count){return (action==1||action==3)&&slot<24&&count>0&&count<=12;}
 void Push(const Event& e){std::lock_guard<std::mutex> lock(mutex);if(events.size()<96)events.push_back(e);}
 void Capture(uintptr_t base,void* packet){
  __try{if(!base||!Readable(packet,0x24))return;auto p=reinterpret_cast<uintptr_t>(packet);auto action=*reinterpret_cast<uint8_t*>(p+0x18),slot=*reinterpret_cast<uint8_t*>(p+0x15);auto count=*reinterpret_cast<unsigned*>(p+0x1c);auto list=*reinterpret_cast<uintptr_t*>(p+0x20);
   if(!Accept(action,slot,count)||!Readable(reinterpret_cast<void*>(list),count*8))return;
   auto world=*reinterpret_cast<uintptr_t*>(base+0xd3b6f4);if(!world)return;
   for(unsigned i=0;i<count;++i){auto low=*reinterpret_cast<uint32_t*>(list+i*8),high=*reinterpret_cast<uint32_t*>(list+i*8+4);if(low||high)Push({slot,low,high,world});}
  }__except(EXCEPTION_EXECUTE_HANDLER){}
 }
 bool Allow(const Event& e,uint64_t now){recent.erase(std::remove_if(recent.begin(),recent.end(),[&](const Alert& a){return now-a.tick>=5000;}),recent.end());for(auto& a:recent)if(a.event.world==e.world&&a.event.slot==e.slot&&a.event.low==e.low&&a.event.high==e.high&&now-a.tick<2000)return false;if(recent.size()>=96)recent.erase(recent.begin());recent.push_back({e,now});return true;}
 bool Hidden(uintptr_t base,const Event& e,unsigned& owner){
  __try{if(!base||*reinterpret_cast<uintptr_t*>(base+0xd3b6f4)!=e.world)return false;
   auto drop=ResolveDropItems129(base);auto inv=ResolveInventoryActions129(base);auto players=ResolveShareControl129(base);
   if(!drop||!inv.owner||!inv.handle||!players)return false;
   if(memcmp((void*)(base+0x74530),"\x55\x8b\xec\x53\x56\x8b\x75\x08",8)||memcmp((void*)(base+0x992d0),"\x55\x8b\xec\x83\xec\x08",6)||memcmp((void*)(base+0x9a3e0),"\x55\x8b\xec\x56\xff\x75\x08",7))return false;
   auto agent=drop.agent(e.low,e.high);if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20))return false;
   auto raw=*reinterpret_cast<uintptr_t*>(agent+0x54);
   if(!raw||*reinterpret_cast<uintptr_t*>(raw)!=base+0xa9c504)return false;
   auto ctx=*reinterpret_cast<uintptr_t*>(e.world+0x1c);auto h=ctx?inv.handle(ctx,raw,0):0;auto clicker=players.player(e.slot);if(!h||!clicker)return false;
   auto own=inv.owner(h);owner=24;for(unsigned i=0;i<24;++i)if(players.player(i)==own){owner=i;break;}if(owner==e.slot||owner>=24)return false;
   if(players.get(own,clicker,5))return false;
   auto xy=ResolveTeleportApi129(base);if(!xy.position)return false;float pos[3]={NAN,NAN,NAN};xy.position(raw+0x26c,pos);if(!std::isfinite(pos[0])||!std::isfinite(pos[1]))return false;
   auto location=reinterpret_cast<int(__cdecl*)(const float*,const float*,uint32_t)>(base+0x992d0);auto visible=reinterpret_cast<int(__cdecl*)(uint32_t,uint32_t)>(base+0x9a3e0);
   return !location(&pos[0],&pos[1],clicker)||!visible(h,clicker);
  }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 template<class Notify> void Tick(uintptr_t base,bool enabled,uint64_t now,Notify notify){std::deque<Event> pending;{std::lock_guard<std::mutex> lock(mutex);pending.swap(events);}if(!enabled){recent.clear();return;}for(const auto& e:pending){unsigned owner;if(Hidden(base,e,owner)&&Allow(e,now))notify("Fog Click: Player "+std::to_string(e.slot+1)+" selected a hidden unit of Player "+std::to_string(owner+1));}}
};
}
