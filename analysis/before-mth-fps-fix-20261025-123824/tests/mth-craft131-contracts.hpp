#pragma once
#include "runtime131.hpp"
#include "mth_craft_buttons131.hpp"
#include <fstream>
inline bool craftAbiOk131=false;
#ifdef _WIN64
inline void __fastcall CraftAbi131(uintptr_t manager,const wht::NativeUnitList131* list,unsigned a,unsigned b,unsigned c,unsigned d,unsigned e){
#else
inline void __fastcall CraftAbi131(uintptr_t manager,void*,const wht::NativeUnitList131* list,unsigned a,unsigned b,unsigned c,unsigned d,unsigned e){
#endif
 craftAbiOk131=manager==123&&list&&list->count==12&&list->units[11]==111&&a==0&&b==1&&c==0&&d==0&&e==0;
}
struct CraftMock131 {
 wht::SelfCastSource131 hero{1,10,3,{4,5},0,0};uintptr_t selected=10;
 std::array<uint32_t,6> bag{};int gold=100000;bool valid=true,shopValid=true,ready=true,selectOk=true,capture=true;
 unsigned restores=0,failAt=999;std::vector<unsigned> clicks;std::vector<uint32_t> shops;
 bool Capture(wht::SelfCastSource131& h,std::array<uint32_t,6>& b,int& g){h=hero;b=bag;g=gold;return capture;}
 bool Valid(const wht::SelfCastSource131&){return valid;}
 uintptr_t Selected(){return selected;}
 bool FindShop(const wht::SelfCastSource131&,uint32_t code,wht::SelfCastSource131& s){shops.push_back(code);s={1,uintptr_t(code),3,{6,7},0,0};return shopValid;}
 bool Select(const wht::SelfCastSource131& s){if(selectOk)selected=s.unit;return selectOk;}
 void Restore(const wht::SelfCastSource131&){++restores;selected=hero.unit;}
 bool ShopValid(const wht::SelfCastSource131&,const wht::SelfCastSource131&,uint32_t){return shopValid;}
 bool Ready(const wht::MthStage&){return ready;}
 bool Click(unsigned slot){clicks.push_back(slot);return clicks.size()!=failAt;}
};
inline void CheckMthCraft131(){
 using namespace wht;
 uintptr_t units[12]{};units[11]=111;NativeUnitList131 list{12,units};InvokeCraftSelect131(reinterpret_cast<CraftSelect131>(CraftAbi131),123,list);check(craftAbiOk131,"native group selection ABI and exact six policy arguments");
 for(const auto& p:craftSelectionProfiles131){
#ifdef _WIN64
  const char* file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const char* file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);if(!input)continue;std::vector<char> image((std::istreambuf_iterator<char>(input)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());
  for(size_t n=0;n<p.count;++n){const auto& code=p.code[n];for(size_t k=0;k<code.relocCount;++k){const auto offset=code.reloc[k];unsigned value=0;memcpy(&value,code.bytes+offset,4);value+=unsigned(base);memcpy(image.data()+code.rva+offset,&value,4);}}
  check(CraftSelectionGuard131(base,p),"captured native local selection entry, caller ABI and UI member verified");image[p.code[1].rva]^=1;check(!CraftSelectionGuard131(base,p),"altered UI member guard refused");image[p.code[1].rva]^=1;image[p.code[2].rva]^=1;check(!CraftSelectionGuard131(base,p),"altered caller argument policy refused");
 }
 for(int kind=0;kind<5;++kind){CraftMock131 api;MthCraftRun131 run;auto recipe=MakeMthRecipe(kind,api.bag);std::vector<unsigned> expected;for(unsigned i=0;i<recipe.count;++i)for(unsigned j=0;j<recipe.stages[i].count;++j)expected.push_back(CraftSlot131(recipe.stages[i].slots[j]));
  check(run.Start(kind,0,true,api)==MthResult::Running,"all five recipes start");
  check(run.Start(kind,0,true,api)==MthResult::Busy,"second request cannot replace active purchases");
  check(run.Tick(99,true,true,api)==MthResult::Running&&api.clicks.empty(),"shop selection waits 100ms");
  MthResult result=MthResult::Idle;for(unsigned i=1;i<=recipe.count;++i)result=run.Tick(i*100,true,true,api);
  check(result==MthResult::Submitted&&!run.active&&api.clicks==expected&&api.restores==1&&api.selected==10,"exact mapped recipe slots once and selection restored");
  check(run.Tick(9000,true,true,api)==MthResult::Idle&&api.clicks==expected,"completed purchase never replays");
 }
 const unsigned mapping[]={0,4,8,1,5,9,2,6,10,3,7,11};for(int i=0;i<12;++i)check(CraftSlot131(i)==mapping[i],"memory indices transpose to native row grid");
 check(CraftSlot131(-1)==12&&CraftSlot131(12)==12,"invalid recipe slot refused");
 CraftMock131 api;MthCraftRun131 run;api.gold=9999;check(run.Start(0,0,true,api)==MthResult::Gold&&api.shops.empty(),"gold gate precedes shop selection");
 api={};api.bag[5]=0x49303245;api.gold=45000;check(run.Start(4,0,true,api)==MthResult::Running&&run.recipe.gold==45000,"Phoenix sees Frozen in final inventory slot");run.Reset();
 api={};api.bag={0x49303244,0x49303151,0x49303245};check(run.Start(0,0,true,api)==MthResult::Running&&run.recipe.stages[0].count==0,"owned Shield ingredients excluded");run.Reset();
 api={};run.Start(1,0,true,api);api.selected=99;check(run.Tick(100,true,true,api)==MthResult::Changed&&api.restores==0&&api.clicks.empty()&&api.selected==99,"user selection never stolen back");
 api={};run.Start(1,0,true,api);api.valid=false;check(run.Tick(100,true,true,api)==MthResult::Changed&&api.restores==0&&api.clicks.empty(),"dead/recycled hero or changed session stops");
 api={};run.Start(1,0,true,api);check(run.Tick(100,true,false,api)==MthResult::Running&&api.clicks.empty()&&api.restores==0,"chat menu focus macro pause has no native actions");check(run.Tick(100,false,false,api)==MthResult::Disabled&&api.restores==1,"disabled master restores selection and cancels");
 api={};run.Start(1,0,true,api);api.ready=false;check(run.Tick(1499,true,true,api)==MthResult::Running,"bounded card loading");check(run.Tick(1500,true,true,api)==MthResult::Unavailable&&api.clicks.empty()&&api.restores==1,"missing card expires without purchases");
 api={};run.Start(1,0,true,api);api.failAt=2;check(run.Tick(100,true,true,api)==MthResult::Failed&&api.clicks.size()==2&&api.restores==1,"partial purchase failure stops without replay");check(run.Tick(200,true,true,api)==MthResult::Idle&&api.clicks.size()==2,"partial failure not retried");
 api={};run.Start(0,0,true,api);api.shopValid=false;check(run.Tick(100,true,true,api)==MthResult::NoShop&&api.clicks.empty(),"shop lost or outside radius stops");
 api={};api.capture=false;check(run.Start(0,0,true,api)==MthResult::NoHero&&api.shops.empty(),"invalid hero refused before selection");
 api={};check(run.Start(0,0,false,api)==MthResult::Disabled&&api.shops.empty(),"authorization and master gate");
 api={};check(run.Queue(1,true,api)==MthResult::Running&&api.shops.empty(),"chat command queues without changing selection");check(run.Tick(10,true,false,api)==MthResult::Running&&api.shops.empty(),"queued recipe waits for chat close and ready context");check(run.Tick(20,true,true,api)==MthResult::Running&&api.shops.size()==1&&!run.queued,"ready UI starts queued recipe after recapture");run.Reset();
 api={};run.Queue(1,true,api);api.selected=99;check(run.Tick(20,true,true,api)==MthResult::Changed&&api.shops.empty(),"queued recipe refuses changed selection before any shop action");
 Config c;unsigned writes=0,reloads=0;auto save=[&](const wchar_t* section,const wchar_t*,const std::wstring& v){check(std::wstring(section)==L"MTHKEY"&&v==L"0","canonical craft display persistence");++writes;return true;};
 check(CraftDrawCommand131(L"#mthdraw")&&ApplyCraftDraw131(*CraftDrawCommand131(L"#mthdraw"),L"",c,save,[&](){++reloads;})&&writes==5&&reloads==1,"bare crafting group disables all when any is enabled");
 check(!ApplyCraftDraw131(*CraftDrawCommand131(L"#icedraw"),L"1 extra",c,save,[&](){++reloads;})&&writes==5,"malformed draw command never writes");
 for(const auto& d:kToolMenuControls)if(std::wstring(d.section)==L"MTHKEY"&&(CraftDrawSetting131(d.key)||std::wstring(d.key)==L"Shield"||std::wstring(d.key)==L"Ice"||std::wstring(d.key)==L"Wild"||std::wstring(d.key)==L"Frozen"||std::wstring(d.key)==L"Phoenix"))check(ControlAvailable131(d),"craft controls enabled");
 for(const auto& s:routeDrawSettings131)c.*s.field=true;c.pickGokuDraw=true;check(CraftToolbarStart131(c)==7,"craft buttons follow seven existing buttons without overlap");
}
