#pragma once
#include "presentation131.hpp"
struct PresentationMock131 {
 std::vector<std::pair<unsigned,unsigned>> writes;unsigned fps=200,sync=1,reads=0;bool fail=false;
 bool Read(unsigned key,unsigned& value){++reads;if(fail)return false;value=key==17?fps:sync;return true;}
 bool Apply(unsigned key,unsigned value){if(fail)return false;writes.emplace_back(key,value);return true;}
 bool Unlock(){if(fail)return false;writes.emplace_back(17,0);return true;}
};
inline void CheckPresentation131(){
 using namespace wht;
 for(bool allowed:{false,true})for(bool unlock:{false,true})for(bool sync:{false,true})check(EffectiveUnlock131(allowed,unlock,sync)==(allowed&&unlock&&!sync),"VSync takes priority over FPS unlock");
 PresentationState131 state;PresentationMock131 api;
 check(state.Update(false,false,true,api)&&api.writes.empty()&&!api.reads,"unauthorized idle makes no native reads or changes");
 check(state.Update(true,false,true,api)&&state.limitOwned&&state.syncOwned&&api.writes==std::vector<std::pair<unsigned,unsigned>>{{19,0},{17,0}},"disable VSync before requesting zero frame interval");
 api.writes.clear();for(unsigned i=0;i<1000;++i)state.Update(true,false,true,api);check(api.writes.empty()&&!api.reads,"unchanged settings never repeat native writes");
 check(state.Update(true,true,true,api)&&!state.limitOwned&&state.syncValue&&api.writes==std::vector<std::pair<unsigned,unsigned>>{{17,200},{19,1}},"restore native frame limit before enabling VSync");
 api.writes.clear();state.Update(true,false,true,api);api.writes.clear();api.fps=144;api.sync=1;
 check(state.Update(false,false,false,api)&&!state.limitOwned&&!state.syncOwned&&api.writes==std::vector<std::pair<unsigned,unsigned>>{{17,144},{19,1}},"authorization loss restores current native settings, including stock menu edits");
 api.writes.clear();state.Update(true,false,true,api);api.writes.clear();api.fail=true;
 check(!state.Update(false,false,false,api)&&state.limitOwned&&state.syncOwned&&api.writes.empty(),"failed restoration retains ownership for retry");
 api.fail=false;check(state.Update(false,false,false,api)&&!state.limitOwned&&!state.syncOwned,"restoration retries successfully");
 api.fps=0;state.Update(true,false,true,api);api.writes.clear();check(state.Update(false,false,false,api)&&!state.limitOwned&&!api.writes.empty()&&api.writes[0]==std::make_pair(17u,0u),"zero native maxfps restores unlimited without dividing by zero");
 for(unsigned build:{12164u,12173u}){
  const auto* p=PresentationProfileFor131(build);check(p&&p->settings&&p->renderer&&p->read&&p->apply&&p->unlock,"independent native presentation profile exists");
  size_t size=0;for(size_t i=0;i<p->count;++i)size=std::max(size,size_t(p->code[i].rva+p->code[i].length));
  auto memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));check(memory!=nullptr,"allocate isolated signature image");if(!memory)continue;
  const uintptr_t base=reinterpret_cast<uintptr_t>(memory);
  for(size_t i=0;i<p->count;++i){const auto& c=p->code[i];memcpy(memory+c.rva,c.bytes,c.length);for(size_t j=0;j<c.relocCount;++j){unsigned val=0;memcpy(&val,memory+c.rva+c.reloc[j],4);val+=unsigned(base);memcpy(memory+c.rva+c.reloc[j],&val,4);}}
  check(PresentationGuard131(base,*p),"relocated profile accepts all captured dispatcher/case/scheduler signatures");
  for(size_t i=0;i<p->count;++i){auto& byte=memory[p->code[i].rva];byte^=1;check(!PresentationGuard131(base,*p),"tampering any presentation code range refuses profile");byte^=1;}
  PresentationNative131 native{base,p};check(!native.Ready()&&!native.Unlock(),"uninitialized native settings/renderer refuse writes");VirtualFree(memory,0,MEM_RELEASE);
 }
 check(!PresentationProfileFor131(99999),"unsupported presentation build refused");
 unsigned found=0;for(const auto& d:kToolMenuControls)if(std::wstring(d.section)==L"Graphics"&&(std::wstring(d.key)==L"VSync"||std::wstring(d.key)==L"Unlock Limit FPS")){++found;check(PresentationControl131(d.section,d.key)&&d.page==4,"presentation controls enabled in Graphics tab");}check(found==2,"both presentation controls exposed");
}
