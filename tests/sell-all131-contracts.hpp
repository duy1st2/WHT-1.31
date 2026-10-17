#pragma once
#include "sell_native131.hpp"
inline uintptr_t sellTestAbility=0;inline unsigned sellTestAbilityCalls=0;
#ifdef _WIN64
#define SELL_TEST_EDX
#else
#define SELL_TEST_EDX ,void*
#endif
inline uintptr_t __fastcall SellAbilityTest(uintptr_t SELL_TEST_EDX,unsigned code,unsigned inherited,unsigned byType,unsigned active,unsigned inactive,unsigned exact){++sellTestAbilityCalls;check(code==0x41706974&&!inherited&&!byType&&active==1&&inactive==1&&!exact,"purchase ability lookup uses verified six-argument ABI");return sellTestAbility;}
inline void CheckSellAll131(){
 const uintptr_t base=0x10000000;
 for(const auto& p:wht::sellProfiles131){alignas(uintptr_t) unsigned char ability[0x100]{};*reinterpret_cast<uintptr_t*>(ability)=base+p.purchaseTable;sellTestAbility=reinterpret_cast<uintptr_t>(ability);
  wht::SellApi131 api;api.ability=reinterpret_cast<decltype(api.ability)>(SellAbilityTest);
  check(wht::Merchant131(base,p,api,1),"native Purchase Item ability qualifies a merchant");*reinterpret_cast<uintptr_t*>(ability)=base+p.purchaseTable+8;
  check(!wht::Merchant131(base,p,api,1),"unrelated ability and Share Inventory alone never qualify a shop");
 }
 for(const auto& p:wht::inventoryProfiles131){alignas(uintptr_t) unsigned char item[0x100]{};auto object=reinterpret_cast<uintptr_t>(item);*reinterpret_cast<uintptr_t*>(item)=base+p.itemTable;
  const unsigned idOffset=sizeof(uintptr_t)==8?0x18:0xc,flagsOffset=sizeof(uintptr_t)==8?0x38:0x20;const wht::DropObjectId id{11,12};memcpy(item+idOffset,&id,8);
  check(!wht::Pawnable131(base,p,object),"non-pawnable item is preserved");*reinterpret_cast<uint32_t*>(item+flagsOffset)=0x2000;check(wht::Pawnable131(base,p,object),"native pawnable flag permits sale");
 }
 wht::SellView131 view;view.owner.root=1;view.owner.session=2;view.owner.unit=3;view.owner.id={4,5};view.gold=500;
 view.owner.items[0]={1,{10,11}};view.owner.items[2]={1,{20,21}};view.owner.items[5]={1,{30,31}};view.pawnable[0]=view.pawnable[5]=true;
 view.shops={ {40,{41,42},100,0} };view.scanDone=true;
 wht::SellAll131 worker;unsigned sends=0;auto send=[&](const auto&,const wht::DropItem&,const auto&){++sends;return true;};
 check(worker.Start(view,1000)&&worker.remaining==2&&worker.skipped==1&&!worker.Start(view,1000),"sale snapshots eligible originals and refuses duplicate start");
 check(worker.Step(&view,1000,send)==wht::SellResult131::Submitted&&sends==1,"one synchronized shop order per tick");
 check(worker.Step(&view,1100,send)==wht::SellResult131::Working&&sends==1&&worker.sold==0,"submitted order is not yet a confirmed sale");
 view.owner.items[0]={};view.gold=550;check(worker.Step(&view,1350,send)==wht::SellResult131::Submitted&&sends==2&&worker.sold==1,"inventory absence confirms first original then next order");
 view.owner.items[5]={};view.gold=600;check(worker.Step(&view,1400,send)==wht::SellResult131::Working,"sale waits for final resource settlement");
 check(worker.Step(&view,1425,send)==wht::SellResult131::Remaining&&!worker.active&&worker.sold==2&&worker.skipped==1&&worker.goldDelta==100&&view.owner.items[2].handle,"unpawnable original remains held and gold delta is reported");
 view.owner.items[0]={1,{10,11}};view.shops.clear();worker.Reset();worker.Start(view,2000);check(worker.Step(&view,2000,send)==wht::SellResult131::NoShop&&sends==2,"no purchase shop sends no item-transfer order");
 worker.Reset();worker.Start(view,3000);view.owner.unit=333;check(worker.Step(&view,3000,send)==wht::SellResult131::Cancelled&&sends==2,"selection change cancels sale");view.owner.unit=3;
 worker.Reset();worker.Start(view,4000);check(worker.Step(&view,20000,send)==wht::SellResult131::Remaining&&!worker.active,"bounded sale deadline preserves remaining items");
 wht::SellView131 single;single.owner.root=1;single.owner.session=2;single.owner.unit=3;single.owner.id={4,5};single.scanDone=true;single.shops={{40,{41,42},100,0}};
 for(unsigned i=0;i<6;++i){single.owner.items[i]={1,{100+i,200+i}};single.pawnable[i]=true;}
 for(unsigned hovered=0;hovered<6;++hovered){wht::SellAll131 one;check(one.Start(single,1000,int(hovered))&&one.remaining==1&&one.skipped==0,"SellFast snapshots only hovered inventory item");
  unsigned oneSends=0;auto oneSend=[&](const auto&,const wht::DropItem& item,const auto&){check(item.id==single.owner.items[hovered].id,"SellFast never submits another item's ID");++oneSends;return true;};
  check(one.Step(&single,1000,oneSend)==wht::SellResult131::Submitted&&oneSends==1,"single hovered item dispatched");
  auto after=single;after.owner.items[hovered]={};one.Step(&after,1350,oneSend);one.Step(&after,1375,oneSend);check(oneSends==1&&!one.active&&one.sold==1,"other five items remain outside SellFast worker");
 }
 wht::SellAll131 invalidSingle;check(!invalidSingle.Start(single,1000,6)&&!invalidSingle.Start(single,1000,-2),"invalid sell slot cannot become sell all");
 single.owner.items[2]={};check(!invalidSingle.Start(single,1000,2),"empty hovered slot never starts sale of another item");
 single.pawnable[3]=false;check(!invalidSingle.Start(single,1000,3),"unpawnable hovered item never starts bulk sale");
 wht::SellIcon131 icon;icon.Reset();check(icon.area.bottom==.040f&&!strcmp(icon.upAsset,"BTNSellItem.blp"),"sell icon retains independent layout/assets");
}
