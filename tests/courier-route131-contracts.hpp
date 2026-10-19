#pragma once
#include "mth_queue131.hpp"
#include <fstream>
inline std::vector<unsigned> courierAbiEvents131;inline bool courierAbiThrow131=false;
inline void __cdecl CourierSelectAbi131(const wht::NativeUnitList131* list,unsigned mode){courierAbiEvents131.push_back(mode*100+list->count);}
inline void __cdecl CourierPointAbi131(unsigned order,uintptr_t aux,const float* x,const float* y,unsigned flags){check(order==0xd0012&&!aux&&*x==10&&*y==20&&!flags,"point sender ABI preserves move/auxiliary/coordinates/flags");courierAbiEvents131.push_back(999);if(courierAbiThrow131)RaiseException(0xe0000131,0,0,nullptr);}
inline unsigned queuePoints131=0;inline bool queueThrow131=false;
#ifdef _WIN64
#define QUEUE_ABI131 __fastcall
#define QUEUE_PAD131
#else
#define QUEUE_ABI131 __fastcall
#define QUEUE_PAD131 uintptr_t,
#endif
inline void QUEUE_ABI131 QueueStopMock131(uintptr_t ui,QUEUE_PAD131 unsigned order,uintptr_t ability,unsigned flags,unsigned mode){check(ui==123&&order==0xd0004&&!ability&&flags==1&&mode==4,"native queue stop ABI matches reference");courierAbiEvents131.push_back(700);}
inline void QUEUE_ABI131 QueuePointMock131(uintptr_t ui,QUEUE_PAD131 unsigned order,uintptr_t ability,float x,float y,uintptr_t target,unsigned flags,unsigned queue){check(ui==123&&order==0xd0012&&!ability&&!target&&flags==0xffffffffu&&queue==1,"native queue move flags/ABI match reference");check(x==(queuePoints131%2?30.f:10.f)&&y==(queuePoints131%2?40.f:20.f),"all route endpoint pairs are enqueued in order");++queuePoints131;courierAbiEvents131.push_back(800);if(queueThrow131)RaiseException(0xe0000131,0,0,nullptr);}
#undef QUEUE_PAD131
#undef QUEUE_ABI131
inline void CheckCourierRoute131(){
 for(const auto& p:wht::courierSelectProfiles131){
#ifdef _WIN64
  const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);if(!input)continue;std::vector<char> memory((std::istreambuf_iterator<char>(input)),{});const auto base=reinterpret_cast<uintptr_t>(memory.data());
  check(bool(wht::MthQueueNative131(base,p.build)),"captured full native stop/queued point wrappers verified");
  check(wht::CourierSelectNative131(base,p.build)!=nullptr,"captured native selection sender/callers/set links verified");memory[p.removeCall]^=1;check(!wht::CourierSelectNative131(base,p.build),"changed native remove caller refuses selection dispatch");
 }
 const auto* p=wht::InventoryProfile(12164);const uintptr_t base=0x10000000;alignas(uintptr_t) uintptr_t set[8]{},node1[4]{},node2[4]{};alignas(uintptr_t) unsigned char unit1[0x100]{},unit2[0x100]{};
 *reinterpret_cast<uintptr_t*>(unit1)=*reinterpret_cast<uintptr_t*>(unit2)=base+p->unitTable;const wht::DropObjectId id1{10,11},id2{20,21};memcpy(unit1+(sizeof(void*)==8?0x18:0xc),&id1,8);memcpy(unit2+(sizeof(void*)==8?0x18:0xc),&id2,8);
 set[3]=reinterpret_cast<uintptr_t>(node1);*reinterpret_cast<unsigned*>(&set[4])=2;node1[1]=reinterpret_cast<uintptr_t>(node2);node1[2]=reinterpret_cast<uintptr_t>(unit1);node2[1]=1;node2[2]=reinterpret_cast<uintptr_t>(unit2);
 wht::SelectionRoster131 roster;check(wht::ReadSelectionSet131(reinterpret_cast<uintptr_t>(set),base,*p,roster)&&roster.count==2&&roster.ids[1]==id2,"native linked selection set preserves complete original roster");
 node2[1]=reinterpret_cast<uintptr_t>(node1);check(!wht::ReadSelectionSet131(reinterpret_cast<uintptr_t>(set),base,*p,roster),"cyclic/extra native selection nodes refused");node2[1]=1;
 *reinterpret_cast<unsigned*>(&set[4])=13;check(!wht::ReadSelectionSet131(reinterpret_cast<uintptr_t>(set),base,*p,roster),"oversized selection refused before sending");
 roster.count=2;roster.units[0]=101;roster.units[1]=102;std::vector<unsigned> events;auto select=[&](const wht::NativeUnitList131* list,unsigned mode){events.push_back(mode*100+list->count);if(list->count==2)check(list->units[0]==101&&list->units[1]==102,"restore uses entire original selection");else check(list->units[0]==200,"only assigned courier enters temporary selection");};
 check(wht::CourierPointFlow131(roster,200,select,[&](){events.push_back(999);}),"courier movement transaction");check(events==std::vector<unsigned>({202,101,999,201,102}),"reference remove/add/order/remove/restore ordering");
 events.clear();roster.count=1;roster.units[0]=200;check(wht::CourierPointFlow131(roster,200,select,[&](){events.push_back(999);})&&events==std::vector<unsigned>({999}),"already selected courier requires no redundant selection actions");
 roster.count=2;roster.units[0]=101;roster.units[1]=102;courierAbiEvents131.clear();
 check(wht::SendCourierPointNative131(roster,200,CourierSelectAbi131,CourierPointAbi131,10,20)&&courierAbiEvents131==std::vector<unsigned>({202,101,999,201,102}),"native ABI submits courier point and restores original roster");
 courierAbiEvents131.clear();courierAbiThrow131=true;check(!wht::SendCourierPointNative131(roster,200,CourierSelectAbi131,CourierPointAbi131,10,20)&&courierAbiEvents131==std::vector<unsigned>({202,101,999,201,102}),"native exception still restores original selection without retry");courierAbiThrow131=false;
 wht::MthRoute131 queuedRoute;wht::SelfCastSource131 source{};source.ui=1;source.unit=2;source.session=3;source.id={4,5};unsigned submissions=0;
 check(queuedRoute.Start(0,3,source),"queued route captures request");auto readQueued=[&](wht::SelfCastSource131& out){out=source;return true;};auto sendQueued=[&](const wht::SelfCastSource131&,const wht::Route131&,int laps){++submissions;check(laps==3,"queue preserves configured laps");return true;};
 check(!queuedRoute.TickQueued(true,false,readQueued,sendQueued)&&submissions==0,"menu/input gate delays submission");check(queuedRoute.TickQueued(true,true,readQueued,sendQueued)&&submissions==1&&queuedRoute.route<0,"queued route dispatches once and stops polling");queuedRoute.TickQueued(true,true,readQueued,sendQueued);check(submissions==1,"subsequent ticks never replay queued route");
 wht::MthQueueApi131 queueApi{reinterpret_cast<decltype(wht::MthQueueApi131::stop)>(QueueStopMock131),reinterpret_cast<decltype(wht::MthQueueApi131::point)>(QueuePointMock131)};
 courierAbiEvents131.clear();queuePoints131=0;check(wht::SubmitCourierRoute131(roster,200,CourierSelectAbi131,queueApi,123,{10,20,30,40},20)&&queuePoints131==40,"twenty laps queue exactly forty moves in one submission");check(courierAbiEvents131.front()==202&&courierAbiEvents131[2]==700&&courierAbiEvents131[courierAbiEvents131.size()-2]==201&&courierAbiEvents131.back()==102,"queue stop before movement and restore complete selection after final move");
 courierAbiEvents131.clear();queuePoints131=0;queueThrow131=true;check(!wht::SubmitCourierRoute131(roster,200,CourierSelectAbi131,queueApi,123,{10,20,30,40},1)&&courierAbiEvents131.back()==102,"queue exception restores selection without replay");queueThrow131=false;
 courierAbiEvents131.clear();check(!wht::SubmitCourierRoute131(roster,200,CourierSelectAbi131,queueApi,123,{10,20,30,40},21)&&courierAbiEvents131.empty(),"invalid lap count sends no actions");
 alignas(uintptr_t) unsigned char frame[0x200]{};auto bounds=reinterpret_cast<float*>(frame+(sizeof(void*)==8?0x88:0x44));bounds[0]=.142f;bounds[1]=.20f;bounds[2]=.166f;bounds[3]=.25f;float left=0,bottom=0,right=0,top=0;
 check(wht::MthResolvedRect131(reinterpret_cast<uintptr_t>(frame),left,bottom,right,top)&&left==.20f&&right==.25f,"toolbar reads actual native frame bounds for unscaled font placement");bounds[3]=bounds[1];check(!wht::MthResolvedRect131(reinterpret_cast<uintptr_t>(frame),left,bottom,right,top),"unresolved or empty frame bounds refused");
}
