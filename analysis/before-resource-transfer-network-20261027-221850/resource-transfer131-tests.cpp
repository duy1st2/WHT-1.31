#include "../native/resource_transfer131.hpp"
#include <cstdio>
#include <stdexcept>
#include <vector>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
unsigned calls=0;
#ifdef _WIN64
void Setter(uintptr_t player,unsigned state,int value){
#else
void __fastcall Setter(uintptr_t player,void*,unsigned state,int value){
#endif
 Check(player==123&&state==1&&value==0,"native zero balance ABI");++calls;
}
wht::ResourceTransferSource131 Players(){wht::ResourceTransferSource131 v;v.source.world=v.target.world=456;v.source.local=v.target.local=0;v.source.player=123;v.target.player=789;v.source.physical=0;v.target.physical=1;v.source.values[0]=500;v.source.values[1]=100;v.target.values[0]=20;v.target.values[1]=30;return v;}
int main(){try{
 wht::ResourceTransferRequest131 r;Check(wht::ParseResourceTransfer131(L"#gg",L"2 500",r)&&r.slot==1&&r.amounts[0]==500&&!r.amounts[1],"single gold one-based");Check(wht::ParseResourceTransfer131(L"#gl",L"24 100",r)&&r.slot==23&&r.amounts[1]==100,"single lumber max player");Check(wht::ParseResourceTransfer131(L"#ggl",L"0 500 100",r)&&r.slot==0&&r.amounts[0]==500&&r.amounts[1]==100,"dual zero-based");Check(wht::ParseResourceTransfer131(L"#gll",L"23 100 500",r)&&r.slot==23&&r.amounts[0]==500&&r.amounts[1]==100,"lumber first");
 for(const auto* bad:{L"0 1",L"25 1",L"2 0",L"2 -1",L"2 1.5",L"2 2147483648",L"2 1 extra",L"2 1\n"})Check(!wht::ParseResourceTransfer131(L"#gg",bad,r),"invalid single refused");Check(!wht::ParseResourceTransfer131(L"#gg",std::wstring(L"2 1\0x",6),r),"NUL refused");for(const auto* bad:{L"24 1 0",L"-1 1 0",L"1 0 0",L"1 1",L"1 -1 2",L"1 1 2 extra"})Check(!wht::ParseResourceTransfer131(L"#ggl",bad,r),"invalid dual refused");
 const auto native=reinterpret_cast<wht::ResourceSetNative131>(Setter);Check(wht::InvokeResourceTransferSet131(native,123,1,0)&&calls==1,"debit all balance supported");Check(!wht::InvokeResourceTransferSet131(native,123,1,-1)&&!wht::InvokeResourceTransferSet131(native,123,1,10000000)&&!wht::InvokeResourceTransferSet131(native,123,4,0)&&calls==1,"invalid native args no call");
 auto target=Players();unsigned reads=0,writes=0,changedAt=0,refuseAt=0,noOpAt=0;bool otherChanged=false;std::vector<unsigned> order;std::string report;
 auto read=[&](unsigned slot,wht::ResourceTransferSource131& out){Check(slot==1,"original recipient slot retained");out=target;if(++reads==changedAt)++out.target.player;if(otherChanged&&writes==1)++out.target.values[1];return true;};
 auto set=[&](unsigned slot,const wht::ResourceTransferSource131& before,bool recipient,unsigned state,int value){Check(slot==1&&wht::ResourceTransferBalances131(before,target),"stable players and balances");order.push_back((recipient?10:0)+state);if(++writes==refuseAt)return false;if(writes!=noOpAt)(recipient?target.target:target.source).values[state-1]=value;return true;};
 auto run=[&](const std::wstring& name,const std::wstring& args){reads=writes=0;order.clear();return wht::ResourceTransferCommand131(name,args,read,set,report);};
 Check(run(L"#ggl",L"1 500 100")&&target.source.values[0]==0&&target.source.values[1]==0&&target.target.values[0]==520&&target.target.values[1]==130&&order==std::vector<unsigned>({1,11,2,12}),"full balances debit then credit per currency");
 target=Players();Check(run(L"#gl",L"2 100")&&order==std::vector<unsigned>({2,12})&&target.source.values[0]==500,"single lumber skips gold");target=Players();Check(run(L"#gll",L"1 100 500")&&target.target.values[0]==520,"reverse amount order");
 target=Players();Check(!run(L"#gg",L"2 501")&&!writes,"insufficient funds no writes");target.target.values[0]=9999999;Check(!run(L"#gg",L"2 1")&&!writes,"native target cap no writes");target=Players();target.target.player=target.source.player;Check(!run(L"#gg",L"2 1")&&!writes,"self refused");target=Players();target.target.physical=target.source.physical;Check(!run(L"#gg",L"2 1")&&!writes,"duplicate physical player refused");target=Players();
 changedAt=2;Check(!run(L"#gg",L"2 1")&&!writes,"recipient changed before debit");changedAt=4;Check(!run(L"#gg",L"2 1")&&writes==1&&target.source.values[0]==499&&target.target.values[0]==20&&report.find("some debits")!=report.npos,"recipient callback change stops credit no rollback");changedAt=0;
 target=Players();refuseAt=2;Check(!run(L"#ggl",L"1 100 50")&&writes==2&&target.source.values[0]==400&&target.source.values[1]==100,"credit refusal stops later currency");refuseAt=0;target=Players();noOpAt=1;Check(!run(L"#gg",L"2 100")&&writes==1,"unconfirmed debit stops credit");noOpAt=0;target=Players();otherChanged=true;Check(!run(L"#gg",L"2 100")&&writes==1,"callback changed other balance stops credit");otherChanged=false;
 Check(!run(L"#gg",L"2 1 extra")&&!reads&&!writes,"malformed request no game access");puts("resource transfer contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
