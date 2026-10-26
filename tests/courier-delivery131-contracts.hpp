#pragma once
#include "courier_delivery131.hpp"
inline void CheckCourierDelivery131(){
 using namespace wht;
 DeliveryBag131 c,h;c.source={1,10,3,{4,5},0,0};h.source={1,20,3,{6,7},100,0};c.capacity=h.capacity=6;c.items[5]={30,31};
 CourierDelivery131 run;unsigned moves=0,sends=0,returns=0;bool readable=true,sendOk=true;
 auto read=[&](const SelfCastSource131& s,bool hero,DeliveryBag131& out){check(hero==(s.unit==20),"recipient requires hero validation");out=hero?h:c;return readable;};
 auto move=[&](const SelfCastSource131&,const SelfCastSource131&){++moves;return true;};
 auto transfer=[&](const SelfCastSource131&,const SelfCastSource131&,const DropObjectId& item){check(item==c.items[5],"transfer exact last-slot item identity");++sends;return sendOk;};
 auto back=[&](const SelfCastSource131&){++returns;};
 check(run.Start(c,h,0),"assigned courier with last-slot item starts");
 check(!run.Start(c,h,0),"delivery cannot be replaced while active");
 check(run.Tick(0,true,true,read,move,transfer,back)==DeliveryResult131::Sent&&sends==1,"one transfer submitted");
 check(run.Tick(50,true,true,read,move,transfer,back)==DeliveryResult131::Working&&sends==1,"100ms sampling cadence");
 check(run.Tick(100,true,true,read,move,transfer,back)==DeliveryResult131::Working&&sends==1&&returns==0,"no repeated transfer while item remains in courier");
 h.items[5]=c.items[5];c.items[5]={};
 check(run.Tick(200,true,true,read,move,transfer,back)==DeliveryResult131::Complete&&run.delivered==1&&returns==1,"receipt in hero last slot confirms delivery then return");
 check(run.Tick(300,true,true,read,move,transfer,back)==DeliveryResult131::Idle&&returns==1,"completed delivery never repeats");
 c.items[5]={30,31};h.items={};h.source.x=1000;run.Reset();run.Start(c,h,0);
 check(run.Tick(0,true,true,read,move,transfer,back)==DeliveryResult131::Working&&moves==1,"distant hero receives move before transfer");
 run.Tick(999,true,true,read,move,transfer,back);check(moves==1,"approach no faster than one second");
 h.source.x=100;check(run.Tick(1000,true,true,read,move,transfer,back)==DeliveryResult131::Sent&&sends==2,"approaching then transfer");
 c.items[5]={};check(run.Tick(1100,true,true,read,move,transfer,back)==DeliveryResult131::LostItem&&returns==1,"missing item without exact receipt stops");
 c.items[5]={30,31};for(unsigned i=0;i<6;++i)h.items[i]={100+i,1};run.Reset();run.Start(c,h,0);
 check(run.Tick(0,true,true,read,move,transfer,back)==DeliveryResult131::Full&&sends==2&&returns==2&&DeliveryHas131(c,{30,31}),"full hero retains courier items and returns");
 h.items={};run.Reset();run.Start(c,h,0);check(run.Tick(0,true,false,read,move,transfer,back)==DeliveryResult131::Working&&sends==2,"menu chat focus pause does not submit");
 check(run.Tick(1,false,false,read,move,transfer,back)==DeliveryResult131::Cancelled&&!run.active,"master loss cancels without return");
 run.Reset();run.Start(c,h,0);check(run.Tick(60000,true,true,read,move,transfer,back)==DeliveryResult131::Timeout&&sends==2,"60-second deadline");
 run.Reset();run.Start(c,h,0);h.source.id.low++;check(run.Tick(0,true,true,read,move,transfer,back)==DeliveryResult131::Unavailable&&sends==2,"recycled recipient identity refuses actions");h.source.id.low--;
 run.Reset();run.Start(c,h,0);sendOk=false;check(run.Tick(0,true,true,read,move,transfer,back)==DeliveryResult131::Refused&&sends==3,"native failure stops");run.Tick(100,true,true,read,move,transfer,back);check(sends==3,"uncertain native transfer never replayed");
 run.Reset();c.capacity=0;check(!run.Start(c,h,0),"missing courier inventory refused");c.capacity=6;h.source.session=9;check(!run.Start(c,h,0),"different sessions refused");
 for(const auto* command:{L"#ga",L"#soi",L"#courier",L"#chuyendo"})check(DeliveryCommand131(command),"all reference delivery aliases");
 check(!DeliveryCommand131(L"#ga1draw"),"delivery aliases exact");
 DropSnapshot target;target.id={6,7};target.x=100;target.y=200;auto bytes=DropAction(target,{1,{30,31}});bytes[1]=0;uint32_t value=0;
 check(bytes[0]==0x13&&bytes[1]==0&&bytes.size()==39,"transfer action and nonqueued flags");memcpy(&value,bytes.data()+23,4);check(value==6,"transfer targets hero ID not courier ID");memcpy(&value,bytes.data()+31,4);check(value==30,"transfer contains exact item ID");
}
