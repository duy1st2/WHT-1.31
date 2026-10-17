#pragma once
#include "quick_drop131.hpp"
#include "quick_drop_network131.hpp"
#include "drop_icon131.hpp"
inline uintptr_t dropTestItems[6]{};inline unsigned dropTestSize=6,dropTestCalls=0;inline bool dropTestRefuse=false,dropTestKeep=false;
inline void(*dropTestAfter)()=nullptr;
#ifdef _WIN64
#define DROP_TEST_EDX
#else
#define DROP_TEST_EDX ,void*
#endif
inline uintptr_t __fastcall DropTestItem(uintptr_t DROP_TEST_EDX,unsigned slot){return slot<6?dropTestItems[slot]:0;}
inline unsigned __fastcall DropTestSize(uintptr_t DROP_TEST_EDX){return dropTestSize;}
inline bool __fastcall DropTestDrop(uintptr_t DROP_TEST_EDX,uintptr_t item){++dropTestCalls;if(dropTestRefuse)return false;if(!dropTestKeep)for(auto& held:dropTestItems)if(held==item)held=0;if(dropTestAfter)dropTestAfter();return true;}
inline unsigned char netTestTls[0x40]{},netTestGlobal[0x100]{},netTestSession[0x6400]{};
inline unsigned netTestSends=0;inline unsigned char netTestPacket[39]{};
inline void(*netTestPositionAfter)()=nullptr;
inline uintptr_t __cdecl NetTestContext(unsigned index){return index==13?reinterpret_cast<uintptr_t>(netTestTls):0;}
inline void __cdecl NetTestSend(wht::DropBuffer131* buffer,unsigned channel){check(channel==0&&buffer->size==39&&buffer->capacity==39&&!buffer->position&&!buffer->reserved,"synchronized buffer ABI and channel");++netTestSends;memcpy(netTestPacket,buffer->data,39);}
inline uintptr_t __fastcall NetTestPosition(uintptr_t DROP_TEST_EDX,float* xyz){xyz[0]=123.5f;xyz[1]=-67.25f;if(netTestPositionAfter)netTestPositionAfter();return reinterpret_cast<uintptr_t>(xyz);}
inline unsigned __fastcall NetTestOwner(uintptr_t DROP_TEST_EDX){return 0;}
inline void CheckNetworkDrop131(){
 const uintptr_t base=0x10000000;
 wht::DropNetworkApi131 network{NetTestContext,NetTestSend,reinterpret_cast<decltype(wht::DropNetworkApi131::position)>(NetTestPosition),reinterpret_cast<decltype(wht::DropNetworkApi131::owner)>(NetTestOwner),base+0x100};
 wht::InventoryApi131 inventory{reinterpret_cast<decltype(wht::InventoryApi131::item)>(DropTestItem),reinterpret_cast<decltype(wht::InventoryApi131::size)>(DropTestSize),reinterpret_cast<decltype(wht::InventoryApi131::drop)>(DropTestDrop)};
 *reinterpret_cast<uintptr_t*>(netTestTls+(sizeof(uintptr_t)==8?0x28:0x14))=reinterpret_cast<uintptr_t>(netTestGlobal);
 *reinterpret_cast<uintptr_t*>(netTestGlobal+(sizeof(uintptr_t)==8?0xb8:0x5c))=reinterpret_cast<uintptr_t>(netTestSession);
 const auto state=sizeof(uintptr_t)==8?0x4ec:0x3b8,replay=sizeof(uintptr_t)==8?0x6281:0x5dd1;
 *reinterpret_cast<int*>(netTestSession+state)=3;
 for(const auto& p:wht::inventoryProfiles131){
  alignas(uintptr_t) unsigned char unit[0x700]{},holder[0x100]{},item[0x100]{};
  auto selectedUnit=reinterpret_cast<uintptr_t>(unit),itemPtr=reinterpret_cast<uintptr_t>(item);
  *reinterpret_cast<uintptr_t*>(unit)=base+p.unitTable;*reinterpret_cast<uintptr_t*>(holder)=base+p.inventoryTable;*reinterpret_cast<uintptr_t*>(item)=base+p.itemTable;
  const unsigned offset=sizeof(uintptr_t)==8?0x550:p.build==12164?0x44c:0x444;
  *reinterpret_cast<uintptr_t*>(unit+offset)=reinterpret_cast<uintptr_t>(holder);
  const unsigned identity=sizeof(uintptr_t)==8?0x18:0xc;
  const wht::DropObjectId source{0x12345678,0x91abcdef},target{0x11112222,0x33334444};memcpy(unit+identity,&source,8);memcpy(item+identity,&target,8);
  for(auto& held:dropTestItems)held=0;dropTestItems[2]=itemPtr;dropTestSize=6;dropTestCalls=0;netTestSends=0;
  auto selected=[&](){return selectedUnit;};auto alive=[](uintptr_t){return true;};auto own=[](uintptr_t){return true;};
  auto submit=[&](){return wht::QuickDropNetwork131(base,p,inventory,network,2,selected,alive,own);};
  check(submit()&&netTestSends==1&&dropTestCalls==0&&dropTestItems[2]==itemPtr,"quick drop submits without local inventory mutation");
  uint32_t order=0;float x=0,y=0;memcpy(&order,netTestPacket+3,4);memcpy(&x,netTestPacket+15,4);memcpy(&y,netTestPacket+19,4);
  check(netTestPacket[0]==0x13&&netTestPacket[1]==1&&netTestPacket[2]==0&&order==0xd0021&&x==123.5f&&y==-67.25f&&!memcmp(netTestPacket+23,&source,8)&&!memcmp(netTestPacket+31,&target,8),"drop packet contains queue flag, world position and both object identities");
  netTestSession[replay]=1;check(!submit()&&netTestSends==1,"replay refuses network drop");netTestSession[replay]=0;
  check(!wht::QuickDropNetwork131(base,p,inventory,network,2,selected,alive,[](uintptr_t){return false;}),"other owner refuses drop");
  check(!wht::QuickDropNetwork131(base,p,inventory,network,2,selected,[](uintptr_t){return false;},own),"dead owner refuses drop");
  netTestPositionAfter=[](){dropTestItems[2]=0;};check(!submit()&&netTestSends==1,"changed slot refuses queued drop");netTestPositionAfter=nullptr;dropTestItems[2]=itemPtr;
  unsigned reads=0;check(!wht::QuickDropNetwork131(base,p,inventory,network,2,[&](){return ++reads==1?selectedUnit:uintptr_t(0);},alive,own)&&netTestSends==1,"changed selection refuses queued drop");
 }
}
inline void CheckDropAllNetwork131(){
 wht::DropSnapshot snapshot;snapshot.root=1;snapshot.session=2;snapshot.unit=3;snapshot.handle=1;snapshot.id={4,5};snapshot.x=6;snapshot.y=7;
 snapshot.items[0]={1,{8,9}};snapshot.items[4]={1,{10,11}};
 wht::DropAllState worker;unsigned sent=0;auto send=[&](const wht::DropSnapshot&,const wht::DropItem& item){++sent;check(item.id==snapshot.items[sent==1?0:4].id,"batch preserves original native item IDs");return true;};
 check(worker.Start(snapshot,1000)&&!worker.Start(snapshot,1000),"one active batch per icon click");
 check(worker.Step(&snapshot,1000,send)==wht::DropResult::Submitted&&sent==1,"first batch tick submits exactly one item");
 check(worker.Step(&snapshot,1033,send)==wht::DropResult::Waiting&&sent==1,"batch does not burst all six orders");
 snapshot.items[0]={};check(worker.Step(&snapshot,1100,send)==wht::DropResult::Submitted&&sent==2,"batch skips gaps and waits between orders");
 snapshot.items[4]={};check(worker.Step(&snapshot,1133,send)==wht::DropResult::Complete&&!worker.active,"completion requires original items absent from inventory");
 snapshot.items[0]={1,{8,9}};check(worker.Start(snapshot,2000),"new batch after completion");snapshot.unit=33;
 check(worker.Step(&snapshot,2000,send)==wht::DropResult::Cancelled&&sent==2,"changed selection cancels without another order");
 snapshot.unit=3;worker.Start(snapshot,3000);check(worker.Step(nullptr,3000,send)==wht::DropResult::Cancelled,"denied input gate cancels active batch");
 worker.Start(snapshot,4000);check(worker.Step(&snapshot,16000,send)==wht::DropResult::Remaining&&worker.remaining==1&&!worker.active,"unexecuted orders stop at deadline without claiming success");
}
inline void CheckQuickDrop131(){
 CheckNetworkDrop131();
 CheckDropAllNetwork131();
 const uintptr_t base=0x10000000;
 const wht::InventoryApi131 api{reinterpret_cast<decltype(wht::InventoryApi131::item)>(DropTestItem),reinterpret_cast<decltype(wht::InventoryApi131::size)>(DropTestSize),reinterpret_cast<decltype(wht::InventoryApi131::drop)>(DropTestDrop)};
 for(const auto& p:wht::inventoryProfiles131){
  alignas(uintptr_t) unsigned char unit[0x700]{},inventory[0x100]{},item[0x100]{};
  const auto object=reinterpret_cast<uintptr_t>(unit),held=reinterpret_cast<uintptr_t>(item);
  *reinterpret_cast<uintptr_t*>(unit)=base+p.unitTable;*reinterpret_cast<uintptr_t*>(inventory)=base+p.inventoryTable;*reinterpret_cast<uintptr_t*>(item)=base+p.itemTable;
  // Observed native CUnit inventory field, independent of the profile read.
  const unsigned nativeOffset=sizeof(uintptr_t)==8?0x550:p.build==12164?0x44c:0x444;
  *reinterpret_cast<uintptr_t*>(unit+nativeOffset)=reinterpret_cast<uintptr_t>(inventory);
  dropTestCalls=0;dropTestSize=6;dropTestRefuse=dropTestKeep=false;for(auto& value:dropTestItems)value=0;dropTestItems[4]=held;
  auto selected=[&](){return object;};auto alive=[](uintptr_t){return true;};
  check(wht::DropSlot131(base,p,api,4,selected,alive)&&dropTestCalls==1&&!dropTestItems[4],"requested inventory slot removed through native callback");
  check(!wht::DropSlot131(base,p,api,4,selected,alive)&&dropTestCalls==1,"empty slot never dropped");
  dropTestItems[4]=held;dropTestRefuse=true;check(!wht::DropSlot131(base,p,api,4,selected,alive)&&dropTestItems[4]==held,"native drop refusal preserves item");
  dropTestRefuse=false;dropTestKeep=true;check(!wht::DropSlot131(base,p,api,4,selected,alive),"native return alone does not prove item dropped");
  dropTestSize=7;const auto calls=dropTestCalls;check(!wht::DropSlot131(base,p,api,4,selected,alive)&&dropTestCalls==calls,"invalid inventory capacity blocks mutation");
  dropTestSize=6;check(!wht::DropSlot131(base,p,api,6,selected,alive)&&dropTestCalls==calls,"invalid slot blocks mutation");
  check(!wht::DropSlot131(base,p,api,4,selected,[](uintptr_t){return false;})&&dropTestCalls==calls,"dead unit blocks mutation");
  dropTestKeep=false;dropTestItems[0]=held;dropTestItems[4]=held;dropTestCalls=0;
  // Two real distinct item objects, with a gap between occupied slots.
  alignas(uintptr_t) unsigned char secondItem[0x100]{};*reinterpret_cast<uintptr_t*>(secondItem)=base+p.itemTable;dropTestItems[4]=reinterpret_cast<uintptr_t>(secondItem);
  auto batch=wht::DropAll131(base,p,api,selected,alive);check(batch.complete&&batch.dropped==2&&dropTestCalls==2,"drop all skips gaps and drops each original item once");
  batch=wht::DropAll131(base,p,api,selected,alive);check(batch.complete&&batch.dropped==0&&dropTestCalls==2,"empty batch performs no mutation");
  dropTestItems[0]=held;dropTestItems[4]=reinterpret_cast<uintptr_t>(secondItem);dropTestCalls=0;
  dropTestAfter=[](){dropTestItems[4]=0;};batch=wht::DropAll131(base,p,api,selected,alive);dropTestAfter=nullptr;
  check(!batch.complete&&batch.dropped==1&&dropTestCalls==1,"batch stops when another slot changes during native drop");
  *reinterpret_cast<uintptr_t*>(inventory)=0;const auto afterBatch=dropTestCalls;check(!wht::DropSlot131(base,p,api,4,selected,alive)&&dropTestCalls==afterBatch,"wrong native inventory object rejected");
 }
 alignas(uintptr_t) unsigned char button[0x200]{};const unsigned geometry=sizeof(uintptr_t)==8?0x88:0x44,visible=sizeof(uintptr_t)==8?0xf1:0x9d;
 auto bounds=reinterpret_cast<float*>(button+geometry);bounds[0]=.04f;bounds[1]=.58f;bounds[2]=.08f;bounds[3]=.62f;button[visible]=1;
 auto get=[&](unsigned slot){return slot==3?reinterpret_cast<uintptr_t>(button):0;};
 check(wht::InventorySlotHit131({1300,810},2000,900,get)==3,"widescreen hit maps to native inventory bounds");
 check(wht::InventorySlotHit131({900,810},1200,900,get)==3,"classic hit uses same native bounds");
 button[visible]=0;check(wht::InventorySlotHit131({1300,810},2000,900,get)==-1,"hidden inventory button excluded");
 button[visible]=1;check(wht::InventorySlotHit131({-1,810},2000,900,get)==-1,"outside client rejected");
 wht::QuickDropInput131 input;unsigned sent=0;auto hit=[](){return 3;};auto send=[&](unsigned slot){check(slot==3,"mouse dispatch slot");++sent;};
 check(!input.Mouse(WM_RBUTTONDOWN,false,hit,send)&&sent==0,"disabled gesture passes through");
 check(input.Mouse(WM_RBUTTONDOWN,true,hit,send)&&sent==1,"one drop per pressed gesture");
 check(input.Mouse(WM_RBUTTONUP,false,hit,send)&&sent==1,"consumed mouse release cannot issue normal order");
 check(!input.Mouse(WM_RBUTTONUP,true,hit,send)&&sent==1,"release consumed only once");
 check(!input.Mouse(WM_RBUTTONDOWN,true,[](){return -1;},send)&&sent==1,"noninventory right click passes through");
 wht::DropIcon131 icon;icon.shown=true;unsigned batches=0;auto action=[&](){++batches;};
 const POINT iconPoint{691,786}; // Center of the shared rect on 1000x900.
 check(!icon.Mouse(WM_LBUTTONDOWN,iconPoint,1000,900,false,action)&&batches==0,"menu/chat gating blocks icon action");
 check(icon.Mouse(WM_LBUTTONDOWN,iconPoint,1000,900,true,action)&&batches==1&&icon.pressed,"drop icon executes one batch");
 icon.shown=false;check(icon.Mouse(WM_LBUTTONUP,{0,0},1000,900,false,action)&&!icon.pressed,"release outside hidden icon remains consumed");
 icon.shown=true;check(icon.Mouse(WM_LBUTTONDBLCLK,iconPoint,1000,900,true,action)&&batches==1,"double click does not repeat the batch");
}
