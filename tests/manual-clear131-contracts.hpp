#pragma once
#include "manual_clear131.hpp"
inline void CheckManualClear131(){
 for(auto name:{L"#clear",L"#cls"})check(wht::ManualClearKind131(name)==1,"text clear reference aliases");
 for(auto name:{L"#clearc",L"#clsc"})check(wht::ManualClearKind131(name)==2,"chat clear reference aliases");
 check(!wht::ManualClearKind131(L"#clearall"),"unknown alias does not clear");
 wht::ManualClear131 state;std::vector<bool> calls;bool typing=true;unsigned reads=0;
 auto chat=[&](bool& value){++reads;value=typing;return true;};auto clear=[&](bool value){calls.push_back(value);return true;};
 check(!state.Queue(0,1,0)&&!state.Queue(1,3,0),"invalid owner and scope rejected");
 check(state.Queue(11,1,100)&&state.Queue(11,1,100),"duplicates coalesce");
 check(!state.Tick(11,true,true,101,chat,clear)&&calls.empty(),"native editor remains untouched before Enter closes");
 typing=false;check(!state.Tick(11,true,false,102,chat,clear)&&reads==1,"menu/focus/input gate does not even inspect chat");
 check(state.Tick(11,true,true,103,chat,clear)==1&&calls==std::vector<bool>{false}&&!state.pending,"one text clear after native chat closes");
 check(!state.Tick(11,true,true,104,chat,clear)&&calls.size()==1,"request never repeats");
 state.Queue(11,1,200);state.Queue(11,2,201);calls.clear();check(state.Tick(11,true,true,202,chat,clear)==3&&calls==std::vector<bool>({false,true}),"independent text and chat scopes coalesce once each");
 for(unsigned gate=0;gate<3;++gate){state.Queue(11,2,300);calls.clear();state.Tick(gate==0?22:11,gate!=1,true,gate==2?2300:301,chat,clear);check(!state.pending&&calls.empty(),"owner change, denied session and timeout cancel instead of replaying");}
 state.Queue(11,2,400);check(!state.Tick(11,true,true,401,chat,[](bool){return false;})&&!state.pending,"native refusal consumed without retry");
 state.Queue(11,1,500);check(!state.Tick(11,true,true,501,[](bool&){return false;},clear)&&state.pending,"unknown native editor cannot clear");
 state.Reset();check(!state.owner&&!state.pending,"match teardown clears request");
 state.Queue(11,1,600);unsigned nested=99;check(state.Tick(11,true,true,601,chat,[&](bool){nested=state.Tick(11,true,true,601,chat,clear);return true;})==1&&!nested,"native reentry cannot execute request twice");
}
