#pragma once
#include "mth_recipe.hpp"
#include "inventory_actions129.hpp"
#include "inventory_info129.hpp"
#include "resource_commands129.hpp"
#include "tool_menu.hpp"
namespace wht {
inline const char* MthName129(int kind){static const char* names[]={"Shield","Ice","Wild","Frozen","Phoenix","Goku","Ch1","Ch2","Ch3","Wolf1","Wolf2","Wolf3"};return kind>=0&&kind<12?names[kind]:"MTH";}
inline int MthUiSlot129(int memory){return memory>=0&&memory<12?(memory%3)*4+memory/3:-1;}
struct MthAdapter129 {
 uintptr_t base;ClickFn129 click;
 InventoryActionsApi129 inv;DropItemsApi129 drop;CreateUnitApi129 units;ResourceCommandApi129 res;
 void(__cdecl* clear)()=nullptr;void(__cdecl* select)(uint32_t,int)=nullptr;int(__cdecl* isType)(uint32_t,int)=nullptr;
 MthAdapter129(uintptr_t b,ClickFn129 c):base(b),click(c),inv(ResolveInventoryActions129(b)),drop(ResolveDropItems129(b)),units(ResolveCreateUnit129(b)),res(ResolveResourceCommandApi129(b)){
  if(b&&!memcmp((void*)(b+0x8a810),"\x8b\x0d",2)&&*reinterpret_cast<uintptr_t*>(b+0x8a812)==b+0xd3b6f4&&!memcmp((void*)(b+0xa67c0),"\x55\x8b\xec\x56\xff\x75\x08\xe8",8)&&!memcmp((void*)(b+0x9a210),"\x55\x8b\xec\x56\xff\x75\x08\xe8",8)){
   if(memcmp((void*)(b+0x97d70),"\x55\x8b\xec",3)||memcmp((void*)(b+0x97db0),"\x55\x8b\xec",3))return;
   clear=reinterpret_cast<decltype(clear)>(b+0x8a810);select=reinterpret_cast<decltype(select)>(b+0xa67c0);isType=reinterpret_cast<decltype(isType)>(b+0x9a210);
  }
 }
 uintptr_t World()const{return base?*reinterpret_cast<uintptr_t*>(base+0xd3b6f4):0;}
 uint32_t Handle(uintptr_t raw)const{auto w=World();auto ctx=w?*reinterpret_cast<uintptr_t*>(w+0x1c):0;return ctx&&raw&&inv.handle?inv.handle(ctx,raw,0):0;}
 bool Available()const{return base&&click&&inv.query&&drop&&units&&res&&clear&&select&&isType&&ResolveTeleportApi129(base).position;}
 uint32_t Selected()const{return Handle(SelectedSkill129(base).unit);}
 bool Living(uint32_t h)const{return h&&inv.owner(h)==inv.local()&&InventoryReal129(inv.life,h)>.405f&&isType(h,0)&&!isType(h,1);}
 bool Capture(MthHero& h)const{
  if(!Available())return false;auto s=SelectedSkill129(base);h={};h.session=World();h.object=s.unit;h.handle=Handle(s.unit);
  if(!h.session||!LiveDropUnit129(base,s,drop)||!Living(h.handle))return false;
  h.low=uint32_t(s.identity>>32);h.high=uint32_t(s.identity);h.gold=res.get(inv.local(),1);
  InventoryInfo129 bag;if(!ReadInventoryInfo129(base,bag))return false;h.bag=bag.codes;return true;
 }
 bool Valid(const MthHero& h)const{
  if(!Available()||World()!=h.session||!h.object)return false;
  auto agent=drop.agent(h.low,h.high);
  return agent&&!*reinterpret_cast<uint32_t*>(agent+0x20)&&*reinterpret_cast<uintptr_t*>(h.object)==base+0xa9c504&&*reinterpret_cast<uint32_t*>(h.object+0xc)==h.low&&*reinterpret_cast<uint32_t*>(h.object+0x10)==h.high&&Handle(h.object)==h.handle&&Living(h.handle);
 }
 bool ShopValid(const MthHero& h,uint32_t shop,uint32_t code)const{
  if(!shop||units.type(shop)!=code||InventoryReal129(inv.life,shop)<=.405f)return false;
  auto xy=ResolveTeleportApi129(base);float hero[3]={NAN,NAN,NAN};xy.position(h.object+0x26c,hero);
  // Native unit position getters (distinct from the item getters in inv).
  auto x=reinterpret_cast<InventoryActionsApi129::Getter>(base+0x97d70),y=reinterpret_cast<InventoryActionsApi129::Getter>(base+0x97db0);
  float sx=InventoryReal129(x,shop),sy=InventoryReal129(y,shop);float dx=sx-hero[0],dy=sy-hero[1];
  return std::isfinite(dx)&&std::isfinite(dy)&&dx*dx+dy*dy<=777.f*777.f;
 }
 uint32_t FindShop(const MthHero& h,uint32_t code)const{
  if(!Valid(h))return 0;auto xy=ResolveTeleportApi129(base);float hero[3]={NAN,NAN,NAN};xy.position(h.object+0x26c,hero);
  if(!std::isfinite(hero[0])||!std::isfinite(hero[1]))return 0;
  auto rect=NearbyRect129(hero[0],hero[1],777.f);NearbyObjects129 nearby;inv.query(rect.data(),0,0x14fd,0,0,CollectNearby129,&nearby);
  for(unsigned i=0;i<nearby.count;++i){auto raw=nearby.objects[i];if(*reinterpret_cast<uintptr_t*>(raw)!=base+0xa9c504)continue;auto handle=Handle(raw);if(ShopValid(h,handle,code))return handle;}return 0;
 }
 bool Select(uint32_t h)const{if(!Available()||!h||!units.type(h))return false;clear();select(h,1);return true;}
 bool Ready(uint32_t shop,const MthStage& stage)const{
  if(Selected()!=shop)return false;
  for(unsigned i=0;i<stage.count;++i)if(!SkillValue129(base,MthUiSlot129(stage.slots[i]),8))return false;
  return true;
 }
 bool Click(int memory)const{return ClickButton129(base,false,MthUiSlot129(memory),click);}
};
struct Mth129 {
 MthRecipeRun run;int pending=-1,kind=-1;SkillSelection129 requested{};uintptr_t owner=0;
 struct Route {float x1,y1,x2,y2;};
 inline static constexpr Route routes[6]={{-1160,-358,-1125,-398},{-1119,-81,-1163,-125},{-882,-375,-934,-388},{1158,-397,1206,-357},{1160,-82,1212,-138},{948,-399,897,-330}};
 SkillSelection129 courier{};uintptr_t courierSession=0;bool routePinned=false;
 bool ValidCourier(uintptr_t base,const SkillSelection129& s,uintptr_t session)const{
  if(!base||!s.unit||!s.identity||!session)return false;
  __try{MthAdapter129 a(base,nullptr);auto drop=a.drop;
   if(!drop||!a.inv.owner||!a.inv.local||!a.inv.life||!a.isType||a.World()!=session)return false;
   auto agent=drop.agent(uint32_t(s.identity>>32),uint32_t(s.identity));
   if(!agent||*reinterpret_cast<uint32_t*>(agent+0xc)!=0x2b61676c||*reinterpret_cast<uint32_t*>(agent+0x20)||*reinterpret_cast<uintptr_t*>(s.unit)!=base+0xa9c504||*reinterpret_cast<uint32_t*>(s.unit+0xc)!=uint32_t(s.identity>>32)||*reinterpret_cast<uint32_t*>(s.unit+0x10)!=uint32_t(s.identity))return false;
   auto h=a.Handle(s.unit);return h&&a.inv.owner(h)==a.inv.local()&&InventoryReal129(a.inv.life,h)>.405f;
  }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 bool SetCourier(uintptr_t base){
  __try{auto s=SelectedSkill129(base);MthAdapter129 a(base,nullptr);auto session=a.World();if(!ValidCourier(base,s,session))return false;courier=s;courierSession=session;return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 int route=-1,remaining=0;bool second=false,sent=false;uint32_t routeDue=0,routeDeadline=0;SkillSelection129 routeUnit{};
 bool QueueRoute(uintptr_t base,int index,int count){if(index<0||index>=6||pending>=0||run.Active())return false;auto selected=SelectedSkill129(base);routePinned=false;if(ValidCourier(base,courier,courierSession)){selected=courier;routePinned=true;}if(!selected.unit)return false;MthAdapter129 a(base,nullptr);if(!ValidCourier(base,selected,a.World())||(!routePinned&&a.isType(a.Handle(selected.unit),0)))return false;route=index;remaining=std::clamp(count,1,20);second=sent=false;routeDue=0;routeUnit=selected;return true;}
 bool Reached(){sent=false;if(second&&!--remaining){route=-1;return true;}second=!second;return false;}
 const char* TickRoute(uintptr_t base,bool enabled,bool ready,uint32_t now){
  if(route<0)return nullptr;if(!enabled){route=-1;return "MTH Route: OFF";}if(!ready)return nullptr;
  __try{
   auto api=ResolveInventoryActions129(base);auto drop=ResolveDropItems129(base);auto xy=ResolveTeleportApi129(base);
   MthAdapter129 adapter(base,nullptr);
   if(!api.owner||!api.local||!api.life||!drop||!xy.position||!adapter.isType||!(routePinned?ValidCourier(base,routeUnit,courierSession):LiveDropUnit129(base,routeUnit,drop))){route=-1;return "MTH Route: stopped (unit or selection changed)";}
   auto unit=adapter.Handle(routeUnit.unit);
   if(!unit||api.owner(unit)!=api.local()||InventoryReal129(api.life,unit)<=.405f||(!routePinned&&adapter.isType(unit,0))){route=-1;return "MTH Route: select a living owned Chicken/Wolf unit";}
   if(routeDue&&int32_t(now-routeDue)<0)return nullptr;routeDue=now+100;
   const auto r=routes[route];const float x=second?r.x2:r.x1,y=second?r.y2:r.y1;
   float pos[3]={NAN,NAN,NAN};xy.position(routeUnit.unit+0x26c,pos);
   if(!std::isfinite(pos[0])||!std::isfinite(pos[1])){route=-1;return "MTH Route: unit coordinates unavailable";}
   if(sent){
    if((pos[0]-x)*(pos[0]-x)+(pos[1]-y)*(pos[1]-y)<=48.f*48.f){if(Reached())return "MTH Route: completed";}
    else if(int32_t(now-routeDeadline)>=0){route=-1;return "MTH Route: stopped (destination not reached)";}else return nullptr;
   }
   const auto target=routes[route];const float tx=second?target.x2:target.x1,ty=second?target.y2:target.y1;
   if(memcmp((void*)(base+0x9aca0),"\x55\x8b\xec",3)||!reinterpret_cast<int(__cdecl*)(uint32_t,uint32_t,const float*,const float*)>(base+0x9aca0)(unit,0xd0012,&tx,&ty)){route=-1;return "MTH Route: movement order refused";}
   sent=true;routeDeadline=now+30000;return nullptr;
  }__except(EXCEPTION_EXECUTE_HANDLER){route=-1;return "MTH Route: native action failed";}
 }
 std::array<void*,12> frames{},labels{};std::array<bool,12> shown{},down{};std::array<uint64_t,12> pressUntil{};int pressed=-1;
 void Press(int i){if(i>=0&&i<12){pressed=i;pressUntil[i]=GetTickCount64()+140;}}
 void Release(){pressed=-1;}
 static ToolMenuRect Rect(unsigned i){return {.305f+.041f*(i%6),.142f+.028f*(i/6),.038f,.024f};}
 ToolMenuRect ButtonRect(unsigned i)const{unsigned slot=0;for(unsigned j=0;j<i;++j)if(shown[j])++slot;return Rect(slot);}
 void Reset(){courier={};courierSession=0;routePinned=false;run.Reset();route=-1;remaining=0;routeUnit={};pending=kind=-1;requested={};owner=0;frames={};labels={};shown={};down={};pressUntil={};pressed=-1;}
 bool Queue(uintptr_t base,int action){if(action<0||action>4||route>=0||pending>=0||run.Active())return false;requested=SelectedSkill129(base);if(!requested.unit)return false;pending=action;return true;}
 const char* Tick(uintptr_t base,ClickFn129 click,bool enabled,bool ready,uint32_t now){
  if(!enabled){pending=-1;if(!run.Active())return nullptr;}
  if(!ready&&enabled)return nullptr;
  __try {
   MthAdapter129 api(base,click);MthResult result=MthResult::Idle;
   if(pending>=0){auto current=SelectedSkill129(base);if(current.unit!=requested.unit||current.identity!=requested.identity){pending=-1;return "selection changed";}kind=pending;pending=-1;result=run.Start(kind,now,enabled,api);}
   else result=run.Tick(now,enabled,api);
   switch(result){case MthResult::Idle:case MthResult::Running:return nullptr;case MthResult::Submitted:return "purchase orders submitted";case MthResult::Gold:return "not enough gold";case MthResult::NoHero:return "select a living owned hero";case MthResult::NoShop:return "required shop not within 777 range";case MthResult::Changed:return "stopped: selection or hero changed";case MthResult::Disabled:return "OFF";case MthResult::Unavailable:return "shop command card unavailable";default:return "purchase failed";}
  }__except(EXCEPTION_EXECUTE_HANDLER){run.Reset();pending=-1;return "native action failed";}
 }
 void Update(uintptr_t base,uintptr_t parent,const Config& c,bool allowed,ToolMenuOverlay& menu){
  if(!menu.initialized||!menu.independentHost)return;if(owner&&owner!=parent)Reset();owner=parent;
  if(!(GetAsyncKeyState(VK_LBUTTON)&0x8000))Release();
  const bool draw[]={c.shieldDraw,c.iceDraw,c.wildDraw,c.frozenDraw,c.phoenixDraw,c.pickGokuDraw,c.autoGaDraw1,c.autoGaDraw2,c.autoGaDraw3,c.autoSoiDraw1,c.autoSoiDraw2,c.autoSoiDraw3};
  for(unsigned i=0;i<12;++i)shown[i]=allowed&&!menu.open&&(c.mth49Enable||c.g17Enable)&&draw[i];
  for(unsigned i=0;i<12;++i){auto r=ButtonRect(i);
   // The game UI owner can occupy the last native frame level. Use the
   // menu's independent level-6 host, which remains visible when it closes.
   if(shown[i]&&!frames[i]){
    frames[i]=menu.Frame(base,menu.independentHost,r);
    if(frames[i])NativeSetFrameBackdropEx(menu.api,frames[i],"UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp","UI\\Widgets\\EscMenu\\Human\\human-options-button-border-up.blp",.0125f,.0045f);
   }
   if(shown[i]&&frames[i]&&!labels[i]){
    auto title=std::string("|cffffcc00")+MthName129(i)+"|r";
    labels[i]=menu.Font(frames[i],.0100f,title.c_str());
    if(labels[i])menu.api.setTextAlign(labels[i],2);
   }
   shown[i]=shown[i]&&frames[i]&&labels[i];
   const bool push=shown[i]&&(GetTickCount64()<pressUntil[i]);
   if(frames[i]&&labels[i]){down[i]=push;float dx=push?.0012f:0.f,dy=push?-.0012f:0.f;
    menu.api.setAbsPoint(frames[i],0,r.right-r.width+dx,r.bottom+r.height+dy,1);
    menu.api.setAbsPoint(frames[i],8,r.right+dx,r.bottom+dy,1);
    auto title=std::string(push?"|cffff9900":"|cffffcc00")+MthName129(i)+"|r";menu.api.setText(labels[i],title.c_str());
   }

   if(frames[i]){if(shown[i]){
    NativeFrameShow(frames[i]);
    // Read the button's resolved native rectangle. Fonts must use that
    // coordinate space directly, without applying widescreen conversion again.
    const auto at=reinterpret_cast<uintptr_t>(frames[i]);
    const float left=*reinterpret_cast<float*>(at+0x48),right=*reinterpret_cast<float*>(at+0x50);
    const float bottom=*reinterpret_cast<float*>(at+0x44),top=*reinterpret_cast<float*>(at+0x4c);
    if(std::isfinite(left)&&std::isfinite(right)&&right>left&&std::isfinite(bottom)&&std::isfinite(top)&&top>bottom){
     reinterpret_cast<void(__thiscall*)(void*,float)>(base+0x473b40)(labels[i],right-left);
     menu.api.setHeight(labels[i],top-bottom);
     reinterpret_cast<void(__thiscall*)(void*,int,float,float,int)>(base+0x473950)(labels[i],4,(left+right)*.5f,(bottom+top)*.5f,1);
    }
   }else NativeFrameHide(frames[i]);}
  }
 }
 void GokuVisual(ToolMenuOverlay& menu,bool enabled){if(labels[5]&&menu.api.setText)menu.api.setText(labels[5],enabled?"|cff00ff00Goku|r":"|cffffcc00Goku|r");}
 int Hit(POINT p,int w,int h)const{for(unsigned i=0;i<12;++i)if(shown[i]&&ButtonRect(i).Hit(p,w,h))return int(i);return -1;}
};
}
