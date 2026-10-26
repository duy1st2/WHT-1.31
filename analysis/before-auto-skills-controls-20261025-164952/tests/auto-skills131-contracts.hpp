#pragma once
#include "auto_skills_native131.hpp"
#include <fstream>
inline bool autoFogAbiOk131=false;
#ifdef _WIN64
inline void __fastcall AutoFogAbi131(uintptr_t context,const float* point,unsigned* visible,unsigned* explored,const unsigned* views){
#else
inline void __fastcall AutoFogAbi131(uintptr_t context,void*,const float* point,unsigned* visible,unsigned* explored,const unsigned* views){
#endif
 autoFogAbiOk131=context==123&&point[0]==10&&point[1]==20&&point[2]==0&&views[0]==24&&views[1]==1;*visible=4;*explored=2;
}
struct AutoSkillMock131 {
 wht::SelfCastSource131 source{1,10,3,{4,5},0,0},target{1,20,3,{6,7},10,0};
 std::array<wht::SkillButton131,12> buttons{};unsigned reads=0,scans=0,casts=0,castSlot=99;bool readable=true,working=false,scanFails=false;
 bool ReadSource(wht::SelfCastSource131& out){++reads;out=source;return readable;}
 wht::SkillButton131 Data(unsigned slot){return buttons[slot];}
 wht::AutoScanResult131 Scan(const wht::SelfCastSource131&,float,wht::AutoSkillScan131& scan){++scans;scan.cursor+=128;scan.best=target;return scanFails?wht::AutoScanResult131::Failed:working?wht::AutoScanResult131::Working:wht::AutoScanResult131::Done;}
 bool Activate(const wht::SelfCastSource131& owner,const wht::SelfCastSource131& destination,unsigned slot,const wht::SkillButton131& requested,float range){check(wht::SameSelfCastSource131(owner,source)&&wht::SameSelfCastSource131(destination,target)&&wht::SameAutoSkill131(buttons[slot],requested)&&range==1024,"activation preserves exact owner target binding and range");++casts;castSlot=slot;return false;}
};
inline void CheckAutoSkills131(){
 {AutoSkillMock131 a;wht::AutoSkills131 state;wht::SkillButton131 skill{};skill.ability=123;skill.order=0x852000;
 check(state.Bind(a.source,0,skill)&&state.Bind(a.source,8,skill)&&state.Toggle(a.source,true),"editable sequence starts");
 state.scanning=true;check(state.Clear(a.source,0)&&!state.scanning&&state.running&&state.Summary(a.source)=="Auto Skills: ON; slots: 9","single removal cancels scan and retains other step");
 auto changed=a.source;changed.session++;check(!state.Clear(changed,-1)&&state.Current(a.source),"clear rejects stale identity");
 check(state.Clear(a.source,8)&&!state.running&&!state.Current(a.source),"last removal stops and releases sequence");
 check(state.Bind(a.source,1,skill)&&state.Clear(a.source,-1)&&!state.Current(a.source),"clear all removes selected sequence");
 int index=99;check(AutoSkillClearIndex131(L" all ",index)&&index==-1&&AutoSkillClearIndex131(L"9",index)&&index==8,"clear parser bounds");
 for(const auto* bad:{L"",L"0",L"10",L"1 extra",L"all\n"})check(!AutoSkillClearIndex131(bad,index),"clear parser refuses malformed args");}

 using namespace wht;AutoSkills131 state;AutoSkillMock131 api;const SkillButton131 first{0x41303031,0x852000},second{0x41303032,0x852001};api.buttons[3]=first;api.buttons[8]=second;
 for(unsigned i=0;i<10000;++i)state.Tick(i,true,true,.5f,1024,api);check(!api.reads&&!api.scans&&!api.casts,"inactive Auto Skills performs no source reads or scans across 10000 UI ticks");
 check(!state.Toggle(api.source,true),"activation requires bindings for selected identity");check(state.Bind(api.source,0,first)&&state.Bind(api.source,8,second)&&state.Toggle(api.source,true),"nine-slot per-unit sequence bound and enabled");
 state.Tick(0,true,true,.5f,1024,api);check(api.casts==1&&api.castSlot==3&&state.due==500,"first sequence item resolves current native grid position");const auto reads=api.reads;state.Tick(499,true,true,.5f,1024,api);check(api.reads==reads&&api.casts==1,"configured delay prevents polling or retry");
 state.Tick(500,true,true,.5f,1024,api);check(api.casts==2&&api.castSlot==8,"sparse sequence advances once even after native refusal");state.Tick(1000,true,false,.5f,1024,api);check(api.casts==2,"chat menu macro focus gates pause actions");
 api.working=true;state.Tick(1000,true,true,.5f,1024,api);check(state.scanning&&api.casts==2,"bounded target search can span UI updates");api.source.id.low++;state.Tick(1033,true,true,.5f,1024,api);check(!state.scanning&&api.casts==2,"selection identity change cancels unfinished search");api.source.id.low--;
 state.due=0;state.Tick(1100,true,true,.5f,1024,api);api.source.x=49;state.Tick(1133,true,true,.5f,1024,api);check(!state.scanning&&api.casts==2,"moving source beyond search snapshot tolerance cancels");api.source.x=0;
 state.due=0;state.Tick(1200,true,true,.5f,1024,api);api.buttons[state.slot]={};state.Tick(1233,true,true,.5f,1024,api);check(!state.scanning&&api.casts==2,"replaced command card never casts a stale slot");api.buttons[3]=first;api.buttons[8]=second;
 api.working=false;api.scanFails=true;state.due=0;state.Tick(1300,true,true,.5f,1024,api);check(!state.scanning&&api.casts==2,"failed native search submits no cast");
 state.Tick(1400,false,true,.5f,1024,api);check(!state.running&&!state.scanning,"disabled configuration stops automation");
 auto changed=api.source;changed.session++;check(!state.Toggle(changed,true),"old match bindings cannot be reused");changed=api.source;changed.id.low++;check(!state.Toggle(changed,true),"recycled unit pointer cannot reuse bindings");
 for(unsigned key='1';key<='9';++key)check(AutoSkillNumber131(key)==int(key-'1'),"number-row sequence slots");check(AutoSkillNumber131(VK_NUMPAD9)==8&&AutoSkillNumber131('0')==-1,"numpad supported and zero refused");
 for(unsigned order:{0u,0xd0003u,0xd0004u,0xd000fu,0xd0012u,0xd0016u,0xd0019u,0xd0020u,0xd0007u})check(!state.Bind(api.source,0,{0,order}),"empty/basic move attack stop and learn navigation never bind");
 check(!state.Bind(api.source,0,{AllSkills131::heroAbility,0x41303031}),"hero learn commands cannot become automatic casts");
 state.Reset();check(!state.running&&state.sequences.empty(),"UI teardown discards sequences and search");
 for(unsigned local=0;local<24;++local){const unsigned bit=1u<<local;check(AutoVisibilityPolicy131(0,bit,local,bit,bit),"visible and detected local target eligible");check(!AutoVisibilityPolicy131(0,0,local,bit,bit)&&!AutoVisibilityPolicy131(0,bit,local,0,bit)&&!AutoVisibilityPolicy131(0,bit,local,bit,0),"undetected fogged and missing viewport masks refused");check(!AutoVisibilityPolicy131(0x10,bit,local,bit,bit)&&!AutoVisibilityPolicy131(0x20,bit,local,bit,bit),"native bypass-fog flags refused conservatively");}check(!AutoVisibilityPolicy131(0,~0u,32,~0u,~0u),"invalid local slot never shifts mask");
 const float point[]={10,20,0};const unsigned views[]={24,1};unsigned visible=0,explored=0;InvokeAutoFog131(reinterpret_cast<AutoFogQuery131>(AutoFogAbi131),123,point,visible,explored,views);check(autoFogAbiOk131&&visible==4&&explored==2&&views[0]==24&&views[1]==1,"native fog-query ABI and independent output buffers with untouched vision input");
 SelfCastSource131 owner{1,10,3,{4,5},0,0},target{1,20,3,{6,7},10,0};uintptr_t mode=0;unsigned clicks=0,finishes=0,cancels=0,sourceReads=0;bool postValid=true,instant=false;
 auto readSource=[&](SelfCastSource131& out){out=owner;return ++sourceReads==1||postValid;};auto readTarget=[&](SelfCastSource131& out){out=target;return true;};auto getMode=[&](){return mode;};auto click=[&](){++clicks;if(!instant)mode=99;return true;};auto order=[](uintptr_t){return 0x852000u;};auto finish=[&](const SelfCastSource131& out,uintptr_t m,unsigned o){++finishes;check(out.unit==20&&m==99&&o==0x852000,"target mode/order and unit preserved");mode=0;return false;};auto cancel=[&](uintptr_t m){check(m==99,"only newly created mode cancelled");++cancels;mode=0;};
 check(!ActivateAutoSkill131(readSource,readTarget,getMode,click,order,finish,cancel)&&clicks==1&&finishes==1&&!cancels,"native submit refusal has one click one finish and no fallback replay");
 sourceReads=0;postValid=false;check(!ActivateAutoSkill131(readSource,readTarget,getMode,click,order,finish,cancel)&&cancels==1&&finishes==1,"changed owner after click cancels new targeting without submitting");
 sourceReads=0;postValid=true;instant=true;check(ActivateAutoSkill131(readSource,readTarget,getMode,click,order,finish,cancel)&&finishes==1,"instant ability not followed by a second action");mode=77;const auto beforeClicks=clicks;check(!ActivateAutoSkill131(readSource,readTarget,getMode,click,order,finish,cancel)&&clicks==beforeClicks&&mode==77,"manual target mode remains untouched");
 for(const auto& p:autoVisibilityProfiles131){
#ifdef _WIN64
  const auto file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const auto file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);if(!input)continue;std::vector<char> image((std::istreambuf_iterator<char>(input)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());for(size_t n=0;n<p.count;++n){const auto& code=p.code[n];for(size_t k=0;k<code.relocCount;++k){const auto at=code.reloc[k];unsigned v=0;memcpy(&v,code.bytes+at,4);v+=unsigned(base);memcpy(image.data()+code.rva+at,&v,4);}}
  check(AutoVisibilityCode131(base,p),"captured fog query ABI and native renderer visibility field offsets verified");image[p.code[1].rva]^=1;check(!AutoVisibilityCode131(base,p),"altered renderer visibility policy refused");image[p.code[1].rva]^=1;image[p.query]^=1;check(!AutoVisibilityCode131(base,p),"altered fog-query code refused");
 }
 unsigned controls=0;for(const auto& control:kToolMenuControls)if(std::wstring(control.section)==L"AutoSkills"){++controls;check(ControlAvailable131(control),"Auto Skills menu controls enabled");}check(controls==4,"exactly four existing Auto Skills settings exposed");
}
